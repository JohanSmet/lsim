// serialize.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Serialize / deserialize circuits & components

#include "serialize.h"
#include "lsim_context.h"
#include "gate.h"
#include "extra.h"

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
    void serialize_pins(Simulator *sim, pugi::xml_node *parent, const char *node_name, const Component::pin_container_t &pins) {
        if (pins.empty()) {
            return;
        }
        auto pins_node = parent->append_child(node_name);
        for (size_t idx = 0; idx < pins.size(); ++idx) {
            auto pin_node = pins_node.append_child(XML_EL_PIN);
            pin_node.append_attribute(XML_ATTR_INDEX).set_value(idx);
            pin_node.append_attribute(XML_ATTR_NODE).set_value(sim->pin_node(pins[idx]));
        }
    }

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
        serialize_pins(component->sim(), &comp_node, XML_EL_INPUTS, component->input_pins());
        serialize_pins(component->sim(), &comp_node, XML_EL_OUTPUTS, component->output_pins());
        serialize_pins(component->sim(), &comp_node, XML_EL_CONTROLS, component->control_pins());

        // properties
        for (const auto &prop : component->properties()) {
            auto prop_node = comp_node.append_child(XML_EL_PROPERTY);
            prop_node.append_attribute(XML_ATTR_KEY).set_value(prop.second->key());
            prop_node.append_attribute(XML_ATTR_VALUE).set_value(prop.second->value_as_string().c_str());
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

        // pins
        serialize_pins(circuit->sim(), &comp_node, XML_EL_INPUTS, circuit->input_ports_pins());
        serialize_pins(circuit->sim(), &comp_node, XML_EL_OUTPUTS, circuit->output_ports_pins());
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
    Deserializer(Simulator *sim) : m_sim(sim) {
    }

    bool load_from_file(const char *filename) {
        auto result = m_xml.load_file(filename);
        if (!result) {
            return false;
        }

        return true;
    }

    bool parse_component(pugi::xml_node &comp_node, Circuit *circuit) {
        // determine type of component
        REQUIRED_ATTR(type_attr, comp_node, XML_ATTR_TYPE);

        auto type_result = name_to_component_type.find(type_attr.as_string());
        if (type_result == name_to_component_type.end()) {
            return false;
        }

        auto type = type_result->second;

        // determine number of pins
        auto input_els = comp_node.child(XML_EL_INPUTS).children();
        auto output_els = comp_node.child(XML_EL_OUTPUTS).children();
        auto control_els = comp_node.child(XML_EL_CONTROLS).children();
        size_t num_inputs = std::distance(input_els.begin(), input_els.end());
        size_t num_outputs = std::distance(output_els.begin(), output_els.end());
        size_t num_controls = std::distance(control_els.begin(), control_els.end());

        Component *component = nullptr;
        Circuit *sub_circuit = nullptr;

        switch (type) {
            case COMPONENT_CONNECTOR_IN : {
                assert(num_inputs == 0);
                assert(num_outputs > 0);
                assert(num_controls == 0);
                REQUIRED_PROP(prop_name, comp_node, "name");
                REQUIRED_PROP(prop_tristate, comp_node, "tri_state");
                component = ConnectorInput(circuit, prop_name.as_string(), num_outputs, prop_tristate.as_bool());
                break;
            }
            case COMPONENT_CONNECTOR_OUT : {
                assert(num_inputs > 0);
                assert(num_outputs == 0);
                assert(num_controls == 0);
                REQUIRED_PROP(prop_name, comp_node, "name");
                REQUIRED_PROP(prop_tristate, comp_node, "tri_state");
                component = ConnectorOutput(circuit, prop_name.as_string(), num_inputs, prop_tristate.as_bool());
                break;
            }
            case COMPONENT_CONSTANT : {
                assert(num_inputs == 0);
                assert(num_outputs == 1);
                assert(num_controls == 0);
                REQUIRED_PROP(prop_value, comp_node, "value");
                component = Constant(circuit, static_cast<Value>(prop_value.as_int()));
                break;
            }
            case COMPONENT_PULL_RESISTOR : {
                assert(num_inputs == 0);
                assert(num_outputs == 1);
                assert(num_controls == 0);
                REQUIRED_PROP(prop_value, comp_node, "pull_to");
                component = PullResistor(circuit, static_cast<Value>(prop_value.as_int()));
                break;
            }
            case COMPONENT_BUFFER : 
                assert(num_inputs == num_outputs);
                assert(num_controls == 0);
                component = Buffer(circuit, num_inputs);
                break;
            case COMPONENT_TRISTATE_BUFFER :
                assert(num_inputs == num_outputs);
                assert(num_controls == 1);
                component = TriStateBuffer(circuit, num_inputs);
                break;
            case COMPONENT_AND_GATE : 
                assert(num_inputs > 0);
                assert(num_outputs == 1);
                assert(num_controls == 0);
                component = AndGate(circuit, num_inputs);
                break;
            case COMPONENT_OR_GATE :
                assert(num_inputs > 0);
                assert(num_outputs == 1);
                assert(num_controls == 0);
                component = OrGate(circuit, num_inputs);
                break;
            case COMPONENT_NOT_GATE :
                assert(num_inputs == 1);
                assert(num_outputs == 1);
                assert(num_controls == 0);
                component = NotGate(circuit);
                break;
            case COMPONENT_NAND_GATE :
                assert(num_inputs > 0);
                assert(num_outputs == 1);
                assert(num_controls == 0);
                component = NandGate(circuit, num_inputs);
                break;
            case COMPONENT_NOR_GATE :
                assert(num_inputs > 0);
                assert(num_outputs == 1);
                assert(num_controls == 0);
                component = NorGate(circuit, num_inputs);
                break;
            case COMPONENT_XOR_GATE :
                assert(num_inputs == 2);
                assert(num_outputs == 1);
                assert(num_controls == 0);
                component = XorGate(circuit);
                break;
            case COMPONENT_XNOR_GATE :
                assert(num_inputs == 2);
                assert(num_outputs == 1);
                assert(num_controls == 0);
                component = XnorGate(circuit);
                break;
            case COMPONENT_SUB_CIRCUIT : {
                REQUIRED_ATTR(attr_name, comp_node, "name");
                auto template_circuit = m_lib->circuit_by_name(attr_name.as_string());
                sub_circuit = circuit->integrate_circuit_clone(template_circuit);
                break;
            }
            default :
                return false;
        }

        // visual component
        auto pos_node = comp_node.child(XML_EL_POSITION);
        auto orient_node = comp_node.child(XML_EL_ORIENTATION);

        if (!!pos_node && !!orient_node) {
            VisualComponent *vis_comp = nullptr;
            if (component) {
                vis_comp = circuit->create_visual_component(component);
            } else if (sub_circuit) {
                vis_comp = circuit->create_visual_component(sub_circuit);
            } 

            if (vis_comp) {
                REQUIRED_ATTR(x_attr, pos_node, XML_ATTR_X);
                REQUIRED_ATTR(y_attr, pos_node, XML_ATTR_Y);
                vis_comp->set_position({x_attr.as_float(), y_attr.as_float()});

                REQUIRED_ATTR(angle_attr, orient_node, XML_ATTR_ANGLE);
                vis_comp->set_orientation(static_cast<VisualComponent::Orientation>(angle_attr.as_int()));
            }
        }

        // connections
        auto connect_pin = [&component, this](pugi::xml_node el, std::function<pin_t(size_t idx)> get_pin) {
            REQUIRED_ATTR(index_attr, el, XML_ATTR_INDEX);
            REQUIRED_ATTR(node_attr, el, XML_ATTR_NODE);

            pin_t pin = get_pin(index_attr.as_int());

            auto result = m_node_pins.find(node_attr.as_int());
            if (result == m_node_pins.end()) {
                m_node_pins[node_attr.as_int()] = pin;
            } else {
                m_sim->connect_pins(result->second, pin);
            }

            return true;
        };

        if (component) {
            for (auto el : input_els) {
                connect_pin(el, [=](size_t idx) {return component->input_pin(idx);});
            }

            for (auto el : output_els) {
                connect_pin(el, [=](size_t idx) {return component->output_pin(idx);});
            }

            for (auto el : control_els) {
                connect_pin(el, [=](size_t idx) {return component->control_pin(idx);});
            }
        }

        if (sub_circuit) {
            const auto &input_pins = sub_circuit->input_ports_pins();
            for (auto el : input_els) {
                connect_pin(el, [=](size_t idx) {return input_pins[idx];});
            }

            const auto &output_pins = sub_circuit->output_ports_pins();
            for (auto el : output_els) {
                connect_pin(el, [=](size_t idx) {return output_pins[idx];});
            }
        }

        return true;
    }

    bool parse_circuit(pugi::xml_node &circuit_node) {
        const char *name = circuit_node.attribute(XML_ATTR_NAME).as_string();
        if (!name) {
            return false;
        }

        Circuit *circuit = m_lib->create_circuit(name);
        if (!circuit) {
            return false;
        }

        for (auto comp_node : circuit_node.children(XML_EL_COMPONENT)) {
            parse_component(comp_node, circuit);
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

        return true;
    }

private: 
    pugi::xml_document  m_xml;
    CircuitLibrary *    m_lib;
    Simulator *         m_sim;

    std::unordered_map<node_t, pin_t>   m_node_pins;
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


bool deserialize_library(LSimContext *context, CircuitLibrary *lib, const char *filename) {
    assert(context);
    assert(lib);
    assert(filename);

    Deserializer deserializer(context->sim());
    
    if (!deserializer.load_from_file(filename)) {
        return false;
    }

    deserializer.parse_library(lib);

    return true;
}