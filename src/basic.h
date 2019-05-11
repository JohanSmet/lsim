// basic.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Basic component parts

#ifndef LSIM_BASIC_H
#define LSIM_BASIC_H

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

#include "property.h"

class Circuit;
class Simulator;

typedef uint32_t pin_t;
typedef uint32_t node_t;

const pin_t PIN_UNDEFINED = static_cast<pin_t>(-1);
const node_t NOT_CONNECTED = (node_t) -1;

enum Value {
    VALUE_FALSE         = 0,
    VALUE_TRUE          = 1,
    VALUE_UNDEFINED     = 2,
    VALUE_ERROR         = 3,
};

enum Priority {
    PRIORITY_NORMAL     = 0,
    PRIORITY_DEFERRED   = 1
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

inline Value negate_value(Value input) {
    if (input == VALUE_TRUE) {
        return VALUE_FALSE; 
    } else if (input == VALUE_FALSE) {
        return VALUE_TRUE;
    } else {
        return VALUE_UNDEFINED;
    }
}

class Component {
public:
    // nested types
    typedef std::vector<pin_t>  pin_container_t;
    typedef std::vector<Value>  value_container_t;
    typedef std::unique_ptr<Component> uptr_t;

    typedef std::unordered_map<std::string, Property::uptr_t> property_lut_t;

    typedef std::function<void(Component *comp)> process_func_t;
    typedef std::function<bool(Component *comp)> check_dirty_func_t;

public:
    // construction
    Component(Simulator *sim,
              size_t num_input, size_t num_output, size_t num_control,
              ComponentType type,
              Priority priority = PRIORITY_NORMAL);
    Component(const Component &other) = delete;
    void set_process_func(process_func_t func);
    void set_check_dirty_func(check_dirty_func_t func);

    // accessors
    Simulator *sim() {return m_sim;}
    Priority get_priority() const {return m_priority;}
    ComponentType type() const {return m_type;}

    // clone: create a copy of this component to integrate in another circuit
    Component::uptr_t clone() const;

    // access pins
    size_t num_pins() const {return m_pins.size();}
    size_t num_input_pins() const {return m_output_start;}
    size_t num_output_pins() const {return m_control_start - m_output_start;}
    size_t num_control_pins() const {return m_pins.size() - m_control_start;}
    size_t input_pin_index(size_t index) const {return index;}
    size_t output_pin_index(size_t index) const {return m_output_start + index;}
    size_t control_pin_index(size_t index) const {return m_control_start + index;}

    pin_t pin(uint32_t index) const;
    pin_t input_pin(uint32_t index) const;
    pin_t output_pin(uint32_t index) const;
    pin_t control_pin(uint32_t index) const;
    const pin_container_t &pins() const {return m_pins;}
    pin_container_t pins(size_t start, size_t end) const;
    pin_container_t input_pins() const;
    pin_container_t output_pins() const;
    pin_container_t control_pins() const;

    // properties
    void add_property(Property::uptr_t &&prop);
    Property *property(const char *key);
    std::string property_value(const char *key, const char *def_value);
    int64_t property_value(const char *key, int64_t def_value);
    bool property_value(const char *key, bool def_value);
    const property_lut_t &properties() const {return m_properties;}

    // processing
    void tick();

    // read/write_pin: read/write the value of the node the specified pin connects to
    Value read_pin(uint32_t index) const;
    void write_pin(uint32_t index, Value value);
    void write_output_pins(value_container_t values);
    void write_output_pins(uint64_t data);
    void write_output_pins(Value value);

    // read/write_checked: convenience functions that make it easy to check if 
    //      all the read nodes (since last bad-read reset) had a valid boolean
    //      value (not undefined or error)
    bool read_pin_checked(uint32_t index);
    void write_pin_checked(uint32_t index, bool value);
    void reset_bad_read_check() {m_read_bad = false;}

    // read_value() : read value that was last written to the specified pin
    //      i.e. to get the value of input connectors / constants etc.
    Value read_value(uint32_t index) const;

private:
    Simulator *m_sim;
    uint32_t m_type;
    pin_container_t  m_pins;
    value_container_t m_values;
    size_t m_output_start;
    size_t m_control_start;
    bool m_read_bad;
    Priority m_priority;

    property_lut_t m_properties;

    process_func_t  m_process_func;
    check_dirty_func_t m_check_dirty_func;
};

Component *ConnectorInput(Circuit *circuit, const char *name, size_t data_bits, bool tri_state = false);
Component *ConnectorOutput(Circuit *circuit, const char *name, size_t data_bits, bool tri_state = false);


#if 0

class Delay {
public:
    Delay(size_t delay);

    Value push_value(Value new_value);
    Value push_last();

private:
    // member variables
    std::deque<Value>  m_values;
};

#endif


#endif // LSIM_BASIC_H
