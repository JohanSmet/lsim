// circuit_description.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// describe the composition of a logic circuit

#include "circuit_description.h"
#include "circuit_instance.h"
#include "lsim_context.h"
#include "simulator.h"

#include <cassert>
#include <algorithm>

namespace lsim {

CircuitDescription::CircuitDescription(const char *name, LSimContext *context) :
        m_context(context),
        m_name(name),
        m_component_id(0),
        m_wire_id(0) {
}

void CircuitDescription::change_name(const char *name) {
    assert(name);
    m_name = name;
}

Component *CircuitDescription::create_component(ComponentType type, size_t input_pins, size_t output_pins, size_t control_pins) {
    assert(type != COMPONENT_SUB_CIRCUIT);

    auto component = std::make_unique<Component>(m_component_id++, type, input_pins, output_pins, control_pins);
    auto result = component.get();
    m_components[result->id()] = std::move(component);

    if (type == COMPONENT_CONNECTOR_IN || type == COMPONENT_CONNECTOR_OUT) {
        result->add_property(make_property("name", (std::string("c#") + std::to_string(result->id())).c_str()));
        result->add_property(make_property("tri_state", false));
    } else if (type == COMPONENT_CONSTANT) {
        result->add_property(make_property("value", static_cast<int64_t>(VALUE_FALSE)));
    } else if (type == COMPONENT_PULL_RESISTOR) {
        result->add_property(make_property("pull_to", static_cast<int64_t>(VALUE_FALSE)));
        result->change_priority(PRIORITY_DEFERRED);
    }

    return result;
}

Component *CircuitDescription::create_component(CircuitDescription *nested_circuit) {
    auto component = std::make_unique<Component>(m_component_id++, nested_circuit);
    auto result = component.get();
    m_components[result->id()] = std::move(component);
    return result;
}

Component *CircuitDescription::component_by_id(uint32_t id) {
    auto found = m_components.find(id);
    if (found != m_components.end()) {
        return found->second.get();
    } else {
        return nullptr;
    }
}

std::vector<uint32_t> CircuitDescription::component_ids() const {
    std::vector<uint32_t> result;
    result.reserve(m_components.size());

    for (const auto &comp : m_components) {
        result.push_back(comp.second->id());
    }

    std::sort(result.begin(), result.end());
    return std::move(result);
}

std::vector<uint32_t> CircuitDescription::component_ids_of_type(ComponentType type) const {
    std::vector<uint32_t> result;

    for (const auto &comp : m_components) {
        if (comp.second->type() == type) {
            result.push_back(comp.second->id());
        }
    }

    std::sort(result.begin(), result.end());
    return std::move(result);
}

void CircuitDescription::disconnect_component(uint32_t id) {
    for (auto &pair : m_wires) {
        pair.second->remove_component_pins(id);
    }
}

void CircuitDescription::remove_component(uint32_t id) {
    auto found = m_components.find(id);
    if (found == m_components.end()) {
        return;
    }
    disconnect_component(id);
    m_components.erase(found);
}

Wire *CircuitDescription::create_wire() {
    auto wire = std::make_unique<Wire>(m_wire_id++);
    auto result = wire.get();
    m_wires[result->id()] = std::move(wire);
    return result;
}

Wire *CircuitDescription::connect(pin_id_t pin_a, pin_id_t pin_b) {
    auto wire = create_wire();
    wire->add_pin(pin_a);
    wire->add_pin(pin_b);
    return wire;
}

std::vector<uint32_t> CircuitDescription::wire_ids() const {
    std::vector<uint32_t> result;
    result.reserve(m_wires.size());

    for (const auto &wire : m_wires) {
        result.push_back(wire.second->id());
    }

    std::sort(result.begin(), result.end());
    return std::move(result);
}

Wire *CircuitDescription::wire_by_id(uint32_t id) const {
    auto found = m_wires.find(id);
    if (found != m_wires.end()) {
        return found->second.get();
    } else {
        return nullptr;
    }
}

void CircuitDescription::remove_wire(uint32_t id) {
    auto found = m_wires.find(id);
    if (found != m_wires.end()) {
        m_wires.erase(found);
    }
}

void CircuitDescription::add_port(Component *connector) {
    size_t num_ports = connector->num_inputs() + connector->num_outputs();
    std::string name = connector->property("name")->value_as_string();
    auto &container = (connector->type() == COMPONENT_CONNECTOR_IN) ? m_input_ports : m_output_ports;

    if (num_ports == 1) {
        m_ports_lut[name] = connector->pin_id(0);
        container.push_back(name);
    } else {
        for (size_t idx = 0; idx < num_ports; ++idx) {
            std::string pin_name = name + "[" + std::to_string(idx) + "]";
            m_ports_lut[pin_name] = connector->pin_id(idx);
            container.push_back(pin_name);
        }
    }
}

pin_id_t CircuitDescription::port_by_name(const char *name) const {
    auto found = m_ports_lut.find(name);
    if (found == m_ports_lut.end()) {
        return PIN_ID_INVALID;
    }

    return found->second;
}

pin_id_t CircuitDescription::port_by_index(bool input, size_t index) const {
    auto &container = (input) ? m_input_ports : m_output_ports;
    assert(index < container.size());
    return port_by_name(container[index].c_str());
}

const std::string &CircuitDescription::port_name(bool input, size_t index) const {
    auto &container = (input) ? m_input_ports : m_output_ports;
    assert(index < container.size());
    return container[index];
}

Component *CircuitDescription::add_connector_in(const char *name, size_t data_bits, bool tri_state) {
    auto result = create_component(COMPONENT_CONNECTOR_IN, 0, data_bits, 0);
    result->property("name")->value(name);
    result->property("tri_state")->value(tri_state);
    add_port(result);
    return result;
}

Component *CircuitDescription::add_connector_out(const char *name, size_t data_bits, bool tri_state) {
    auto result = create_component(COMPONENT_CONNECTOR_OUT, data_bits, 0, 0);
    result->property("name")->value(name);
    result->property("tri_state")->value(tri_state);
    add_port(result);
    return result;
}

Component *CircuitDescription::add_constant(Value value) {
    auto result = create_component(COMPONENT_CONSTANT, 0, 1, 0);
    result->property("value")->value(static_cast<int64_t>(value));
    return result;
}

Component *CircuitDescription::add_pull_resistor(Value pull_to) {
    auto result = create_component(COMPONENT_PULL_RESISTOR, 0, 1, 0);
    result->property("pull_to")->value(static_cast<int64_t>(pull_to));
    return result;
}

Component *CircuitDescription::add_buffer(size_t data_bits) {
    assert(data_bits >= 1);
    return create_component(COMPONENT_BUFFER, data_bits, data_bits, 0);
}

Component *CircuitDescription::add_tristate_buffer(size_t data_bits) {
    assert(data_bits >= 1);
   return create_component(COMPONENT_TRISTATE_BUFFER, data_bits, data_bits, 1);
}

Component *CircuitDescription::add_and_gate(size_t num_inputs) {
    assert(num_inputs >= 2);
    return create_component(COMPONENT_AND_GATE, num_inputs, 1, 0);
}

Component *CircuitDescription::add_or_gate(size_t num_inputs) {
    assert(num_inputs >= 2);
    return create_component(COMPONENT_OR_GATE, num_inputs, 1, 0);
}

Component *CircuitDescription::add_not_gate() {
    return create_component(COMPONENT_NOT_GATE, 1, 1, 0);
}

Component *CircuitDescription::add_nand_gate(size_t num_inputs) {
    assert(num_inputs >= 2);
    return create_component(COMPONENT_NAND_GATE, num_inputs, 1, 0);
}

Component *CircuitDescription::add_nor_gate(size_t num_inputs) {
    assert(num_inputs >= 2);
    return create_component(COMPONENT_NOR_GATE, num_inputs, 1, 0);
}

Component *CircuitDescription::add_xor_gate() {
    return create_component(COMPONENT_XOR_GATE, 2, 1, 0);
}

Component *CircuitDescription::add_xnor_gate() {
    return create_component(COMPONENT_XNOR_GATE, 2, 1, 0);
}

Component *CircuitDescription::add_sub_circuit(const char *circuit) {
    return create_component(m_context->user_library()->circuit_by_name(circuit));
}

std::unique_ptr<CircuitInstance> CircuitDescription::instantiate(Simulator *sim) {
    auto instance = std::make_unique<CircuitInstance>(sim, this);

    for (auto &comp : m_components) {
        instance->add_component(comp.second.get());
    }

    for (auto &wire : m_wires) {
        instance->add_wire(wire.second.get());
    }

    return std::move(instance);
}

} // namespace lsim