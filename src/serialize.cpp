// serialize.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Serialize / deserialize circuits & components

#include "serialize.h"
#include "lsim_context.h"
#include "circuit_library.h"
#include "circuit_description.h"
#include "error.h"

#include <cassert>
#include <unordered_map>
#include <pugixml.hpp>

namespace {

using namespace lsim;

static const char *LSIM_XML_VERSION="2";

static const char *XML_EL_LSIM = "lsim";
static const char *XML_EL_CIRCUIT = "circuit";
static const char *XML_EL_COMPONENT = "component";
static const char *XML_EL_PIN = "pin";
static const char *XML_EL_PROPERTY = "property";
static const char *XML_EL_POSITION = "position";
static const char *XML_EL_ORIENTATION = "orientation";
static const char *XML_EL_MAIN = "main";
static const char *XML_EL_WIRE = "wire";
static const char *XML_EL_SEGMENT = "segment";

static const char *XML_ATTR_VERSION = "version";
static const char *XML_ATTR_NAME = "name";
static const char *XML_ATTR_ID = "id";
static const char *XML_ATTR_TYPE = "type";
static const char *XML_ATTR_PRIORITY = "priority";
static const char *XML_ATTR_INPUTS = "inputs";
static const char *XML_ATTR_OUTPUTS = "outputs";
static const char *XML_ATTR_CONTROLS = "controls";
static const char *XML_ATTR_NESTED = "nested";
static const char *XML_ATTR_INDEX = "index";
static const char *XML_ATTR_NODE = "node";
static const char *XML_ATTR_KEY = "key";
static const char *XML_ATTR_VALUE = "value";
static const char *XML_ATTR_X = "x";
static const char *XML_ATTR_Y = "y";
static const char *XML_ATTR_X1 = "x1";
static const char *XML_ATTR_Y1 = "y1";
static const char *XML_ATTR_X2 = "x2";
static const char *XML_ATTR_Y2 = "y2";
static const char *XML_ATTR_ANGLE = "angle";
static const char *XML_ATTR_INSTANCE = "instance";

static const std::unordered_map<ComponentType, std::string> component_type_to_name = {
    {COMPONENT_CONNECTOR_IN, "ConnectorIn"},
    {COMPONENT_CONNECTOR_OUT, "ConnectorOut"},
    {COMPONENT_CONSTANT, "Constant"},
    {COMPONENT_PULL_RESISTOR, "PullResistor"},
    {COMPONENT_BUFFER, "Buffer"},
    {COMPONENT_TRISTATE_BUFFER, "TriStateBuffer"},
    {COMPONENT_AND_GATE, "AndGate"},
    {COMPONENT_OR_GATE, "OrGate"},
    {COMPONENT_NOT_GATE, "NotGate"},
    {COMPONENT_NAND_GATE, "NandGate"},
    {COMPONENT_NOR_GATE, "NorGate"},
    {COMPONENT_XOR_GATE, "XorGate"},
    {COMPONENT_XNOR_GATE, "XnorGate"},
    {COMPONENT_SUB_CIRCUIT, "SubCircuit"}
};

static const std::unordered_map<Priority, std::string> priority_to_string = {
    {PRIORITY_NORMAL, "normal"},
    {PRIORITY_DEFERRED, "deferred"}
};

static std::unordered_map<std::string, ComponentType> build_component_type_lut() {
    std::unordered_map<std::string, ComponentType> result;
    for (auto entry : component_type_to_name) {
        result[entry.second] = entry.first;
    }
    return result;
}

static const auto name_to_component_type = build_component_type_lut();


class XmlStringWriter: public pugi::xml_writer
{
public:
    XmlStringWriter(std::string *output) : m_output(output) {
    }

    void write(const void* data, size_t size) override
    {
        m_output->append(static_cast<const char*>(data), size);
    }
private:
    std::string *m_output;
};

class Serializer {
public:
    Serializer() {
        m_root = m_xml.append_child(XML_EL_LSIM);
        m_root.append_attribute(XML_ATTR_VERSION).set_value(LSIM_XML_VERSION);
    }

    void serialize_library(CircuitLibrary *library) {
        for (size_t idx = 0; idx < library->num_circuits(); ++idx) {
            serialize_circuit(library->circuit_by_idx(idx));
        }

        if (library->main_circuit()) {
            auto main_node = m_root.append_child(XML_EL_MAIN);
            main_node.append_attribute(XML_ATTR_NAME).set_value(library->main_circuit_name());
        }
    }

    void dump_to_string(std::string *output) {
        XmlStringWriter writer(output);
        m_xml.save(writer);
    }

    void dump_to_file(const char *filename) {
        m_xml.save_file(filename);
    }

private:
    void serialize_component(Component *component, pugi::xml_node *circuit_node) {
        auto comp_node = circuit_node->append_child(XML_EL_COMPONENT);

        // id
        comp_node.append_attribute(XML_ATTR_ID).set_value(component->id());

        // type
        auto type_name = component_type_to_name.find(component->type());
        if (type_name != component_type_to_name.end()) {
            comp_node.append_attribute(XML_ATTR_TYPE).set_value(type_name->second.c_str());
        } else {
            comp_node.append_attribute(XML_ATTR_TYPE).set_value(component->type());
        }

        // priority
        auto prio = priority_to_string.find(component->priority());
        if (prio != priority_to_string.end()) {
            comp_node.append_attribute(XML_ATTR_PRIORITY).set_value(prio->second.c_str());
        }

        // pins
        comp_node.append_attribute(XML_ATTR_INPUTS).set_value(component->num_inputs());
        comp_node.append_attribute(XML_ATTR_OUTPUTS).set_value(component->num_outputs());
        comp_node.append_attribute(XML_ATTR_CONTROLS).set_value(component->num_controls());

        // nested circuit
        if (component->nested_circuit()) {
            comp_node.append_attribute(XML_ATTR_NESTED).set_value(component->nested_circuit()->name().c_str());
        }

        // properties
        for (const auto &prop : component->properties()) {
            auto prop_node = comp_node.append_child(XML_EL_PROPERTY);
            prop_node.append_attribute(XML_ATTR_KEY).set_value(prop.second->key());
            prop_node.append_attribute(XML_ATTR_VALUE).set_value(prop.second->value_as_string().c_str());
        }

        // position
        auto pos_node = comp_node.append_child(XML_EL_POSITION);
        pos_node.append_attribute(XML_ATTR_X).set_value(component->position().x);
        pos_node.append_attribute(XML_ATTR_Y).set_value(component->position().y);

        // orientation
        auto orient_node = comp_node.append_child(XML_EL_ORIENTATION);
        orient_node.append_attribute(XML_ATTR_ANGLE).set_value(component->angle());
    }

    void serialize_wire(Wire *wire, pugi::xml_node *circuit_node) {
        auto wire_node = circuit_node->append_child(XML_EL_WIRE);

        wire_node.append_attribute(XML_ATTR_ID).set_value(wire->id());

        for (size_t idx = 0; idx < wire->num_segments(); ++idx) {
            auto segment_node = wire_node.append_child(XML_EL_SEGMENT);
            const auto &p0 = wire->segment_point(idx, 0);
            segment_node.append_attribute(XML_ATTR_X1).set_value(p0.x);
            segment_node.append_attribute(XML_ATTR_Y1).set_value(p0.y);
            const auto &p1 = wire->segment_point(idx, 1);
            segment_node.append_attribute(XML_ATTR_X2).set_value(p1.x);
            segment_node.append_attribute(XML_ATTR_Y2).set_value(p1.y);
        }

        for (size_t idx = 0; idx < wire->num_pins(); ++idx) {
            auto pin_node = wire_node.append_child(XML_EL_PIN);
            auto comp_id = component_id_from_pin_id(wire->pin(idx));
            auto pin_idx = pin_index_from_pin_id(wire->pin(idx));
            pin_node.append_attribute(XML_ATTR_VALUE).set_value(
                (std::to_string(comp_id) + "#" + std::to_string(pin_idx)).c_str());
        }
    }

    void serialize_circuit(CircuitDescription *circuit) {
        auto circuit_node = m_root.append_child(XML_EL_CIRCUIT);
        circuit_node.append_attribute(XML_ATTR_NAME).set_value(circuit->name().c_str());

        // components
        auto comp_ids = circuit->component_ids();
        for (const auto &comp_id : comp_ids) {
            serialize_component(circuit->component_by_id(comp_id), &circuit_node);
        }

        // wires
        auto wire_ids = circuit->wire_ids();
        for (const auto &wire_id : wire_ids) {
            serialize_wire(circuit->wire_by_id(wire_id), &circuit_node);
        }
    }

private:
    pugi::xml_document  m_xml;
    pugi::xml_node      m_root;
};

#define REQUIRED_ATTR(var_name, node, attr_name)    \
    auto var_name = (node).attribute((attr_name));  \
    if (!var_name) {                                \
        return false;                               \
    }

#define REQUIRED_PROP(var_name, node, prop_key)      \
    auto var_name##_prop = (node).find_child_by_attribute(XML_EL_PROPERTY, XML_ATTR_KEY, (prop_key));   \
    if (!var_name##_prop) {                         \
        return false;                               \
    }                                               \
    REQUIRED_ATTR(var_name, var_name##_prop, XML_ATTR_VALUE);

class Deserializer {
public:
    Deserializer(LSimContext *context) : m_context(context) {
    }

    bool load_from_file(const char *filename) {
        auto result = m_xml.load_file(filename);
        if (!result) {
            return false;
        }

        return true;
    }

    bool parse_component(pugi::xml_node &comp_node, CircuitDescription *circuit) {
        REQUIRED_ATTR(id_attr, comp_node, XML_ATTR_ID);

        // determine type of component
        REQUIRED_ATTR(type_attr, comp_node, XML_ATTR_TYPE);

        auto type_result = name_to_component_type.find(type_attr.as_string());
        if (type_result == name_to_component_type.end()) {
            return false;
        }

        auto type = type_result->second;

        // determine number of pins
        REQUIRED_ATTR(inputs_attr, comp_node, XML_ATTR_INPUTS);
        REQUIRED_ATTR(outputs_attr, comp_node, XML_ATTR_OUTPUTS);
        REQUIRED_ATTR(controls_attr, comp_node, XML_ATTR_CONTROLS);
        size_t num_inputs = inputs_attr.as_int();
        size_t num_outputs = outputs_attr.as_int();
        size_t num_controls = controls_attr.as_int();

        Component *component = nullptr;

        switch (type) {
            case COMPONENT_CONNECTOR_IN : {
                assert(num_inputs == 0);
                assert(num_outputs > 0);
                assert(num_controls == 0);
                REQUIRED_PROP(prop_name, comp_node, "name");
                REQUIRED_PROP(prop_tristate, comp_node, "tri_state");
                component = circuit->add_connector_in(prop_name.as_string(), num_outputs, prop_tristate.as_bool());
                break;
            }
            case COMPONENT_CONNECTOR_OUT : {
                assert(num_inputs > 0);
                assert(num_outputs == 0);
                assert(num_controls == 0);
                REQUIRED_PROP(prop_name, comp_node, "name");
                REQUIRED_PROP(prop_tristate, comp_node, "tri_state");
                component = circuit->add_connector_out(prop_name.as_string(), num_inputs, prop_tristate.as_bool());
                break;
            }
            case COMPONENT_CONSTANT : {
                assert(num_inputs == 0);
                assert(num_outputs == 1);
                assert(num_controls == 0);
                REQUIRED_PROP(prop_value, comp_node, "value");
                component = circuit->add_constant(static_cast<Value>(prop_value.as_int()));
                break;
            }
            case COMPONENT_PULL_RESISTOR : {
                assert(num_inputs == 0);
                assert(num_outputs == 1);
                assert(num_controls == 0);
                REQUIRED_PROP(prop_value, comp_node, "pull_to");
                component = circuit->add_pull_resistor(static_cast<Value>(prop_value.as_int()));
                break;
            }
            case COMPONENT_BUFFER : 
                assert(num_inputs == num_outputs);
                assert(num_controls == 0);
                component = circuit->add_buffer(num_inputs);
                break;
            case COMPONENT_TRISTATE_BUFFER :
                assert(num_inputs == num_outputs);
                assert(num_controls == 1);
                component = circuit->add_tristate_buffer(num_inputs);
                break;
            case COMPONENT_AND_GATE : 
                assert(num_inputs > 0);
                assert(num_outputs == 1);
                assert(num_controls == 0);
                component = circuit->add_and_gate(num_inputs);
                break;
            case COMPONENT_OR_GATE :
                assert(num_inputs > 0);
                assert(num_outputs == 1);
                assert(num_controls == 0);
                component = circuit->add_or_gate(num_inputs);
                break;
            case COMPONENT_NOT_GATE :
                assert(num_inputs == 1);
                assert(num_outputs == 1);
                assert(num_controls == 0);
                component = circuit->add_not_gate();
                break;
            case COMPONENT_NAND_GATE :
                assert(num_inputs > 0);
                assert(num_outputs == 1);
                assert(num_controls == 0);
                component = circuit->add_nand_gate(num_inputs);
                break;
            case COMPONENT_NOR_GATE :
                assert(num_inputs > 0);
                assert(num_outputs == 1);
                assert(num_controls == 0);
                component = circuit->add_nor_gate(num_inputs);
                break;
            case COMPONENT_XOR_GATE :
                assert(num_inputs == 2);
                assert(num_outputs == 1);
                assert(num_controls == 0);
                component = circuit->add_xor_gate();
                break;
            case COMPONENT_XNOR_GATE :
                assert(num_inputs == 2);
                assert(num_outputs == 1);
                assert(num_controls == 0);
                component = circuit->add_xnor_gate();
                break;
            case COMPONENT_SUB_CIRCUIT : {
                REQUIRED_ATTR(attr_name, comp_node, XML_ATTR_NESTED);
                component = circuit->add_sub_circuit(attr_name.as_string());
                break;
            }
            default :
                return false;
        }

        // visual properties
        auto pos_node = comp_node.child(XML_EL_POSITION);
        if (!!pos_node) {
            REQUIRED_ATTR(x_attr, pos_node, XML_ATTR_X);
            REQUIRED_ATTR(y_attr, pos_node, XML_ATTR_Y);
            component->set_position({x_attr.as_float(), y_attr.as_float()});
        }

        auto orient_node = comp_node.child(XML_EL_ORIENTATION);
        if (!!orient_node) {
            REQUIRED_ATTR(angle_attr, orient_node, XML_ATTR_ANGLE);
            component->set_angle(angle_attr.as_int());
        }

        if (component) {
            m_component_id_map[id_attr.as_int()] = component->id();
        }

        return true;
    }

    bool parse_wire(pugi::xml_node wire_node, CircuitDescription *circuit) {
        Wire *wire = circuit->create_wire();

        for (auto segment_node : wire_node.children(XML_EL_SEGMENT)) {
            REQUIRED_ATTR(x1_attr, segment_node, XML_ATTR_X1);
            REQUIRED_ATTR(y1_attr, segment_node, XML_ATTR_Y1);
            REQUIRED_ATTR(x2_attr, segment_node, XML_ATTR_X2);
            REQUIRED_ATTR(y2_attr, segment_node, XML_ATTR_Y2);

            wire->add_segment(Point(x1_attr.as_float(), y1_attr.as_float()),
                              Point(x2_attr.as_float(), y2_attr.as_float()));
        }

        for (auto pin_node : wire_node.children(XML_EL_PIN)) {
            REQUIRED_ATTR(value_attr, pin_node, XML_ATTR_VALUE);
            std::string pin_string = value_attr.as_string();

            auto hash = pin_string.find_first_of("#");
            if (hash == pin_string.npos) {
                ERROR_MSG("Unparseable location \"%s\"; should contain a comma", pin_string.c_str());
                return false;
            }

            // map component id to new value
            auto old_id = std::strtol(pin_string.c_str(), nullptr, 0);
            auto found = m_component_id_map.find(old_id);
            if (found == m_component_id_map.end()) {
                ERROR_MSG("Unknown component-id \"%d\" in wire", old_id);
            }
            auto comp_id = found->second;

            auto pin_idx = std::strtol(pin_string.c_str() + hash + 1, nullptr, 0);
            wire->add_pin(pin_id_assemble(comp_id, pin_idx));
        }
        return true;
    }

    bool parse_circuit(pugi::xml_node &circuit_node) {
        const char *name = circuit_node.attribute(XML_ATTR_NAME).as_string();
        if (!name) {
            return false;
        }

        auto *circuit = m_lib->create_circuit(name, m_context);
        if (!circuit) {
            return false;
        }

        for (auto comp_node : circuit_node.children(XML_EL_COMPONENT)) {
            parse_component(comp_node, circuit);
        }

        for (auto wire_node : circuit_node.children(XML_EL_WIRE)) {
            parse_wire(wire_node, circuit);
        }

        return true;
    }

    bool parse_library(CircuitLibrary *lib) {
        m_lib = lib;

        auto lsim_node = m_xml.child(XML_EL_LSIM);
        if (!lsim_node) {
            return false;
        }

        for (auto circuit_node : lsim_node.children(XML_EL_CIRCUIT)) {
            parse_circuit(circuit_node);
        }

        auto main_node = lsim_node.child(XML_EL_MAIN);
        if (!!main_node) {
            REQUIRED_ATTR(attr_name, main_node, XML_ATTR_NAME);
            lib->change_main_circuit(attr_name.as_string());
        }

        return true;
    }

private: 
    pugi::xml_document  m_xml;
    LSimContext *       m_context;
    CircuitLibrary *    m_lib;

    std::unordered_map<uint32_t, uint32_t>   m_component_id_map;
};

} // unnamed namespace

namespace lsim {

bool serialize_library(LSimContext *context, CircuitLibrary *lib, const char *filename) {
    assert(context);
    assert(lib);
    assert(filename);

    Serializer  serializer;
    serializer.serialize_library(lib);
    serializer.dump_to_file(filename);

    return true;
}

bool deserialize_library(LSimContext *context, CircuitLibrary *lib, const char *filename) {
    assert(context);
    assert(lib);
    assert(filename);

    Deserializer deserializer(context);
    
    if (!deserializer.load_from_file(filename)) {
        return false;
    }

    deserializer.parse_library(lib);

    return true;
}

} // namespace lsim
