// circuit_library.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// a collection of circuits

#ifndef LSIM_CIRCUIT_LIBRARY_H
#define LSIM_CIRCUIT_LIBRARY_H

#include <vector>
#include <unordered_map>
#include <string>

#include "circuit_description.h"

namespace lsim {

class LSimContext;

class CircuitLibrary {
public:
    typedef std::unique_ptr<CircuitLibrary> uptr_t;
    typedef std::vector<std::string> reference_container_t;

public:
    CircuitLibrary(const char *name, const char *path= "");
    CircuitLibrary(const CircuitLibrary &other) = delete;

    const char *name() const {return m_name.c_str();}
    const char *path() const {return m_path.c_str();}

    const char *main_circuit_name() const {return m_main_circuit.c_str();}
    CircuitDescription *main_circuit() const {return circuit_by_name(m_main_circuit.c_str());}
    void change_main_circuit(const char *main) {m_main_circuit = main;}

    // circuit management
    CircuitDescription *create_circuit(const char *name, LSimContext *context);
    void delete_circuit(CircuitDescription *circuit);
    void rename_circuit(CircuitDescription *circuit, const char *name);
    void swap_circuits(size_t idx_a, size_t idx_b);
    size_t num_circuits() const {return m_circuits.size();}
    CircuitDescription *circuit_by_idx(size_t idx) const;
    CircuitDescription *circuit_by_name(const char *name) const;
    size_t circuit_idx(CircuitDescription *circuit) const;
    void clear_circuits();

    // references
    void add_reference(const char *name);
    void remove_reference(const char *name);
    void clear_references();
    const reference_container_t &references() const {return m_references;}

private:
    void remove_circuit_from_lut(CircuitDescription *circuit);

private:
    typedef std::vector<CircuitDescription::uptr_t>     circuit_container_t;
    typedef std::unordered_map<std::string, CircuitDescription *>  circuit_map_t;

private:
    std::string             m_name;
    std::string             m_path;

    circuit_container_t     m_circuits;
    circuit_map_t           m_circuit_lut;
    std::string             m_main_circuit;

    reference_container_t   m_references;
};

} // namespace lsim

#endif // LSIM_CIRCUIT_LIBRARY_H
