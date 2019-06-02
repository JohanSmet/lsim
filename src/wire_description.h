// wire_description.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// describe the wires that connect components

#ifndef LSIM_WIRE_DESCRIPTION_H
#define LSIM_WIRE_DESCRIPTION_H

#include <memory>
#include "sim_types.h"

namespace lsim {

class Wire {
public:
    typedef std::unique_ptr<Wire>   uptr_t;

public:
    Wire(uint32_t id);
    uint32_t id() const {return m_id;}

    // pins
    void add_pin(pin_id_t pin);
    size_t num_pins() const {return m_pins.size();}
    pin_id_t pin(size_t index) const;

private:
    uint32_t m_id;
    pin_id_container_t m_pins;
};


} // namespace lsim


#endif // LSIM_WIRE_DESCRIPTION_H