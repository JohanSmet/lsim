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

void Circuit::change_name(const char *name) {
    assert(name);
    m_name = name;
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

void Circuit::add_ports(const std::string &name, Component *connector) {
    assert(connector);
    uint32_t comp_idx = std::find_if(m_components.begin(), m_components.end(), 
                                  [&](const Component::uptr_t &o) {return o.get() == connector;}) 
                          - m_components.begin();
    auto &ports = (connector->type() == COMPONENT_CONNECTOR_IN) ? m_input_ports : m_output_ports;

    // remove any ports that were created for the connector earlier
    std::remove_if(ports.begin(), ports.end(), [=](const auto &port) {return port.m_component_index == comp_idx;});

    // create new ports
    if (connector->num_pins() == 1) {
        port_t port = {name, comp_idx, 0};
        ports.push_back(port);
        m_ports_lut[name] = port;
    } else {
        for (uint32_t idx = 0; idx < connector->num_pins(); ++idx) {
            auto port_name = name + "[" + std::to_string(idx) + "]";
            port_t port = {port_name, comp_idx, idx};
            ports.push_back(port);
            m_ports_lut[port_name] = port;
        }
    }
}

void Circuit::initialize_input_ports() {
    for (const auto &port : m_input_ports) {
        auto connector = m_components[port.m_component_index].get();
        if (!connector->property_value("tri_state", false) && 
            connector->sim()->read_pin_current_step(connector->pin(port.m_pin_index)) == VALUE_UNDEFINED) {
            connector->write_pin(port.m_pin_index, VALUE_FALSE);
        }
    }
}

pin_t Circuit::pin_port_by_name(const char *name) {
    auto result = m_ports_lut.find(name);
    if (result != m_ports_lut.end()) {
        return pin_from_port(result->second);
    } else {
        return PIN_UNDEFINED;
    }
}

Component::pin_container_t Circuit::input_ports_pins() const {
    return ports_pins(m_input_ports);
}

Component::pin_container_t Circuit::output_ports_pins() const {
    return ports_pins(m_output_ports);
}

Component::pin_container_t Circuit::ports_pins() const {
    auto ports = input_ports_pins();
    const auto &output_ports = output_ports_pins();
    ports.insert(ports.end(), output_ports.begin(), output_ports.end());
    return ports;
}

const char *Circuit::input_port_name(size_t index) const {
    assert(index <= m_input_ports.size());
    return m_input_ports[index].m_name.c_str();
}

const char *Circuit::output_port_name(size_t index) const {
    assert(index <= m_input_ports.size());
    return m_output_ports[index].m_name.c_str();
}

pin_t Circuit::pin_from_port(const port_t &port) const {
    auto connector = m_components[port.m_component_index].get();
    return connector->pin(port.m_pin_index);
}

Component::pin_container_t Circuit::ports_pins(const port_container_t &ports) const {
    Component::pin_container_t result;
    for (const auto &port : ports) {
        result.push_back(pin_from_port(port));
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
        clone_connections(orig_nested->input_ports_pins(), clone_nested->input_ports_pins(), context);
        clone_connections(orig_nested->output_ports_pins(), clone_nested->output_ports_pins(), context);
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

    new_circuit->m_input_ports = m_input_ports;
    new_circuit->m_output_ports = m_output_ports;
    new_circuit->m_ports_lut = m_ports_lut;

    return std::move(new_circuit);
}

Circuit *Circuit::nested_circuit_by_index(size_t idx) {
    assert(idx < m_nested_circuits.size());
    return m_nested_circuits[idx].get();
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

Wire *Circuit::create_wire(size_t num_points, Point *points) {
    m_wires.push_back(std::make_unique<Wire>(points, num_points));
    return m_wires.back().get();
}

Wire *Circuit::create_wire() {
    m_wires.push_back(std::make_unique<Wire>());
    return m_wires.back().get();
}