// component_description.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// description of a single component

#include "component_description.h"
#include "circuit_description.h"
#include "lsim_context.h"

#include <cassert>

namespace lsim {

Component::Component(CircuitDescription *parent, uint32_t id, ComponentType type, uint32_t inputs, uint32_t outputs, uint32_t controls) :
        m_circuit(parent),
        m_id(id),
        m_type(type),
        m_priority(PRIORITY_NORMAL),
        m_inputs(inputs),
        m_outputs(outputs),
        m_controls(controls),
        m_nested_name(""),
        m_nested_circuit(nullptr),
        m_position(0,0),
        m_angle(0) {
}

Component::Component(CircuitDescription *parent, uint32_t id, const char *circuit_name, uint32_t inputs, uint32_t outputs) :
        m_circuit(parent),
        m_id(id),
        m_type(COMPONENT_SUB_CIRCUIT),
        m_priority(PRIORITY_NORMAL),
        m_inputs(inputs),
        m_outputs(outputs),
        m_controls(0),
        m_nested_name(circuit_name),
        m_nested_circuit(nullptr),
        m_position(0,0),
        m_angle(0) {
}

pin_id_t Component::pin_id(uint32_t index) const {
    assert(index < m_inputs + m_outputs + m_controls);
    return (static_cast<pin_id_t>(m_id) << 32) | (index & 0xffffffff);
}

pin_id_t Component::input_pin_id(uint32_t index) const {
    assert(index < m_inputs);
    return pin_id(index);
}

pin_id_t Component::output_pin_id(uint32_t index) const {
    assert(index < m_outputs);
    return pin_id(m_inputs + index);
}

pin_id_t Component::control_pin_id(uint32_t index) const {
    assert(index < m_controls);
    return pin_id(m_inputs + m_outputs + index);
}

void Component::change_input_pins(uint32_t new_count) {
    m_inputs = new_count;
}

void Component::change_output_pins(uint32_t new_count) {
    m_outputs = new_count;
}

pin_id_t Component::port_by_name(const char *name) const {
    assert(m_nested_circuit != nullptr);
    auto found = m_port_lut.find(name);
    if (found != m_port_lut.end()) {
        return found->second;
    } else {
        return PIN_ID_INVALID;
    }
}

void Component::add_property(Property::uptr_t &&prop) {
    m_properties[prop->key()] = std::move(prop);
}

Property *Component::property(const char *key) {
    auto result = m_properties.find(key);
    if (result != m_properties.end()) {
        return result->second.get();
    } else {
        return nullptr;
    }
}

std::string Component::property_value(const char *key, const char *def_value) {
    auto result = property(key);
    return (result != nullptr) ? result->value_as_string() : def_value;
}

int64_t Component::property_value(const char *key, int64_t def_value) {
    auto result = property(key);
    return (result != nullptr) ? result->value_as_integer() : def_value;
}

bool Component::property_value(const char *key, bool def_value) {
    auto result = property(key);
    return (result != nullptr) ? result->value_as_boolean() : def_value;
}

Value Component::property_value(const char *key, Value def_value) {
    auto result = property(key);
    return (result != nullptr) ? result->value_as_lsim_value() : def_value;
}

void Component::set_position(const Point &pos) {
    m_position = pos;
}

void Component::set_angle(int angle) {
    m_angle = angle;
}

bool Component::sync_nested_circuit(LSimContext *lsim_context) {

    m_nested_circuit = lsim_context->find_circuit(m_nested_name.c_str(), m_circuit->lib());
    if (!m_nested_circuit) {
        return false;
    }

    m_inputs = m_nested_circuit->num_input_ports();
    m_outputs = m_nested_circuit->num_output_ports();

    m_port_lut.clear();

    for (auto idx = 0u; idx < m_inputs; ++idx) {
        m_port_lut[m_nested_circuit->port_name(true, idx)] = input_pin_id(idx);
    }

    for (auto idx = 0u; idx < m_outputs; ++idx) {
        m_port_lut[m_nested_circuit->port_name(false, idx)] = output_pin_id(idx);
    }

    return true;
}

Component::uptr_t Component::copy() const {
    auto clone = std::make_unique<Component>(nullptr, -1, m_type, m_inputs, m_outputs, m_controls);
    clone->m_priority = m_priority;
    clone->m_nested_name = m_nested_name;
    clone->m_nested_circuit = m_nested_circuit;
    clone->m_port_lut = m_port_lut;
    for (const auto &prop_it : m_properties) {
        clone->add_property(prop_it.second->clone());
    }
    clone->m_position = m_position;
    clone->m_angle = m_angle;

    return std::move(clone);
}

void Component::integrate_into_circuit(CircuitDescription *circuit, uint32_t id) {
    m_circuit = circuit;
    m_id = id;
}

} // namespace lsim