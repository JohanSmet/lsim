// circuit_library.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// a collection of circuits

#ifndef LSIM_CIRCUIT_LIBRARY_H
#define LSIM_CIRCUIT_LIBRARY_H

#include <vector>
#include <unordered_map>
#include <string>

#include "circuit.h"

class Simulator;

class CircuitLibrary {
public:
    CircuitLibrary(const char *name, Simulator *sim);
    CircuitLibrary(const CircuitLibrary &other) = delete;

    const char *name() const {return m_name.c_str();}

    // circuit management
    Circuit *create_circuit(const char *name);
    void delete_circuit(Circuit *circuit);
    size_t num_circuits() const {return m_circuits.size();}
    Circuit *circuit_by_idx(size_t idx) const;
    Circuit *circuit_by_name(const char *name) const;
    size_t circuit_idx(Circuit *circuit) const;
    void set_main_circuit(Circuit *circuit);
    Circuit *main_circuit() const {return m_main_circuit;}

private:
    typedef std::vector<Circuit::uptr_t>                circuit_container_t;
    typedef std::unordered_map<std::string, Circuit *>  circuit_map_t;

private:
    std::string             m_name;
    Simulator *             m_sim;
    circuit_container_t     m_circuits;
    circuit_map_t           m_circuit_lut;
    Circuit *               m_main_circuit;
};

#endif // LSIM_CIRCUIT_LIBRARY_H
