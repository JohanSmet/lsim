// serialize.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Serialize / deserialize circuits & components

#include "serialize.h"
#include "lsim_context.h"

#include <cassert>
#include <unordered_map>
#include <pugixml.hpp>

static const char *LSIM_XML_VERSION="1";

static const char *XML_EL_LSIM = "lsim";
static const char *XML_EL_CIRCUIT = "circuit";
static const char *XML_EL_COMPONENT = "component";
static const char *XML_EL_INPUTS = "inputs";
static const char *XML_EL_OUTPUTS = "outputs";
static const char *XML_EL_CONTROLS = "controls";
static const char *XML_EL_PIN = "pin";
static const char *XML_EL_PROPERTY = "property";
static const char *XML_EL_POSITION = "position";
static const char *XML_EL_ORIENTATION = "orientation";

static const char *XML_ATTR_VERSION = "version";
static const char *XML_ATTR_NAME = "name";
static const char *XML_ATTR_TYPE = "type";
static const char *XML_ATTR_PRIORITY = "priority";
static const char *XML_ATTR_INDEX = "index";
static const char *XML_ATTR_NODE = "node";
static const char *XML_ATTR_KEY = "key";
static const char *XML_ATTR_VALUE = "value";
static const char *XML_ATTR_X = "x";
static const char *XML_ATTR_Y = "y";
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
        m_component_nodes[component] = comp_node;

        // name
        auto name = component_type_to_name.find(component->type());
        if (name != component_type_to_name.end()) {
            comp_node.append_attribute(XML_ATTR_TYPE).set_value(name->second.c_str());
        } else {
            comp_node.append_attribute(XML_ATTR_TYPE).set_value(component->type());
        }

        // priority
        auto prio = priority_to_string.find(component->get_priority());
        if (prio != priority_to_string.end()) {
            comp_node.append_attribute(XML_ATTR_PRIORITY).set_value(prio->second.c_str());
        }

        // pins
        auto pin_func = [&comp_node, &component](const char *node_name, const Component::pin_container_t &pins) {
            if (pins.empty()) {
                return;
            }
            auto pins_node = comp_node.append_child(node_name);
            for (size_t idx = 0; idx < pins.size(); ++idx) {
                auto pin_node = pins_node.append_child(XML_EL_PIN);
                pin_node.append_attribute(XML_ATTR_INDEX).set_value(idx);
                pin_node.append_attribute(XML_ATTR_NODE).set_value(component->sim()->pin_node(pins[idx]));
            }
        };

        pin_func(XML_EL_INPUTS, component->input_pins());
        pin_func(XML_EL_OUTPUTS, component->output_pins());
        pin_func(XML_EL_CONTROLS, component->control_pins());

        // properties
        for (const auto &prop : component->properties()) {
            auto prop_node = comp_node.append_child(XML_EL_PROPERTY);
            prop_node.append_attribute(XML_ATTR_KEY).set_value(prop.second->key());
            prop_node.append_attribute(XML_ATTR_VALUE).set_value(prop.second->value_as_string());
        }

    }

    void serialize_nested_circuit(Circuit *circuit, pugi::xml_node *circuit_node) {
        auto comp_node = circuit_node->append_child(XML_EL_COMPONENT);
        m_component_nodes[circuit] = comp_node;

        comp_node.append_attribute(XML_ATTR_TYPE).set_value("SubCircuit");

        std::string name = circuit->name();
        auto hash = name.find_first_of('#');

        comp_node.append_attribute(XML_ATTR_NAME).set_value(name.substr(0, hash).c_str());
        comp_node.append_attribute(XML_ATTR_INSTANCE).set_value(name.substr(hash).c_str());

    }

    void serialize_visual_component(VisualComponent *vis_comp) {
        // add child nodes to existing node
        void *search_ptr = vis_comp->get_component();
        if (!search_ptr) {
            search_ptr = vis_comp->get_circuit();
        }

        auto result = m_component_nodes.find(search_ptr);
        if (result == m_component_nodes.end()) {
            return;
        }
        auto comp_node = result->second;

        auto pos_node = comp_node.append_child(XML_EL_POSITION);
        pos_node.append_attribute(XML_ATTR_X).set_value(vis_comp->get_position()[0]);
        pos_node.append_attribute(XML_ATTR_Y).set_value(vis_comp->get_position()[1]);

       auto orient_node = comp_node.append_child(XML_EL_ORIENTATION);
       orient_node.append_attribute(XML_ATTR_ANGLE).set_value(vis_comp->get_orientation());
    }

    void serialize_circuit(Circuit *circuit) {
        auto circuit_node = m_root.append_child(XML_EL_CIRCUIT);
        circuit_node.append_attribute(XML_ATTR_NAME).set_value(circuit->name());

        // components
        for (const auto &comp : circuit->components()) {
            serialize_component(comp.get(), &circuit_node);
        }

        // nested circuits
        for (size_t idx = 0; idx < circuit->num_nested_circuits(); ++idx) {
            serialize_nested_circuit(circuit->nested_circuit_by_index(idx), &circuit_node);
        }

        // visual components
        for (const auto &vis_comp : circuit->visual_components()) {
            serialize_visual_component(vis_comp.get());
        }

    }


private:
    pugi::xml_document  m_xml;
    pugi::xml_node      m_root;

    std::unordered_map<void *, pugi::xml_node> m_component_nodes;
};



bool serialize_library(LSimContext *context, CircuitLibrary *lib, const char *filename) {
    assert(context);
    assert(lib);
    assert(filename);

    Serializer  serializer;
    serializer.serialize_library(lib);
    serializer.dump_to_file(filename);

    return true;
}