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

    // circuits
    Circuit *create_circuit();
    Circuit *clone_circuit(Circuit *circuit);
    void add_circuit(std::unique_ptr<Circuit> circuit);
    void set_main_circuit(Circuit *main);

    // pins
    pin_t assign_pin(node_t connect_to_pin = PIN_UNDEFINED);
    void connect_pins(pin_t pin_a, pin_t pin_b);

    bool write_pin(pin_t pin, Value value);
    Value read_pin(pin_t pin) const;
    bool pin_changed_last_step(pin_t pin) const;

    node_t pin_node(pin_t pin);
    void pin_set_node(pin_t pin, node_t node);

    // nodes
    node_t assign_node();
    void release_node(node_t node_id);

    bool write_node(node_t node_id, Value value);
    Value read_node(node_t node_id) const;

    bool node_changed(node_t node_id) const;
    bool node_changed_last_step(node_t node_id) const;

    // values
    uint8_t read_nibble(std::vector<pin_t> pins) const;
    uint8_t read_byte(std::vector<pin_t> pins) const;

    // simulation
    void init();
    void step();
    void run_until_change(node_t node_id);
    void run_until_stable(size_t stable_ticks);

private:
    typedef std::vector<std::unique_ptr<Circuit>> circuit_container_t;
    typedef std::vector<Value> value_container_t;
    typedef std::vector<node_t> node_container_t;

    typedef uint64_t timestamp_t;
    typedef std::vector<timestamp_t> timestamp_container_t;

private:
    timestamp_t             m_time;

    circuit_container_t     m_circuits;
    Circuit *               m_main_circuit;

    node_container_t        m_pin_nodes;

    node_container_t                    m_free_nodes;
    std::array<value_container_t,2>     m_node_values;
    timestamp_container_t               m_node_write_time;          // timestamp when node was last written to
    std::array<timestamp_container_t,2> m_node_change_time;         // timestamp when node last changed value
    int                                 m_read_idx;
    int                                 m_write_idx;

};

#endif