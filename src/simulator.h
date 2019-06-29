// simulator.h - Johan Smet - BSD-3-Clause (see LICENSE)

#ifndef LSIM_SIMULATOR_H
#define LSIM_SIMULATOR_H

// includes
#include "sim_functions.h"
#include "sim_types.h"
#include "circuit_instance.h"

#include <vector>
#include <memory>
#include <array>

namespace lsim {

class Simulator;

class SimComponent {
public:
    typedef std::unique_ptr<SimComponent>   uptr_t;
public:
    SimComponent(Simulator *sim, Component *comp);
    Component *description() const {return m_comp_desc;}

    // pins
    pin_t pin_by_index(size_t index) const;
    size_t input_pin_index(size_t index) const {return index;}
    size_t output_pin_index(size_t index) const {return m_output_start + index;}
    size_t control_pin_index(size_t index) const {return m_control_start + index;}
    pin_container_t input_pins() const;
    pin_container_t output_pins() const;
    pin_container_t control_pins() const;
    size_t num_inputs() const {return m_output_start;}
    size_t num_outputs() const {return m_control_start - m_output_start;}
    size_t num_controls() const {return m_pins.size() - m_control_start;}

    // read/write_pin: read/write the value of the node the specified pin connects to
    Value read_pin(uint32_t index) const;
    void write_pin(uint32_t index, Value value);

    // read/write_checked: convenience functions that make it easy to check if 
    //      all the read nodes (since last bad-read reset) had a valid boolean
    //      value (not undefined or error)
    bool read_pin_checked(uint32_t index);
    void write_pin_checked(uint32_t index, bool value);
    void reset_bad_read_check() {m_read_bad = false;}

    // output_value: retrieve the value last written by this component to the specified pin
    Value output_value(uint32_t index) const;

    // nested circuits
    void set_nested_instance(std::unique_ptr<CircuitInstance> instance);
    CircuitInstance *nested_instance() const {return m_nested_circuit.get();}

    // simulation
    void tick();

private:
    Simulator *m_sim;
    Component *m_comp_desc;
    pin_container_t m_pins;
    value_container_t m_values;

    size_t m_output_start;
    size_t m_control_start;
    bool m_read_bad;

    simulation_func_t m_sim_func;
    simulation_needed_func_t m_sim_needed_func;

    std::unique_ptr<CircuitInstance>    m_nested_circuit;
};

class Simulator {
public:
    Simulator();
    Simulator(const Simulator &) = delete;

    // components
    SimComponent *create_component(Component *desc);
    void clear_components();

    // pins
    pin_t assign_pin();
    node_t connect_pins(pin_t pin_a, pin_t pin_b);
    void write_pin(pin_t pin, Value value);
    Value read_pin(pin_t pin) const;
    Value read_pin_current_step(pin_t pin) const;
    bool pin_changed_previous_step(pin_t pin) const;

    // nodes
    node_t assign_node();
    void release_node(node_t node_id);
    node_t merge_nodes(node_t node_a, node_t node_b);

    void write_node(node_t node_id, Value value);
    Value read_node(node_t node_id) const;
    Value read_node_current_step(node_t node_id) const;

    bool node_changed_previous_step(node_t node_id) const;

    // simulation
    void init();
    void step();
    void run_until_stable(size_t stable_ticks);

private:
    typedef uint64_t timestamp_t;

    typedef std::vector<timestamp_t> timestamp_container_t;
    typedef std::vector<SimComponent::uptr_t> component_container_t;
    typedef std::array<component_container_t, 2> component_prio_container_t;

private:
    timestamp_t    m_time;

    component_prio_container_t  m_components;
    node_container_t            m_pin_nodes;

    node_container_t          m_free_nodes;
    value_container_t         m_node_values_read;
    value_container_t         m_node_values_write;
    timestamp_container_t     m_node_write_time;          // timestamp when node was last written to
    timestamp_container_t     m_node_change_time;         // timestamp when node last changed value
};

} // namespace lsim

#endif // LSIM_SIMULATOR_H