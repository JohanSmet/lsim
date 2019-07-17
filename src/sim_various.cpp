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

    SIM_INDEPENDENT_FUNC_BEGIN(CONSTANT) {
        auto value = comp->description()->property("value")->value_as_lsim_value();
        comp->write_pin(0, value);
    } SIM_FUNC_END;

    SIM_SETUP_FUNC_BEGIN(PULL_RESISTOR) {
        auto value = comp->description()->property("pull_to")->value_as_lsim_value();
        sim->pin_set_default(comp->pin_by_index(0), value);
    } SIM_FUNC_END;

    SIM_NEEDED_FUNC_BEGIN(SUB_CIRCUIT) {
        return true;
    } SIM_FUNC_END;

    SIM_NEEDED_FUNC_BEGIN(TEXT) {
        return false;
    } SIM_FUNC_END;

    SIM_NEEDED_FUNC_BEGIN(VIA) {
        return false;
    } SIM_FUNC_END;

    SIM_INDEPENDENT_FUNC_BEGIN(OSCILLATOR) {
        auto cycle_len = sim->current_time() - sim->pin_last_change_time(comp->pin_by_index(0));
        auto part = (comp->output_value(0) == VALUE_TRUE) ? "high_duration" : "low_duration";
        auto max = comp->description()->property_value(part, 1l);

        if (cycle_len >= max) {
            auto new_value = comp->output_value(0) == VALUE_TRUE ? VALUE_FALSE : VALUE_TRUE;
            comp->write_pin(comp->output_pin_index(0), new_value);
        }
    } SIM_FUNC_END;

    SIM_NEEDED_FUNC_BEGIN(7_SEGMENT_LED) {
        return false;
    } SIM_FUNC_END;
}

} // namespace lsim