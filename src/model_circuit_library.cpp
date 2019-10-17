// model_circuit_library.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// a collection of circuits

#include "model_circuit_library.h"
#include "std_helper.h"
#include <cassert>

namespace lsim {

ModelCircuitLibrary::ModelCircuitLibrary(const char *name, const char *path) :
        m_name(name),
        m_path(path) {
}

ModelCircuit *ModelCircuitLibrary::create_circuit(const char *name, LSimContext *context) {
    assert(name);

    m_circuits.push_back(std::make_unique<ModelCircuit>(name, context, this));
    auto circuit = m_circuits.back().get();
    m_circuit_lut[name] = circuit;

    return circuit;
}

void ModelCircuitLibrary::delete_circuit(ModelCircuit *circuit) {
    assert (std::find_if(m_circuits.begin(), m_circuits.end(), [=](auto &o) {return o.get() == circuit;}) != m_circuits.end());
	remove_value(m_circuit_lut, circuit);
	remove_owner(m_circuits, circuit);
}

void ModelCircuitLibrary::rename_circuit(ModelCircuit *circuit, const char *name) {
    assert(circuit);
    assert(name);

    // FIXME: check for duplicates
	remove_value(m_circuit_lut, circuit);
    m_circuit_lut[name] = circuit;

    circuit->change_name(name);
}

void ModelCircuitLibrary::swap_circuits(size_t idx_a, size_t idx_b) {
    assert(idx_a < m_circuits.size());
    assert(idx_b < m_circuits.size());

    std::swap(m_circuits[idx_a], m_circuits[idx_b]);
}

ModelCircuit *ModelCircuitLibrary::circuit_by_idx(size_t idx) const {
    assert(idx < num_circuits());
    return m_circuits[idx].get();
}

ModelCircuit *ModelCircuitLibrary::circuit_by_name(const char *name) const {
    assert(name);

    auto res = m_circuit_lut.find(name);
    if (res != m_circuit_lut.end()) {
        return res->second;
    }

    return nullptr;
}

uint32_t ModelCircuitLibrary::circuit_idx(ModelCircuit *circuit) const {
    for (auto i = 0u; i < m_circuits.size(); ++i) {
        if (m_circuits[i].get() == circuit) {
            return i;
        }
    }

    return m_circuits.size();
}

void ModelCircuitLibrary::clear_circuits() {
    m_circuits.clear();
    m_circuit_lut.clear();
}

void ModelCircuitLibrary::add_reference(const char *name) {
    if (std::find(m_references.begin(), m_references.end(), name) != m_references.end()) {
        return;
    }

    m_references.push_back(name);
}

void ModelCircuitLibrary::remove_reference(const char *name) {
	remove(m_references, string(name));
}

void ModelCircuitLibrary::clear_references() {
    m_references.clear();
}

} // namespace lsim