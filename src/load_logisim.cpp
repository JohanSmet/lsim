// load_logisim.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Load circuit from a LogiSim circuit

#include <pugixml.hpp>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cassert>

#include "circuit.h"
#include "gate.h"

#define DEF_REQUIRED_ATTR(res, node, attr) \
    auto attr_##attr = (node).attribute(#attr); \
    if (!attr_##attr) { \
        return false;   \
    }                   \
    std::string res = attr_##attr.value();

class LogisimParser {
public:
    LogisimParser(pugi::xml_document *xml_doc, Circuit *circuit) : 
            m_xml_doc(xml_doc), 
            m_circuit(circuit) {
    }

    bool parse_xml();

private:
    union Position {
        struct {
            int32_t m_x;
            int32_t m_y;
        };
        int64_t m_full;
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
        bool                m_negate_output;
        bool                m_negate_input;
    };

    typedef std::vector<int64_t>        wire_node_t;
    typedef std::vector<wire_node_t>    wire_container_t;

private:
    bool parse_component(pugi::xml_node &comp_node);
    bool parse_wire(pugi::xml_node &wire_node);
    bool connect_components();

    void handle_gate(Component *component, ComponentProperties &props);
    void handle_not_gate(Component *component, ComponentProperties &props);
    void handle_buffer(Component *component, ComponentProperties &props, bool tri_state, bool left);

    bool parse_location(const std::string &loc_string, Position &pos);
    bool parse_facing(const std::string &facing_string, LogisimDirection &facing);
    void add_pin_location(pin_t pin, const Position &loc);

    Position input_pin_location(Position base, 
                                size_t index, 
                                ComponentProperties &props);
    wire_node_t *point_on_wire(Position position);

private: 
    pugi::xml_document *m_xml_doc;
    Circuit *m_circuit;
    std::unordered_map<int64_t, pin_t> m_pin_locs;
    wire_container_t m_wires;
};

bool LogisimParser::parse_xml() {

    auto project_node = m_xml_doc->child("project");
    if (!project_node) {
        return false;
    }

    auto circuit_node = project_node.child("circuit");
    if (!circuit_node) {
        return false;
    }

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

    return true;
}

bool LogisimParser::parse_component(pugi::xml_node &comp_node) {
    DEF_REQUIRED_ATTR(comp_type, comp_node, name);
    DEF_REQUIRED_ATTR(comp_loc, comp_node, loc);

    ComponentProperties comp_props = {0};
    comp_props.m_facing = LS_EAST;
    comp_props.m_inputs = 2;
    bool tristate_left = false;

    if (!parse_location(comp_loc, comp_props.m_location)) {
        return false;
    }

    for (auto prop : comp_node.children("a")) {
        DEF_REQUIRED_ATTR(prop_name, prop, name);
        DEF_REQUIRED_ATTR(prop_val, prop, val);

        if (prop_name == "label") {
            comp_props.m_label = prop_val;
        } else if (prop_name == "size") {
            comp_props.m_size = std::stoi(prop_val);
        } else if (prop_name == "facing") {
            parse_facing(prop_val, comp_props.m_facing);
        } else if (prop_name == "inputs") {
            comp_props.m_inputs = std::stoi(prop_val);
        } else if (prop_name == "control") {
            tristate_left = (prop_val == "left");
        }
    }

    Component *component = nullptr;
    if (comp_type == "Buffer") {
        component = m_circuit->create_component<Buffer>(1);
        handle_buffer(component, comp_props, false, false);
    } else if (comp_type == "Controlled Buffer") {
        component = m_circuit->create_component<TriStateBuffer>(1);
        handle_buffer(component, comp_props, true, tristate_left);
    } else if (comp_type == "Pin") {
        component = m_circuit->create_component<Connector>(1);
        add_pin_location(component->pin(0), comp_props.m_location);
    } else if (comp_type == "AND Gate") {
        component = m_circuit->create_component<AndGate>(comp_props.m_inputs);
        handle_gate(component, comp_props);
    } else if (comp_type == "OR Gate") {
        component = m_circuit->create_component<OrGate>(comp_props.m_inputs);
        handle_gate(component, comp_props);
    } else if (comp_type == "NOT Gate") {
        component = m_circuit->create_component<NotGate>();
        comp_props.m_inputs = 1;
        handle_not_gate(component, comp_props);
    } else if (comp_type == "NAND Gate") {
        component = m_circuit->create_component<AndGate>(comp_props.m_inputs);
        comp_props.m_negate_output = true;
        handle_gate(component, comp_props);
    } else if (comp_type == "NOR Gate") {
        component = m_circuit->create_component<OrGate>(comp_props.m_inputs);
        comp_props.m_negate_output = true;
        handle_gate(component, comp_props);
    } else if (comp_type == "XOR Gate") {
        component = m_circuit->create_component<XorGate>();
        comp_props.m_extra_size = 10;
        comp_props.m_inputs = 2;                // FIXME: report violations
        handle_gate(component, comp_props);
    } else if (comp_type == "XNOR Gate") {
        component = m_circuit->create_component<XnorGate>();
        comp_props.m_extra_size = 10;
        comp_props.m_inputs = 2;                // FIXME: report violations
        comp_props.m_negate_output = true;
        handle_gate(component, comp_props);
    } else {
        return false;
    }

    if (!comp_props.m_label.empty()) {
        m_circuit->register_component_name(comp_props.m_label, component);
    }

    return true;
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
        m_wires.push_back(new_node);
        return true;
    }

    if (node_1 && node_2) {
        // wire connects two existing nodes (merge node_2 into node_1)
        for (auto &w : *node_2) {
            node_1->push_back(w);
        }
        node_2->clear();
        m_wires.erase(std::find(std::begin(m_wires), std::end(m_wires), *node_2));
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

    for (const auto &node : m_wires) {
        pin_t pin_1 = PIN_UNDEFINED;

        for (const auto &point : node) {
            auto pin = m_pin_locs.find(point);
            if (pin != std::end(m_pin_locs)) {
                if (pin_1 == PIN_UNDEFINED) {
                    pin_1 = pin->second;
                } else {
                    m_circuit->connect_pins(pin_1, pin->second);
                }
            }
        }
    }

    return true;
}

void LogisimParser::handle_gate(Component *component, ComponentProperties &props) {
    // default size = MEDIUM (50)
    if (props.m_size == 0) {
        props.m_size = 50;
    }

    // inputs
    for (auto idx = 0u; idx < props.m_inputs; ++idx) {
        add_pin_location(component->pin(idx), 
                         input_pin_location( props.m_location, idx, props)
        );
    }

    // output
    add_pin_location(component->pin(component->num_pins() - 1), props.m_location);
}

void LogisimParser::handle_not_gate(Component *component, ComponentProperties &props) {
    // default size = WIDE (30)
    if (props.m_size == 0) {
        props.m_size = 30;
    }

    // input
    add_pin_location(component->pin(0), 
                     input_pin_location(props.m_location, 0, props)
    );

    // output
    add_pin_location(component->pin(1), props.m_location);
}

void LogisimParser::handle_buffer(Component *component, ComponentProperties &props, bool tri_state, bool left) {

    // buffers have a fixed size
    props.m_size = 20;
    props.m_inputs = 1;

    // input
    add_pin_location(component->pin(0), input_pin_location(props.m_location, 0, props));

    // enable pin
    if (tri_state) {
        auto en_loc = props.m_location;
        en_loc.m_x -= props.m_size / 2;
        en_loc.m_y += (left) ? -10 : 10;
        add_pin_location(component->pin(1), en_loc);
    }

    // output
    add_pin_location(component->pin(component->num_pins() - 1), props.m_location);
}

bool LogisimParser::parse_location(const std::string &loc_string, Position &pos) {
    if (loc_string.front() != '(' || loc_string.back() != ')') {
        return false;
    }

    auto comma = loc_string.find_first_of(",");
    if (comma == loc_string.npos) {
        return false;
    }

    pos.m_x = std::stoi(loc_string.substr(1, comma - 1));
    pos.m_y = std::stoi(loc_string.substr(comma + 1));

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
        return false;
    }

    return true;
}

void LogisimParser::add_pin_location(pin_t pin, const Position &loc) {
    m_pin_locs[loc.m_full] = pin;
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

    for (auto &node : m_wires) {
        auto res = std::find(std::begin(node), std::end(node), position.m_full);
        if (res != std::end(node)) {
            return &node;
        }
    }

    return nullptr;
}

//
// interface
//

bool load_logisim(Circuit *circuit, const char *filename) {
    pugi::xml_document xml_doc;

    auto result = xml_doc.load_file(filename);
    if (!result) {
        return false;
    }

    auto parser = LogisimParser(&xml_doc, circuit);
    return parser.parse_xml();
}

bool load_logisim(Circuit *circuit, const char *data, size_t len) {
    pugi::xml_document xml_doc;

    auto result = xml_doc.load_buffer(data, len);
    if (!result) {
        return false;
    }

    auto parser = LogisimParser(&xml_doc, circuit);
    return parser.parse_xml();
}