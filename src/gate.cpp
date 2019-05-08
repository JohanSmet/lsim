// gate.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// base for gates

#include "gate.h"
#include "circuit.h"
#include "simulator.h"
#include <assert.h>

///////////////////////////////////////////////////////////////////////////////
//
// constant
//

Component *Constant(Circuit *circuit, Value value) {
    auto constant = circuit->create_component(0, 1, 0, COMPONENT_CONSTANT);
    constant->set_check_dirty_func([](Component *constant) {return false;});
    constant->write_pin(0, value);
    return constant;
}

///////////////////////////////////////////////////////////////////////////////
//
// Buffer
//

Component *Buffer(Circuit *circuit, size_t data_bits) {
    assert(data_bits >= 1);
    auto buffer = circuit->create_component(data_bits, data_bits, 0, COMPONENT_BUFFER);

    buffer->set_process_func([](Component *buffer) {
        for (size_t pin = 0; pin < buffer->num_input_pins(); ++pin) {
            auto value = buffer->read_pin(buffer->input_pin_index(pin));
            buffer->write_pin(buffer->output_pin_index(pin), value);
        }
    });

    return buffer;
}

///////////////////////////////////////////////////////////////////////////////
//
// TriStateBuffer
//


Component *TriStateBuffer(Circuit *circuit, size_t data_bits) {
    assert(data_bits >= 1);
    auto buffer = circuit->create_component(data_bits, data_bits, 1, COMPONENT_TRISTATE_BUFFER);

    buffer->set_process_func([](Component *buffer) {
        if (buffer->read_pin(buffer->control_pin_index(0)) != VALUE_TRUE) {
            for (size_t pin = 0; pin < buffer->num_output_pins(); ++pin) {
                buffer->write_pin(buffer->output_pin_index(pin), VALUE_UNDEFINED);
            }
        } else {
            for (size_t pin = 0; pin < buffer->num_input_pins(); ++pin) {
                auto value = buffer->read_pin(buffer->input_pin_index(pin));
                buffer->write_pin(buffer->output_pin_index(pin), value);
            }
        }
    });

    return buffer;
}


///////////////////////////////////////////////////////////////////////////////
//
// AND gate
//

Component *AndGate(Circuit *circuit, size_t num_inputs) {
    assert(num_inputs >= 2);
    auto gate = circuit->create_component(num_inputs, 1, 0, COMPONENT_AND_GATE);

    gate->set_process_func([] (Component *gate) {
        gate->reset_bad_read_check();

        bool output = gate->read_pin_checked(0);
        for (auto idx = 1u; idx < gate->num_input_pins(); ++idx) {
            output &= gate->read_pin_checked(idx);
        }
        gate->write_pin_checked(gate->output_pin_index(0), output);
    }); 

    return gate;
}


///////////////////////////////////////////////////////////////////////////////
//
// OR gate
//

Component *OrGate(Circuit *circuit, size_t num_inputs) {
    assert(num_inputs >= 2);
    auto gate = circuit->create_component(num_inputs, 1, 0, COMPONENT_OR_GATE);

    gate->set_process_func([] (Component *gate) {
        gate->reset_bad_read_check();

        bool output = gate->read_pin_checked(0);
        for (auto idx = 1u; idx < gate->num_input_pins(); ++idx) {
            output |= gate->read_pin_checked(idx);
        }
        gate->write_pin_checked(gate->output_pin_index(0), output);
    }); 

    return gate;
}


///////////////////////////////////////////////////////////////////////////////
//
// NOT gate
//

Component *NotGate(Circuit *circuit) {
    auto gate = circuit->create_component(1, 1, 0, COMPONENT_NOT_GATE);

    gate->set_process_func([] (Component *gate) {
        gate->reset_bad_read_check();
        auto input = gate->read_pin_checked(0);
        gate->write_pin_checked(1, !input);
    });

    return gate;
}


///////////////////////////////////////////////////////////////////////////////
//
// NAND gate
//

Component *NandGate(Circuit *circuit, size_t num_inputs) {
    assert(num_inputs >= 2);
    auto gate = circuit->create_component(num_inputs, 1, 0, COMPONENT_NAND_GATE);

    gate->set_process_func([] (Component *gate) {
        gate->reset_bad_read_check();

        bool output = gate->read_pin_checked(0);
        for (auto idx = 1u; idx < gate->num_input_pins(); ++idx) {
            output &= gate->read_pin_checked(idx);
        }
        gate->write_pin_checked(gate->output_pin_index(0), !output);
    }); 

    return gate;
}

///////////////////////////////////////////////////////////////////////////////
//
// NOR gate
//

Component *NorGate(Circuit *circuit, size_t num_inputs) {
    assert(num_inputs >= 2);
    auto gate = circuit->create_component(num_inputs, 1, 0, COMPONENT_NOR_GATE);

    gate->set_process_func([] (Component *gate) {
        gate->reset_bad_read_check();

        bool output = gate->read_pin_checked(0);
        for (auto idx = 1u; idx < gate->num_input_pins(); ++idx) {
            output |= gate->read_pin_checked(idx);
        }
        gate->write_pin_checked(gate->output_pin_index(0), !output);
    }); 

    return gate;
}

///////////////////////////////////////////////////////////////////////////////
//
// XOR gate
//

Component *XorGate(Circuit *circuit) {
    auto gate = circuit->create_component(2, 1, 0, COMPONENT_XOR_GATE);

    gate->set_process_func([] (Component *gate) {
        gate->reset_bad_read_check();

        auto output = gate->read_pin_checked(0);
        output ^= gate->read_pin_checked(1);
        gate->write_pin_checked(2, output);
    });

    return gate;
}

///////////////////////////////////////////////////////////////////////////////
//
// XNOR gate
//

Component *XnorGate(Circuit *circuit) {
    auto gate = circuit->create_component(2, 1, 0, COMPONENT_XNOR_GATE);

    gate->set_process_func([] (Component *gate) {
        gate->reset_bad_read_check();

        auto output = gate->read_pin_checked(0);
        output ^= gate->read_pin_checked(1);
        gate->write_pin_checked(2, !output);
    });

    return gate;
}