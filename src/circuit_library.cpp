// circuit_library.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// a collection of circuits

#include "circuit_library.h"
#include "std_helper.h"
#include <cassert>

namespace lsim {

CircuitLibrary::CircuitLibrary(const char *name, const char *path) :
        m_name(name),
        m_path(path) {
}

CircuitDescription *CircuitLibrary::create_circuit(const char *name, LSimContext *context) {
    assert(name);

    m_circuits.push_back(std::make_unique<CircuitDescription>(name, context, this));
    auto circuit = m_circuits.back().get();
    m_circuit_lut[name] = circuit;

    return circuit;
}

void CircuitLibrary::delete_circuit(CircuitDescription *circuit) {
    assert (std::find_if(m_circuits.begin(), m_circuits.end(), [=](auto &o) {return o.get() == circuit;}) != m_circuits.end());
	remove_value(m_circuit_lut, circuit);
	remove_owner(m_circuits, circuit);
}

void CircuitLibrary::rename_circuit(CircuitDescription *circuit, const char *name) {
    assert(circuit);
    assert(name);

    // FIXME: check for duplicates
	remove_value(m_circuit_lut, circuit);
    m_circuit_lut[name] = circuit;

    circuit->change_name(name);
}

void CircuitLibrary::swap_circuits(size_t idx_a, size_t idx_b) {
    assert(idx_a < m_circuits.size());
    assert(idx_b < m_circuits.size());

    std::swap(m_circuits[idx_a], m_circuits[idx_b]);
}

CircuitDescription *CircuitLibrary::circuit_by_idx(size_t idx) const {
    assert(idx < num_circuits());
    return m_circuits[idx].get();
}

CircuitDescription *CircuitLibrary::circuit_by_name(const char *name) const {
    assert(name);

    auto res = m_circuit_lut.find(name);
    if (res != m_circuit_lut.end()) {
        return res->second;
    } else {
        return nullptr;
    }
}

uint32_t CircuitLibrary::circuit_idx(CircuitDescription *circuit) const {
    for (auto i = 0u; i < m_circuits.size(); ++i) {
        if (m_circuits[i].get() == circuit) {
            return i;
        }
    }

    return m_circuits.size();
}

void CircuitLibrary::clear_circuits() {
    m_circuits.clear();
    m_circuit_lut.clear();
}

void CircuitLibrary::add_reference(const char *name) {
    if (std::find(m_references.begin(), m_references.end(), name) != m_references.end()) {
        return;
    }

    m_references.push_back(name);
}

void CircuitLibrary::remove_reference(const char *name) {
	remove(m_references, string(name));
}

void CircuitLibrary::clear_references() {
    m_references.clear();
}

} // namespace lsim