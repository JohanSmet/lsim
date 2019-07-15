// sim_functions.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// simulation functions for basic logic components

#include "sim_functions.h"
#include "simulator.h"

#include <unordered_map>
#include <algorithm>

namespace { 

using namespace lsim;

typedef std::unordered_map<lsim::ComponentType, simulation_func_t> function_lut_t;
typedef std::unordered_map<lsim::ComponentType, simulation_needed_func_t> sim_needed_lut_t;

function_lut_t g_sim_function;
function_lut_t g_sim_setup_functions;
sim_needed_lut_t g_sim_needed_functions;

static void dummy_sim_func(Simulator *sim, SimComponent *comp) {
    // do nothing
}

static bool default_sim_needed_func(Simulator *sim, SimComponent *comp) {

    auto input_pins = comp->input_pins();
    auto control_pins = comp->control_pins();

    bool input_changed = std::any_of(input_pins.begin(), input_pins.end(), 
                            [sim](auto node) {return sim->pin_changed_previous_step(node);});

    bool control_changed = std::any_of(control_pins.begin(), control_pins.end(), 
                            [sim](auto node) {return sim->pin_changed_previous_step(node);});

    return input_changed || control_changed;
}

} // unnamed namespace

namespace lsim {

void sim_register_setup_function(ComponentType type, simulation_func_t func) {
    g_sim_setup_functions[type] = func;
}

void sim_register_function(ComponentType type, simulation_func_t func) {
    g_sim_function[type] = func;
}

void sim_register_needed_function(ComponentType type, simulation_needed_func_t func) {
    g_sim_needed_functions[type] = func;
}

bool sim_has_setup_function(ComponentType type) {
    auto found = g_sim_setup_functions.find(type);
    return found != g_sim_setup_functions.end();
}

simulation_func_t sim_setup_function(ComponentType type) {
    auto found = g_sim_setup_functions.find(type);
    if (found != g_sim_setup_functions.end()) {
        return found->second;
    } else {
        return dummy_sim_func;
    }
}

simulation_func_t sim_function(ComponentType type) {
    auto found = g_sim_function.find(type);
    if (found != g_sim_function.end()) {
        return found->second;
    } else {
        return dummy_sim_func;
    }
}

simulation_needed_func_t sim_needed_function(ComponentType type) {
    auto found = g_sim_needed_functions.find(type);
    if (found != g_sim_needed_functions.end()) {
        return found->second;
    } else {
        return default_sim_needed_func;
    }
}

void sim_register_component_functions() {
    void sim_register_gate_functions();
    void sim_register_various_functions();

    sim_register_gate_functions();
    sim_register_various_functions();
}

} // namespace lsim