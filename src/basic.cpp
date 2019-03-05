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

Component::Component(Circuit *circuit, size_t pin_count) : m_circuit(circuit) {
    for (size_t i = 0; i < pin_count; ++i) {
        m_pins.push_back(circuit->create_pin(this));
    }
}

pin_t Component::pin(uint32_t index) {
    assert(index < m_pins.size());
    return m_pins[index];
}

void Component::tick() {
    if (is_dirty()) {
        process();
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

Connector::Connector(Circuit *circuit, const char *name, size_t data_bits) : 
            Component(circuit, data_bits),
            m_data(0),
            m_changed(false),
            m_name(name) {
    assert(data_bits > 0 && data_bits < 64);
}

void Connector::tick() {
    if (m_changed) {
        process();
        m_changed = false;
    }
}

void Connector::process() {
    for (auto idx = 0u; idx < m_pins.size(); ++idx) {
        int data = (m_data & (1 << idx)) >> idx;
        m_circuit->write_value(m_pins[idx], static_cast<Value>(data));
    }
}

void Connector::change_data(uint64_t data) {
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