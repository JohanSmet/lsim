// circuit.h - Johan Smet - BSD-3-Clause (see LICENSE)

#ifndef LSIM_CIRCUIT_H
#define LSIM_CIRCUIT_H

#include <vector>
#include <array>
#include <memory>
#include "basic.h"

class Component;

const node_t NOT_CONNECTED = (node_t) -1;

typedef std::vector<Value> value_container_t;
typedef std::vector<std::unique_ptr<Component>> component_container_t;
typedef uint64_t sim_timestamp_t;

class Circuit {
public:
    Circuit();

    pin_t create_pin(Component *component);
    void connect_pins(pin_t pin_a, pin_t pin_b);

    void write_value(pin_t pin, Value value);
    Value read_value(pin_t pin);
    Value read_value(pin_t pin, Value value_for_undefined);

    void register_component(std::unique_ptr<Component> component);

    void simulation_init();
    void simulation_tick();

private:
    node_t create_node();

private:
    std::vector<Component *>    m_pins;
    std::vector<node_t>         m_pin_nodes;

    node_t m_next_node_id;
    std::vector<node_t>         m_free_nodes;

    std::array<value_container_t, 2>  m_values;
    std::vector<sim_timestamp_t>      m_node_write_time;
    int m_read_idx;
    int m_write_idx;

    sim_timestamp_t             m_sim_time;
    component_container_t       m_components;

};



#endif // LSIM_CIRCUIT_H