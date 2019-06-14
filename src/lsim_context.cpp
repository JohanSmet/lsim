// lsim_context.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "lsim_context.h"
#include "serialize.h"

namespace lsim {

void LSimContext::load_reference_library(const char *name, const char *filename) {
    if (m_reference_libraries.find(name) != m_reference_libraries.end()) {
        return;
    }

    auto lib = std::make_unique<CircuitLibrary>(name, filename);
    if (!deserialize_library(this, lib.get(), filename)) {
        lib = nullptr;
        return;
    }

    m_reference_libraries[name] = std::move(lib);
}

void LSimContext::clear_reference_libraries() {
    m_reference_libraries.clear();
}

CircuitDescription *LSimContext::find_circuit(const char *name) {
    std::string qualified = name;
    auto separator = qualified.find_first_of('.');

    if (separator == std::string::npos) {
        return m_user_library.circuit_by_name(name);
    } else {
        auto lib_name = qualified.substr(0, separator);
        auto circuit_name = qualified.substr(separator+1);

        auto found = m_reference_libraries.find(lib_name);
        if (found != m_reference_libraries.end()) {
            return found->second->circuit_by_name(circuit_name.c_str());
        }
    }

    return nullptr;
}

CircuitLibrary *LSimContext::library_by_name(const char *name) {
    std::string sname = name;
    if (sname == "user") {
        return &m_user_library;
    }

    auto found = m_reference_libraries.find(name);
    if (found != m_reference_libraries.end()) {
        return found->second.get();
    } else {
        return nullptr;
    }
}

} // namespace lsim