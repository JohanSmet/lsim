// gate.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// base for gates

#include "gate.h"
#include "circuit.h"
#include <assert.h>

///////////////////////////////////////////////////////////////////////////////
//
// constant
//

Constant::Constant(Circuit *circuit, size_t outputs, Value value) : 
            Component(circuit, outputs),
            m_value(value) {
    assert(circuit);
    assert(outputs >= 1);
}

void Constant::process() {
    for (const auto &pin : m_pins) {
        m_circuit->write_value(pin, m_value);
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// AND gate
//

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