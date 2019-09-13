// sim_functions.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// simulation functions for basic logic components

#ifndef LSIM_SIM_FUNCTIONS_H
#define LSIM_SIM_FUNCTIONS_H

#include <array>
#include <functional>
#include <vector>

#include "sim_types.h"


namespace lsim {

using SimFuncType = unsigned int;
constexpr SimFuncType SIM_FUNCTION_SETUP = 0;
constexpr SimFuncType SIM_FUNCTION_INPUT_CHANGED = 1;
constexpr SimFuncType SIM_FUNCTION_INDEPENDENT = 2;

using simulation_func_t = std::function<void (Simulator *, SimComponent *comp)>;
using sim_component_functions_t = std::array<simulation_func_t, 3>;

#define SIM_SETUP_FUNC_BEGIN(type)                          \
    sim->register_sim_function(COMPONENT_##type,            \
        SIM_FUNCTION_SETUP,                                 \
        [](Simulator *sim, SimComponent *comp) {

#define SIM_INPUT_CHANGED_FUNC_BEGIN(type)                  \
    sim->register_sim_function(COMPONENT_##type,            \
        SIM_FUNCTION_INPUT_CHANGED,                         \
        [](Simulator *sim, SimComponent *comp) {

#define SIM_INDEPENDENT_FUNC_BEGIN(type)                    \
    sim->register_sim_function(COMPONENT_##type,            \
        SIM_FUNCTION_INDEPENDENT,                           \
        [](Simulator *sim, SimComponent *comp) {

#define SIM_FUNC_END   }); 

void sim_register_component_functions(Simulator *sim);

} // namespace lsim


#endif // LSIM_SIM_FUNCTIONS_H