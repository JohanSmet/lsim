// lsim_context.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "lsim_context.h"
#include "serialize.h"

#include <cassert>

namespace lsim {

void LSimContext::load_reference_library(const char *name, const char *filename) {
    if (m_reference_libraries.find(name) != m_reference_libraries.end()) {
        return;
    }

    auto lib = std::make_unique<ModelCircuitLibrary>(name, filename);
    if (!deserialize_library(this, lib.get(), full_file_path(filename).c_str())) {
        lib = nullptr;
        return;
    }

    m_reference_libraries[name] = std::move(lib);
}

void LSimContext::clear_reference_libraries() {
    m_reference_libraries.clear();
}

ModelCircuit *LSimContext::find_circuit(const char *name, ModelCircuitLibrary *fallback_lib) {
    std::string qualified = name;
    auto separator = qualified.find_first_of('.');

    if (separator == std::string::npos) {
        ModelCircuit *result = nullptr;
        if (fallback_lib != nullptr) {
            result = fallback_lib->circuit_by_name(name);
        }
        if (result == nullptr) {
            result = m_user_library.circuit_by_name(name);
        }
        return result;
    }

    auto lib_name = qualified.substr(0, separator);
    auto circuit_name = qualified.substr(separator+1);

    auto found = m_reference_libraries.find(lib_name);
    if (found != m_reference_libraries.end()) {
        return found->second->circuit_by_name(circuit_name.c_str());
    }

    return nullptr;
}

ModelCircuitLibrary *LSimContext::library_by_name(const char *name) {
    std::string sname = name;
    if (sname == "user") {
        return &m_user_library;
    }

    auto found = m_reference_libraries.find(name);
    if (found != m_reference_libraries.end()) {
        return found->second.get();
    }

    return nullptr;
}

void LSimContext::add_folder(const char *name, const char *path) {
    m_folders.emplace_back(name);
    m_folder_lut[name] = path;
}

std::string LSimContext::folder_name(size_t folder_idx) {
    assert(folder_idx < m_folders.size());
    return m_folders[folder_idx];
}

std::string LSimContext::folder_path(size_t folder_idx) {
    assert(folder_idx < m_folders.size());
    auto found = m_folder_lut.find(folder_name(folder_idx));
    if (found != m_folder_lut.end()) {
        return found->second;
    } 
    return "";
}

std::string LSimContext::full_file_path(const std::string &file) {
    for (const auto &folder : m_folder_lut) {
        const auto &name = folder.first;
        const auto &path = folder.second;

        if (file.find_first_of(name) == 0) {
            auto result = path + file.substr(name.size());
            return result;
        }
    }

    return file;
}

std::string LSimContext::relative_file_path(const std::string &file) {
    for (const auto &folder : m_folder_lut) {
        const auto &name = folder.first;
        const auto &path = folder.second;

        if (file.find_first_of(path) == 0) {
            auto result = name + file.substr(path.size());
            return result;
        }
    }

    return file;
}

} // namespace lsim