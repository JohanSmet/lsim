// circuit.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "circuit.h"
#include "simulator.h"
#include <algorithm>
#include <cassert>

Circuit::Circuit(Simulator *sim) : 
            m_sim(sim) {
}

pin_t Circuit::create_pin(Component *component, pin_t connect_to_pin) {
    auto pin = m_sim->assign_pin(connect_to_pin);
    m_pins.push_back(pin);
    return pin;
}

void Circuit::connect_pins(pin_t pin_a, pin_t pin_b) {
    return m_sim->connect_pins(pin_a, pin_b);
}

void Circuit::add_interface_pin(const char *name, pin_t pin) {
    m_interface_pins.push_back({name, pin});
}

void Circuit::write_value(pin_t pin, Value value) {
    m_sim->write_pin(pin, value);
}

Value Circuit::read_value(pin_t pin) {
    return m_sim->read_pin(pin);
}

Value Circuit::read_value(pin_t pin, Value value_for_undefined) {
    auto value = read_value(pin);
    return (value != VALUE_UNDEFINED) ? value : value_for_undefined;
}

bool Circuit::value_changed(pin_t pin) {
    return m_sim->pin_changed_last_step(pin);
}

CircuitComponent *Circuit::integrate_circuit(Circuit *sub) {
    auto comp = create_component<CircuitComponent>(sub);

    for (const auto &ipin : sub->m_interface_pins) {
        const auto &sub_pin = std::get<1>(ipin);
        const auto &sub_name = std::get<0>(ipin);
        comp->add_pin(create_pin(comp, sub_pin), sub_name.c_str());
    }

    return comp;
}

std::unique_ptr<Circuit> Circuit::clone() const {
    auto new_circuit = std::make_unique<Circuit>(m_sim);

    std::unordered_map<Component *, Component *>    component_map;

    // components
    for (const auto &comp : m_components) { 
        auto new_comp = comp->clone();
        new_comp->materialize(new_circuit.get());
        component_map[comp.get()] = new_comp.get();
        new_circuit->m_components.push_back(std::move(new_comp));
    }

    // component - names
    for (const auto &entry : m_component_name_lut) {
        new_circuit->register_component_name(
                entry.first,
                component_map.find(entry.second)->second);
    }

    // connections
    std::unordered_map<node_t, node_t> node_map;

    for (size_t idx = 0; idx < m_pins.size(); ++idx) {
        auto old_node = m_sim->pin_node(m_pins[idx]);

        auto res = node_map.find(old_node);
        if (res != std::end(node_map)) {
            m_sim->pin_set_node(new_circuit->m_pins[idx], res->second);
        } else {
            auto new_node = m_sim->assign_node();
            m_sim->pin_set_node(new_circuit->m_pins[idx], new_node);
            node_map[old_node] = new_node;
        }
    }

    return std::move(new_circuit);
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

CircuitComponent::CircuitComponent(Circuit *nested) : 
                        CloneComponent(0),
                        m_nested(nested) {
}

void CircuitComponent::add_pin(pin_t pin, const char *name) {
    m_pins.push_back(pin);
    if (name) {
        m_interface_pins[std::string(name)] = pin;
    }
}

pin_t CircuitComponent::interface_pin_by_name(const char *name) {
    auto res = m_interface_pins.find(name);

    if (res != std::end(m_interface_pins)) {    
        return res->second;
    } else {
        return PIN_UNDEFINED;
    }
}

void CircuitComponent::tick() {
    // FIXME: check why is_dirty() check doesn't seem to work
    process();
}

void CircuitComponent::process() {
    m_nested->process();
}