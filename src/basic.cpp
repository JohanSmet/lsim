// basic.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Basic component parts

#include "basic.h"
#include "circuit.h"
#include <assert.h>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////////
//
// Component
//

Component::Component(size_t pin_count) : 
                m_circuit(nullptr),
                m_pin_count(pin_count),
                m_read_bad(false) {
}

Component::Component(const Component &other) : 
                m_circuit(nullptr),
                m_pin_count(other.m_pin_count) {
}

void Component::materialize(Circuit *circuit) {
    assert(m_circuit == nullptr);
    m_circuit = circuit;

    for (size_t i = 0; i < m_pin_count; ++i) {
        m_pins.push_back(circuit->create_pin(this));
        m_values.push_back(VALUE_UNDEFINED);
    }
}

pin_t Component::pin(uint32_t index) const {
    assert(index < m_pins.size());
    return m_pins[index];
}

Component::pin_container_t Component::pins(size_t start, size_t end) {
    assert(start < m_pins.size());
    assert(end < m_pins.size());
    assert(start <= end);
    return pin_container_t(m_pins.begin() + start, m_pins.begin() + end);
}

void Component::write_pin(uint32_t index, Value value) {
    assert(index < m_pins.size());
    m_values[index] = value;
    m_circuit->write_value(m_pins[index], value);
}

Value Component::read_pin(uint32_t index) const {
    assert(index < m_pins.size());
    return m_circuit->read_value(m_pins[index]);
}

void Component::write_pin_checked(uint32_t index, bool value) {
    auto output = m_read_bad ? VALUE_ERROR : value;
    write_pin(index, static_cast<Value>(output));
}

bool Component::read_pin_checked(uint32_t index) {
    assert(index < m_pins.size());
    auto value = m_circuit->read_value(m_pins[index]);
    m_read_bad |= (value != VALUE_TRUE && value != VALUE_FALSE);
    return static_cast<bool>(value);
}

void Component::tick() {
    if (is_dirty()) {
        process();
    } else {
        for (size_t i = 0; i < m_pins.size(); ++i) {
            if (m_values[i] != VALUE_UNDEFINED) {
                m_circuit->write_value(m_pins[i], m_values[i]);
            }
        }
    }
}

bool Component::is_dirty() const {
    return std::any_of(std::begin(m_pins), std::end(m_pins),
                       [=] (auto pin) {return m_circuit->value_changed(pin);}
    );
}

//////////////////////////////////////////////////////////////////////////////
//
// Connection - I/O between circuits
//

Connector::Connector(const char *name, size_t data_bits) : 
            CloneComponent(data_bits),
            m_name(name),
            m_changed(false) {
    assert(data_bits > 0 && data_bits < 64);
}

void Connector::materialize(Circuit *circuit) {
    Component::materialize(circuit);

    if (num_pins() == 1) {
        circuit->add_interface_pin(m_name.c_str(), pin(0));
        m_data.push_back(VALUE_UNDEFINED);
    } else {
        for (size_t idx = 0; idx < num_pins(); ++idx) {
            circuit->add_interface_pin((m_name + "[" + std::to_string(idx) + "]").c_str(), pin(idx));
            m_data.push_back(VALUE_UNDEFINED);
        }
    }

    circuit->register_component_name(m_name, this);
}

bool Connector::is_dirty() const {
    return m_changed;
}

void Connector::process() {
    for (auto idx = 0u; idx < num_pins(); ++idx) {
        write_pin(idx, m_data[idx]);
    }
    m_changed = false;
}

void Connector::change_data(uint64_t data) {
    for (auto idx = 0u; idx < num_pins(); ++idx) {
        m_data[idx] = static_cast<Value>((data >> idx) & 1);
    }
    m_changed = true;
}

void Connector::change_data(Value value) {
    assert(num_pins() == 1);
    m_data[0] = value;
    m_changed = true;
}

void Connector::change_data(Component::value_container_t data) {
    assert(num_pins() == data.size());
    m_data = data;
    m_changed = true;
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