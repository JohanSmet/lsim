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

    typedef std::vector<Wire::uptr_t>   wire_container_t;
public:
    Circuit(Simulator *sim, const char *name);

    Simulator *sim() const {return m_sim;}

    const std::string &name() const {return m_name;}
    void change_name(const char *name);

    pin_t create_pin(Component *component, pin_t connect_to_pin = PIN_UNDEFINED);
    void connect_pins(pin_t pin_a, pin_t pin_b);

    void add_ports(const std::string &name, Component *connector);
    void initialize_input_ports();
    pin_t pin_port_by_name(const char *name);
    Component::pin_container_t input_ports_pins() const;
    Component::pin_container_t output_ports_pins() const;
    Component::pin_container_t ports_pins() const;
    const char *input_port_name(size_t index) const;
    const char *output_port_name(size_t index) const;
    size_t num_input_ports() const {return m_input_ports.size();}
    size_t num_output_ports() const {return m_output_ports.size();}

    void write_value(pin_t pin, Value value);
    Value read_value(pin_t pin);
    Value read_value(pin_t pin, Value value_for_undefined);

    bool value_changed(pin_t pin);

    Component *create_component(
        size_t num_input, size_t num_output, size_t num_control,
        uint32_t type,
        Priority priority = PRIORITY_NORMAL);
    const component_container_t &components() const {return m_components;}

    Circuit *integrate_circuit_clone(Circuit *sub, CircuitCloneContext *context = nullptr);
    Circuit::uptr_t clone(CircuitCloneContext *context = nullptr) const;
    size_t num_nested_circuits() const {return m_nested_circuits.size();}
    Circuit *nested_circuit_by_index(size_t idx);

    void register_component_name(const std::string &name, Component *component);
    Component *component_by_name(const std::string &name);

    void activate();

    VisualComponent *create_visual_component(Component *comp);
    VisualComponent *create_visual_component(Circuit *circuit);
    visual_component_container_t &visual_components() {return m_visual_components;}

    Wire *create_wire(size_t num_points, Point *points);
    Wire *create_wire();
    const wire_container_t &wires() const {return m_wires;}

private:
    struct port_t {
        std::string m_name;
        uint32_t    m_component_index;
        uint32_t    m_pin_index;
    };
    typedef std::vector<pin_t>      pin_container_t;

    typedef std::vector<port_t>     port_container_t;
    typedef std::unordered_map<std::string, port_t> port_lut_t;


    typedef std::vector<Circuit::uptr_t>   circuit_container_t;

private:
    void clone_connections(Component::pin_container_t orig_pins, Component::pin_container_t new_pins, CircuitCloneContext *context) const;
    pin_t pin_from_port(const port_t &port) const;
    Component::pin_container_t ports_pins(const port_container_t &ports) const;

private:
    Simulator *m_sim;
    std::string m_name;
    mutable size_t m_clone_count;

    port_container_t                m_input_ports;
    port_container_t                m_output_ports;
    port_lut_t                      m_ports_lut;

    component_container_t           m_components;
    circuit_container_t             m_nested_circuits;
    component_name_lut_t            m_component_name_lut;

    visual_component_container_t    m_visual_components;
    wire_container_t                m_wires;
};

#endif // LSIM_CIRCUIT_H