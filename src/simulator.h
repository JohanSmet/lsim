// simulator.h - Johan Smet - BSD-3-Clause (see LICENSE)

#ifndef LSIM_SIMULATOR_H
#define LSIM_SIMULATOR_H

// includes
#include "sim_component.h"
#include "sim_functions.h"


#include <vector>
#include <array>

namespace lsim {

struct NodeMetadata {
    using component_set_t = std::set<SimComponent *>;
    using pin_set_t = std::set<pin_t>;

    // construction
    NodeMetadata() : m_default(VALUE_UNDEFINED),
					 m_time_dirty_write(0) {
    }

    // data
    Value               m_default;
    component_set_t     m_dependents;
	pin_container_t		m_pins;
    pin_set_t           m_active_pins;
	timestamp_t			m_time_dirty_write;
};

class Simulator {
public:
    Simulator();
    Simulator(const Simulator &) = delete;

    // components
    SimComponent *create_component(ModelComponent *desc);
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

    // simulation functions
    void register_sim_function(ComponentType comp_type, SimFuncType func_type, simulation_func_t func);
    bool component_has_function(ComponentType comp_type, SimFuncType func_type);

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
    using timestamp_container_t = std::vector<timestamp_t>;
    using component_container_t = std::vector<std::unique_ptr<SimComponent> >;
    using component_refs_t = std::vector<SimComponent *>;
    using node_metadata_container_t = std::vector<NodeMetadata>;
    using sim_func_container_t = std::vector<sim_component_functions_t>;

private:
    timestamp_t    m_time;									// current simulation timestamp

	// components
    component_container_t		m_components;				// all simulator components
	timestamp_container_t		m_input_changed;			// timestamp when component was last added to "to simulate" list
    component_refs_t            m_init_components;			// components with an init function
    component_refs_t            m_independent_components;	// components with an input independent update function
	component_refs_t			m_dirty_components;			// components with changed input values

	// pins
    node_container_t            m_pin_nodes;				// node assignment for each pin
    value_container_t           m_pin_values;				// last value written to a pin

	// nodes
    node_metadata_container_t m_node_metadata;				// assorted metadata
    node_container_t          m_free_nodes;					// list of node-ids that can be reused
    value_container_t         m_node_values_read;			// values of the nodes after the last simulation run
    value_container_t         m_node_values_write;			// values of the nodes in the current simulation run
    node_container_t          m_dirty_nodes_read;			// nodes that were changed in the last simulation run
    node_container_t          m_dirty_nodes_write;			// nodes that were changed in the current simulation run

    timestamp_container_t     m_node_write_time;			// timestamp when node was last written to
    timestamp_container_t     m_node_change_time;			// timestamp when node last changed value

    // simulation functions
    sim_func_container_t        m_sim_functions;
};

} // namespace lsim

#endif // LSIM_SIMULATOR_H