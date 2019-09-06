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

function_lut_t g_sim_function;
function_lut_t g_sim_setup_functions;
function_lut_t g_sim_independent_functions;

static void dummy_sim_func(Simulator *sim, SimComponent *comp) {
    // do nothing
}

} // unnamed namespace

namespace lsim {

// prototypes for registration functions defined in separate modules. (I guess somebody is too lazy to put them in a header?)
void sim_register_gate_functions();
void sim_register_various_functions();

void sim_register_setup_function(ComponentType type, simulation_func_t func) {
    g_sim_setup_functions[type] = func;
}

void sim_register_independent_function(ComponentType type, simulation_func_t func) {
    g_sim_independent_functions[type] = func;
}

void sim_register_function(ComponentType type, simulation_func_t func) {
    g_sim_function[type] = func;
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

bool sim_has_independent_function(ComponentType type) {
    auto found = g_sim_independent_functions.find(type);
    return found != g_sim_independent_functions.end();
}

simulation_func_t sim_independent_function(ComponentType type) {
    auto found = g_sim_independent_functions.find(type);
    if (found != g_sim_independent_functions.end()) {
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


void sim_register_component_functions() {
    sim_register_gate_functions();
    sim_register_various_functions();
}

} // namespace lsim