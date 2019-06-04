// circuit_instance.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// instantiation of a circuit description

#include "circuit_instance.h"

#include <cassert>

namespace lsim {

CircuitInstance::CircuitInstance(Simulator *sim, CircuitDescription *circuit_desc) :
        m_sim(sim),
        m_circuit_desc(circuit_desc) {
    assert(sim);
    assert(circuit_desc);
}

SimComponent *CircuitInstance::add_component(Component *comp) {
    assert(comp);
    auto sim_comp = m_sim->create_component(comp);
    m_components[comp->id()] = sim_comp;

    if (comp->type() == COMPONENT_SUB_CIRCUIT) {
        auto nested_instance = comp->nested_circuit()->instantiate(m_sim);

        for (size_t idx = 0; idx < sim_comp->num_inputs(); ++idx) {
            auto nested_pin = nested_instance->pin_from_pin_id(comp->nested_circuit()->port_by_index(true, idx));
            auto node = m_sim->connect_pins(nested_pin, sim_comp->pin_by_index(sim_comp->input_pin_index(idx)));
        }        

        for (size_t idx = 0; idx < sim_comp->num_outputs(); ++idx) {
            auto nested_pin = nested_instance->pin_from_pin_id(comp->nested_circuit()->port_by_index(false, idx));
            auto node = m_sim->connect_pins(nested_pin, sim_comp->pin_by_index(sim_comp->output_pin_index(idx)));
        }        

        sim_comp->set_nested_instance(std::move(nested_instance));
    }
    return sim_comp;
}

node_t CircuitInstance::add_wire(Wire *wire) {
    assert(wire);
    assert(wire->num_pins() >= 1);

    auto first_pin = pin_from_pin_id(wire->pin(0));
    auto node = NODE_INVALID;
    for (size_t index = 1; index < wire->num_pins(); ++index) {
        node = m_sim->connect_pins(first_pin, pin_from_pin_id(wire->pin(index)));
    }

    return node;
}

Value CircuitInstance::read_pin(pin_id_t pin_id) {
    return m_sim->read_pin(pin_from_pin_id(pin_id));
}

uint8_t CircuitInstance::read_nibble(uint32_t comp_id) {
    auto comp = component_by_id(comp_id);
    assert(comp);

    assert(comp->num_inputs() == 4);
    uint8_t result = 0;

    for (int i = 0; i < comp->num_inputs(); ++i) {
        result |= ((int) m_sim->read_pin(comp->pin_by_index(comp->input_pin_index(i)))) << i;
    }

    return result;
}

uint8_t CircuitInstance::read_nibble(const pin_id_container_t &pins) {
    assert(pins.size() == 4);
    uint8_t result = 0;

    for (int i = 0; i < pins.size(); ++i) {
        result |= ((int) m_sim->read_pin(pin_from_pin_id(pins[i]))) << i;
    }

    return result;
}

uint8_t CircuitInstance::read_byte(uint32_t comp_id) {
    auto comp = component_by_id(comp_id);
    assert(comp);

    assert(comp->num_inputs() == 8);
    uint8_t result = 0;

    for (int i = 0; i < comp->num_inputs(); ++i) {
        result |= ((int) m_sim->read_pin(comp->pin_by_index(comp->input_pin_index(i)))) << i;
    }

    return result;
}

uint8_t CircuitInstance::read_byte(const pin_id_container_t &pins) {
    assert(pins.size() == 8);
    uint8_t result = 0;

    for (int i = 0; i < pins.size(); ++i) {
        result |= ((int) m_sim->read_pin(pin_from_pin_id(pins[i]))) << i;
    }

    return result;
}

void CircuitInstance::write_pin(pin_id_t pin_id, Value value) {
    auto comp = component_by_id(component_id_from_pin_id(pin_id));
    assert(comp);

    comp->write_pin(pin_index_from_pin_id(pin_id), value);
}

void CircuitInstance::write_output_pins(uint32_t comp_id, value_container_t values) {
    auto comp = component_by_id(comp_id);
    assert(comp);

    assert(values.size() == comp->num_outputs());

    for (size_t idx = 0; idx < values.size(); ++idx) {
        comp->write_pin(comp->output_pin_index(idx), values[idx]);
    }
}

void CircuitInstance::write_output_pins(uint32_t comp_id, uint64_t data) {
    auto comp = component_by_id(comp_id);
    assert(comp);

    for (size_t idx = 0; idx < comp->num_outputs(); ++idx) {
        comp->write_pin(comp->output_pin_index(idx), static_cast<Value>((data >> idx) & 1));
    }
}

void CircuitInstance::write_output_pins(uint32_t comp_id, Value value) {
    auto comp = component_by_id(comp_id);
    assert(comp);

    comp->write_pin(comp->output_pin_index(0), value);
}

SimComponent *CircuitInstance::component_by_id(uint32_t comp_id) {

    auto found = m_components.find(comp_id);
    if (found == m_components.end()) {
        return nullptr;
    }

    return found->second;
}

pin_t CircuitInstance::pin_from_pin_id(pin_id_t pin_id) {

    auto found = m_components.find(component_id_from_pin_id(pin_id));
    if (found == m_components.end()) {
        return PIN_UNDEFINED;
    }

    return found->second->pin_by_index(pin_index_from_pin_id(pin_id));
}

} // namespace lsim