// circuit_description.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// describe the composition of a logic circuit

#ifndef LSIM_CIRCUIT_DESCRIPTION_H
#define LSIM_CIRCUIT_DESCRIPTION_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

#include "property.h"
#include "algebra.h"
#include "sim_types.h"

namespace lsim {

typedef uint64_t pin_id_t;
typedef std::vector<pin_id_t>   pin_id_container_t;
typedef std::unordered_map<std::string, pin_id_t> port_lut_t;

inline uint32_t component_id_from_pin_id(pin_id_t pin_id) {
    return pin_id >> 32;
}

inline uint32_t pin_index_from_pin_id(pin_id_t pin_id) {
    return pin_id & 0xffffffff;
}

inline pin_id_t pin_id_assemble(uint32_t component_id, uint32_t pin_index) {
    return ((static_cast<uint64_t>(component_id)) << 32) | pin_index;
}

constexpr const auto PIN_ID_INVALID = static_cast<pin_id_t>(-1);

class CircuitDescription;

class Component {
public:
    typedef std::unique_ptr<Component> uptr_t;
    typedef std::unordered_map<std::string, Property::uptr_t> property_lut_t;
public:
    Component(uint32_t id, ComponentType type, size_t inputs, size_t outputs, size_t controls);
    Component(uint32_t id, CircuitDescription *nested);
    uint32_t id() const {return m_id;}
    ComponentType type() const {return m_type;}
    CircuitDescription *nested_circuit() const {return m_nested_circuit;}

    // priority
    Priority priority() const {return m_priority;}
    void change_priority(Priority priority) {m_priority = priority;}

    // pins
    size_t num_inputs() const {return m_inputs;}
    size_t num_outputs() const {return m_outputs;}
    size_t num_controls() const {return m_controls;}
    pin_id_t pin_id(size_t index) const;
    pin_id_t input_pin_id(size_t index) const;
    pin_id_t output_pin_id(size_t index) const;
    pin_id_t control_pin_id(size_t index) const;

    pin_id_t port_by_name(const char *name) const;

    // properties
    void add_property(Property::uptr_t &&prop);
    Property *property(const char *key);
    std::string property_value(const char *key, const char *def_value);
    int64_t property_value(const char *key, int64_t def_value);
    bool property_value(const char *key, bool def_value);
    const property_lut_t &properties() const {return m_properties;}

    // position / orientation
    const Point &position() const {return m_position;}
    int angle() const {return m_angle;}
    void set_position(const Point &pos);
    void set_angle(int angle);

private:
    uint32_t m_id;
    ComponentType m_type;
    Priority m_priority;
    size_t m_inputs;
    size_t m_outputs;
    size_t m_controls;
    CircuitDescription *m_nested_circuit;
    port_lut_t m_port_lut;
    property_lut_t m_properties;
    Point m_position;
    int m_angle;            // in degrees
};

class Wire {
public:
    typedef std::unique_ptr<Wire>   uptr_t;

public:
    Wire(uint32_t id);
    uint32_t id() const {return m_id;}

    // pins
    void add_pin(pin_id_t pin);
    size_t num_pins() const {return m_pins.size();}
    pin_id_t pin(size_t index) const;

private:
    uint32_t m_id;
    pin_id_container_t m_pins;
};

class CircuitDescription {
public:
    typedef std::unique_ptr<CircuitDescription> uptr_t;
public:
    CircuitDescription(const char *name, class LSimContext *context);

    // name
    const std::string &name() const {return m_name;}
    void change_name(const char *name);

    // components
protected:
    Component *create_component(ComponentType type, size_t input_pins, size_t output_pins = 1, size_t control_pins = 0);
    Component *create_component(CircuitDescription *nested_circuit);
public:
    Component *component_by_id(uint32_t id);
    std::vector<uint32_t> component_ids() const;

    // connections
    Wire *create_wire();
    Wire *connect(pin_id_t pin_a, pin_id_t pin_b);
    std::vector<uint32_t> wire_ids() const;
    Wire *wire_by_id(uint32_t id) const;

    // ports
    void add_port(Component *connector);
    pin_id_t port_by_name(const char *name) const;
    pin_id_t port_by_index(bool input, size_t index) const;
    const std::string &port_name(bool input, size_t index) const;
    size_t num_input_ports() const {return m_input_ports.size();}
    size_t num_output_ports() const {return m_output_ports.size();}

    // specialized component creation functions
    Component *add_connector_in(const char *name, size_t data_bits, bool tri_state = false);
    Component *add_connector_out(const char *name, size_t data_bits, bool tri_state = false);
    Component *add_constant(Value value);
    Component *add_pull_resistor(Value pull_to);
    Component *add_buffer(size_t data_bits);
    Component *add_tristate_buffer(size_t data_bits);
    Component *add_and_gate(size_t num_inputs);
    Component *add_or_gate(size_t num_inputs);
    Component *add_not_gate();
    Component *add_nand_gate(size_t num_inputs);
    Component *add_nor_gate(size_t num_inputs);
    Component *add_xor_gate();
    Component *add_xnor_gate();
    Component *add_sub_circuit(const char *circuit);

    // instantiate into a simulator
    std::unique_ptr<class CircuitInstance> instantiate(class Simulator *sim);

private:
    typedef std::unordered_map<uint32_t, Component::uptr_t> component_lut_t;
    typedef std::unordered_map<uint32_t, Wire::uptr_t> wire_lut_t;
    typedef std::vector<std::string> port_container_t;

private:
    class LSimContext *m_context;
    std::string     m_name;

    uint32_t        m_component_id;
    component_lut_t m_components;

    uint32_t        m_wire_id;
    wire_lut_t      m_wires;

    port_lut_t       m_ports_lut;
    port_container_t m_input_ports;
    port_container_t m_output_ports;

};

} // namespace lsim

#endif // LSIM_CIRCUIT_DESCRIPTION_H