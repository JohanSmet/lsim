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

CircuitComponent::CircuitComponent(Circuit *parent, Circuit *nested) : 
                        Component(parent, 0),
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

void CircuitComponent::process() {
    m_nested->process();
}