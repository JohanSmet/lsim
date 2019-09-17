// model_circuit.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// describe the composition of a logic circuit

#include "model_circuit.h"
#include "circuit_instance.h"
#include "lsim_context.h"
#include "simulator.h"

#include <cassert>
#include "std_helper.h"

namespace { 

std::string unqualified_circuit_name(const std::string &circuit_name) {
    auto sep = circuit_name.find_first_of('.');
    if (sep == std::string::npos) {
        return circuit_name;
    } else {
        return circuit_name.substr(sep + 1);
    }
}

std::string unique_subcircuit_name(const std::string &circuit_name, uint32_t comp_id) {
    auto result = unqualified_circuit_name(circuit_name);
    result += "#";
    result += std::to_string(comp_id);
    return result;
}


} // unnamed namespace

namespace lsim {

ModelCircuit::ModelCircuit(const char *name, LSimContext *context, CircuitLibrary *ref_lib) :
        m_context(context),
        m_lib(ref_lib),
        m_name(name),
        m_component_id(0),
        m_wire_id(0) {
}

void ModelCircuit::change_name(const char *name) {
    assert(name);
    m_name = name;
}

std::string ModelCircuit::qualified_name() const {
    std::string qname = m_lib->name();
    if (qname.empty() || qname == "user") {
        return m_name;
    }
    qname += "." + m_name;
    return qname;
}

ModelComponent *ModelCircuit::create_component(ComponentType type, uint32_t input_pins, uint32_t output_pins, uint32_t control_pins) {
    assert(type != COMPONENT_SUB_CIRCUIT);

    auto component = std::make_unique<ModelComponent>(this, m_component_id++, type, input_pins, output_pins, control_pins);
    auto result = component.get();
    m_components[result->id()] = std::move(component);

    if (type == COMPONENT_CONNECTOR_IN || type == COMPONENT_CONNECTOR_OUT) {
        result->add_property(make_property("name", (std::string("c#") + std::to_string(result->id())).c_str()));
        result->add_property(make_property("tri_state", false));
        result->add_property(make_property("descending", false));
    } else if (type == COMPONENT_CONSTANT) {
        result->add_property(make_property("value", VALUE_FALSE));
    } else if (type == COMPONENT_PULL_RESISTOR) {
        result->add_property(make_property("pull_to", VALUE_FALSE));
    } else if (type == COMPONENT_TEXT) {
        result->add_property(make_property("text", "text"));
    } else if (type == COMPONENT_VIA) {
        result->add_property(make_property("name", "via"));
        result->add_property(make_property("right", false));
    } else if (type == COMPONENT_OSCILLATOR) {
        int64_t default_cycle = 5;
        result->add_property(make_property("low_duration", default_cycle));
        result->add_property(make_property("high_duration", default_cycle));
        result->add_property(make_property("initial_output", VALUE_FALSE));
    } else if (type == COMPONENT_7_SEGMENT_LED) {
    } else {
        result->add_property(make_property("initial_output", VALUE_UNDEFINED));
    }

    return result;
}

ModelComponent *ModelCircuit::create_component(const char *circuit_name, uint32_t input_pins, uint32_t output_pins) {
    auto component = std::make_unique<ModelComponent>(this, m_component_id++, circuit_name, input_pins, output_pins);
    auto result = component.get();
    m_components[result->id()] = std::move(component);
    result->add_property(make_property("flip", false));
    result->add_property(make_property("caption", unique_subcircuit_name(circuit_name, result->id()).c_str()));
    return result;
}

ModelComponent *ModelCircuit::component_by_id(uint32_t id) {
    auto found = m_components.find(id);
    if (found != m_components.end()) {
        return found->second.get();
    } else {
        return nullptr;
    }
}

std::vector<uint32_t> ModelCircuit::component_ids() const {
    std::vector<uint32_t> result;
    result.reserve(m_components.size());

    for (const auto &comp : m_components) {
        result.push_back(comp.second->id());
    }

    std::sort(result.begin(), result.end());
    return std::move(result);
}

std::vector<uint32_t> ModelCircuit::component_ids_of_type(ComponentType type) const {
    std::vector<uint32_t> result;

    for (const auto &comp : m_components) {
        if (comp.second->type() == type) {
            result.push_back(comp.second->id());
        }
    }

    std::sort(result.begin(), result.end());
    return std::move(result);
}

void ModelCircuit::disconnect_component(uint32_t id) {
    for (auto &pair : m_wires) {
        pair.second->remove_component_pins(id);
    }
}

void ModelCircuit::remove_component(uint32_t id) {
    auto found = m_components.find(id);
    if (found == m_components.end()) {
        return;
    }

    bool was_connector = found->second->type() == COMPONENT_CONNECTOR_IN ||
                         found->second->type() == COMPONENT_CONNECTOR_OUT;

    disconnect_component(id);
    m_components.erase(found);

    if (was_connector) {
        rebuild_port_list();
    }
}

void ModelCircuit::sync_sub_circuit_components() {
    for (auto id : component_ids_of_type(COMPONENT_SUB_CIRCUIT)) {
        auto comp = component_by_id(id);
        comp->sync_nested_circuit(m_context);
    }
}

ModelComponent *ModelCircuit::paste_component(ModelComponent *comp) {
    auto paste = comp->copy();
    auto result = paste.get(); 

    result->integrate_into_circuit(this, m_component_id++);
    m_components[paste->id()] = std::move(paste);

    return result;
}

ModelWire *ModelCircuit::create_wire() {
    auto wire = std::make_unique<ModelWire>(m_wire_id++);
    auto result = wire.get();
    m_wires[result->id()] = std::move(wire);
    return result;
}

ModelWire *ModelCircuit::connect(pin_id_t pin_a, pin_id_t pin_b) {
    auto wire = create_wire();
    wire->add_pin(pin_a);
    wire->add_pin(pin_b);
    return wire;
}

void ModelCircuit::disconnect_pin(pin_id_t pin) {
    std::vector<uint32_t> empty_wires;

    for (auto &pair : m_wires) {
        pair.second->remove_pin(pin);
        if (pair.second->num_pins() == 0) {
            empty_wires.push_back(pair.second->id());
        }
    }

    for (const auto id : empty_wires) {
        remove_wire(id);
    }
}

std::vector<uint32_t> ModelCircuit::wire_ids() const {
    std::vector<uint32_t> result;
    result.reserve(m_wires.size());

    for (const auto &wire : m_wires) {
        result.push_back(wire.second->id());
    }

    std::sort(result.begin(), result.end());
    return std::move(result);
}

ModelWire *ModelCircuit::wire_by_id(uint32_t id) const {
    auto found = m_wires.find(id);
    if (found != m_wires.end()) {
        return found->second.get();
    } else {
        return nullptr;
    }
}

void ModelCircuit::remove_wire(uint32_t id) {
	m_wires.erase(id);
}

void ModelCircuit::rebuild_port_list() {
    // clear current port list
    m_ports_lut.clear();
    m_input_ports.clear();
    m_output_ports.clear();

    // helper function to process one type of connector
    auto process_connectors = [this](ComponentType type, port_container_t &ports) {
        auto conn_ids = component_ids_of_type(type);
        for (auto id : conn_ids) {
            auto connector = component_by_id(id);
            auto num_ports = connector->num_inputs() + connector->num_outputs();
            std::string name = connector->property("name")->value_as_string();

            if (num_ports == 1) {
                m_ports_lut[name] = connector->pin_id(0);
                ports.push_back(name);
            } else {
                for (auto idx = 0u; idx < num_ports; ++idx) {
                    std::string pin_name = name + "[" + std::to_string(idx) + "]";
                    m_ports_lut[pin_name] = connector->pin_id(idx);
                    ports.push_back(pin_name);
                }
            }
        }
    };

    // build the port lists
    process_connectors(COMPONENT_CONNECTOR_IN, m_input_ports);
    process_connectors(COMPONENT_CONNECTOR_OUT, m_output_ports);
}

void ModelCircuit::change_port_pin_count(uint32_t comp_id, uint32_t new_count) {
    auto comp = component_by_id(comp_id);
    assert(comp);

    // disconnect any ports that are going to be removed
    auto cur_count = comp->num_inputs() + comp->num_outputs();
    while (cur_count > new_count) {
        disconnect_pin(comp->pin_id(--cur_count));
    }

    if (comp->type() == COMPONENT_CONNECTOR_IN) {
        comp->change_output_pins(new_count);
    } else {
        comp->change_input_pins(new_count);
    }

    rebuild_port_list();
}

pin_id_t ModelCircuit::port_by_name(const char *name) const {
    auto found = m_ports_lut.find(name);
    if (found == m_ports_lut.end()) {
        return PIN_ID_INVALID;
    }

    return found->second;
}

pin_id_t ModelCircuit::port_by_index(bool input, size_t index) const {
    auto &container = (input) ? m_input_ports : m_output_ports;
    assert(index < container.size());
    return port_by_name(container[index].c_str());
}

const std::string &ModelCircuit::port_name(bool input, size_t index) const {
    auto &container = (input) ? m_input_ports : m_output_ports;
    assert(index < container.size());
    return container[index];
}

ModelComponent *ModelCircuit::add_connector_in(const char *name, uint32_t data_bits, bool tri_state) {
    auto result = create_component(COMPONENT_CONNECTOR_IN, 0, data_bits, 0);
    result->property("name")->value(name);
    result->property("tri_state")->value(tri_state);
    rebuild_port_list();
    return result;
}

ModelComponent *ModelCircuit::add_connector_out(const char *name, uint32_t data_bits, bool tri_state) {
    auto result = create_component(COMPONENT_CONNECTOR_OUT, data_bits, 0, 0);
    result->property("name")->value(name);
    result->property("tri_state")->value(tri_state);
    rebuild_port_list();
    return result;
}

ModelComponent *ModelCircuit::add_constant(Value value) {
    auto result = create_component(COMPONENT_CONSTANT, 0, 1, 0);
    result->property("value")->value(value);
    return result;
}

ModelComponent *ModelCircuit::add_pull_resistor(Value pull_to) {
    auto result = create_component(COMPONENT_PULL_RESISTOR, 0, 1, 0);
    result->property("pull_to")->value(pull_to);
    return result;
}

ModelComponent *ModelCircuit::add_buffer(uint32_t data_bits) {
    assert(data_bits >= 1);
    return create_component(COMPONENT_BUFFER, data_bits, data_bits, 0);
}

ModelComponent *ModelCircuit::add_tristate_buffer(uint32_t data_bits) {
    assert(data_bits >= 1);
   return create_component(COMPONENT_TRISTATE_BUFFER, data_bits, data_bits, 1);
}

ModelComponent *ModelCircuit::add_and_gate(uint32_t num_inputs) {
    assert(num_inputs >= 2);
    return create_component(COMPONENT_AND_GATE, num_inputs, 1, 0);
}

ModelComponent *ModelCircuit::add_or_gate(uint32_t num_inputs) {
    assert(num_inputs >= 2);
    return create_component(COMPONENT_OR_GATE, num_inputs, 1, 0);
}

ModelComponent *ModelCircuit::add_not_gate() {
    return create_component(COMPONENT_NOT_GATE, 1, 1, 0);
}

ModelComponent *ModelCircuit::add_nand_gate(uint32_t num_inputs) {
    assert(num_inputs >= 2);
    return create_component(COMPONENT_NAND_GATE, num_inputs, 1, 0);
}

ModelComponent *ModelCircuit::add_nor_gate(uint32_t num_inputs) {
    assert(num_inputs >= 2);
    return create_component(COMPONENT_NOR_GATE, num_inputs, 1, 0);
}

ModelComponent *ModelCircuit::add_xor_gate() {
    return create_component(COMPONENT_XOR_GATE, 2, 1, 0);
}

ModelComponent *ModelCircuit::add_xnor_gate() {
    return create_component(COMPONENT_XNOR_GATE, 2, 1, 0);
}

ModelComponent *ModelCircuit::add_via(const char *name, uint32_t data_bits) {
    auto comp = create_component(COMPONENT_VIA, data_bits, 0, 0);
    comp->property("name")->value(name);
    return comp;
}

ModelComponent *ModelCircuit::add_oscillator(uint32_t low_duration, uint32_t high_duration) {
    auto comp = create_component(COMPONENT_OSCILLATOR, 0, 1, 0);
    comp->property("low_duration")->value(static_cast<int64_t>(low_duration));
    comp->property("high_duration")->value(static_cast<int64_t>(high_duration));
    return comp;
}

ModelComponent *ModelCircuit::add_7_segment_led() {
    return create_component(COMPONENT_7_SEGMENT_LED, 8, 0, 1);
}

ModelComponent *ModelCircuit::add_sub_circuit(const char *circuit, uint32_t num_inputs, uint32_t num_outputs) {
    return create_component(circuit, num_inputs, num_outputs);
}

ModelComponent *ModelCircuit::add_sub_circuit(const char *circuit) {
    auto comp = create_component(circuit, 0, 0);
    comp->sync_nested_circuit(m_context);
    return comp;
}

ModelComponent *ModelCircuit::add_text(const char *text) {
    auto comp = create_component(COMPONENT_TEXT, 0, 0, 0);
    comp->property("text")->value(text);
    return comp;
}

std::unique_ptr<CircuitInstance> ModelCircuit::instantiate(Simulator *sim, bool top_level) {
    auto instance = std::make_unique<CircuitInstance>(sim, this);
    std::unordered_map<std::string, ModelComponent *> via_lut;

    // helper function to connect all pins of two vias
    auto connect_vias = [&instance](ModelComponent *via_a, ModelComponent *via_b) {
        assert(via_a->num_inputs() == via_b->num_inputs());
        for (uint32_t i = 0u; i < via_a->num_inputs(); ++i) {
            instance->connect_pins(via_a->input_pin_id(i), via_b->input_pin_id(i));
        }
    };

    for (auto &comp_it : m_components) {
        auto comp = comp_it.second.get();
        auto sim_comp = instance->add_component(comp);

        if (comp->type() == COMPONENT_VIA) {
            auto name = comp->property_value("name", "via");
            auto found = via_lut.find(name);
            if (found != via_lut.end()) {
                connect_vias(comp, found->second);
            } else {
                via_lut[name] = comp;
            }
        }

        if (top_level && comp->type() == COMPONENT_CONNECTOR_IN) {
            sim_comp->enable_user_values();
        }
    }

    for (auto &wire : m_wires) {
        instance->add_wire(wire.second.get());
    }

    return std::move(instance);
}

} // namespace lsim