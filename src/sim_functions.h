// sim_functions.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// simulation functions for basic logic components

#ifndef LSIM_SIM_FUNCTIONS_H
#define LSIM_SIM_FUNCTIONS_H

#include <functional>
#include <vector>

#include "sim_types.h"

#define SIM_FUNC_BEGIN(type)                    \
    sim_register_function(COMPONENT_##type,     \
        [](Simulator *sim, SimComponent *comp) {

#define SIM_SETUP_FUNC_BEGIN(type)                    \
    sim_register_setup_function(COMPONENT_##type,     \
        [](Simulator *sim, SimComponent *comp) {

#define SIM_INDEPENDENT_FUNC_BEGIN(type)                    \
    sim_register_independent_function(COMPONENT_##type,     \
        [](Simulator *sim, SimComponent *comp) {

#define SIM_FUNC_END   }); 

namespace lsim {

typedef std::function<void (Simulator *, SimComponent *comp)> simulation_func_t;
typedef std::function<bool (Simulator *, SimComponent *comp)> simulation_needed_func_t;

void sim_register_setup_function(ComponentType type, simulation_func_t func);
void sim_register_independent_function(ComponentType type, simulation_func_t func);
void sim_register_function(ComponentType type, simulation_func_t func);
void sim_register_component_functions();

bool sim_has_setup_function(ComponentType type);
simulation_func_t sim_setup_function(ComponentType type);
bool sim_has_independent_function(ComponentType type);
simulation_func_t sim_independent_function(ComponentType type);
simulation_func_t sim_function(ComponentType type);

} // namespace lsim


#endif // LSIM_SIM_FUNCTIONS_H