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
#include <set>

namespace lsim {

class Simulator;

class SimComponent {
public:
    typedef std::unique_ptr<SimComponent>   uptr_t;
public:
    SimComponent(Simulator *sim, Component *comp, uint32_t id);
    Component *description() const {return m_comp_desc;}
	uint32_t id() const {return m_id;}

    void apply_initial_values();

    // pins
    pin_t pin_by_index(uint32_t index) const;
    uint32_t input_pin_index(uint32_t index) const {return index;}
    uint32_t output_pin_index(uint32_t index) const {return m_output_start + index;}
    uint32_t control_pin_index(uint32_t index) const {return m_control_start + index;}
    const pin_container_t &pins() const {return m_pins;}
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

    // user_values: input from outside the circuit
    void enable_user_values();
    Value user_value(uint32_t index) const;
    void set_user_value(uint32_t index, Value value);
    bool user_values_enabled() const {return !m_user_values.empty();}

    // nested circuits
    void set_nested_instance(std::unique_ptr<CircuitInstance> instance);
    CircuitInstance *nested_instance() const {return m_nested_circuit.get();}

    // simulation
    void sim_input_changed();
    void sim_independent();

    // extra-data: component specific data structure
    void set_extra_data_size(size_t size) {m_extra_data.resize(size);};
    uint8_t *extra_data() {return m_extra_data.data();}


private:
    Simulator *m_sim;
    Component *m_comp_desc;
	uint32_t m_id;

    pin_container_t m_pins;
    value_container_t m_user_values;
    std::vector<uint8_t> m_extra_data;

    uint32_t m_output_start;
    uint32_t m_control_start;
    bool m_read_bad;

    simulation_func_t m_sim_input_changed_func;
    simulation_func_t m_sim_independent_func;

    std::unique_ptr<CircuitInstance>    m_nested_circuit;
};

struct NodeMetadata {
    typedef std::set<SimComponent *> component_set_t;
    typedef std::set<pin_t> pin_set_t;

    // construction
    NodeMetadata() : m_default(VALUE_UNDEFINED) {
    }

    // data
    Value               m_default;
    component_set_t     m_dependents;
    pin_set_t           m_active_pins;
};

class Simulator {
public:
    Simulator();
    Simulator(const Simulator &) = delete;

    // components
    SimComponent *create_component(Component *desc);
    void clear_components();

    // pins
    pin_t assign_pin(SimComponent *component, bool used_as_input);
    node_t connect_pins(pin_t pin_a, pin_t pin_b);
    void clear_pins();
    void pin_set_default(pin_t pin, Value value);
    void pin_set_initial_value(pin_t pin, Value value);
    void write_pin(pin_t pin, Value value);
    Value read_pin(pin_t pin) const;
    Value read_pin_current_step(pin_t pin) const;
    bool pin_changed_previous_step(pin_t pin) const;
    timestamp_t pin_last_change_time(pin_t pin) const;

    node_t pin_node(pin_t pin) const;
    Value pin_output_value(pin_t pin) const;
    void pin_set_output_value(pin_t pin, Value value);

    // nodes
    node_t assign_node(SimComponent *component, bool used_as_input);
    void release_node(node_t node_id);
    node_t merge_nodes(node_t node_a, node_t node_b);
    void clear_nodes();

    void node_set_default(node_t node_id, Value value);
    void node_set_initial_value(node_t node_id, Value value);

    void write_node(node_t node_id, Value value, pin_t from_pin);
    Value read_node(node_t node_id) const;
    Value read_node_current_step(node_t node_id) const;

    bool node_changed_previous_step(node_t node_id) const;
    timestamp_t node_last_change_time(node_t node_id) const;

    bool node_dirty(node_t node_id) const;

    // simulation
    void init();
    void step();
    void run_until_stable(size_t stable_ticks);
    timestamp_t current_time() const {return m_time;}

    void activate_independent_simulation_func(SimComponent *comp);
    void deactivate_independent_simulation_func(SimComponent *comp);

private:
    void postprocess_dirty_nodes();

private:
    typedef std::vector<timestamp_t> timestamp_container_t;
    typedef std::vector<SimComponent::uptr_t> component_container_t;
    typedef std::vector<SimComponent *> component_refs_t;
    typedef std::set<node_t> node_set_t;
    typedef std::vector<NodeMetadata> node_metadata_container_t;

private:
    timestamp_t    m_time;

    component_container_t		m_components;				// all simulator components
	timestamp_container_t		m_input_changed;			// timestamp when component was last added to "to simulate" list
    component_refs_t            m_init_components;			// components with an init function
    component_refs_t            m_independent_components;	// components with an input independent update function
	component_refs_t			m_dirty_components;			// components with changed input values

    node_container_t            m_pin_nodes;
    value_container_t           m_pin_values;

    node_metadata_container_t m_node_metadata;
    node_container_t          m_free_nodes;
    value_container_t         m_node_values_read;
    value_container_t         m_node_values_write;
    node_set_t                m_dirty_nodes_read;
    node_set_t                m_dirty_nodes_write;

    timestamp_container_t     m_node_write_time;          // timestamp when node was last written to
    timestamp_container_t     m_node_change_time;         // timestamp when node last changed value
};

} // namespace lsim

#endif // LSIM_SIMULATOR_H