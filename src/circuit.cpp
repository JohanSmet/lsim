// circuit.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "circuit.h"
#include <algorithm>
#include <cassert>

Circuit::Circuit() : 
            m_read_idx(0), 
            m_write_idx(1), 
            m_sim_time(0),
            m_next_node_id(0) {
}

pin_t Circuit::create_pin(Component *component) {
    auto len = m_pins.size();
    m_pins.push_back(component);
    m_pin_nodes.push_back(NOT_CONNECTED);
    return len;
}

void Circuit::connect_pins(pin_t pin_a, pin_t pin_b) {

    const auto &node_a = m_pin_nodes[pin_a];
    const auto &node_b = m_pin_nodes[pin_b];

    // both pins not connected - create a new node
    if (node_a == NOT_CONNECTED && node_b == NOT_CONNECTED) {
        auto node_id = create_node();
        m_pin_nodes[pin_a] = node_id;
        m_pin_nodes[pin_b] = node_id;
        m_node_pins[node_id].push_back(pin_a);
        m_node_pins[node_id].push_back(pin_b);
        return;
    }

    // both pins connected - merge node b into node a
    if (node_a != NOT_CONNECTED && node_b != NOT_CONNECTED) {
        m_free_nodes.push_back(node_b);
        std::replace(std::begin(m_pin_nodes), std::end(m_pin_nodes), node_b, node_a);
        for (auto pin : m_node_pins[node_b]) {
            m_node_pins[node_a].push_back(pin);
        }
        m_node_pins[node_b].clear();
        return;
    }

    // one pin connected, the other node: add pin to node
    if (node_a == NOT_CONNECTED) {
        m_pin_nodes[pin_a] = node_b;
    } else {
        m_pin_nodes[pin_b] = node_a;
    }
}

node_t Circuit::create_node() {
    if (!m_free_nodes.empty()) {
        auto id = m_free_nodes.back();
        m_free_nodes.pop_back();
        return id;
    }
    
    m_values[0].push_back(VALUE_UNDEFINED);
    m_values[1].push_back(VALUE_UNDEFINED);
    m_node_write_time.push_back(0);
    m_node_change_time.push_back(0);
    m_node_pins.push_back({});

    return m_next_node_id++;
}

void Circuit::write_value(pin_t pin, Value value) {
    auto node_id = m_pin_nodes[pin];
    if (node_id == NOT_CONNECTED) {
        return;
    }

    assert(m_node_write_time[node_id] < m_sim_time);
    if (m_values[m_write_idx][node_id] != value) {
        m_values[m_write_idx][node_id] = value;
        m_node_change_time[node_id] = m_sim_time;

        for (auto i_pin : m_node_pins[node_id]) {
            if (i_pin != pin) {
                m_pins[i_pin]->set_dirty();
            }
        }
    }

    m_node_write_time[node_id] = m_sim_time;
}

Value Circuit::read_value(pin_t pin) {
    auto node_id = m_pin_nodes[pin];
    if (node_id == NOT_CONNECTED) {
        return VALUE_UNDEFINED;
    }
    return m_values[m_read_idx][node_id];
}

Value Circuit::read_value(pin_t pin, Value value_for_undefined) {
    auto value = read_value(pin);
    return (value != VALUE_UNDEFINED) ? value : value_for_undefined;
}

bool Circuit::value_changed(pin_t pin) {
    auto node_id = m_pin_nodes[pin];
    if (node_id == NOT_CONNECTED) {
        return false;
    }

    return m_node_change_time[node_id] == m_sim_time;
}

void Circuit::simulation_init() {
    m_sim_time = 0;
}

void Circuit::simulation_tick() {
    m_sim_time = m_sim_time + 1;

    for (auto &component : m_components) {
        component->prepare();
    }

    for (auto &component : m_components) {
        component->tick();
    }

    m_values[m_read_idx] = m_values[m_write_idx];
    m_read_idx ^= 1;
    m_write_idx ^= 1;
}

