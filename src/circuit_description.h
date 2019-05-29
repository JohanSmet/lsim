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

namespace lsim {

enum Value {
    VALUE_FALSE         = 0,
    VALUE_TRUE          = 1,
    VALUE_UNDEFINED     = 2,
    VALUE_ERROR         = 3,
};

// component types - seems okay for now for these to be all listed here, not really expecting much extra types
typedef uint32_t ComponentType;
const ComponentType COMPONENT_CONNECTOR_IN = 0x0001;
const ComponentType COMPONENT_CONNECTOR_OUT = 0x0002;
const ComponentType COMPONENT_CONSTANT = 0x0003;
const ComponentType COMPONENT_PULL_RESISTOR = 0x0004;
const ComponentType COMPONENT_BUFFER = 0x0011;
const ComponentType COMPONENT_TRISTATE_BUFFER = 0x0012;
const ComponentType COMPONENT_AND_GATE = 0x0013;
const ComponentType COMPONENT_OR_GATE = 0x0014;
const ComponentType COMPONENT_NOT_GATE = 0x0015;
const ComponentType COMPONENT_NAND_GATE = 0x0016;
const ComponentType COMPONENT_NOR_GATE = 0x0017;
const ComponentType COMPONENT_XOR_GATE = 0x0018;
const ComponentType COMPONENT_XNOR_GATE = 0x0019;
const ComponentType COMPONENT_SUB_CIRCUIT = 0x1001;

typedef uint64_t pin_id_t;
typedef std::vector<pin_id_t>   pin_container_t;

class Component {
public:
    typedef std::unique_ptr<Component> uptr_t;
    typedef std::unordered_map<std::string, Property::uptr_t> property_lut_t;
public:
    Component(uint32_t id, ComponentType type, size_t inputs, size_t outputs, size_t controls);
    uint32_t id() const {return m_id;}

    // pins
    size_t num_inputs() const {return m_inputs;}
    size_t num_outputs() const {return m_outputs;}
    size_t num_controls() const {return m_controls;}
    pin_id_t pin_id(size_t index) const;
    pin_id_t input_pin_id(size_t index) const;
    pin_id_t output_pin_id(size_t index) const;
    pin_id_t control_pin_id(size_t index) const;

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
    size_t m_inputs;
    size_t m_outputs;
    size_t m_controls;
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

private:
    uint32_t m_id;
    pin_container_t m_pins;
};

class CircuitDescription {
public:
    CircuitDescription(const char *name);

    // name
    const std::string &name() const {return m_name;}
    void change_name(const char *name);

    // components
    Component *create_component(ComponentType type, size_t input_pins, size_t output_pins = 1, size_t control_pins = 0);
    Component *component_by_id(uint32_t id);

    // connections
    Wire *create_wire();
    Wire *connect(pin_id_t pin_a, pin_id_t pin_b);

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

private:
    typedef std::unordered_map<uint32_t, Component::uptr_t> component_lut_t;
    typedef std::unordered_map<uint32_t, Wire::uptr_t> wire_lut_t;

private:
    std::string     m_name;

    uint32_t        m_component_id;
    component_lut_t m_components;

    uint32_t        m_wire_id;
    wire_lut_t      m_wires;
};

} // namespace lsim

#endif // LSIM_CIRCUIT_DESCRIPTION_H