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
    SimComponent *component_by_id(uint32_t comp_id);

    // name
    void build_name(uint32_t component_id);
    const char *name() const {return m_name.c_str();}

    // read/write
    Value read_pin(pin_id_t pin_id);
    uint8_t read_nibble(uint32_t comp_id);
    uint8_t read_nibble(const pin_id_container_t &pins);
    uint8_t read_byte(uint32_t comp_id);
    uint8_t read_byte(const pin_id_container_t &pins);

    void write_pin(pin_id_t pin_id, Value value);
    void write_output_pins(uint32_t comp_id, value_container_t values);
    void write_output_pins(uint32_t comp_id, uint64_t data);
    void write_output_pins(uint32_t comp_id, Value value);

    void write_nibble(const pin_id_container_t &pins, uint8_t nibble);
    void write_byte(const pin_id_container_t &pins, uint8_t byte);

private: 
    pin_t pin_from_pin_id(pin_id_t pin_id);

private:
    typedef std::unordered_map<uint32_t, SimComponent *> sim_component_lut_t;

private:
    CircuitDescription *    m_circuit_desc;
    Simulator *             m_sim;
    sim_component_lut_t     m_components;
    std::string             m_name;
};


}


#endif // LSIM_CIRCUIT_INSTANCE_H