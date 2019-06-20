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

    // circuits
    CircuitDescription *create_user_circuit(const char *name) {
        return m_user_library.create_circuit(name, this);
    }
    CircuitDescription *find_circuit(const char *name, CircuitLibrary *fallback_lib = nullptr);

    // reference libraries
    void load_reference_library(const char *name, const char *filename);
    void clear_reference_libraries();
    CircuitLibrary *library_by_name(const char *name);

    // folders
    void add_folder(const char *name, const char *path);
    size_t num_folders() const {return m_folders.size();}
    std::string folder_name(size_t folder_idx);
    std::string folder_path(size_t folder_idx);
    std::string full_file_path(const std::string &file);

private:
    typedef std::unordered_map<std::string, CircuitLibrary::uptr_t> library_lut_t;
    typedef std::unordered_map<std::string, std::string> folder_lut_t;

private:
    Simulator m_sim;
    CircuitLibrary  m_user_library;
    library_lut_t m_reference_libraries;

    std::vector<std::string> m_folders;
    folder_lut_t m_folder_lut;
};

} // namespace lsim

#endif // LSIM_LSIM_CONTEXT_H