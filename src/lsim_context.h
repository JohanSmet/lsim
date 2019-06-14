// lsim_context.h - Johan Smet - BSD-3-Clause (see LICENSE)

#ifndef LSIM_LSIM_CONTEXT_H
#define LSIM_LSIM_CONTEXT_H

#include "simulator.h"
#include "circuit_library.h"

namespace lsim {

class CircuitDescription;

class LSimContext {
public:
    LSimContext() : m_user_library("user") {
        sim_register_component_functions();
    };

    Simulator *sim() {return &m_sim;}
    CircuitLibrary *user_library() {return &m_user_library;}

    CircuitDescription *create_user_circuit(const char *name) {
        return m_user_library.create_circuit(name, this);
    }
    CircuitDescription *find_circuit(const char *name);

    void load_reference_library(const char *name, const char *filename);
    void clear_reference_libraries();
    CircuitLibrary *library_by_name(const char *name);

private:
    typedef std::unordered_map<std::string, CircuitLibrary::uptr_t> library_lut_t;

private:
    Simulator m_sim;
    CircuitLibrary  m_user_library;
    library_lut_t m_reference_libraries;
};

} // namespace lsim

#endif // LSIM_LSIM_CONTEXT_H