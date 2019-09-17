// sim_various.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// simulation functions for various components

#include "sim_functions.h"
#include "simulator.h"
#include "model_circuit.h"

namespace lsim {

void sim_register_various_functions(Simulator *sim) {
    SIM_SETUP_FUNC_BEGIN(CONNECTOR_IN) {
        if (!comp->user_values_enabled()) {
            return;
        }
        for (auto pin = 0u; pin < comp->num_outputs(); ++pin) {
            auto user_value = comp->user_value(comp->output_pin_index(pin));
            if (user_value != VALUE_UNDEFINED) {
                sim->pin_set_initial_value(comp->pin_by_index(pin), user_value);
                sim->pin_set_output_value(comp->pin_by_index(pin), user_value);
            }
        }

        // disable the independent simulation function until the user interacts with the connector
        sim->deactivate_independent_simulation_func(comp);
    } SIM_FUNC_END;

    SIM_INDEPENDENT_FUNC_BEGIN(CONNECTOR_IN) {
        for (auto pin = 0u; pin < comp->num_outputs(); ++pin) {
            auto last_value = sim->pin_output_value(comp->pin_by_index(comp->output_pin_index(pin)));
            auto user_value = comp->user_value(comp->output_pin_index(pin));
            if (last_value != user_value || user_value != VALUE_UNDEFINED) {
                comp->write_pin(pin, user_value);
            }
        }

        sim->deactivate_independent_simulation_func(comp);
    } SIM_FUNC_END;

    SIM_SETUP_FUNC_BEGIN(CONSTANT)  {
        auto value = comp->description()->property("value")->value_as_lsim_value();
        sim->pin_set_initial_value(comp->pin_by_index(0), value);
    } SIM_FUNC_END;

    SIM_SETUP_FUNC_BEGIN(PULL_RESISTOR) {
        auto value = comp->description()->property("pull_to")->value_as_lsim_value();
        sim->pin_set_default(comp->pin_by_index(0), value);
        sim->pin_set_initial_value(comp->pin_by_index(0), value);
    } SIM_FUNC_END;

    SIM_SETUP_FUNC_BEGIN(OSCILLATOR) {
        comp->set_extra_data_size(sizeof(ExtraDataOscillator));
        auto *extra = reinterpret_cast<ExtraDataOscillator *>(comp->extra_data());

        auto value = sim->pin_output_value(comp->pin_by_index(0));

        extra->m_duration[0] = comp->description()->property_value("low_duration", static_cast<int64_t>(1));
        extra->m_duration[1] = comp->description()->property_value("high_duration", static_cast<int64_t>(1));
        extra->m_next_change = sim->current_time() + extra->m_duration[value];
    } SIM_FUNC_END;

    SIM_INDEPENDENT_FUNC_BEGIN(OSCILLATOR) {
        auto *extra = reinterpret_cast<ExtraDataOscillator *>(comp->extra_data());
        if (sim->current_time() >= extra->m_next_change) {
            auto cur_value = sim->pin_output_value(comp->pin_by_index(0));
            auto new_value = cur_value == VALUE_TRUE ? VALUE_FALSE : VALUE_TRUE;
            
            extra->m_next_change = sim->current_time() + extra->m_duration[new_value];
            comp->write_pin(comp->output_pin_index(0), new_value);
        }
    } SIM_FUNC_END;

    SIM_SETUP_FUNC_BEGIN(7_SEGMENT_LED) {
        comp->set_extra_data_size(sizeof(ExtraData7SegmentLED));
    } SIM_FUNC_END;

    SIM_INDEPENDENT_FUNC_BEGIN(7_SEGMENT_LED) {
        auto *extra = reinterpret_cast<ExtraData7SegmentLED *>(comp->extra_data());
        auto led_on = comp->read_pin(comp->control_pin_index(0));

        if (led_on == VALUE_TRUE) {
            for (auto pin_idx = 0u; pin_idx < comp->num_inputs(); ++pin_idx) {
                extra->m_samples[pin_idx] += comp->read_pin(comp->input_pin_index(pin_idx)) == VALUE_TRUE ? 1: 0;
            }
        }
        extra->m_num_samples += 1;
    } SIM_FUNC_END;
}

} // namespace lsim