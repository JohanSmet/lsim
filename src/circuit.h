// circuit.h - Johan Smet - BSD-3-Clause (see LICENSE)

#ifndef LSIM_CIRCUIT_H
#define LSIM_CIRCUIT_H

#include <vector>
#include <array>
#include <memory>
#include <unordered_map>
#include <string>
#include <tuple>

#include "basic.h"

class Simulator;
class CircuitComponent;


typedef std::vector<Value> value_container_t;
typedef std::vector<bool> bool_container_t;
typedef std::vector<std::unique_ptr<Component>> component_container_t;
typedef std::unordered_map<std::string, Component *> component_name_lut_t;
typedef uint64_t sim_timestamp_t;

class Circuit {
public:
    Circuit(Simulator *sim);

    pin_t create_pin(Component *component, pin_t connect_to_pin = PIN_UNDEFINED);
    void connect_pins(pin_t pin_a, pin_t pin_b);

    void add_interface_pin(const char *name, pin_t pin);

    void write_value(pin_t pin, Value value);
    Value read_value(pin_t pin);
    Value read_value(pin_t pin, Value value_for_undefined);

    bool value_changed(pin_t pin);

    template<typename T, typename... Args>
    inline T *create_component(Args&&... args) {
        auto comp = std::make_unique<T>(this, std::forward<Args>(args)...);
        auto raw = comp.get();
        m_components.push_back(std::move(comp));
        return raw;
    }

    CircuitComponent *integrate_circuit(Circuit *sub);

    void register_component_name(const std::string &name, Component *component);
    Component *component_by_name(const std::string &name);

    void process();

private:
    typedef std::tuple<std::string, pin_t>  interface_pin_t;
    typedef std::vector<interface_pin_t>    interface_pin_container_t;
    typedef std::vector<pin_t>              pin_container_t;

private:
    Simulator *m_sim;

    pin_container_t             m_pins;
    interface_pin_container_t   m_interface_pins;

    component_container_t       m_components;
    component_name_lut_t        m_component_name_lut;

};

class CircuitComponent : public Component {
public: 
    CircuitComponent(Circuit *parent, Circuit *nested);
    void add_pin(pin_t pin, const char *name = nullptr);
    pin_t interface_pin_by_name(const char *name);
    void process();
private:
    typedef std::unordered_map<std::string, pin_t> named_pin_map_t;
private:
    class Circuit *m_nested;
    named_pin_map_t m_interface_pins;
};



#endif // LSIM_CIRCUIT_H