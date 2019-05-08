// circuit.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "circuit.h"
#include "simulator.h"
#include <algorithm>
#include <cassert>

struct CircuitCloneContext {
    std::unordered_map<node_t, node_t> node_map;
};

Circuit::Circuit(Simulator *sim, const char *name) : 
            m_sim(sim),
            m_name(name),
            m_clone_count(0) {
}

Component *Circuit::create_component(
            size_t num_input, size_t num_output, size_t num_control,
            uint32_t type, Priority priority) {
    m_components.push_back(std::make_unique<Component>(
        m_sim,
        num_input, num_output, num_control,
        type, priority
    ));
    return m_components.back().get();
}

pin_t Circuit::create_pin(Component *component, pin_t connect_to_pin) {
    auto pin = m_sim->assign_pin(connect_to_pin);
    return pin;
}

void Circuit::connect_pins(pin_t pin_a, pin_t pin_b) {
    return m_sim->connect_pins(pin_a, pin_b);
}

void Circuit::add_external_pins(const std::string &name, Component *connector) {
    assert(connector);
    uint32_t comp_idx = std::find_if(m_components.begin(), m_components.end(), 
                                  [&](const Component::uptr_t &o) {return o.get() == connector;}) 
                          - m_components.begin();
    bool is_input = connector->type() == COMPONENT_CONNECTOR_IN;

    if (connector->num_pins() == 1) {
        m_external_pins.push_back({name, is_input, comp_idx, 0});
    } else {
        for (uint32_t idx = 0; idx < connector->num_pins(); ++idx) {
            m_external_pins.push_back({name + "[" + std::to_string(idx) + "]", is_input, comp_idx, idx});
        }
    }
}

void Circuit::initialize_external_pins() {
    for (const auto &ext_pin : m_external_pins) {
        auto connector = m_components[ext_pin.m_component_index].get();
        if (ext_pin.m_is_input && /*!ipin.m_connector->is_tristate() && */
            connector->sim()->read_pin_current_step(connector->pin(ext_pin.m_pin_index)) == VALUE_UNDEFINED) {
            connector->write_pin(ext_pin.m_pin_index, VALUE_FALSE);
        }
    }
}

bool Circuit::external_pin_is_input(size_t index) const {
    assert(index < m_external_pins.size());
    return m_external_pins[index].m_is_input;
}

const char *Circuit::external_pin_name(size_t index) const {
    assert(index < m_external_pins.size());
    return m_external_pins[index].m_name.c_str();
}

pin_t Circuit::external_pin(size_t index) const {
    assert(index < m_external_pins.size());
    auto connector = m_components[m_external_pins[index].m_component_index].get();
    return connector->pin(m_external_pins[index].m_pin_index);
}

pin_t Circuit::external_pin_by_name(const char *name) {
    auto result = std::find_if(m_external_pins.begin(), m_external_pins.end(),
                               [&](const external_pin_t &ext_pin) {return ext_pin.m_name == name;}
    );

    if (result == m_external_pins.end()) {
        return PIN_UNDEFINED;
    } else {
        return external_pin(result - m_external_pins.begin());
    }
}

Component::pin_container_t Circuit::external_pins() const {
    Component::pin_container_t result;
    for (size_t idx = 0; idx < m_external_pins.size(); ++idx) {
        result.push_back(external_pin(idx));
    }

    return result;
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

Circuit *Circuit::integrate_circuit_clone(Circuit *sub, CircuitCloneContext *context) {
    m_nested_circuits.push_back(std::move(sub->clone(context)));
    return m_nested_circuits.back().get(); 
}

Circuit::uptr_t Circuit::clone(CircuitCloneContext *context) const {
    m_clone_count += 1;
    auto new_circuit = std::make_unique<Circuit>(m_sim, (m_name + "#" + std::to_string(m_clone_count)).c_str());
    std::unique_ptr<CircuitCloneContext> context_ptr = nullptr;

    if (!context) {
        context_ptr = std::make_unique<CircuitCloneContext>();
        context = context_ptr.get();
    }

    std::unordered_map<Component *, Component *>    component_map;

    // nested circuits
    for (const auto &nested : m_nested_circuits) {
        auto orig_nested = nested.get();
        auto clone_nested = new_circuit->integrate_circuit_clone(orig_nested, context);
        clone_connections(orig_nested->external_pins(), clone_nested->external_pins(), context);
    }

    // components
    for (const auto &comp : m_components) { 
        auto new_comp = comp->clone();
        component_map[comp.get()] = new_comp.get();
        clone_connections(comp->pins(), new_comp->pins(), context);
        new_circuit->m_components.push_back(std::move(new_comp));
    }
    
    // component - names
    for (const auto &entry : m_component_name_lut) {
        new_circuit->register_component_name(
                entry.first,
                component_map.find(entry.second)->second);
    }

    new_circuit->m_external_pins = m_external_pins;

    return std::move(new_circuit);
}

void Circuit::clone_connections(Component::pin_container_t orig_pins, Component::pin_container_t new_pins, CircuitCloneContext *context) const {

    for (size_t idx = 0; idx < orig_pins.size(); ++idx) {
        auto old_node = m_sim->pin_node(orig_pins[idx]);

        auto res = context->node_map.find(old_node);
        if (res != std::end(context->node_map)) {
            m_sim->pin_set_node(new_pins[idx], res->second);
        } else {
            auto new_node = m_sim->assign_node();
            m_sim->pin_set_node(new_pins[idx], new_node);
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

void Circuit::activate() {
    for (auto &nested : m_nested_circuits) {
        nested->activate();
    } 

    for (auto &component : m_components) {
        m_sim->add_active_component(component.get());
    } 
}

VisualComponent *Circuit::create_visual_component(Component *comp) {
    m_visual_components.push_back(std::make_unique<VisualComponent>(comp));
    return m_visual_components.back().get();
}

VisualComponent *Circuit::create_visual_component(Circuit *circuit) {
    m_visual_components.push_back(std::make_unique<VisualComponent>(circuit));
    return m_visual_components.back().get();
}