// simulator.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "simulator.h"
#include "sim_functions.h"
#include "circuit_description.h"
#include "circuit_instance.h"

#include <cassert>
#include <algorithm>

namespace lsim {

///////////////////////////////////////////////////////////////////////////////
//
// SimComponent
//

SimComponent::SimComponent(Simulator *sim, Component *comp) :
        m_sim(sim),
        m_comp_desc(comp),
        m_read_bad(false),
        m_nested_circuit(nullptr) {
    
    size_t num_pins = comp->num_inputs() + comp->num_outputs() + comp->num_controls();
    m_output_start = comp->num_inputs();
    m_control_start = m_output_start + comp->num_outputs();
    for (size_t idx = 0; idx < num_pins; ++idx) {
        m_pins.push_back(sim->assign_pin(this, idx < m_output_start || idx >= m_control_start));
    }
    m_sim_input_changed_func = sim_function(comp->type());
    m_sim_independent_func = sim_independent_function(comp->type());
    m_sim_needed_func = sim_needed_function(comp->type());
}

void SimComponent::apply_initial_values() {
    auto initial_out = m_comp_desc->property_value("initial_output", VALUE_UNDEFINED);
    if (initial_out != VALUE_UNDEFINED) {
        for (size_t pin = m_output_start; pin < m_control_start; ++pin) {
            m_sim->pin_set_initial_value(m_pins[pin], initial_out);
        }
    }

    if (m_comp_desc->type() == COMPONENT_CONNECTOR_IN && 
        !m_user_values.empty() &&
        !m_comp_desc->property_value("tri_state", false)) {
        for (size_t pin = m_output_start; pin < m_control_start; ++pin) {
            m_user_values[pin] = VALUE_FALSE;
            m_sim->pin_set_initial_value(m_pins[pin], initial_out);
        }
    }
}

pin_t SimComponent::pin_by_index(size_t index) const {
    assert(index < m_pins.size());
    return m_pins[index];
}

pin_container_t SimComponent::input_pins() const {
    return pin_container_t(m_pins.begin(), m_pins.begin() + m_output_start);
}

pin_container_t SimComponent::output_pins() const {
    return pin_container_t(m_pins.begin() + m_output_start, m_pins.begin() + m_control_start);
}

pin_container_t SimComponent::control_pins() const {
    return pin_container_t(m_pins.begin() + m_control_start, m_pins.end());
}

Value SimComponent::read_pin(uint32_t index) const {
    assert(index < m_pins.size());
    return m_sim->read_pin(m_pins[index]);
}

void SimComponent::write_pin(uint32_t index, Value value) {
    assert(index < m_pins.size());
    // XXX: is the second test really necessary?
    if (value == VALUE_UNDEFINED && m_sim->pin_output_value(m_pins[index]) == value) {
        return;
    }
    m_sim->write_pin(m_pins[index], value);
}

bool SimComponent::read_pin_checked(uint32_t index) {
    assert(index < m_pins.size());
    auto value = m_sim->read_pin(m_pins[index]);
    m_read_bad |= (value != VALUE_TRUE && value != VALUE_FALSE);
    return static_cast<bool>(value);
}

void SimComponent::write_pin_checked(uint32_t index, bool value) {
    auto output = m_read_bad ? VALUE_ERROR : value;
    write_pin(index, static_cast<Value>(output));
}

void SimComponent::enable_user_values() {
    m_user_values.clear();
    m_user_values.resize(m_pins.size(), VALUE_UNDEFINED);
}

Value SimComponent::user_value(uint32_t index) const {
    if (index < m_user_values.size()) {
        return m_user_values[index];
    } else {
        return VALUE_UNDEFINED;
    }
}

void SimComponent::set_user_value(uint32_t index, Value value) {
    assert(index < m_pins.size());
    m_user_values[index] = value;
}

void SimComponent::set_nested_instance(std::unique_ptr<class CircuitInstance> instance) {
    m_nested_circuit = std::move(instance);
}

void SimComponent::sim_input_changed() {
    m_sim_input_changed_func(m_sim, this);
}

void SimComponent::sim_independent() {
    m_sim_independent_func(m_sim, this);
}


///////////////////////////////////////////////////////////////////////////////
//
// Simulator
//

Simulator::Simulator() :
    m_time(0) {
}

SimComponent *Simulator::create_component(Component *desc) {
    auto sim_comp = std::make_unique<SimComponent>(this, desc);
    auto result = sim_comp.get();
    m_components[desc->priority()].push_back(std::move(sim_comp));

    if (sim_has_setup_function(desc->type())) {
        m_init_components.push_back(result);       
    }

    if (sim_has_independent_function(desc->type())) {
        m_independent_components.push_back(result);
    }

    return result;
}

void Simulator::clear_components() {
    m_components[0].clear();
    m_components[1].clear();
    m_init_components.clear();
    m_independent_components.clear();
    clear_pins();
    clear_nodes();
}

pin_t Simulator::assign_pin(SimComponent *component, bool used_as_input) {
    auto result = m_pin_nodes.size();
    m_pin_nodes.push_back(assign_node(component, used_as_input));
    m_pin_values.push_back(VALUE_UNDEFINED);
    return result;
}

node_t Simulator::connect_pins(pin_t pin_a, pin_t pin_b) {
    assert(pin_a != pin_b);
    assert(pin_a < m_pin_nodes.size());
    assert(pin_b < m_pin_nodes.size());

    const auto node_a = m_pin_nodes[pin_a];
    const auto node_b = m_pin_nodes[pin_b];

    if (node_a == node_b) {
        // pins already connected to each other
        return node_a;
    }

    return merge_nodes(node_a, node_b);
}

void Simulator::clear_pins() {
    m_pin_nodes.clear();
    m_pin_values.clear();
}

void Simulator::pin_set_default(pin_t pin, Value value) {
    assert(pin < m_pin_nodes.size());

    auto node_id = m_pin_nodes[pin];
    node_set_default(node_id, value);
}

void Simulator::pin_set_initial_value(pin_t pin, Value value) {
    assert(pin < m_pin_nodes.size());

    auto node_id = m_pin_nodes[pin];
    m_pin_values[pin] = value;
    node_set_initial_value(node_id, value);
}

void Simulator::write_pin(pin_t pin, Value value) {
    assert(pin < m_pin_nodes.size());

    auto node_id = m_pin_nodes[pin];
    m_pin_values[pin] = value;
    write_node(node_id, value, pin);
}

Value Simulator::read_pin(pin_t pin) const {
    assert(pin < m_pin_nodes.size());

    auto node_id = m_pin_nodes[pin];
    return read_node(node_id);
}

Value Simulator::read_pin_current_step(pin_t pin) const {
    assert(pin < m_pin_nodes.size());

    auto node_id = m_pin_nodes[pin];
    return read_node_current_step(node_id);
}

bool Simulator::pin_changed_previous_step(pin_t pin) const {
    assert(pin < m_pin_nodes.size());

    auto node_id = m_pin_nodes[pin];
    return node_changed_previous_step(node_id);
}

Simulator::timestamp_t Simulator::pin_last_change_time(pin_t pin) const {
    assert(pin < m_pin_nodes.size());

    auto node_id = m_pin_nodes[pin];
    return node_last_change_time(node_id);
}

node_t Simulator::pin_node(pin_t pin) const {
    assert(pin < m_pin_nodes.size());
    return m_pin_nodes[pin];
}

Value Simulator::pin_output_value(pin_t pin) const {
    assert(pin < m_pin_nodes.size());
    return m_pin_values[pin];
}

void Simulator::pin_set_output_value(pin_t pin, Value value) {
    assert(pin < m_pin_nodes.size());
    m_pin_values[pin] = value;
}

node_t Simulator::assign_node(SimComponent *component, bool used_as_input) {
    if (!m_free_nodes.empty()) {
        auto id = m_free_nodes.back();
        m_free_nodes.pop_back();
        m_node_values_read[id] = VALUE_UNDEFINED;
        m_node_values_write[id] = VALUE_UNDEFINED;
        m_node_metadata[id].m_default = VALUE_UNDEFINED;
        m_node_metadata[id].m_dependents = {};
        m_node_write_time[id] = 0;
        m_node_change_time[id] = 0;
        if (used_as_input) {
            m_node_metadata[id].m_dependents.insert(component);
        }
        return id;
    }

    m_node_values_read.push_back(VALUE_UNDEFINED);
    m_node_values_write.push_back(VALUE_UNDEFINED);
    m_node_metadata.push_back(NodeMetadata());
    m_node_write_time.push_back(0);
    m_node_change_time.push_back(0);
    if (used_as_input) {
        m_node_metadata.back().m_dependents.insert(component);
    }

    return m_node_values_read.size() - 1;
}

void Simulator::release_node(node_t node_id) {
    assert(node_id < m_node_values_read.size());
    m_free_nodes.push_back(node_id);
}

void Simulator::clear_nodes() {
    m_free_nodes.clear();
    m_node_values_read.clear();
    m_node_values_write.clear();
    m_node_metadata.clear();
    m_dirty_nodes_read.clear();
    m_dirty_nodes_write.clear();
    m_node_write_time.clear();
    m_node_change_time.clear();
}

node_t Simulator::merge_nodes(node_t node_a, node_t node_b) {
    assert(node_a != node_b);
    release_node(node_b);
    std::replace(std::begin(m_pin_nodes), std::end(m_pin_nodes), node_b, node_a);
    for (const auto &comp : m_node_metadata[node_b].m_dependents) {
        m_node_metadata[node_a].m_dependents.insert(comp);
    }
    return node_a;
}

void Simulator::node_set_default(node_t node_id, Value value) {
    assert(node_id < m_node_metadata.size());
    m_node_metadata[node_id].m_default = value;
}

void Simulator::node_set_initial_value(node_t node_id, Value value) {
    assert(node_id < m_node_metadata.size());
    m_node_values_read[node_id] = value;
    m_node_values_write[node_id] = value;
    m_node_write_time[node_id] = m_time;
    m_node_change_time[node_id] = m_time;
}

void Simulator::write_node(node_t node_id, Value value, pin_t from_pin) {
    assert(node_id < m_node_values_write.size());

    m_dirty_nodes_write.insert(node_id);

    if (value == VALUE_UNDEFINED) {
        // remove from active pin list
        auto iter = std::find(m_node_metadata[node_id].m_active_pins.begin(), m_node_metadata[node_id].m_active_pins.end(), from_pin);
        if (iter != m_node_metadata[node_id].m_active_pins.end()) {
            m_node_metadata[node_id].m_active_pins.erase(iter);
        }
        return;
    }

    m_node_write_time[node_id] = m_time;
    m_node_values_write[node_id] = value;
    m_node_metadata[node_id].m_active_pins.insert(from_pin);
}

Value Simulator::read_node(node_t node_id) const {
    assert(node_id < m_node_values_read.size());
    return m_node_values_read[node_id];
}

Value Simulator::read_node_current_step(node_t node_id) const {
    assert(node_id < m_node_values_write.size());
    return m_node_values_write[node_id];
}

bool Simulator::node_changed_previous_step(node_t node_id) const {
    assert(node_id < m_node_change_time.size());
    return m_node_change_time[node_id] == m_time - 1;
}

Simulator::timestamp_t Simulator::node_last_change_time(node_t node_id) const {
    assert(node_id < m_node_change_time.size());
    return m_node_change_time[node_id];
}

bool Simulator::node_dirty(node_t node_id) const {
    assert(node_id < m_node_change_time.size());
    return m_dirty_nodes_read.find(node_id) != m_dirty_nodes_read.end();
}

void Simulator::init() {
    m_time = 1;

    std::fill(std::begin(m_node_values_read), std::end(m_node_values_read), VALUE_FALSE);
    std::fill(std::begin(m_node_values_write), std::end(m_node_values_write), VALUE_FALSE);
    std::fill(std::begin(m_node_write_time), std::end(m_node_write_time), 0);
    std::fill(std::begin(m_node_change_time), std::end(m_node_change_time), 0);

    for (auto &meta : m_node_metadata) {
        meta.m_default = VALUE_UNDEFINED;
    }

    // apply initial values
    for (const auto &prio_comps : m_components) {
        for (auto &comp : prio_comps) {
            comp->apply_initial_values();
        }
    }

    // run one time setup functions
    for (auto comp : m_init_components) {
        auto setup_func = sim_setup_function(comp->description()->type());
        setup_func(this, comp);
    }

    // mark all nodes as dirty for the first run
    for (node_t node = 0; node < m_node_values_read.size(); ++node) {
        m_dirty_nodes_read.insert(node);
    }
}

void Simulator::step() {
    component_set_t sim_components;

    m_time = m_time + 1;

    // >> build a unique list of components with changed input values
    for (auto node_id : m_dirty_nodes_read) {
        for (auto comp : m_node_metadata[node_id].m_dependents) {
            sim_components.insert(comp);
        }
    }

    // >> run simulation: changed inputs
    for (auto comp : sim_components) {
        comp->sim_input_changed();
    }

    // >> run simulation: independent components
    for (auto comp : m_independent_components) {
        comp->sim_independent();
    }

    // >> post-process the dirty nodes
    m_dirty_nodes_read.clear();
    postprocess_dirty_nodes();
}

void Simulator::run_until_stable(size_t stable_ticks) {
    bool stop = false;
    auto remaining = stable_ticks;

    while (!stop) {
        step();

        bool stable = std::none_of(std::begin(m_node_change_time), std::end(m_node_change_time), 
                            [=] (auto t) {return t == m_time;}
        );

        if (!stable) {
            remaining = stable_ticks;
        } else {
            stop = --remaining == 0;
        }
    }
}

void Simulator::postprocess_dirty_nodes() {

    for (auto node_id : m_dirty_nodes_write) {

        switch (m_node_metadata[node_id].m_active_pins.size()) {
            case 0 :        // no active writers: use default value (i.e. pull-up/down resistor)
                m_node_values_write[node_id] = m_node_metadata[node_id].m_default;
                m_node_write_time[node_id] = m_time;
                break;
            case 1 : {      // normal case - 1 active writer
                pin_t pin = *m_node_metadata[node_id].m_active_pins.begin();
                m_node_values_write[node_id] = m_pin_values[pin];
                m_node_write_time[node_id] = m_time;
                break;
            }
            default :       // multiple active writers
               m_node_values_write[node_id] = VALUE_ERROR;
               break;
        }

        if (m_node_values_read[node_id] != m_node_values_write[node_id]) {
            m_node_change_time[node_id] = m_time;
            m_node_values_read[node_id] = m_node_values_write[node_id];
            m_dirty_nodes_read.insert(node_id);
        }
    }

    m_dirty_nodes_write.clear();
}

} // namespace lsim