// lsim_context.h - Johan Smet - BSD-3-Clause (see LICENSE)

#ifndef LSIM_LSIM_CONTEXT_H
#define LSIM_LSIM_CONTEXT_H

#include "simulator.h"
#include "model_circuit_library.h"

namespace lsim {

class ModelCircuit;

class LSimContext {
public:
    LSimContext() : m_user_library("user") {
        sim_register_component_functions(&m_sim);
    };

    Simulator *sim() {return &m_sim;}
    ModelCircuitLibrary *user_library() {return &m_user_library;}

    // circuits
    ModelCircuit *create_user_circuit(const char *name) {
        return m_user_library.create_circuit(name, this);
    }
    ModelCircuit *find_circuit(const char *name, ModelCircuitLibrary *fallback_lib = nullptr);

    // reference libraries
    void load_reference_library(const char *name, const char *filename);
    void clear_reference_libraries();
    ModelCircuitLibrary *library_by_name(const char *name);

    // folders
    void add_folder(const char *name, const char *path);
    size_t num_folders() const {return m_folders.size();}
    std::string folder_name(size_t folder_idx);
    std::string folder_path(size_t folder_idx);
    std::string full_file_path(const std::string &file);
    std::string relative_file_path(const std::string &file);

private:
    using library_lut_t = std::unordered_map<std::string, ModelCircuitLibrary::uptr_t>;
    using folder_lut_t = std::unordered_map<std::string, std::string>;

private:
    Simulator m_sim;
    ModelCircuitLibrary  m_user_library;
    library_lut_t m_reference_libraries;

    std::vector<std::string> m_folders;
    folder_lut_t m_folder_lut;
};

} // namespace lsim

#endif // LSIM_LSIM_CONTEXT_H