// simulator.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "simulator.h"
#include "circuit.h"

#include <cassert>
#include <algorithm>

Simulator::Simulator() : 
            m_time(0),
            m_main_circuit(nullptr),
            m_read_idx(0),
            m_write_idx(1) {

}

Circuit *Simulator::create_circuit(const char *name) {
    m_circuits.push_back(std::make_unique<Circuit>(this, name));
    return m_circuits.back().get();
}

void Simulator::set_main_circuit(Circuit *main) {
    m_main_circuit = main;
}

pin_t Simulator::assign_pin(node_t connect_to_pin) {
    auto result = m_pin_nodes.size();
    m_pin_nodes.push_back(NOT_CONNECTED);
    if (connect_to_pin != PIN_UNDEFINED && connect_to_pin < m_pin_nodes.size()) {
        connect_pins(result, connect_to_pin);
    }

    return result;
}

void Simulator::connect_pins(pin_t pin_a, pin_t pin_b) {
    const auto node_a = m_pin_nodes[pin_a];
    const auto node_b = m_pin_nodes[pin_b];

    // both pins not connected - create a new node
    if (node_a == NOT_CONNECTED && node_b == NOT_CONNECTED) {
        auto node_id = assign_node();
        m_pin_nodes[pin_a] = node_id;
        m_pin_nodes[pin_b] = node_id;
        return;
    }

    // both pins connected - merge node b into node a
    if (node_a != NOT_CONNECTED && node_b != NOT_CONNECTED) {
        if (node_a == node_b) {
            // pins already connected to each other
            return;
        }

        release_node(node_b);
        std::replace(std::begin(m_pin_nodes), std::end(m_pin_nodes), node_b, node_a);
        return;
    }

    // one pin connected, the other node: add pin to node
    if (node_a == NOT_CONNECTED) {
        m_pin_nodes[pin_a] = node_b;
    } else {
        m_pin_nodes[pin_b] = node_a;
    }
}

void Simulator::write_pin(pin_t pin, Value value) {
    assert(pin < m_pin_nodes.size());

    auto node_id = m_pin_nodes[pin];
    if (node_id != NOT_CONNECTED) {
       write_node(node_id, value);
    }
}

Value Simulator::read_pin(pin_t pin) const {
    assert(pin < m_pin_nodes.size());

    auto node_id = m_pin_nodes[pin];
    if (node_id == NOT_CONNECTED) {
        return VALUE_UNDEFINED;
    }

    return read_node(node_id);
}

bool Simulator::pin_changed_last_step(pin_t pin) const {
    assert(pin < m_pin_nodes.size());

    auto node_id = m_pin_nodes[pin];
    if (node_id == NOT_CONNECTED) {
        return VALUE_UNDEFINED;
    }

    return node_changed_last_step(node_id);
}

node_t Simulator::pin_node(pin_t pin) {
    assert(pin < m_pin_nodes.size());
    return m_pin_nodes[pin];
}

void Simulator::pin_set_node(pin_t pin, node_t node) {
    assert(pin < m_pin_nodes.size());
    assert(node == NOT_CONNECTED || node < m_node_values[0].size());
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

void Simulator::write_node(node_t node_id, Value value) {
    assert(node_id < m_node_values[m_write_idx].size());

    if (value == VALUE_UNDEFINED) {
        return;
    }

    assert(m_node_write_time[node_id] < m_time);

    m_node_write_time[node_id] = m_time;
    m_node_values[m_write_idx][node_id] = value;
}

Value Simulator::read_node(node_t node_id) const {
    assert(node_id < m_node_values[m_read_idx].size());
    return m_node_values[m_read_idx][node_id];
}

bool Simulator::node_changed(node_t node_id) const {
    assert(node_id < m_node_change_time.size());
    return m_time != 0 && m_node_change_time[node_id] == m_time;
}

bool Simulator::node_changed_last_step(node_t node_id) const {
    assert(node_id < m_node_change_time.size());
    return m_time != 1 && m_node_change_time[node_id] == m_time - 1;
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

void Simulator::init() {
    m_time = 0;
    m_read_idx = 0;
    m_write_idx = 1;

    std::fill(std::begin(m_node_values[0]), std::end(m_node_values[0]), VALUE_UNDEFINED);
    std::fill(std::begin(m_node_values[1]), std::end(m_node_values[1]), VALUE_UNDEFINED);
    std::fill(std::begin(m_node_write_time), std::end(m_node_write_time), 0);
    std::fill(std::begin(m_node_change_time), std::end(m_node_change_time), 0);

    if (!m_main_circuit && !m_circuits.empty()) {
        m_main_circuit = m_circuits.front().get();
    }
}

void Simulator::step() {
    assert(m_main_circuit);

    m_time = m_time + 1;

    m_main_circuit->process();

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