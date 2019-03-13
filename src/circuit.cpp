// circuit.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "circuit.h"
#include "simulator.h"
#include <algorithm>
#include <cassert>

struct CircuitCloneContext {
    std::unordered_map<node_t, node_t> node_map;
};

Circuit::Circuit(Simulator *sim) : 
            m_sim(sim) {
}

pin_t Circuit::create_pin(Component *component, pin_t connect_to_pin) {
    auto pin = m_sim->assign_pin(connect_to_pin);
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
    auto comp = std::make_unique<CircuitComponent>(sub);
    comp->materialize(this);

    for (const auto &ipin : sub->m_interface_pins) {
        const auto &sub_pin = std::get<1>(ipin);
        const auto &sub_name = std::get<0>(ipin);
        comp->add_pin(sub_pin, sub_name.c_str());
    }

    m_nested_circuits.push_back(std::move(comp));
    return m_nested_circuits.back().get();
}

Circuit *Circuit::clone(CircuitCloneContext *context) const {
    auto new_circuit = m_sim->create_circuit();
    std::unique_ptr<CircuitCloneContext> context_ptr = nullptr;

    if (!context) {
        context_ptr = std::make_unique<CircuitCloneContext>();
        context = context_ptr.get();
    }

    std::unordered_map<Component *, Component *>    component_map;

    // nested circuits
    for (const auto &nest_comp : m_nested_circuits) {
        auto cloned_circuit = nest_comp->nested_circuit()->clone(context);
        auto cloned_comp = new_circuit->integrate_circuit(cloned_circuit);
        clone_connections(nest_comp.get(), cloned_comp, context);
    }

    // components
    for (const auto &comp : m_components) { 
        auto new_comp = comp->clone();
        new_comp->materialize(new_circuit);
        component_map[comp.get()] = new_comp.get();
        clone_connections(comp.get(), new_comp.get(), context);
        new_circuit->m_components.push_back(std::move(new_comp));
    }

    // component - names
    for (const auto &entry : m_component_name_lut) {
        new_circuit->register_component_name(
                entry.first,
                component_map.find(entry.second)->second);
    }

    return std::move(new_circuit);
}

void Circuit::clone_connections(Component *orig, Component *clone, CircuitCloneContext *context) const { 

    for (size_t idx = 0; idx < orig->num_pins(); ++idx) {
        auto old_node = m_sim->pin_node(orig->pin(idx));

        auto res = context->node_map.find(old_node);
        if (res != std::end(context->node_map)) {
            m_sim->pin_set_node(clone->pin(idx), res->second);
        } else {
            auto new_node = m_sim->assign_node();
            m_sim->pin_set_node(clone->pin(idx), new_node);
            context->node_map[old_node] = new_node;
        }
    }
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

    for (auto &nested : m_nested_circuits) {
        nested->tick();
    }

    for (auto &component : m_components) {
        component->tick();
    }
}

CircuitComponent::CircuitComponent(Circuit *nested) : 
                        Component(0),
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

std::unique_ptr<Component> CircuitComponent::clone() const {
    return nullptr;
}

bool CircuitComponent::is_dirty() const {
    // FIXME: check why is_dirty() check doesn't seem to work
    return true;
}

void CircuitComponent::process() {
    m_nested->process();
}