// circuit_instance.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// instantiation of a circuit description

#ifndef LSIM_CIRCUIT_INSTANCE_H
#define LSIM_CIRCUIT_INSTANCE_H

#include "circuit_description.h"
#include "simulator.h"

namespace lsim {

class CircuitInstance {
public:
    CircuitInstance(Simulator *sim, CircuitDescription *circuit_desc);

    // instantiation
    SimComponent *add_component(Component *comp);
    node_t add_wire(Wire *wire);

    // read/write
    Value read_pin(pin_id_t pin_id);
    uint8_t read_nibble(uint32_t comp_id);
    uint8_t read_byte(uint32_t comp_id);

    void write_pin(pin_id_t pin_id, Value value);
    void write_output_pins(uint32_t comp_id, value_container_t values);
    void write_output_pins(uint32_t comp_id, uint64_t data);
    void write_output_pins(uint32_t comp_id, Value value);

private: 
    SimComponent *component_by_id(uint32_t comp_id);
    pin_t pin_from_pin_id(pin_id_t pin_id);

private:
    typedef std::unordered_map<uint32_t, SimComponent *> sim_component_lut_t;

private:
    CircuitDescription *    m_circuit_desc;
    Simulator *             m_sim;
    sim_component_lut_t     m_components;
};


}


#endif // LSIM_CIRCUIT_INSTANCE_H