// sim_circuit.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// instantiation of a circuit description

#include "sim_circuit.h"
#include "simulator.h"
#include "sim_component.h"

#include <cassert>

namespace lsim {

SimCircuit::SimCircuit(Simulator *sim, ModelCircuit *circuit_desc) :
        m_sim(sim),
        m_circuit_desc(circuit_desc),
        m_name("<unnamed>") {
    assert(sim);
    assert(circuit_desc);
}

SimComponent *SimCircuit::add_component(ModelComponent *comp) {
    assert(comp);
    auto sim_comp = m_sim->create_component(comp);
    m_components[comp->id()] = sim_comp;

    if (comp->type() == COMPONENT_SUB_CIRCUIT) {
        auto nested_instance = comp->nested_circuit()->instantiate(m_sim, false);
        nested_instance->build_name(comp->id());

        for (auto idx = 0u; idx < sim_comp->num_inputs(); ++idx) {
            auto nested_pin = nested_instance->pin_from_pin_id(comp->nested_circuit()->port_by_index(true, idx));
            m_sim->connect_pins(nested_pin, sim_comp->pin_by_index(sim_comp->input_pin_index(idx)));
        }        

        for (auto idx = 0u; idx < sim_comp->num_outputs(); ++idx) {
            auto nested_pin = nested_instance->pin_from_pin_id(comp->nested_circuit()->port_by_index(false, idx));
            m_sim->connect_pins(nested_pin, sim_comp->pin_by_index(sim_comp->output_pin_index(idx)));
        }        

        sim_comp->set_nested_instance(std::move(nested_instance));
    }
    return sim_comp;
}

node_t SimCircuit::add_wire(ModelWire *wire) {
    assert(wire);

    if (wire->num_pins() < 2) {
        return NODE_INVALID;
    }

    auto first_pin = pin_from_pin_id(wire->pin(0));
    auto node = NODE_INVALID;
    for (auto index = 1u; index < wire->num_pins(); ++index) {
        node = m_sim->connect_pins(first_pin, pin_from_pin_id(wire->pin(index)));
    }

    return node;
}

void SimCircuit::connect_pins(pin_id_t pin_a, pin_id_t pin_b) {
    auto a = pin_from_pin_id(pin_a);
    auto b = pin_from_pin_id(pin_b);

    if (a != PIN_UNDEFINED && b != PIN_UNDEFINED) {
        m_sim->connect_pins(a, b);
    }
}

void SimCircuit::build_name(uint32_t comp_id) {
    m_name = m_circuit_desc->name();
    m_name += "#" + std::to_string(comp_id);
}

Value SimCircuit::read_pin(pin_id_t pin_id) {
    return m_sim->read_pin(pin_from_pin_id(pin_id));
}

uint8_t SimCircuit::read_nibble(uint32_t comp_id) {
    auto comp = component_by_id(comp_id);
    assert(comp);

    assert(comp->num_inputs() == 4);
    uint8_t result = 0;

    for (int i = 0; i < comp->num_inputs(); ++i) {
        result |= ((int) m_sim->read_pin(comp->pin_by_index(comp->input_pin_index(i)))) << i;
    }

    return result;
}

uint8_t SimCircuit::read_nibble(const pin_id_container_t &pins) {
    assert(pins.size() == 4);
    uint8_t result = 0;

    for (int i = 0; i < pins.size(); ++i) {
        result |= ((int) m_sim->read_pin(pin_from_pin_id(pins[i]))) << i;
    }

    return result;
}

uint8_t SimCircuit::read_byte(uint32_t comp_id) {
    auto comp = component_by_id(comp_id);
    assert(comp);

    assert(comp->num_inputs() == 8);
    uint8_t result = 0;

    for (int i = 0; i < comp->num_inputs(); ++i) {
        result |= ((int) m_sim->read_pin(comp->pin_by_index(comp->input_pin_index(i)))) << i;
    }

    return result;
}

uint8_t SimCircuit::read_byte(const pin_id_container_t &pins) {
    assert(pins.size() == 8);
    uint8_t result = 0;

    for (int i = 0; i < pins.size(); ++i) {
        result |= ((int) m_sim->read_pin(pin_from_pin_id(pins[i]))) << i;
    }

    return result;
}

uint64_t SimCircuit::read_pins(const pin_id_container_t &pins) {
    assert(pins.size() <= 64);
    uint64_t result = 0;

    for (int i = 0; i < pins.size(); ++i) {
        result |= ((int) m_sim->read_pin(pin_from_pin_id(pins[i]))) << i;
    }

    return result;
}

void SimCircuit::write_pin(pin_id_t pin_id, Value value) {
    auto comp = component_by_id(component_id_from_pin_id(pin_id));
    assert(comp);

    comp->set_user_value(pin_index_from_pin_id(pin_id), value);
}

void SimCircuit::write_output_pins(uint32_t comp_id, value_container_t values) {
    auto comp = component_by_id(comp_id);
    assert(comp);

    assert(values.size() == comp->num_outputs());

    for (auto idx = 0u; idx < values.size(); ++idx) {
        comp->set_user_value(comp->output_pin_index(idx), values[idx]);
    }
}

void SimCircuit::write_output_pins(uint32_t comp_id, uint64_t data) {
    auto comp = component_by_id(comp_id);
    assert(comp);

    for (auto idx = 0u; idx < comp->num_outputs(); ++idx) {
        comp->set_user_value(comp->output_pin_index(idx), static_cast<Value>((data >> idx) & 1));
    }
}

void SimCircuit::write_output_pins(uint32_t comp_id, Value value) {
    auto comp = component_by_id(comp_id);
    assert(comp);

    comp->set_user_value(comp->output_pin_index(0), value);
}

void SimCircuit::write_nibble(const pin_id_container_t &pins, uint8_t data) {
    assert(pins.size() == 4);

    for (size_t idx = 0; idx < pins.size(); ++idx) {
        auto value = static_cast<Value>((data >> idx) & 1);

        auto comp = component_by_id(component_id_from_pin_id(pins[idx]));
        assert(comp);
        comp->set_user_value(pin_index_from_pin_id(pins[idx]), value);
    }
}

void SimCircuit::write_byte(const pin_id_container_t &pins, uint8_t data) {
    assert(pins.size() == 8);

    for (size_t idx = 0; idx < pins.size(); ++idx) {
        auto value = static_cast<Value>((data >> idx) & 1);

        auto comp = component_by_id(component_id_from_pin_id(pins[idx]));
        assert(comp);
        comp->set_user_value(pin_index_from_pin_id(pins[idx]), value);
    }
}

void SimCircuit::write_pins(const pin_id_container_t &pins, const value_container_t &values) {
    assert(pins.size() == values.size());

    for (size_t idx = 0; idx < pins.size(); ++idx) {
        auto comp = component_by_id(component_id_from_pin_id(pins[idx]));
        assert(comp);
        comp->set_user_value(pin_index_from_pin_id(pins[idx]), values[idx]);
    }
}

void SimCircuit::write_pins(const pin_id_container_t &pins, uint64_t data) {
    for (size_t idx = 0; idx < pins.size(); ++idx) {
        auto comp = component_by_id(component_id_from_pin_id(pins[idx]));
        assert(comp);
        comp->set_user_value(pin_index_from_pin_id(pins[idx]), static_cast<Value>((data >> idx) & 1));
    }
}

node_t SimCircuit::pin_node(pin_id_t pin_id) {
    return m_sim->pin_node(pin_from_pin_id(pin_id));
}

bool SimCircuit::node_dirty(node_t node_id) {
    return m_sim->node_dirty(node_id);
}

Value SimCircuit::pin_output(pin_id_t pin_id) {
    return m_sim->pin_output_value(pin_from_pin_id(pin_id));
}

Value SimCircuit::user_value(pin_id_t pin_id) {
    auto comp = component_by_id(component_id_from_pin_id(pin_id));
    assert(comp);
    return comp->user_value(pin_index_from_pin_id(pin_id));
}

SimComponent *SimCircuit::component_by_id(uint32_t comp_id) {

    auto found = m_components.find(comp_id);
    if (found == m_components.end()) {
        return nullptr;
    }

    return found->second;
}

pin_t SimCircuit::pin_from_pin_id(pin_id_t pin_id) {

    auto found = m_components.find(component_id_from_pin_id(pin_id));
    if (found == m_components.end()) {
        return PIN_UNDEFINED;
    }

    return found->second->pin_by_index(pin_index_from_pin_id(pin_id));
}

} // namespace lsim