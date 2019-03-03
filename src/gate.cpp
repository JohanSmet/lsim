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
    set_dirty();
}

void Constant::process() {
    for (const auto &pin : m_pins) {
        m_circuit->write_value(pin, m_value);
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Buffer
//

Buffer::Buffer(Circuit *circuit, size_t data_bits) : Component(circuit, data_bits * 2) {
    assert(circuit);
    assert(data_bits >= 1);
}

void Buffer::process() {
    auto data_bits = m_pins.size() / 2;

    for (auto pin = 0u; pin < data_bits; ++pin) {
        auto value = m_circuit->read_value(m_pins[pin]);
        m_circuit->write_value(m_pins[pin + data_bits], value);
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

///////////////////////////////////////////////////////////////////////////////
//
// OR gate
//

OrGate::OrGate(Circuit *circuit, size_t num_inputs) : Component(circuit, num_inputs + 1) {
    assert(circuit);
    assert(num_inputs >= 2);
}

void OrGate::process() {
    int output = m_circuit->read_value(m_pins[0], VALUE_TRUE);
    for (auto idx = 1u; idx < m_pins.size() - 1; ++idx) {
        output |= m_circuit->read_value(m_pins[idx], VALUE_TRUE);
    }
    m_circuit->write_value(m_pins.back(), static_cast<Value>(output));
}

///////////////////////////////////////////////////////////////////////////////
//
// NOT gate
//

NotGate::NotGate(Circuit *circuit) : Component(circuit, 2) {
    assert(circuit);
}

void NotGate::process() {
    auto input = m_circuit->read_value(m_pins[0]);
    m_circuit->write_value(m_pins[1], negate_value(input));
}

///////////////////////////////////////////////////////////////////////////////
//
// NAND gate
//

NandGate::NandGate(Circuit *circuit, size_t num_inputs) : Component(circuit, num_inputs + 1) {
    assert(circuit);
    assert(num_inputs >= 2);
}

void NandGate::process() {
    int output = m_circuit->read_value(m_pins[0], VALUE_TRUE);
    for (auto idx = 1u; idx < m_pins.size() - 1; ++idx) {
        output &= m_circuit->read_value(m_pins[idx], VALUE_TRUE);
    }
    m_circuit->write_value(m_pins.back(), negate_value(static_cast<Value>(output)));
}

///////////////////////////////////////////////////////////////////////////////
//
// NOR gate
//

NorGate::NorGate(Circuit *circuit, size_t num_inputs) : Component(circuit, num_inputs + 1) {
    assert(circuit);
    assert(num_inputs >= 2);
}

void NorGate::process() {
    int output = m_circuit->read_value(m_pins[0], VALUE_TRUE);
    for (auto idx = 1u; idx < m_pins.size() - 1; ++idx) {
        output |= m_circuit->read_value(m_pins[idx], VALUE_TRUE);
    }
    m_circuit->write_value(m_pins.back(), negate_value(static_cast<Value>(output)));
}

///////////////////////////////////////////////////////////////////////////////
//
// XOR gate
//

XorGate::XorGate(Circuit *circuit) : Component(circuit, 2 + 1) {
    assert(circuit);
}

void XorGate::process() {
    int output = m_circuit->read_value(m_pins[0], VALUE_TRUE);
    output ^= m_circuit->read_value(m_pins[1], VALUE_TRUE);
    m_circuit->write_value(m_pins[2], static_cast<Value>(output));
}

///////////////////////////////////////////////////////////////////////////////
//
// XNOR gate
//

XnorGate::XnorGate(Circuit *circuit) : Component(circuit, 2 + 1) {
    assert(circuit);
}

void XnorGate::process() {
    int output = m_circuit->read_value(m_pins[0], VALUE_TRUE);
    output ^= m_circuit->read_value(m_pins[1], VALUE_TRUE);
    m_circuit->write_value(m_pins[2], negate_value(static_cast<Value>(output)));
}
