// model_component.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// description of a single component

#ifndef LSIM_MODEL_COMPONENT_H
#define LSIM_MODEL_COMPONENT_H

#include <memory>
#include <unordered_map>

#include "sim_types.h"
#include "model_property.h"
#include "algebra.h"

namespace lsim {

using port_lut_t = std::unordered_map<std::string, pin_id_t>;

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

class ModelComponent {
public:
    using uptr_t = std::unique_ptr<ModelComponent>;
    using property_lut_t = std::unordered_map<std::string, Property::uptr_t>;
public:
    ModelComponent(ModelCircuit *parent, uint32_t id, ComponentType type, uint32_t inputs, uint32_t outputs, uint32_t controls);
    ModelComponent(ModelCircuit *parent, uint32_t id, const char *circuit_name, uint32_t inputs, uint32_t outputs);
    ModelComponent(const ModelComponent &) = delete;

    uint32_t id() const {return m_id;}
    ComponentType type() const {return m_type;}

    // pins
    uint32_t num_inputs() const {return m_inputs;}
    uint32_t num_outputs() const {return m_outputs;}
    uint32_t num_controls() const {return m_controls;}
    pin_id_t pin_id(uint32_t index) const;
    pin_id_t input_pin_id(uint32_t index) const;
    pin_id_t output_pin_id(uint32_t index) const;
    pin_id_t control_pin_id(uint32_t index) const;
    void change_input_pins(uint32_t new_count);
    void change_output_pins(uint32_t new_count);

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

    // nested circuits
    ModelCircuit *nested_circuit() const {return m_nested_circuit;}
    bool sync_nested_circuit(class LSimContext *lsim_context);

    // copy & paste
    uptr_t copy() const;
    void integrate_into_circuit(ModelCircuit *circuit, uint32_t id);

private:
    ModelCircuit *m_circuit;
    uint32_t m_id;
    ComponentType m_type;
    uint32_t m_inputs;
    uint32_t m_outputs;
    uint32_t m_controls;

    std::string m_nested_name;
    ModelCircuit *m_nested_circuit;
    port_lut_t m_port_lut;
    property_lut_t m_properties;

    Point m_position;
    int m_angle;            // in degrees
};

} // namespace lsim

#endif // LSIM_MODEL_COMPONENT_H