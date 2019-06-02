// wire_description.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// describe the wires that connect components

#include "wire_description.h"
#include "circuit_description.h"

#include <cassert>

namespace lsim {

///////////////////////////////////////////////////////////////////////////////
//
// Wire
//

Wire::Wire(uint32_t id) : m_id(id) {

}

void Wire::add_pin(pin_id_t pin) {
    m_pins.push_back(pin);
}

pin_id_t Wire::pin(size_t index) const {
    assert(index < m_pins.size());
    return m_pins[index];
}

} // namespace lsim