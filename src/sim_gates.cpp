// sim_gates.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// basic logic gates

#include "sim_functions.h"
#include "simulator.h"

namespace lsim {

void sim_register_gate_functions(Simulator *sim) {

    SIM_INPUT_CHANGED_FUNC_BEGIN(BUFFER) {
        for (auto pin = 0u; pin < comp->num_inputs(); ++pin) {
            auto value = comp->read_pin(comp->input_pin_index(pin));
            comp->write_pin(comp->output_pin_index(pin), value);
        }
    } SIM_FUNC_END

    SIM_INPUT_CHANGED_FUNC_BEGIN(TRISTATE_BUFFER) {
         if (comp->read_pin(comp->control_pin_index(0)) != VALUE_TRUE) {
            for (auto pin = 0u; pin < comp->num_outputs(); ++pin) {
                comp->write_pin(comp->output_pin_index(pin), VALUE_UNDEFINED);
            }
        } else {
            for (auto pin = 0u; pin < comp->num_inputs(); ++pin) {
                auto value = comp->read_pin(comp->input_pin_index(pin));
                comp->write_pin(comp->output_pin_index(pin), value);
            }
        }
    } SIM_FUNC_END

    SIM_INPUT_CHANGED_FUNC_BEGIN(AND_GATE) {
        comp->reset_bad_read_check();

        bool output = comp->read_pin_checked(0);
        for (auto idx = 1u; idx < comp->num_inputs(); ++idx) {
            output &= comp->read_pin_checked(idx);
        }
        comp->write_pin_checked(comp->output_pin_index(0), output);
    } SIM_FUNC_END

    SIM_INPUT_CHANGED_FUNC_BEGIN(OR_GATE) {
        comp->reset_bad_read_check();

        bool output = comp->read_pin_checked(0);
        for (auto idx = 1u; idx < comp->num_inputs(); ++idx) {
            output |= comp->read_pin_checked(idx);
        }
        comp->write_pin_checked(comp->output_pin_index(0), output);
    } SIM_FUNC_END

    SIM_INPUT_CHANGED_FUNC_BEGIN(NOT_GATE) {
        comp->reset_bad_read_check();
        auto input = comp->read_pin_checked(0);
        comp->write_pin_checked(1, !input);
    } SIM_FUNC_END

    SIM_INPUT_CHANGED_FUNC_BEGIN(NAND_GATE) {
        comp->reset_bad_read_check();

        bool output = comp->read_pin_checked(0);
        for (auto idx = 1u; idx < comp->num_inputs(); ++idx) {
            output &= comp->read_pin_checked(idx);
        }
        comp->write_pin_checked(comp->output_pin_index(0), !output);
    } SIM_FUNC_END

    SIM_INPUT_CHANGED_FUNC_BEGIN(NOR_GATE) {
        comp->reset_bad_read_check();

        bool output = comp->read_pin_checked(0);
        for (auto idx = 1u; idx < comp->num_inputs(); ++idx) {
            output |= comp->read_pin_checked(idx);
        }
        comp->write_pin_checked(comp->output_pin_index(0), !output);
    } SIM_FUNC_END

    SIM_INPUT_CHANGED_FUNC_BEGIN(XOR_GATE) {
        comp->reset_bad_read_check();

        auto output = comp->read_pin_checked(0);
        output ^= static_cast<int>(comp->read_pin_checked(1));
        comp->write_pin_checked(2, output);
    } SIM_FUNC_END

    SIM_INPUT_CHANGED_FUNC_BEGIN(XNOR_GATE) {
        comp->reset_bad_read_check();

        auto output = comp->read_pin_checked(0);
        output ^= static_cast<int>(comp->read_pin_checked(1));
        comp->write_pin_checked(2, !output);
    } SIM_FUNC_END
}


} // namespace lsim