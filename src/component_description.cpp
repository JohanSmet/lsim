// component_description.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// description of a single component

#include "component_description.h"
#include "circuit_description.h"

#include <cassert>

namespace lsim {

Component::Component(uint32_t id, ComponentType type, size_t inputs, size_t outputs, size_t controls) :
        m_id(id),
        m_type(type),
        m_priority(PRIORITY_NORMAL),
        m_inputs(inputs),
        m_outputs(outputs),
        m_controls(controls),
        m_nested_circuit(nullptr),
        m_position(0,0),
        m_angle(0) {
}

Component::Component(uint32_t id, CircuitDescription *nested) :
        m_id(id),
        m_type(COMPONENT_SUB_CIRCUIT),
        m_priority(PRIORITY_NORMAL),
        m_inputs(nested->num_input_ports()),
        m_outputs(nested->num_output_ports()),
        m_controls(0),
        m_nested_circuit(nested),
        m_position(0,0),
        m_angle(0) {
    for (size_t idx = 0; idx < m_inputs; ++idx) {
        m_port_lut[nested->port_name(true, idx)] = input_pin_id(idx);
    }

    for (size_t idx = 0; idx < m_outputs; ++idx) {
        m_port_lut[nested->port_name(false, idx)] = output_pin_id(idx);
    }
}

pin_id_t Component::pin_id(size_t index) const {
    assert(index < m_inputs + m_outputs + m_controls);
    return (static_cast<pin_id_t>(m_id) << 32) | (index & 0xffffffff);
}

pin_id_t Component::input_pin_id(size_t index) const {
    assert(index < m_inputs);
    return pin_id(index);
}

pin_id_t Component::output_pin_id(size_t index) const {
    assert(index < m_outputs);
    return pin_id(m_inputs + index);
}

pin_id_t Component::control_pin_id(size_t index) const {
    assert(index < m_controls);
    return pin_id(m_inputs + m_outputs + index);
}

void Component::change_input_pins(size_t new_count) {
    m_inputs = new_count;
}

void Component::change_output_pins(size_t new_count) {
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

} // namespace lsim