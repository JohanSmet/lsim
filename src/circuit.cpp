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

pin_t Circuit::create_pin(Component *component, pin_t connect_to_pin) {
    auto pin = m_sim->assign_pin(connect_to_pin);
    return pin;
}

void Circuit::connect_pins(pin_t pin_a, pin_t pin_b) {
    return m_sim->connect_pins(pin_a, pin_b);
}

void Circuit::add_interface_pin(const char *name, Connector *connector, uint32_t pin_index) {
    m_interface_pins.push_back({name, connector, pin_index});
}

void Circuit::initialize_interface_pins() {
    for (const auto &ipin : m_interface_pins) {
        if (ipin.m_connector->is_input() && !ipin.m_connector->is_tristate() &&
            ipin.m_connector->get_data(ipin.m_pin_index) == VALUE_UNDEFINED) {
            ipin.m_connector->change_data(ipin.m_pin_index, VALUE_FALSE);
        }
    }
}

bool Circuit::interface_pin_is_input(size_t index) const {
    assert(index < m_interface_pins.size());
    return m_interface_pins[index].m_connector->is_input();
}

const char *Circuit::interface_pin_name(size_t index) const {
    assert(index < m_interface_pins.size());
    return m_interface_pins[index].m_name.c_str();
}

pin_t Circuit::interface_pin(size_t index) const {
    assert(index < m_interface_pins.size());
    return m_interface_pins[index].m_connector->pin(m_interface_pins[index].m_pin_index);
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

CircuitComponent *Circuit::integrate_circuit(Circuit::uptr_t sub) {
    auto comp = std::make_unique<CircuitComponent>(std::move(sub));
    comp->materialize(this);

    for (const auto &ipin : comp->nested_circuit()->m_interface_pins) {
        auto sub_pin = ipin.m_connector->pin(ipin.m_pin_index);
        comp->add_pin(sub_pin, ipin.m_name.c_str());
    }

    m_nested_circuits.push_back(std::move(comp));
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
    for (const auto &nest_comp : m_nested_circuits) {
        auto cloned_circuit = nest_comp->nested_circuit()->clone(context);
        auto cloned_comp = new_circuit->integrate_circuit(std::move(cloned_circuit));
        clone_connections(nest_comp.get(), cloned_comp, context);
    }

    // components
    for (const auto &comp : m_components) { 
        auto new_comp = comp->clone();
        new_comp->materialize(new_circuit.get());
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

CircuitComponent::CircuitComponent(Circuit::uptr_t nested) : 
                        Component(0, VisualComponent::SUB_CIRCUIT),
                        m_nested(std::move(nested)) {
}

void CircuitComponent::add_pin(pin_t pin, const char *name) {
    if (name) {
        m_interface_pins[std::string(name)] = pin;
        Component::add_pin(pin);
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
    // always consider this component is dirty to process the nested circuit
    //  signals take time to propagate through the nested circuit and processing
    //  of interal components take place even if the interface nodes haven't changed value.
    return true;
}

void CircuitComponent::process() {
    m_nested->process();
}

VisualComponent *Circuit::create_visual_component(VisualComponent::Type type, Component *comp) {
    m_visual_components.push_back(std::make_unique<VisualComponent>(type, comp));
    return m_visual_components.back().get();
}