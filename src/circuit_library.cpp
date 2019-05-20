// circuit_library.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// a collection of circuits

#include "circuit_library.h"
#include <algorithm>
#include <cassert>

CircuitLibrary::CircuitLibrary(const char *name, Simulator *sim) : 
        m_name(name),
        m_sim(sim),
        m_main_circuit(nullptr) {
}

Circuit *CircuitLibrary::create_circuit(const char *name) {
    assert(name);

    m_circuits.push_back(std::make_unique<Circuit>(m_sim, name));
    auto circuit =  m_circuits.back().get();
    m_circuit_lut[name] = circuit;

    if (!m_main_circuit) {
        m_main_circuit = circuit;
    }

    return circuit;
}

void CircuitLibrary::delete_circuit(Circuit *circuit) {
    assert (std::find_if(m_circuits.begin(), m_circuits.end(), [=](auto &o) {return o.get() == circuit;}) != m_circuits.end());
    m_circuits.erase(std::remove_if(m_circuits.begin(), m_circuits.end(), [&circuit](auto &o) {
        return o.get() == circuit;
    }));
}

void CircuitLibrary::swap_circuits(size_t idx_a, size_t idx_b) {
    assert(idx_a < m_circuits.size());
    assert(idx_b < m_circuits.size());

    std::swap(m_circuits[idx_a], m_circuits[idx_b]);
}

Circuit *CircuitLibrary::circuit_by_idx(size_t idx) const {
    assert(idx < num_circuits());
    return m_circuits[idx].get();
}

Circuit *CircuitLibrary::circuit_by_name(const char *name) const {
    assert(name);

    auto res = m_circuit_lut.find(name);
    if (res != m_circuit_lut.end()) {
        return res->second;
    } else {
        return nullptr;
    }
}

size_t CircuitLibrary::circuit_idx(Circuit *circuit) const {
    for (size_t i = 0; i < m_circuits.size(); ++i) {
        if (m_circuits[i].get() == circuit) {
            return i;
        }
    }

    return m_circuits.size();
}

void CircuitLibrary::set_main_circuit(Circuit *circuit) {
    assert (std::find_if(m_circuits.begin(), m_circuits.end(), [=](auto &other) {return other.get() == circuit;}) != m_circuits.end());
    m_main_circuit = circuit;
}