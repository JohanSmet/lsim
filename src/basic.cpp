// basic.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Basic component parts

#include "basic.h"
#include "simulator.h"
#include "circuit.h"
#include <assert.h>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////////
//
// Component
//

static bool default_check_dirty_func(Component *comp) {
    const auto &input_pins = comp->input_pins();
    const auto &control_pins = comp->control_pins();

    bool input_changed = std::any_of(std::begin(input_pins), std::end(input_pins),
                            [=] (auto pin) {return comp->sim()->pin_changed_last_step(pin);}
    );

    bool cntrl_changed = std::any_of(std::begin(control_pins), std::end(control_pins),
                            [=] (auto pin) {return comp->sim()->pin_changed_last_step(pin);}
    );

    return input_changed || cntrl_changed;
}

Component::Component(
            Simulator *sim,
            size_t num_input, size_t num_output, size_t num_control,
            ComponentType type,
            Priority priority) : 
        m_sim(sim),
        m_output_start(num_input),
        m_control_start(num_input + num_output),
        m_read_bad(false),
        m_type(type),
        m_priority(priority),
        m_process_func(nullptr),
        m_check_dirty_func(default_check_dirty_func) {
    
    auto total_pins = num_input + num_output + num_control;

    for (size_t i = 0; i < total_pins; ++i) {
        m_pins.push_back(sim->assign_pin());
        m_values.push_back(VALUE_UNDEFINED);
    }
}

void Component::set_process_func(Component::process_func_t func) {
    m_process_func = func;
}

void Component::set_check_dirty_func(Component::check_dirty_func_t func) {
    if (func != nullptr) {
        m_check_dirty_func = func; 
    } else {
        m_check_dirty_func = default_check_dirty_func;
    }
}

Component::uptr_t Component::clone() const {
    auto cloned = std::make_unique<Component>(
        m_sim, 
        num_input_pins(), num_output_pins(), num_control_pins(), 
        m_type, 
        m_priority);
    cloned->set_check_dirty_func(m_check_dirty_func);
    cloned->set_process_func(m_process_func);
    cloned->m_values = m_values;
    return cloned;
}

pin_t Component::pin(uint32_t index) const {
    assert(index < m_pins.size());
    return m_pins[index];
}

pin_t Component::input_pin(uint32_t index) const {
    assert(index < m_output_start);
    return m_pins[index];
}

pin_t Component::output_pin(uint32_t index) const {
    assert(m_output_start + index < m_control_start);
    return m_pins[m_output_start + index];
}

pin_t Component::control_pin(uint32_t index) const {
    assert(m_control_start + index < m_pins.size());
    return m_pins[m_control_start + index];
}

Component::pin_container_t Component::pins(size_t start, size_t end) const {
    assert(start < m_pins.size());
    assert(end < m_pins.size());
    assert(start <= end);
    return pin_container_t(m_pins.begin() + start, m_pins.begin() + end + 1);
}

Component::pin_container_t Component::input_pins() const {
    return pin_container_t(m_pins.begin(), m_pins.begin() + m_output_start);
}

Component::pin_container_t Component::output_pins() const {
    return pin_container_t(m_pins.begin() + m_output_start, m_pins.begin() + m_control_start);
}

Component::pin_container_t Component::control_pins() const {
    return pin_container_t(m_pins.begin() + m_control_start, m_pins.end());
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

const char *Component::property_value(const char *key, const char *def_value) {
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

void Component::tick() {
    if (m_check_dirty_func(this) && m_process_func != nullptr) {
        m_process_func(this);
    } else {
        for (size_t i = 0; i < m_pins.size(); ++i) {
            if (m_values[i] != VALUE_UNDEFINED) {
                m_sim->write_pin(m_pins[i], m_values[i]);
            }
        }
    }
}

Value Component::read_pin(uint32_t index) const {
    assert(index < m_pins.size());
    return m_sim->read_pin(m_pins[index]);
}

void Component::write_pin(uint32_t index, Value value) {
    assert(index < m_pins.size());
    m_values[index] = value;
    m_sim->write_pin(m_pins[index], value);
}

void Component::write_output_pins(value_container_t values) {
    assert(values.size() == m_control_start - m_output_start);
    for (size_t idx = 0; idx < values.size(); ++idx) {
        write_pin(m_output_start + idx, values[idx]);
    }
}

void Component::write_output_pins(uint64_t data) {
    for (size_t idx = 0; idx < num_output_pins(); ++idx) {
        write_pin(m_output_start + idx, static_cast<Value>((data >> idx) & 1));
    }
}

void Component::write_output_pins(Value value) {
    write_pin(m_output_start, value);
}

bool Component::read_pin_checked(uint32_t index) {
    assert(index < m_pins.size());
    auto value = m_sim->read_pin(m_pins[index]);
    m_read_bad |= (value != VALUE_TRUE && value != VALUE_FALSE);
    return static_cast<bool>(value);
}

void Component::write_pin_checked(uint32_t index, bool value) {
    auto output = m_read_bad ? VALUE_ERROR : value;
    write_pin(index, static_cast<Value>(output));
}

Value Component::read_value(uint32_t index) const {
    assert(index < m_pins.size());
    return m_values[index];
}

//////////////////////////////////////////////////////////////////////////////
//
// Connector - I/O between circuits
//

Component *ConnectorInput(Circuit *circuit, const char *name, size_t data_bits, bool tri_state) {
    auto connector = circuit->create_component(0, data_bits, 0, COMPONENT_CONNECTOR_IN);
    connector->add_property(make_property("name", name));
    connector->add_property(make_property("tri_state", tri_state));
    connector->set_check_dirty_func([](Component *connector) {return false;});
    circuit->register_component_name(name, connector);
    circuit->add_external_pins(name, connector);
    return connector;
}

Component *ConnectorOutput(Circuit *circuit, const char *name, size_t data_bits, bool tri_state) {
    auto connector = circuit->create_component(data_bits, 0, 0, COMPONENT_CONNECTOR_OUT);
    connector->add_property(make_property("name", name));
    connector->add_property(make_property("tri_state", tri_state));
    circuit->register_component_name(name, connector);
    circuit->add_external_pins(name, connector);
    return connector;
}

#if 0

//////////////////////////////////////////////////////////////////////////////
//
// Delay
//

Delay::Delay(size_t delay) {
    assert(delay >= 1);

    for (size_t i = 0; i < delay; ++i) {
        m_values.push_back(VALUE_UNDEFINED);
    }
}

Value Delay::push_value(Value value) {
    auto result = m_values.front();
    m_values.push_back(value);
    m_values.pop_front();
    return result;
}

Value Delay::push_last() {
    auto result = m_values.front();
    m_values.push_back(m_values.back());
    m_values.pop_front();
    return result;
}

#endif