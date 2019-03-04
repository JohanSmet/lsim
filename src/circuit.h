// circuit.h - Johan Smet - BSD-3-Clause (see LICENSE)

#ifndef LSIM_CIRCUIT_H
#define LSIM_CIRCUIT_H

#include <vector>
#include <array>
#include <memory>
#include <unordered_map>
#include <string>
#include "basic.h"

class Component;
class Simulator;

const node_t NOT_CONNECTED = (node_t) -1;

typedef std::vector<Value> value_container_t;
typedef std::vector<bool> bool_container_t;
typedef std::vector<std::unique_ptr<Component>> component_container_t;
typedef std::unordered_map<std::string, Component *> component_name_lut_t;
typedef std::vector<pin_t> pin_container_t;
typedef uint64_t sim_timestamp_t;

class Circuit {
public:
    Circuit(Simulator *sim);

    pin_t create_pin(Component *component);
    void connect_pins(pin_t pin_a, pin_t pin_b);

    void write_value(pin_t pin, Value value);
    Value read_value(pin_t pin);
    Value read_value(pin_t pin, Value value_for_undefined);

    bool value_changed(pin_t pin);
    node_t pin_node(pin_t pin) const;

    template<typename T, typename... Args>
    inline T *create_component(Args&&... args) {
        auto comp = std::make_unique<T>(this, std::forward<Args>(args)...);
        auto raw = comp.get();
        m_components.push_back(std::move(comp));
        return raw;
    }
    void register_component_name(const std::string &name, Component *component);
    Component *component_by_name(const std::string &name);

    void process();

private:
    Simulator *m_sim;

    std::vector<Component *>        m_pins;
    std::vector<node_t>             m_pin_nodes;

    component_container_t           m_components;
    component_name_lut_t            m_component_name_lut;
};



#endif // LSIM_CIRCUIT_H