// sim_various.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// simulation functions for various components

#include "sim_functions.h"
#include "simulator.h"
#include "circuit_description.h"

namespace lsim {

void sim_register_various_functions() {
    SIM_FUNC_BEGIN(CONSTANT)  {
        auto value = comp->description()->property("value")->value_as_lsim_value();
        comp->write_pin(0, value);
    } SIM_FUNC_END;

    SIM_NEEDED_FUNC_BEGIN(CONSTANT) {
        return true;
    } SIM_FUNC_END;

    SIM_FUNC_BEGIN(PULL_RESISTOR) {
        if (sim->read_pin_current_step(comp->pin_by_index(0)) == VALUE_UNDEFINED) {
            auto value = comp->description()->property("pull_to")->value_as_lsim_value();
            comp->write_pin(0, value);
        }
    } SIM_FUNC_END;

    SIM_NEEDED_FUNC_BEGIN(PULL_RESISTOR) {
        return true;
    } SIM_FUNC_END;

    SIM_NEEDED_FUNC_BEGIN(SUB_CIRCUIT) {
        return true;
    } SIM_FUNC_END;

    SIM_NEEDED_FUNC_BEGIN(TEXT) {
        return false;
    } SIM_FUNC_END;
}

} // namespace lsim