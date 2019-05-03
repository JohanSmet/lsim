// circuit_library.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// a collection of circuits

#include "circuit_library.h"
#include <cassert>

CircuitLibrary::CircuitLibrary(const char *name, Simulator *sim) : 
        m_name(name),
        m_sim(sim) {
}

Circuit *CircuitLibrary::create_circuit(const char *name) {
    assert(name);

    m_circuits.push_back(std::make_unique<Circuit>(m_sim, name));
    auto circuit =  m_circuits.back().get();
    m_circuit_lut[name] = circuit;
    return circuit;
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
    assert(circuit);

    for (size_t i = 0; i < m_circuits.size(); ++i) {
        if (m_circuits[i].get() == circuit) {
            return i;
        }
    }

    return m_circuits.size();
}