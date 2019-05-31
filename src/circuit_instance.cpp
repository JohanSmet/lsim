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

    m_wire_nodes[wire->id()] = node;
    return node;
}

Value CircuitInstance::read_pin(pin_id_t pin_id) {
    return m_sim->read_pin(pin_from_pin_id(pin_id));
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