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
            CloneComponent(1, VisualComponent::CONSTANT),
            m_value(value) {
}

bool Constant::is_dirty() const {
    return true;
}

void Constant::process() {
    write_pin(0, m_value);
}

///////////////////////////////////////////////////////////////////////////////
//
// Buffer
//

Buffer::Buffer(size_t data_bits) : CloneComponent(data_bits * 2, VisualComponent::BUFFER) {
    assert(data_bits >= 1);
}

void Buffer::process() {
    auto data_bits = num_pins() / 2;

    for (auto pin = 0u; pin < data_bits; ++pin) {
        auto value = read_pin(pin);
        write_pin(pin + data_bits, value);
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// TriStateBuffer
//

TriStateBuffer::TriStateBuffer(size_t data_bits) : 
                    CloneComponent(1 + (data_bits * 2), VisualComponent::BUFFER),
                    m_enable_idx(data_bits) {
    assert(data_bits >= 1);
}

void TriStateBuffer::process() {
    auto data_bits = (num_pins() - 1) / 2;

    if (read_pin(m_enable_idx) != VALUE_TRUE) {
        for (auto pin = 0u; pin < data_bits; ++pin) {
            write_pin(pin + data_bits + 1, VALUE_UNDEFINED);
        }
        return;
    }

    for (auto pin = 0u; pin < data_bits; ++pin) {
        auto value = read_pin(pin);
        write_pin(pin + data_bits + 1, value);
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// AND gate
//

AndGate::AndGate(size_t num_inputs) : CloneComponent(num_inputs + 1, VisualComponent::AND_GATE) {
    assert(num_inputs >= 2);
}

void AndGate::process() {
    const auto OUTPUT_PIN = num_pins() - 1;
    reset_bad_read_check();

    bool output = read_pin_checked(0);
    for (auto idx = 1u; idx < OUTPUT_PIN; ++idx) {
        output &= read_pin_checked(idx);
    }
    write_pin_checked(OUTPUT_PIN, output);
}

///////////////////////////////////////////////////////////////////////////////
//
// OR gate
//

OrGate::OrGate(size_t num_inputs) : CloneComponent(num_inputs + 1, VisualComponent::OR_GATE) {
    assert(num_inputs >= 2);
}

void OrGate::process() {
    const auto OUTPUT_PIN = num_pins() - 1;
    reset_bad_read_check();

    bool output = read_pin_checked(0);
    for (auto idx = 1u; idx < OUTPUT_PIN; ++idx) {
        output |= read_pin_checked(idx);
    }
    write_pin_checked(OUTPUT_PIN, output);
}

///////////////////////////////////////////////////////////////////////////////
//
// NOT gate
//

NotGate::NotGate() : CloneComponent(2, VisualComponent::NOT_GATE) {
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

NandGate::NandGate(size_t num_inputs) : CloneComponent(num_inputs + 1, VisualComponent::NAND_GATE) {
    assert(num_inputs >= 2);
}

void NandGate::process() {
    const auto OUTPUT_PIN = num_pins() - 1;
    reset_bad_read_check();

    auto output = read_pin_checked(0);
    for (auto idx = 1u; idx < OUTPUT_PIN; ++idx) {
        output &= read_pin_checked(idx);
    }
    write_pin_checked(OUTPUT_PIN, !output);
}

///////////////////////////////////////////////////////////////////////////////
//
// NOR gate
//

NorGate::NorGate(size_t num_inputs) : CloneComponent(num_inputs + 1, VisualComponent::NOR_GATE) {
    assert(num_inputs >= 2);
}

void NorGate::process() {
    const auto OUTPUT_PIN = num_pins() - 1;
    reset_bad_read_check();

    auto output = read_pin_checked(0);
    for (auto idx = 1u; idx < OUTPUT_PIN; ++idx) {
        output |= read_pin_checked(idx);
    }
    write_pin_checked(OUTPUT_PIN, !output);
}

///////////////////////////////////////////////////////////////////////////////
//
// XOR gate
//

XorGate::XorGate() : CloneComponent(2 + 1, VisualComponent::XOR_GATE) {
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

XnorGate::XnorGate() : CloneComponent(2 + 1, VisualComponent::XNOR_GATE) {
}

void XnorGate::process() {
    reset_bad_read_check();

    auto output = read_pin_checked(0);
    output ^= read_pin_checked(1);
    write_pin_checked(2, !output);
}
