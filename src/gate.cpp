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
    write_pin(0, m_value);
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
        write_pin(pin + data_bits, value);
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
            write_pin(pin + data_bits + 1, VALUE_UNDEFINED);
        }
        return;
    }

    for (auto pin = 0u; pin < data_bits; ++pin) {
        m_circuit->write_value(m_pins[pin + data_bits + 1], value);
        write_pin(pin + data_bits + 1, value);
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
    reset_bad_read_check();

    bool output = read_pin_checked(0);
    for (auto idx = 1u; idx < m_pins.size() - 1; ++idx) {
        output &= read_pin_checked(idx);
    }
    write_pin_checked(m_pins.size() - 1, output);
}

///////////////////////////////////////////////////////////////////////////////
//
// OR gate
//

OrGate::OrGate(size_t num_inputs) : CloneComponent(num_inputs + 1) {
    assert(num_inputs >= 2);
}

void OrGate::process() {
    reset_bad_read_check();

    bool output = read_pin_checked(0);
    for (auto idx = 1u; idx < m_pins.size() - 1; ++idx) {
        output |= read_pin_checked(idx);
    }
    write_pin_checked(m_pins.size() - 1, output);
}

///////////////////////////////////////////////////////////////////////////////
//
// NOT gate
//

NotGate::NotGate() : CloneComponent(2) {
}

void NotGate::process() {
    reset_bad_read_check();

    auto input = read_pin_checked(0);  
    write_pin_checked(1, !input);
}

///////////////////////////////////////////////////////////////////////////////
//
// NAND gate
//

NandGate::NandGate(size_t num_inputs) : CloneComponent(num_inputs + 1) {
    assert(num_inputs >= 2);
}

void NandGate::process() {
    reset_bad_read_check();

    auto output = read_pin_checked(0);
    for (auto idx = 1u; idx < m_pins.size() - 1; ++idx) {
        output &= read_pin_checked(idx);
    }
    write_pin_checked(m_pins.size() - 1, !output);
}

///////////////////////////////////////////////////////////////////////////////
//
// NOR gate
//

NorGate::NorGate(size_t num_inputs) : CloneComponent(num_inputs + 1) {
    assert(num_inputs >= 2);
}

void NorGate::process() {
    reset_bad_read_check();

    auto output = read_pin_checked(0);
    for (auto idx = 1u; idx < m_pins.size() - 1; ++idx) {
        output |= read_pin_checked(idx);
    }
    write_pin_checked(m_pins.size() - 1, !output);
}

///////////////////////////////////////////////////////////////////////////////
//
// XOR gate
//

XorGate::XorGate() : CloneComponent(2 + 1) {
}

void XorGate::process() {
    reset_bad_read_check();

    auto output = read_pin_checked(0);
    output ^= read_pin_checked(1);
    write_pin_checked(2, output);
}

///////////////////////////////////////////////////////////////////////////////
//
// XNOR gate
//

XnorGate::XnorGate() : CloneComponent(2 + 1) {
}

void XnorGate::process() {
    reset_bad_read_check();

    auto output = read_pin_checked(0);
    output ^= read_pin_checked(1);
    write_pin_checked(2, !output);
}
