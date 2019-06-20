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

CircuitDescription::CircuitDescription(const char *name, LSimContext *context, CircuitLibrary *ref_lib) :
        m_context(context),
        m_lib(ref_lib),
        m_name(name),
        m_component_id(0),
        m_wire_id(0) {
}

void CircuitDescription::change_name(const char *name) {
    assert(name);
    m_name = name;
}

std::string CircuitDescription::qualified_name() const {
    std::string qname = m_lib->name();
    if (qname.empty() || qname == "user") {
        return m_name;
    }
    qname += "." + m_name;
    return qname;
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
        result->add_property(make_property("value", VALUE_FALSE));
    } else if (type == COMPONENT_PULL_RESISTOR) {
        result->add_property(make_property("pull_to", VALUE_FALSE));
        result->change_priority(PRIORITY_DEFERRED);
    } else if (type == COMPONENT_TEXT) {
        result->add_property(make_property("text", "text"));
    } else if (type == COMPONENT_VIA) {
        result->add_property(make_property("name", "via"));
    } else {
        result->add_property(make_property("initial_output", VALUE_UNDEFINED));
    }

    return result;
}

Component *CircuitDescription::create_component(const char *circuit_name, size_t input_pins, size_t output_pins) {
    auto component = std::make_unique<Component>(m_component_id++, circuit_name, input_pins, output_pins);
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

    bool was_connector = found->second->type() == COMPONENT_CONNECTOR_IN ||
                         found->second->type() == COMPONENT_CONNECTOR_OUT;

    disconnect_component(id);
    m_components.erase(found);

    if (was_connector) {
        rebuild_port_list();
    }
}

void CircuitDescription::sync_sub_circuit_components() {
    for (auto id : component_ids_of_type(COMPONENT_SUB_CIRCUIT)) {
        auto comp = component_by_id(id);
        comp->sync_nested_circuit(m_context);
    }
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

void CircuitDescription::disconnect_pin(pin_id_t pin) {
    std::vector<uint32_t> empty_wires;

    for (auto &pair : m_wires) {
        pair.second->remove_pin(pin);
        if (pair.second->num_pins() == 0) {
            empty_wires.push_back(pair.second->id());
        }
    }

    for (const auto id : empty_wires) {
        remove_wire(id);
    }
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

void CircuitDescription::rebuild_port_list() {
    // clear current port list
    m_ports_lut.clear();
    m_input_ports.clear();
    m_output_ports.clear();

    // helper function to process one type of connector
    auto process_connectors = [this](ComponentType type, port_container_t &ports) {
        auto conn_ids = component_ids_of_type(type);
        for (auto id : conn_ids) {
            auto connector = component_by_id(id);
            size_t num_ports = connector->num_inputs() + connector->num_outputs();
            std::string name = connector->property("name")->value_as_string();

            if (num_ports == 1) {
                m_ports_lut[name] = connector->pin_id(0);
                ports.push_back(name);
            } else {
                for (size_t idx = 0; idx < num_ports; ++idx) {
                    std::string pin_name = name + "[" + std::to_string(idx) + "]";
                    m_ports_lut[pin_name] = connector->pin_id(idx);
                    ports.push_back(pin_name);
                }
            }
        }
    };

    // build the port lists
    process_connectors(COMPONENT_CONNECTOR_IN, m_input_ports);
    process_connectors(COMPONENT_CONNECTOR_OUT, m_output_ports);
}

void CircuitDescription::change_port_pin_count(uint32_t comp_id, size_t new_count) {
    auto comp = component_by_id(comp_id);
    assert(comp);

    // disconnect any ports that are going to be removed
    auto cur_count = comp->num_inputs() + comp->num_outputs();
    while (cur_count > new_count) {
        disconnect_pin(comp->pin_id(--cur_count));
    }

    if (comp->type() == COMPONENT_CONNECTOR_IN) {
        comp->change_output_pins(new_count);
    } else {
        comp->change_input_pins(new_count);
    }

    rebuild_port_list();
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
    rebuild_port_list();
    return result;
}

Component *CircuitDescription::add_connector_out(const char *name, size_t data_bits, bool tri_state) {
    auto result = create_component(COMPONENT_CONNECTOR_OUT, data_bits, 0, 0);
    result->property("name")->value(name);
    result->property("tri_state")->value(tri_state);
    rebuild_port_list();
    return result;
}

Component *CircuitDescription::add_constant(Value value) {
    auto result = create_component(COMPONENT_CONSTANT, 0, 1, 0);
    result->property("value")->value(value);
    return result;
}

Component *CircuitDescription::add_pull_resistor(Value pull_to) {
    auto result = create_component(COMPONENT_PULL_RESISTOR, 0, 1, 0);
    result->property("pull_to")->value(pull_to);
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

Component *CircuitDescription::add_via(const char *name, size_t data_bits) {
    auto comp = create_component(COMPONENT_VIA, data_bits, 0, 0);
    comp->property("name")->value(name);
    return comp;
}

Component *CircuitDescription::add_sub_circuit(const char *circuit, size_t num_inputs, size_t num_outputs) {
    return create_component(circuit, num_inputs, num_outputs);
}

Component *CircuitDescription::add_sub_circuit(const char *circuit) {
    auto comp = create_component(circuit, 0, 0);
    comp->sync_nested_circuit(m_context);
    return comp;
}

Component *CircuitDescription::add_text(const char *text) {
    auto comp = create_component(COMPONENT_TEXT, 0, 0, 0);
    comp->property("text")->value(text);
    return comp;
}

std::unique_ptr<CircuitInstance> CircuitDescription::instantiate(Simulator *sim) {
    auto instance = std::make_unique<CircuitInstance>(sim, this);
    std::unordered_map<std::string, Component *> via_lut;

    // helper function to connect all pins of two vias
    auto connect_vias = [&instance](Component *via_a, Component *via_b) {
        assert(via_a->num_inputs() == via_b->num_inputs());
        for (size_t i = 0; i < via_a->num_inputs(); ++i) {
            instance->connect_pins(via_a->input_pin_id(i), via_b->input_pin_id(i));
        }
    };

    for (auto &comp_it : m_components) {
        auto comp = comp_it.second.get();
        instance->add_component(comp);

        if (comp->type() == COMPONENT_VIA) {
            auto name = comp->property_value("name", "via");
            auto found = via_lut.find(name);
            if (found != via_lut.end()) {
                connect_vias(comp, found->second);
            } else {
                via_lut[name] = comp;
            }
        }
    }

    for (auto &wire : m_wires) {
        instance->add_wire(wire.second.get());
    }

    return std::move(instance);
}

} // namespace lsim