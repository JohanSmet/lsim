// simulator.h - Johan Smet - BSD-3-Clause (see LICENSE)

#ifndef LSIM_SIMULATOR_H
#define LSIM_SIMULATOR_H

// includes
#include <vector>
#include <memory>
#include <array>

#include "basic.h"

// forward declarations
class Circuit;

// interface

class Simulator {
public:
    Simulator();

    // components
    void add_active_component(Component *comp);

    // pins
    pin_t assign_pin(node_t connect_to_pin = PIN_UNDEFINED);
    void release_pin(pin_t pin);
    void connect_pins(pin_t pin_a, pin_t pin_b);
    void disconnect_pin(pin_t);

    void write_pin(pin_t pin, Value value);
    Value read_pin(pin_t pin) const;
    Value read_pin_current_step(pin_t pin) const;
    bool pin_changed_last_step(pin_t pin) const;

    node_t pin_node(pin_t pin);
    void pin_set_node(pin_t pin, node_t node);

    // nodes
    node_t assign_node();
    void release_node(node_t node_id);
    node_t merge_nodes(node_t node_a, node_t node_b);
    Component::pin_container_t node_pins(node_t node) const;

    void write_node(node_t node_id, Value value);
    Value read_node(node_t node_id) const;
    Value read_node_current_step(node_t node_id) const;

    bool node_changed(node_t node_id) const;
    bool node_changed_last_step(node_t node_id) const;

    // values
    uint8_t read_nibble(std::vector<pin_t> pins) const;
    uint8_t read_byte(std::vector<pin_t> pins) const;

    // simulation
    void init(Circuit *circuit);
    void change_active_circuit(Circuit *circuit);
    Circuit *active_circuit() const {return m_active_circuit;}
    void step();
    void run_until_change(node_t node_id);
    void run_until_stable(size_t stable_ticks);

private:
    size_t num_pins_in_node(node_t node) const;

private:
    typedef std::vector<Value> value_container_t;
    typedef std::vector<node_t> node_container_t;
    typedef std::vector<Component *> component_container_t;
    typedef std::array<component_container_t, 2> component_prio_container_t;

    typedef uint64_t timestamp_t;
    typedef std::vector<timestamp_t> timestamp_container_t;

private:
    timestamp_t             m_time;

    Circuit *               m_active_circuit;

    node_container_t            m_pin_nodes;
    Component::pin_container_t  m_free_pins;

    node_container_t                    m_free_nodes;
    std::array<value_container_t,2>     m_node_values;
    timestamp_container_t               m_node_write_time;          // timestamp when node was last written to
    timestamp_container_t               m_node_change_time;         // timestamp when node last changed value
    int                                 m_read_idx;
    int                                 m_write_idx;

    component_prio_container_t          m_active_components;
};

#endif