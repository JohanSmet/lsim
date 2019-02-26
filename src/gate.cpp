// gate.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// base for gates

#include "gate.h"
#include "circuit.h"
#include <assert.h>

AndGate::AndGate(Circuit *circuit, size_t num_inputs) : Component(circuit, num_inputs + 1) {
    assert(circuit);
    assert(num_inputs >= 2);
}

void AndGate::process() {
    int output = m_circuit->read_value(m_pins[0], VALUE_TRUE);
    for (auto idx = 1u; idx < m_pins.size() - 1; ++idx) {
        output &= m_circuit->read_value(m_pins[idx], VALUE_TRUE);
    }
    m_circuit->write_value(m_pins.back(), static_cast<Value>(output));
}

#if 0

Gate::Gate(size_t delay) : 
        m_delay(delay),
        m_dirty(false) {
}

void Gate::set_dirty() {
    m_dirty = true;
}

void Gate::add_pin(PinDirection direction) {
    m_pins.push_back(std::make_unique<Pin>(this, direction));
}

void Gate::tick() {
    if (!m_dirty) {
        m_delay.push_last();
    } else {
        process();
    }
}

AndGate::AndGate(size_t num_inputs) : 
            Gate(1), 
            m_num_inputs(num_inputs) {
    for (size_t i = 0; i < num_inputs; ++i) {
        add_pin(PIN_INPUT);
    }
    add_pin(PIN_OUTPUT);
}
#endif