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

Circuit *Simulator::create_circuit() {
    m_circuits.push_back(std::make_unique<Circuit>(this));
    return m_circuits.back().get();
}

void Simulator::set_main_circuit(Circuit *main) {
    m_main_circuit = main;
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
    m_node_change_time[0].push_back(0);
    m_node_change_time[1].push_back(0);

    return m_node_values[0].size() - 1;
}

void Simulator::release_node(node_t node_id) {
    m_free_nodes.push_back(node_id);
}

bool Simulator::write_node(node_t node_id, Value value) {
    assert(node_id < m_node_values[m_write_idx].size());
    assert(m_node_write_time[node_id] < m_time);

    m_node_write_time[node_id] = m_time;
    if (m_node_values[m_write_idx][node_id] != value) {
        m_node_values[m_write_idx][node_id] = value;
        m_node_change_time[m_write_idx][node_id] = m_time;
        return true;
    } else {
        return false;
    }
}

Value Simulator::read_node(node_t node_id) const {
    assert(node_id < m_node_values[m_read_idx].size());
    return m_node_values[m_read_idx][node_id];
}

bool Simulator::node_changed(node_t node_id) const {
    assert(node_id < m_node_change_time[m_read_idx].size());
    return m_node_change_time[m_read_idx][node_id] == m_time;
}

bool Simulator::node_changed_last_step(node_t node_id) const {
    assert(node_id < m_node_change_time[m_read_idx].size());
    return m_node_change_time[m_read_idx][node_id] == m_time - 1;
}

void Simulator::init() {
    m_time = 0;
    m_read_idx = 0;
    m_write_idx = 1;

    std::fill(std::begin(m_node_values[0]), std::end(m_node_values[0]), VALUE_UNDEFINED);
    std::fill(std::begin(m_node_values[1]), std::end(m_node_values[1]), VALUE_UNDEFINED);
    std::fill(std::begin(m_node_write_time), std::end(m_node_write_time), 0);
    std::fill(std::begin(m_node_change_time[0]), std::end(m_node_change_time[0]), 0);
    std::fill(std::begin(m_node_change_time[1]), std::end(m_node_change_time[1]), 0);

    if (!m_main_circuit && !m_circuits.empty()) {
        m_main_circuit = m_circuits.front().get();
    }
}

void Simulator::step() {
    assert(m_main_circuit);

    m_time = m_time + 1;

    m_main_circuit->process();

    m_node_values[m_read_idx] = m_node_values[m_write_idx];
    m_node_change_time[m_read_idx] = m_node_change_time[m_write_idx];
    m_read_idx ^= 1;
    m_write_idx ^= 1;
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

        bool stable = std::none_of(std::begin(m_node_change_time[m_read_idx]), std::end(m_node_change_time[m_read_idx]), 
                            [=] (auto t) {return t == m_time;}
        );

        if (!stable) {
            remaining = stable_ticks;
        } else {
            stop = --remaining == 0;
        }
    }
}