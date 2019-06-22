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
    for (size_t idx = 0; idx < num_pins; ++idx) {
        m_pins.push_back(sim->assign_pin());
        m_values.push_back(VALUE_UNDEFINED);
    }
    m_output_start = comp->num_inputs();
    m_control_start = m_output_start + comp->num_outputs();
    m_sim_func = sim_function(comp->type());
    m_sim_needed_func = sim_needed_function(comp->type());

    auto initial_out = comp->property_value("initial_output", VALUE_UNDEFINED);
    if (initial_out != VALUE_UNDEFINED) {
        for (size_t pin = m_output_start; pin < m_control_start; ++pin) {
            m_values[pin] = initial_out;
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
    m_values[index] = value;
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

Value SimComponent::output_value(uint32_t index) const {
    assert(index < m_pins.size());
    return m_values[index];
}

void SimComponent::set_nested_instance(std::unique_ptr<class CircuitInstance> instance) {
    m_nested_circuit = std::move(instance);
}

void SimComponent::tick() {
    if (m_sim_needed_func(m_sim, this)) {
        m_sim_func(m_sim, this);
    } else {
        for (size_t i = 0; i < m_pins.size(); ++i) {
            if (m_values[i] != VALUE_UNDEFINED) {
                m_sim->write_pin(m_pins[i], m_values[i]);
            }
        }
    }
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
    return result;
}

pin_t Simulator::assign_pin() {
    auto result = m_pin_nodes.size();
    m_pin_nodes.push_back(assign_node());
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

void Simulator::write_pin(pin_t pin, Value value) {
    assert(pin < m_pin_nodes.size());

    auto node_id = m_pin_nodes[pin];
    write_node(node_id, value);
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

node_t Simulator::assign_node() {
    if (!m_free_nodes.empty()) {
        auto id = m_free_nodes.back();
        m_free_nodes.pop_back();
        m_node_values_read[id] = VALUE_UNDEFINED;
        m_node_values_write[id] = VALUE_UNDEFINED;
        m_node_write_time[id] = 0;
        m_node_change_time[id] = 0;
        return id;
    }

    m_node_values_read.push_back(VALUE_UNDEFINED);
    m_node_values_write.push_back(VALUE_UNDEFINED);
    m_node_write_time.push_back(0);
    m_node_change_time.push_back(0);

    return m_node_values_read.size() - 1;
}

void Simulator::release_node(node_t node_id) {
    assert(node_id < m_node_values_read.size());
    m_free_nodes.push_back(node_id);
}

node_t Simulator::merge_nodes(node_t node_a, node_t node_b) {
    assert(node_a != node_b);
    release_node(node_b);
    std::replace(std::begin(m_pin_nodes), std::end(m_pin_nodes), node_b, node_a);
    return node_a;
}

void Simulator::write_node(node_t node_id, Value value) {
    assert(node_id < m_node_values_write.size());

    if (value == VALUE_UNDEFINED) {
        return;
    }

    if (m_node_write_time[node_id] < m_time) {
        m_node_write_time[node_id] = m_time;
        m_node_values_write[node_id] = value;
    } else {
        m_node_values_write[node_id] = VALUE_ERROR;
    }
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

void Simulator::init() {
    m_time = 1;

    std::fill(std::begin(m_node_values_read), std::end(m_node_values_read), VALUE_UNDEFINED);
    std::fill(std::begin(m_node_values_write), std::end(m_node_values_write), VALUE_UNDEFINED);
    std::fill(std::begin(m_node_write_time), std::end(m_node_write_time), 0);
    std::fill(std::begin(m_node_change_time), std::end(m_node_change_time), 0);
}

void Simulator::step() {
    m_time = m_time + 1;

    for (const auto &prio_comps : m_components) {
        for (auto &comp : prio_comps) {
            comp->tick();
        }
    }

    for (int i = 0; i < m_node_change_time.size(); ++i) {
        if (m_node_values_write[i] != m_node_values_read[i]) {
            m_node_change_time[i] = m_time;
        }
    }

    m_node_values_read = m_node_values_write;
    std::fill(std::begin(m_node_values_write), std::end(m_node_values_write), VALUE_UNDEFINED);
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

} // namespace lsim