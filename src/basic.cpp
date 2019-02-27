// basic.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Basic component parts

#include "basic.h"
#include "circuit.h"
#include <assert.h>

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

//////////////////////////////////////////////////////////////////////////////
//
// Connection - I/O between circuits
//

Connector::Connector(Circuit *circuit, size_t data_bits) : Component(circuit, data_bits) {

}

void Connector::process() {

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