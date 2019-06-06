// load_logisim.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Load circuit from a LogiSim circuit file
//  This is not intended to be a fully compatible with all the features of Logisim.
//  We're basically using Logisim as an easy way to construct our circuits.
//  Current restrictions / not supported features:
//  - only basic gates
//  - gates are always 1 bit
//  - connectors (Logisim: Pin) and buffers support > 1 data bits but have a pin per bit
//      - NO: disabled again after rewrite / logisim support is low priority for now
//  - XOR gates are limited to two inputs
//  - no custom subcircuit appearance, just the standard inputs on the left and outputs on the right
//      - circuits to be nested should have "Use new box layout" (circuitnamedbox) and 
//        "Use fixed box-size" (circuitnameboxfixedsize) set to Yes (true)
//  - probably lots of other stuff that I'm not aware of 8-)
//  - wires do not have a visual representation

#include <pugixml.hpp>
#include <string>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cassert>
#include <cstdlib>

#include "circuit_description.h"
#include "lsim_context.h"
#include "error.h"

namespace {

using namespace lsim;

#define DEF_REQUIRED_ATTR(res, node, attr) \
    auto attr_##attr = (node).attribute(#attr); \
    if (!attr_##attr) { \
        return false;   \
    }                   \
    std::string res = attr_##attr.value();

class LogisimParser {
public:
    LogisimParser(pugi::xml_document *xml_doc, LSimContext *lsim_context) : 
            m_xml_doc(xml_doc), 
            m_lsim_context(lsim_context) {
    }

    bool parse_xml();

private:
    union Position {
        Position() : m_x(0), m_y(0) {}
        Position(int32_t x, int32_t y) : m_x(x), m_y(y) { }
        struct {
            int32_t m_y;
            int32_t m_x;
        };
        uint64_t m_full;
    };

    struct LogisimConnection {
        Position            m_position;
        pin_id_container_t  m_pins;
    };

    enum LogisimDirection {
        LS_NORTH,
        LS_EAST,
        LS_SOUTH,
        LS_WEST
    };

    struct ComponentProperties {
        size_t              m_size;
        size_t              m_extra_size;
        size_t              m_inputs;
        Position            m_location;
        LogisimDirection    m_facing;
        std::string         m_label;
        size_t              m_width;
        bool                m_negate_output;
        bool                m_negate_input;
        size_t              m_splitter_fanout;
        size_t              m_splitter_incoming;
        int                 m_splitter_justify;
        bool                m_pin_output;
        bool                m_pin_tristate;
    };

    typedef std::vector<uint64_t>       wire_node_t;
    typedef std::vector<wire_node_t>    wire_container_t;
    typedef std::unordered_map<std::string, size_t> tunnel_wire_map_t;

    struct CircuitConstruction {
        CircuitDescription *m_circuit;
        std::unordered_map<uint64_t, LogisimConnection> m_pin_locs;
        std::map<uint64_t, std::string> m_circuit_ipins[2];     
        std::unordered_map<std::string, Position> m_ipin_offsets;
        wire_container_t m_wires;
        tunnel_wire_map_t m_tunnels;
    };

    typedef std::unordered_map<std::string, CircuitConstruction> circuit_map_t;

private:
    bool parse_circuit(pugi::xml_node &circuit_node);
    bool parse_component(pugi::xml_node &comp_node);
    bool parse_wire(pugi::xml_node &wire_node);

    bool connect_components();

    Component *handle_sub_circuit(const std::string &name, ComponentProperties &props);
    bool handle_gate(Component *component, ComponentProperties &props);
    bool handle_not_gate(Component *component, ComponentProperties &props);
    void handle_buffer(Component *component, ComponentProperties &props, bool tri_state, bool left);
    bool handle_pin(Component *connector, ComponentProperties &props);
    bool handle_splitter(ComponentProperties &props);
    bool handle_tunnel(ComponentProperties &props);
    void compute_ipin_offsets();

    bool parse_location(const std::string &loc_string, Position &pos);
    bool parse_facing(const std::string &facing_string, LogisimDirection &facing);
    bool parse_splitter_appearance(const std::string &appear_string, int &justify);

    void add_pin_location(const Position &loc, pin_id_t pin);
    void add_pin_location(const Position &loc, pin_id_t start, size_t count);

    Position input_pin_location(Position base, 
                                size_t index, 
                                ComponentProperties &props);
    wire_node_t *point_on_wire(Position position);

private: 
    pugi::xml_document *m_xml_doc;
    LSimContext *m_lsim_context;
    circuit_map_t m_circuits;
    CircuitConstruction m_context;
};

bool LogisimParser::parse_xml() {

    auto project_node = m_xml_doc->child("project");
    if (!project_node) {
        return false;
    }

    /* iterate all circuits */
    for (auto circuit_node : project_node.children("circuit")) {
        parse_circuit(circuit_node);
    }

    /* get main circuit */
    auto main_node = project_node.child("main");
    if (!main_node) {
        return false;
    }

    auto main_circuit = main_node.attribute("name").value();
    m_lsim_context->user_library()->change_main_circuit(main_circuit);
    return true;
}

bool LogisimParser::parse_circuit(pugi::xml_node &circuit_node) {

    /* find name of the circuit */
    auto name_node = circuit_node.find_child_by_attribute("a", "name", "circuit");
    if (!name_node) {
        return false;
    }
    auto circuit_name = name_node.attribute("val").value();

    /* create the circuit */
    m_context.m_circuit = m_lsim_context->create_user_circuit(circuit_name);
    m_context.m_circuit_ipins[0].clear();
    m_context.m_circuit_ipins[1].clear();
    m_context.m_pin_locs.clear();
    m_context.m_ipin_offsets.clear();
    m_context.m_wires.clear();
    m_context.m_tunnels.clear();

    /* iterate all components */
    for (auto comp : circuit_node.children("comp")) {
        parse_component(comp);
    }

    /* iterate all wires */
    for (auto wire : circuit_node.children("wire")) {
        parse_wire(wire);
    }

    /* connect components */
    connect_components();

    /* sub-circuit interface */
    compute_ipin_offsets();

    m_circuits[circuit_name] = std::move(m_context);
    return true;
}

bool LogisimParser::parse_component(pugi::xml_node &comp_node) {
    DEF_REQUIRED_ATTR(comp_type, comp_node, name);
    DEF_REQUIRED_ATTR(comp_loc, comp_node, loc);

    ComponentProperties comp_props = {0};
    comp_props.m_facing = LS_EAST;
    comp_props.m_inputs = 2;
    comp_props.m_width = 1;
    comp_props.m_splitter_incoming = 2;
    comp_props.m_splitter_fanout = 2;
    comp_props.m_splitter_justify = -1;
    comp_props.m_pin_output = false;
    comp_props.m_pin_tristate = false;
    bool tristate_left = false;
    Value constant_val = VALUE_TRUE;
    Value pull_val = VALUE_FALSE;

    if (!parse_location(comp_loc, comp_props.m_location)) {
        return false;
    }

    for (auto prop : comp_node.children("a")) {
        DEF_REQUIRED_ATTR(prop_name, prop, name);
        DEF_REQUIRED_ATTR(prop_val, prop, val);

        if (prop_name == "label") {
            comp_props.m_label = prop_val;
        } else if (prop_name == "size") {
            comp_props.m_size = attr_val.as_int(comp_props.m_size);
        } else if (prop_name == "facing") {
            parse_facing(prop_val, comp_props.m_facing);
        } else if (prop_name == "inputs") {
            comp_props.m_inputs = attr_val.as_int(comp_props.m_inputs);
        } else if (prop_name == "control") {
            tristate_left = (prop_val == "left");
        } else if (prop_name == "value") {
            constant_val = (prop_val == "0x0") ? VALUE_FALSE : VALUE_TRUE;
        } else if (prop_name == "output") {
            comp_props.m_pin_output = (prop_val == "true");
        } else if (prop_name == "tristate") {
            comp_props.m_pin_tristate = (prop_val == "true");
        } else if (prop_name == "width") {
            comp_props.m_width = attr_val.as_int(comp_props.m_width);
        } else if (prop_name == "fanout") {
            comp_props.m_splitter_fanout = attr_val.as_int(0);
        } else if (prop_name == "incoming") {
            comp_props.m_splitter_incoming = attr_val.as_int(0);
        } else if (prop_name == "appear") {
            parse_splitter_appearance(prop_val, comp_props.m_splitter_justify);
        } else if (prop_name == "pull") {
            pull_val = (prop_val == "1") ? VALUE_TRUE : (prop_val == "X") ? VALUE_ERROR : VALUE_FALSE;
        }
    }

    Component *component = nullptr;
    bool ok = true;

    if (comp_type == "Buffer") {
        component = m_context.m_circuit->add_buffer(comp_props.m_width);
        handle_buffer(component, comp_props, false, false);
    } else if (comp_type == "Controlled Buffer") {
        component = m_context.m_circuit->add_tristate_buffer(comp_props.m_width);
        handle_buffer(component, comp_props, true, tristate_left);
    } else if (comp_type == "Constant") {
        component = m_context.m_circuit->add_constant(constant_val);
        add_pin_location(comp_props.m_location, component->pin_id(0));
    } else if (comp_type == "Pin") {
        if (!comp_props.m_pin_output) {
            component = m_context.m_circuit->add_connector_in(comp_props.m_label.c_str(), comp_props.m_width);
        } else {
            component = m_context.m_circuit->add_connector_out(comp_props.m_label.c_str(), comp_props.m_width);
        }
        ok = handle_pin(component, comp_props);
    } else if (comp_type == "AND Gate") {
        component = m_context.m_circuit->add_and_gate(comp_props.m_inputs);
        ok = handle_gate(component, comp_props);
    } else if (comp_type == "OR Gate") {
        component = m_context.m_circuit->add_or_gate(comp_props.m_inputs);
        ok = handle_gate(component, comp_props);
    } else if (comp_type == "NOT Gate") {
        component = m_context.m_circuit->add_not_gate();
        comp_props.m_inputs = 1;
        ok = handle_not_gate(component, comp_props);
    } else if (comp_type == "NAND Gate") {
        component = m_context.m_circuit->add_nand_gate(comp_props.m_inputs);
        comp_props.m_negate_output = true;
        ok = handle_gate(component, comp_props);
    } else if (comp_type == "NOR Gate") {
        component = m_context.m_circuit->add_nor_gate(comp_props.m_inputs);
        comp_props.m_negate_output = true;
        ok = handle_gate(component, comp_props);
    } else if (comp_type == "XOR Gate") {
        if (comp_props.m_inputs != 2) {
            ERROR_MSG("XOR-gates with %d inputs are not supported", comp_props.m_inputs);
            return false;
        }
        component = m_context.m_circuit->add_xor_gate();
        comp_props.m_extra_size = 10;
        ok = handle_gate(component, comp_props);
    } else if (comp_type == "XNOR Gate") {
        if (comp_props.m_inputs != 2) {
            ERROR_MSG("XOR-gates with %d inputs are not supported", comp_props.m_inputs);
            return false;
        }
        component = m_context.m_circuit->add_xnor_gate();
        comp_props.m_extra_size = 10;
        comp_props.m_negate_output = true;
        ok = handle_gate(component, comp_props);
    } else if (comp_type == "Splitter") {
        ok = handle_splitter(comp_props);
    } else if (comp_type == "Tunnel") {
        ok = handle_tunnel(comp_props);
    } else if (comp_type == "Pull Resistor") {
        component = m_context.m_circuit->add_pull_resistor(pull_val);
        add_pin_location(comp_props.m_location, component->pin_id(0));
    } else if (comp_type == "Text" || comp_type == "Probe") {
        // ignore
    } else {
        component = handle_sub_circuit(comp_type, comp_props); 
        if (!component) {
            ERROR_MSG("Unsupport component (%s) - loading failed", comp_type.c_str());
            ok = false;
        }
    }

    auto convert_facing = [](LogisimDirection facing) {
        switch (facing) {
            case LS_NORTH : return 270;
            case LS_EAST : return 0;
            case LS_SOUTH : return 90;
            default: return 180;
        }
    };

    if (ok && component) {
        component->set_position({2.0f * comp_props.m_location.m_x, 2.0f * comp_props.m_location.m_y});
        component->set_angle(convert_facing(comp_props.m_facing));
    }

    return ok;
}

bool LogisimParser::parse_wire(pugi::xml_node &wire_node) {
    DEF_REQUIRED_ATTR(wire_from, wire_node, from);
    DEF_REQUIRED_ATTR(wire_to, wire_node, to);

    Position w1, w2;
    if (!parse_location(wire_from, w1) || 
        !parse_location(wire_to, w2)) {
        return false;
    }

    // check if wire should be merged with existing wire
    auto node_1 = point_on_wire(w1);
    auto node_2 = point_on_wire(w2);

    if (!node_1 && !node_2) {
        wire_node_t new_node = {w1.m_full, w2.m_full};
        m_context.m_wires.push_back(new_node);
        return true;
    }

    if (node_1 && node_2) {
        // wire connects two existing nodes (merge node_2 into node_1)
        for (auto &w : *node_2) {
            node_1->push_back(w);
        }
        node_2->clear();
        m_context.m_wires.erase(std::find(std::begin(m_context.m_wires), std::end(m_context.m_wires), *node_2));
        return true;
    }

    if (node_1 && !node_2) {
        node_1->push_back(w2.m_full);
        return true;
    }

    if (!node_1 && node_2) {
        node_2->push_back(w1.m_full);
        return true;
    }

    return false;
}

bool LogisimParser::connect_components() {

    for (const auto &node : m_context.m_wires) {
        // XXX this isn't correct, should create a separate wire for each entry in connection
        auto wire = m_context.m_circuit->create_wire();

        for (const auto &point : node) {
            auto pin_pair = m_context.m_pin_locs.find(point);
            if (pin_pair != std::end(m_context.m_pin_locs)) {
                auto &c = pin_pair->second;
                for (size_t i = 0; i < c.m_pins.size(); ++i) {
                    wire->add_pin(c.m_pins[i]);
                }
            }
        }

        if (wire->num_pins() <= 0) {
            m_context.m_circuit->remove_wire(wire->id());
        }
    }

    return true;
}

Component *LogisimParser::handle_sub_circuit(const std::string &name, ComponentProperties &props) {

    // find the required circuit
    auto result = m_circuits.find(name);
    if (result == m_circuits.end()) {
        return nullptr;
    }
    auto sub_context = &result->second;
    auto component = m_context.m_circuit->add_sub_circuit(name.c_str());

    // add interface pins to the current circuit
    for (const auto &offset : sub_context->m_ipin_offsets) {
        auto p = Position({
            props.m_location.m_x + offset.second.m_x, 
            props.m_location.m_y + offset.second.m_y
        });
        // FIXME: this doesn't work with multi-bit pins
        add_pin_location(p, component->port_by_name(offset.first.c_str()));
    }

    return component;
}

bool LogisimParser::handle_gate(Component *component, ComponentProperties &props) {

    if (props.m_width != 1) {
        ERROR_MSG("%d-bit gates are not supported", props.m_width);
        return false;
    }

    // default size = MEDIUM (50)
    if (props.m_size == 0) {
        props.m_size = 50;
    }

    // inputs
    for (auto idx = 0u; idx < props.m_inputs; ++idx) {
        add_pin_location(input_pin_location(props.m_location, idx, props),
                         component->input_pin_id(idx)
        );
    }

    // output
    add_pin_location(props.m_location, component->output_pin_id(0));
    return true;
}

bool LogisimParser::handle_not_gate(Component *component, ComponentProperties &props) {

    if (props.m_width != 1) {
        ERROR_MSG("%d-bit gates are not supported", props.m_width);
        return false;
    }

    // default size = WIDE (30)
    if (props.m_size == 0) {
        props.m_size = 30;
    }

    // input
    add_pin_location(input_pin_location(props.m_location, 0, props),
                     component->input_pin_id(0)
    );

    // output
    add_pin_location(props.m_location, component->output_pin_id(0));
    return true;
}

void LogisimParser::handle_buffer(Component *component, ComponentProperties &props, bool tri_state, bool left) {

    // buffers have a fixed size
    props.m_inputs = 1;
    props.m_size = 20;

    // input
    add_pin_location(input_pin_location(props.m_location, 0, props), component->input_pin_id(0), component->num_inputs());

    // enable pin
    if (tri_state) {
        auto en_loc = props.m_location;
        en_loc.m_x -= props.m_size / 2;
        en_loc.m_y += (left) ? -10 : 10;
        add_pin_location(en_loc, component->control_pin_id(0));
    }

    // output
    add_pin_location(props.m_location, component->output_pin_id(0), component->num_outputs());
}

bool LogisimParser::handle_pin(Component *connector, ComponentProperties &props) {
    add_pin_location(props.m_location, connector->pin_id(0), connector->num_inputs() + connector->num_outputs());
    m_context.m_circuit_ipins[props.m_pin_output ? 1 : 0][props.m_location.m_full] = props.m_label;

    // flip the orientation of output pins
    if (props.m_pin_output) {
        switch (props.m_facing) {
            case LS_NORTH : props.m_facing = LS_SOUTH;
            case LS_EAST : props.m_facing = LS_WEST;
            case LS_SOUTH : props.m_facing = LS_NORTH;
            case LS_WEST : props.m_facing = LS_EAST;
        }
    }

    // XXX if (props.m_pin_tristate) {
    //       connector->set_tristate(true);
    //}

    return true;
}

bool LogisimParser::handle_splitter(ComponentProperties &props) {
/* XXX
    // input
    Component::pin_container_t input_pins;

    for (int i = 0; i < props.m_splitter_incoming; ++i) {
        input_pins.push_back(m_context.m_circuit->create_pin(nullptr));
    }

    add_pin_location(props.m_location, input_pins);

    // output pins
    int width = 20;

    int start_x = 0, start_y = 0;
    int dx = 0, dy = 0;

    if (props.m_facing == LS_NORTH || props.m_facing == LS_SOUTH) {
        int m = (props.m_facing == LS_NORTH) ? 1 : -1;
        if (props.m_splitter_justify == 0) {
            start_x =  10 * (((props.m_splitter_fanout + 1) / 2) - 1);
        } else if (m * props.m_splitter_justify < 0) {
			start_x = -10 ;
        } else {
            start_x = 10 * props.m_splitter_fanout;
        }
        start_y = -m * width;
        dx = -10;
        dy = 0;
    } else {
        int m = props.m_facing == LS_WEST ? -1 : 1;
        start_x = m * width;
        if (props.m_splitter_justify == 0) {
            start_y = -10 * (props.m_splitter_fanout / 2);
        } else if (m * props.m_splitter_justify > 0) {
            start_y = 10;
        } else {
			start_y = -10 * props.m_splitter_fanout;
        }
		dx = 0;
		dy = 10;
    }

    // calculate the number of pins per fanout connection
    std::vector<size_t> fanout_pins(props.m_splitter_fanout);
    size_t pins_left = props.m_splitter_incoming;

    for (int i = 0; i < props.m_splitter_fanout; ++i) {
        size_t num = pins_left / (props.m_splitter_fanout - i);
        if (num * (props.m_splitter_fanout - i) < pins_left) {
            num += 1;
        }

        fanout_pins[i] = num;
        pins_left -= num;
    }

    Position p(props.m_location.m_x + start_x, props.m_location.m_y + start_y);
    int input_idx = 0;

    for (int i = 0; i < props.m_splitter_fanout; ++i) {
        Component::pin_container_t pins;

        for (int j = 0; j < fanout_pins[i]; ++j) {
            pins.push_back(m_context.m_circuit->create_pin(nullptr, input_pins[input_idx++]));
        }
        add_pin_location(p, pins);
        p.m_x += dx;
        p.m_y += dy;
    }
*/
    return true;
}

bool LogisimParser::handle_tunnel(ComponentProperties &props) {
    if (props.m_label.empty()) {
        ERROR_MSG("Cannot process tunnel with an empty label (%s)", props.m_label.c_str());
        return false;
    }
    
    auto res = m_context.m_tunnels.find(props.m_label);
    if (res != m_context.m_tunnels.end()) {
        m_context.m_wires[res->second].push_back(props.m_location.m_full);
        return true;
    }

    wire_node_t new_node = {props.m_location.m_full};
    m_context.m_wires.push_back(new_node);
    m_context.m_tunnels[props.m_label] = m_context.m_wires.size() - 1;
    return true;
}

void LogisimParser::compute_ipin_offsets() {
    Position offsets[2] = {
        {-220, 0},          // inputs (on the left)
        {0,0}               // outputs (on the right)
    };

    for (int io = 0; io < 2; ++io) {
        for (auto ipin : m_context.m_circuit_ipins[io]) {
            m_context.m_ipin_offsets[ipin.second] = offsets[io];
            offsets[io].m_y += 20;
        }
    }
}

bool LogisimParser::parse_location(const std::string &loc_string, Position &pos) {
    if (loc_string.front() != '(' || loc_string.back() != ')') {
        ERROR_MSG("Unparseable location \"%s\"; should start/end with parentheses", loc_string.c_str());
        return false;
    }

    auto comma = loc_string.find_first_of(",");
    if (comma == loc_string.npos) {
        ERROR_MSG("Unparseable location \"%s\"; should contain a comma", loc_string.c_str());
        return false;
    }

    pos.m_x = std::strtol(loc_string.c_str() + 1, nullptr, 0);
    pos.m_y = std::strtol(loc_string.c_str() + comma + 1, nullptr, 0);
    return true;
}

bool LogisimParser::parse_facing(const std::string &facing_string, LogisimDirection &facing) {

    if (facing_string == "north") {
        facing = LS_NORTH;
    } else if (facing_string == "east") {
        facing = LS_EAST;
    } else if (facing_string == "south") {
        facing = LS_SOUTH;
    } else if (facing_string == "west") {
        facing = LS_WEST;
    } else {
        ERROR_MSG("Invalid facing \"%s\"", facing_string.c_str());
        return false;
    }

    return true;
}

bool LogisimParser::parse_splitter_appearance(const std::string &appear_string, int &justify) {

    if (appear_string == "center" || appear_string == "legacy") {
        justify = 0;
    } else if (appear_string == "right") {
        justify = 1;
    } else if (appear_string == "left") {
        justify = -1;
    } else {
        ERROR_MSG("Invalid splitter appearance \"%s\"", appear_string.c_str());
        return false;
    }

    return true;
}

void LogisimParser::add_pin_location(const Position &loc, pin_id_t pin) {
    LogisimConnection connection;
    connection.m_position = loc;
    connection.m_pins.push_back(pin);

    // connect if there is already a pin in the same location
    auto res = m_context.m_pin_locs.find(loc.m_full);
    if (res != m_context.m_pin_locs.end()) {
        // XXX make_connection(connection, res->second);
    } else {
       m_context.m_pin_locs[loc.m_full] = connection;
    }      
}

void LogisimParser::add_pin_location(const Position &loc, pin_id_t start, size_t count) {
    LogisimConnection connection;
    connection.m_position = loc;

    for (size_t i = 0; i < count; ++i) {
       connection.m_pins.push_back(start + i);
    }

    // connect if there is already a pin in the same location
    auto res = m_context.m_pin_locs.find(loc.m_full);
    if (res != m_context.m_pin_locs.end()) {
        // XXX make_connection(connection, res->second);
    } else {
       m_context.m_pin_locs[loc.m_full] = connection;
    }
}

LogisimParser::Position LogisimParser::input_pin_location( 
        Position base, 
        size_t index, 
        ComponentProperties &props) {

    int axis_length = props.m_size + props.m_extra_size + (props.m_negate_output ? 10 : 0);

    int skipStart;
    int skipDist;
    int skipLowerEven = 10;

	if (props.m_inputs <= 3) {
        if (props.m_size < 40) {
            skipStart = -5;
            skipDist = 10;
            skipLowerEven = 10;
        } else if (props.m_size < 60 || props.m_inputs <= 2) {
            skipStart = -10;
            skipDist = 20;
            skipLowerEven = 20;
        } else {
            skipStart = -15;
            skipDist = 30;
            skipLowerEven = 30;
        }
    } else if (props.m_inputs == 4 && props.m_size >= 60) {
        skipStart = -5;
        skipDist = 20;
        skipLowerEven = 0;
    } else {
        skipStart = -5;
        skipDist = 10;
        skipLowerEven = 10;
    }

	int dy;
	if ((props.m_inputs & 1) == 1) {
        dy = skipStart * (props.m_inputs - 1) + skipDist * index;
    } else {
        dy = skipStart * props.m_inputs + skipDist * index;
        if (index >= props.m_inputs / 2)
            dy += skipLowerEven;
    }

	int dx = axis_length + (props.m_negate_input ? 10 : 0);

	if (props.m_facing == LS_NORTH) {
        return {base.m_x + dy, base.m_y + dx};
    } else if (props.m_facing == LS_SOUTH) {
        return {base.m_x + dy, base.m_y - dx};
    } else if (props.m_facing == LS_WEST) {
        return {base.m_x + dx, base.m_y + dy};
    } else {
        return {base.m_x - dx, base.m_y + dy};
    }
}

LogisimParser::wire_node_t *LogisimParser::point_on_wire(Position position) {

    for (auto &node : m_context.m_wires) {
        auto res = std::find(std::begin(node), std::end(node), position.m_full);
        if (res != std::end(node)) {
            return &node;
        }
    }

    return nullptr;
}

} // unnamed namespace

//
// interface
//

namespace lsim {

bool load_logisim(LSimContext *lsim_context, const char *filename) {
    pugi::xml_document xml_doc;

    auto result = xml_doc.load_file(filename);
    if (!result) {
        return false;
    }

    auto parser = LogisimParser(&xml_doc, lsim_context);
    return parser.parse_xml();
}

bool load_logisim(LSimContext *lsim_context, const char *data, size_t len) {
    pugi::xml_document xml_doc;

    auto result = xml_doc.load_buffer(data, len);
    if (!result) {
        return false;
    }

    auto parser = LogisimParser(&xml_doc, lsim_context);
    return parser.parse_xml();
}

} // namespace lsim