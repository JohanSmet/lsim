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

#define SIM_NEEDED_FUNC_BEGIN(type)                 \
    sim_register_needed_function(COMPONENT_##type,  \
        [](Simulator *sim, SimComponent *comp) -> bool {

#define SIM_FUNC_END   }); 

namespace lsim {

typedef std::function<void (Simulator *, SimComponent *comp)> simulation_func_t;
typedef std::function<bool (Simulator *, SimComponent *comp)> simulation_needed_func_t;

void sim_register_function(ComponentType type, simulation_func_t func);
void sim_register_needed_function(ComponentType type, simulation_needed_func_t func);
void sim_register_component_functions();

simulation_func_t sim_function(ComponentType type);
simulation_needed_func_t sim_needed_function(ComponentType type);

} // namespace lsim


#endif // LSIM_SIM_FUNCTIONS_H