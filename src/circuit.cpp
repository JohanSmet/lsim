// circuit.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "circuit.h"
#include "simulator.h"
#include <algorithm>
#include <cassert>

Circuit::Circuit(Simulator *sim) : 
            m_sim(sim) {
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
        auto node_id = m_sim->assign_node();
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

        m_sim->release_node(node_b);
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

void Circuit::write_value(pin_t pin, Value value) {
    auto node_id = m_pin_nodes[pin];
    if (node_id == NOT_CONNECTED) {
        return;
    }

    m_sim->write_node(node_id, value);
}

Value Circuit::read_value(pin_t pin) {
    auto node_id = m_pin_nodes[pin];
    if (node_id == NOT_CONNECTED) {
        return VALUE_UNDEFINED;
    }
    return m_sim->read_node(node_id);
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

    return m_sim->node_changed_last_step(node_id);
}

node_t Circuit::pin_node(pin_t pin) const {
    assert(pin < m_pin_nodes.size());
    return m_pin_nodes[pin];
}

void Circuit::register_component_name(const std::string &name, Component *component) {
    assert(component);
    m_component_name_lut[name] = component;
}

Component *Circuit::component_by_name(const std::string &name) {
    auto res = m_component_name_lut.find(name);
    if (res != std::end(m_component_name_lut)) {
        return res->second;
    } else {
        return nullptr;
    }
}

void Circuit::process() {
    for (auto &component : m_components) {
        component->tick();
    }
}