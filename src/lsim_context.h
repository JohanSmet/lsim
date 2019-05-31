// lsim_context.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// every data structure to make lsim work

#ifndef LSIM_LSIM_CONTEXT_H
#define LSIM_LSIM_CONTEXT_H

#include "simulator.h"
#include "circuit_library.h"

namespace lsim {

class LSimContext {
public:
    LSimContext() : m_user_library("user") {
        sim_register_component_functions();
    };

    Simulator *sim() {return &m_sim;}
    CircuitLibrary *user_library() {return &m_user_library;}

private:
    Simulator m_sim;
    CircuitLibrary  m_user_library;

};

} // namespace lsim

#endif // LSIM_LSIM_CONTEXT_H