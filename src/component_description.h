// component_description.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// description of a single component

#ifndef LSIM_COMPONENT_DESCRIPTION_H
#define LSIM_COMPONENT_DESCRIPTION_H

#include <memory>
#include <unordered_map>

#include "sim_types.h"
#include "property.h"
#include "algebra.h"

namespace lsim {

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

class Component {
public:
    typedef std::unique_ptr<Component> uptr_t;
    typedef std::unordered_map<std::string, Property::uptr_t> property_lut_t;
public:
    Component(uint32_t id, ComponentType type, size_t inputs, size_t outputs, size_t controls);
    Component(uint32_t id, CircuitDescription *nested);
    Component(const Component &) = delete;
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
    void change_input_pins(size_t new_count);
    void change_output_pins(size_t new_count);

    pin_id_t port_by_name(const char *name) const;

    // properties
    void add_property(Property::uptr_t &&prop);
    Property *property(const char *key);
    std::string property_value(const char *key, const char *def_value);
    int64_t property_value(const char *key, int64_t def_value);
    bool property_value(const char *key, bool def_value);
    Value property_value(const char *key, Value def_value);
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

} // namespace lsim

#endif // LSIM_COMPONENT_DESCRIPTION_H