// simulator.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "simulator.h"
#include "circuit.h"

#include <cassert>
#include <algorithm>

Simulator::Simulator() : 
            m_time(0),
            m_active_circuit(nullptr),
            m_read_idx(0),
            m_write_idx(1) {
}

void Simulator::add_active_component(Component *comp) {
    assert(comp);
    m_active_components[comp->get_priority()].push_back(comp);
}

pin_t Simulator::assign_pin(node_t connect_to_pin) {
    auto result = PIN_UNDEFINED;
    node_t pin_node;

    if (connect_to_pin != PIN_UNDEFINED && connect_to_pin < m_pin_nodes.size()) {
        pin_node = m_pin_nodes[connect_to_pin];
    } else {
        pin_node = assign_node();
    }

    if (!m_free_pins.empty()) {
        result = m_free_pins.back();
        m_free_pins.pop_back();
        m_pin_nodes[result] = pin_node;
    } else {
        result = m_pin_nodes.size();
        m_pin_nodes.push_back(pin_node);
    }

    return result;
}

void Simulator::release_pin(pin_t pin) {
    assert(pin <= m_pin_nodes.size());

    // invalidate node entry of the pin
    auto node = m_pin_nodes[pin];
    m_pin_nodes[pin] = NODE_INVALID;

    // store the pin for reuse
    m_free_pins.push_back(pin);

    // check if the node is still in use
    auto connected = num_pins_in_node(node);
    if (connected <= 0) {
        release_node(node);
    }
}

void Simulator::connect_pins(pin_t pin_a, pin_t pin_b) {
    const auto node_a = m_pin_nodes[pin_a];
    const auto node_b = m_pin_nodes[pin_b];

    if (node_a == node_b) {
        // pins already connected to each other
        return;
    }

    merge_nodes(node_a, node_b);
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

bool Simulator::pin_changed_last_step(pin_t pin) const {
    assert(pin < m_pin_nodes.size());

    auto node_id = m_pin_nodes[pin];
    return node_changed_last_step(node_id);
}

node_t Simulator::pin_node(pin_t pin) {
    assert(pin < m_pin_nodes.size());
    return m_pin_nodes[pin];
}

void Simulator::pin_set_node(pin_t pin, node_t node) {
    assert(pin < m_pin_nodes.size());
    assert(node < m_node_values[0].size());
    m_pin_nodes[pin] = node;
}

node_t Simulator::assign_node() {
    if (!m_free_nodes.empty()) {
        auto id = m_free_nodes.back();
        m_free_nodes.pop_back();
        return id;
    }

    m_node_values[0].push_back(VALUE_UNDEFINED);
    m_node_values[1].push_back(VALUE_UNDEFINED);
    m_node_write_time.push_back(0);
    m_node_change_time.push_back(0);

    return m_node_values[0].size() - 1;
}

void Simulator::release_node(node_t node_id) {
    m_free_nodes.push_back(node_id);
}

node_t Simulator::merge_nodes(node_t node_a, node_t node_b) {
    release_node(node_b);
    std::replace(std::begin(m_pin_nodes), std::end(m_pin_nodes), node_b, node_a);
    return node_a;
}

void Simulator::write_node(node_t node_id, Value value) {
    assert(node_id < m_node_values[m_write_idx].size());

    if (value == VALUE_UNDEFINED) {
        return;
    }

    // assert(m_node_write_time[node_id] < m_time);

    if (m_node_write_time[node_id] < m_time) {
        m_node_write_time[node_id] = m_time;
        m_node_values[m_write_idx][node_id] = value;
    } else {
        m_node_values[m_write_idx][node_id] = VALUE_ERROR;
    }
}

Value Simulator::read_node(node_t node_id) const {
    assert(node_id < m_node_values[m_read_idx].size());
    return m_node_values[m_read_idx][node_id];
}

Value Simulator::read_node_current_step(node_t node_id) const {
    assert(node_id < m_node_values[m_write_idx].size());
    return m_node_values[m_write_idx][node_id];
}

bool Simulator::node_changed(node_t node_id) const {
    assert(node_id < m_node_change_time.size());
    return m_time != 0 && m_node_change_time[node_id] == m_time;
}

bool Simulator::node_changed_last_step(node_t node_id) const {
    assert(node_id < m_node_change_time.size());
    return /*m_time != 1 && */m_node_change_time[node_id] == m_time - 1;
}

uint8_t Simulator::read_nibble(std::vector<node_t> pins) const {
    assert(pins.size() == 4);
    uint8_t result = 0;

    for (int i = 0; i < pins.size(); ++i) {
        result |= ((int) read_pin(pins[i])) << i;
    }

    return result;
}

uint8_t Simulator::read_byte(std::vector<pin_t> pins) const {
    assert(pins.size() == 8);
    uint8_t result = 0;

    for (int i = 0; i < pins.size(); ++i) {
        result |= ((int) read_pin(pins[i])) << i;
    }

    return result;
}

void Simulator::init(Circuit *circuit) {
    m_time = 1;
    m_read_idx = 0;
    m_write_idx = 1;

    std::fill(std::begin(m_node_values[0]), std::end(m_node_values[0]), VALUE_UNDEFINED);
    std::fill(std::begin(m_node_values[1]), std::end(m_node_values[1]), VALUE_UNDEFINED);
    std::fill(std::begin(m_node_write_time), std::end(m_node_write_time), 0);
    std::fill(std::begin(m_node_change_time), std::end(m_node_change_time), 0);

    change_active_circuit(circuit);
}

void Simulator::change_active_circuit(Circuit *circuit) {
    m_active_circuit = circuit;
    m_active_components[PRIORITY_NORMAL].clear();
    m_active_components[PRIORITY_DEFERRED].clear();

    if (circuit) {
        circuit->activate();
    }
}

void Simulator::step() {
    assert(m_active_circuit);

    m_time = m_time + 1;

    for (const auto &prio_comps : m_active_components) {
        for (auto &comp : prio_comps) {
            comp->tick();
        }
    }

    for (int i = 0; i < m_node_change_time.size(); ++i) {
        if (m_node_values[m_write_idx][i] != m_node_values[m_read_idx][i]) {
            m_node_change_time[i] = m_time;
        }
    }

    m_node_values[m_read_idx] = m_node_values[m_write_idx];
    std::fill(std::begin(m_node_values[m_write_idx]), std::end(m_node_values[m_write_idx]), VALUE_UNDEFINED);
}

void Simulator::run_until_change(node_t node_id) {
    do {
        step();
    } while (!node_changed(node_id));
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

size_t Simulator::num_pins_in_node(node_t node) const {
    return std::count(m_pin_nodes.begin(), m_pin_nodes.end(), node);
}