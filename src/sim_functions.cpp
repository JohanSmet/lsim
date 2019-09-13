// sim_functions.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// simulation functions for basic logic components

#include "sim_functions.h"
#include "simulator.h"

#include <unordered_map>
#include <algorithm>

namespace lsim {

// prototypes for registration functions defined in separate modules. (I guess somebody is too lazy to put them in a header?)
void sim_register_gate_functions(Simulator *);
void sim_register_various_functions(Simulator *);

void sim_register_component_functions(Simulator *sim) {
    sim_register_gate_functions(sim);
    sim_register_various_functions(sim);
}

} // namespace lsim