// basic.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Basic component parts

#include "basic.h"
#include "circuit.h"
#include <assert.h>

Component::Component(Circuit *circuit, size_t pin_count) : m_circuit(circuit) {
    for (size_t i = 0; i < pin_count; ++i) {
        m_pins.push_back(circuit->create_pin(this));
    }
}


#if 0

//////////////////////////////////////////////////////////////////////////////
//
// Pin
//
Pin::Pin(Gate *parent, PinDirection direction) : 
        m_parent(parent),
        m_direction(direction),
        m_value(VALUE_UNDEFINED) {
}

void Pin::generate_value(Value value) {
    assert (m_direction == PIN_OUTPUT);
    m_value = value;
}

void Pin::receive_value(Value value) {
    assert (m_direction == PIN_INPUT);
    if (m_value != value) {
        m_parent->set_dirty();
        m_value = value;
    }
}

void Pin::change_direction(PinDirection direction) {
    m_direction = direction;
}

//////////////////////////////////////////////////////////////////////////////
//
// Connection
//

Connection::Connection() : 
        m_valid(true) {

}

void Connection::add_pin(Pin *pin) {
    assert(pin);

    m_pins.insert(pin);
}

void Connection::propagate_output() {
    validate();

    if (m_valid) {
        Pin *out = active_output();
        assert(out);

        for (const auto &pin : m_pins) {
            if (pin->get_direction() == PIN_INPUT) {
                pin->receive_value(out->get_value());
            }
        }
    }
}

void Connection::validate() {
    uint32_t c_input = 0;
    uint32_t c_output = 0;
    uint32_t c_undef = 0;

    for (const auto &pin : m_pins) {
        if (pin->get_direction() == PIN_INPUT) {
            ++c_input;
        } else if (pin->get_direction() == PIN_OUTPUT) {
            if (pin->get_value() == VALUE_UNDEFINED) {
                ++c_undef;
            } else {
                ++c_output;
            }
        }
    }

    m_valid = (c_output == 1) && (c_input >= 1);
}

Pin *Connection::active_output() const {
    for (const auto &pin : m_pins) {
        if (pin->get_direction() == PIN_OUTPUT &&
            pin->get_value() != VALUE_UNDEFINED) {
            return pin;
        }
    }

    return nullptr;
}

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