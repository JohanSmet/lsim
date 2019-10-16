// model_circuit_library.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// a collection of circuits

#ifndef LSIM_MODEL_CIRCUIT_LIBRARY_H
#define LSIM_MODEL_CIRCUIT_LIBRARY_H

#include <vector>
#include <unordered_map>
#include <string>

#include "model_circuit.h"

namespace lsim {

class LSimContext;

class ModelCircuitLibrary {
public:
    using uptr_t = std::unique_ptr<ModelCircuitLibrary>;
    using reference_container_t = std::vector<std::string>;

public:
    ModelCircuitLibrary(const char *name, const char *path= "");
    ModelCircuitLibrary(const ModelCircuitLibrary &other) = delete;

    const char *name() const {return m_name.c_str();}
    const char *path() const {return m_path.c_str();}

    const char *main_circuit_name() const {return m_main_circuit.c_str();}
    ModelCircuit *main_circuit() const {return circuit_by_name(m_main_circuit.c_str());}
    void change_main_circuit(const char *main) {m_main_circuit = main;}

    // circuit management
    ModelCircuit *create_circuit(const char *name, LSimContext *context);
    void delete_circuit(ModelCircuit *circuit);
    void rename_circuit(ModelCircuit *circuit, const char *name);
    void swap_circuits(size_t idx_a, size_t idx_b);
    size_t num_circuits() const {return m_circuits.size();}
    ModelCircuit *circuit_by_idx(size_t idx) const;
    ModelCircuit *circuit_by_name(const char *name) const;
    uint32_t circuit_idx(ModelCircuit *circuit) const;
    void clear_circuits();

    // references
    void add_reference(const char *name);
    void remove_reference(const char *name);
    void clear_references();
    const reference_container_t &references() const {return m_references;}

private:
    using circuit_container_t = std::vector<ModelCircuit::uptr_t>;
    using circuit_map_t = std::unordered_map<std::string, ModelCircuit *>;

private:
    std::string             m_name;
    std::string             m_path;

    circuit_container_t     m_circuits;
    circuit_map_t           m_circuit_lut;
    std::string             m_main_circuit;

    reference_container_t   m_references;
};

} // namespace lsim

#endif // LSIM_MODEL_CIRCUIT_LIBRARY_H
