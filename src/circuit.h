// circuit.h - Johan Smet - BSD-3-Clause (see LICENSE)

#ifndef LSIM_CIRCUIT_H
#define LSIM_CIRCUIT_H

#include <vector>
#include <array>
#include <memory>
#include <unordered_map>
#include <string>

#include "basic.h"
#include "visual_component.h"

class Simulator;
struct CircuitCloneContext;

typedef std::vector<Value> value_container_t;
typedef std::vector<bool> bool_container_t;
typedef std::vector<std::unique_ptr<Component>> component_container_t;
typedef std::unordered_map<std::string, Component *> component_name_lut_t;
typedef std::vector<VisualComponent::uptr_t>    visual_component_container_t;
typedef uint64_t sim_timestamp_t;

class Circuit {
public:
    typedef std::unique_ptr<Circuit> uptr_t;
public:
    Circuit(Simulator *sim, const char *name);

    Simulator *sim() const {return m_sim;}
    const char *name() const {return m_name.c_str();}

    pin_t create_pin(Component *component, pin_t connect_to_pin = PIN_UNDEFINED);
    void connect_pins(pin_t pin_a, pin_t pin_b);

    void add_external_pins(const std::string &name, Component *connector);
    void initialize_external_pins();
    size_t num_external_pins() const {return m_external_pins.size();}
    bool external_pin_is_input(size_t index) const;
    const char *external_pin_name(size_t index) const;
    pin_t external_pin(size_t index) const;
    pin_t external_pin_by_name(const char *name);
    Component::pin_container_t external_pins() const;

    void write_value(pin_t pin, Value value);
    Value read_value(pin_t pin);
    Value read_value(pin_t pin, Value value_for_undefined);

    bool value_changed(pin_t pin);

    Component *create_component(
        size_t num_input, size_t num_output, size_t num_control,
        uint32_t type,
        Priority priority = PRIORITY_NORMAL);

    Circuit *integrate_circuit_clone(Circuit *sub, CircuitCloneContext *context = nullptr);
    Circuit::uptr_t clone(CircuitCloneContext *context = nullptr) const;

    void register_component_name(const std::string &name, Component *component);
    Component *component_by_name(const std::string &name);

    void activate();

    VisualComponent *create_visual_component(Component *comp);
    VisualComponent *create_visual_component(Circuit *circuit);
    visual_component_container_t &visual_components() {return m_visual_components;}

private:
    void clone_connections(Component::pin_container_t orig_pins, Component::pin_container_t new_pins, CircuitCloneContext *context) const;

private:
    struct external_pin_t {
        std::string m_name;
        bool        m_is_input;
        uint32_t    m_component_index;
        uint32_t    m_pin_index;
    };

    typedef std::vector<external_pin_t> external_pin_container_t;
    typedef std::vector<pin_t>          pin_container_t;

    typedef std::vector<Circuit::uptr_t>   circuit_container_t;


private:
    Simulator *m_sim;
    std::string m_name;
    mutable size_t m_clone_count;

    external_pin_container_t        m_external_pins;

    component_container_t           m_components;
    circuit_container_t             m_nested_circuits;
    component_name_lut_t            m_component_name_lut;

    visual_component_container_t    m_visual_components;
};

#endif // LSIM_CIRCUIT_H