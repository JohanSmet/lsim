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

Constant::Constant(Value value) : 
            CloneComponent(1),
            m_value(value) {
}

void Constant::tick() {
    process();
}

void Constant::process() {
    m_circuit->write_value(m_pins[0], m_value);
}

///////////////////////////////////////////////////////////////////////////////
//
// Buffer
//

Buffer::Buffer(size_t data_bits) : CloneComponent(data_bits * 2) {
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
// TriStateBuffer
//

TriStateBuffer::TriStateBuffer(size_t data_bits) : 
                    CloneComponent(1 + (data_bits * 2)),
                    m_enable_idx(data_bits) {
    assert(data_bits >= 1);
}

void TriStateBuffer::process() {
    auto data_bits = (m_pins.size() - 1) / 2;

    if (m_circuit->read_value(m_pins[m_enable_idx]) != VALUE_TRUE) {
        for (auto pin = 0u; pin < data_bits; ++pin) {
            m_circuit->write_value(m_pins[pin + data_bits + 1], VALUE_UNDEFINED);
        }
        return;
    }

    for (auto pin = 0u; pin < data_bits; ++pin) {
        auto value = m_circuit->read_value(m_pins[pin]);
        m_circuit->write_value(m_pins[pin + data_bits + 1], value);
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// AND gate
//

AndGate::AndGate(size_t num_inputs) : CloneComponent(num_inputs + 1) {
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

OrGate::OrGate(size_t num_inputs) : CloneComponent(num_inputs + 1) {
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

NotGate::NotGate() : CloneComponent(2) {
}

void NotGate::process() {
    auto input = m_circuit->read_value(m_pins[0]);
    m_circuit->write_value(m_pins[1], negate_value(input));
}

///////////////////////////////////////////////////////////////////////////////
//
// NAND gate
//

NandGate::NandGate(size_t num_inputs) : CloneComponent(num_inputs + 1) {
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

NorGate::NorGate(size_t num_inputs) : CloneComponent(num_inputs + 1) {
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

XorGate::XorGate() : CloneComponent(2 + 1) {
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

XnorGate::XnorGate() : CloneComponent(2 + 1) {
}

void XnorGate::process() {
    int output = m_circuit->read_value(m_pins[0], VALUE_TRUE);
    output ^= m_circuit->read_value(m_pins[1], VALUE_TRUE);
    m_circuit->write_value(m_pins[2], negate_value(static_cast<Value>(output)));
}
