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

const node_t NOT_CONNECTED = (node_t) -1;

typedef std::vector<Value> value_container_t;
typedef std::vector<bool> bool_container_t;
typedef std::vector<std::unique_ptr<Component>> component_container_t;
typedef std::unordered_map<std::string, Component *> component_name_lut_t;
typedef std::vector<pin_t> pin_container_t;
typedef uint64_t sim_timestamp_t;

class Circuit {
public:
    Circuit();

    pin_t create_pin(Component *component);
    void connect_pins(pin_t pin_a, pin_t pin_b);

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
    void register_component_name(const std::string &name, Component *component);
    Component *component_by_name(const std::string &name);

    void simulation_init();
    void simulation_tick();
    void simulation_until_pin_change(pin_t pin);

private:
    node_t create_node();

private:
    std::vector<Component *>        m_pins;
    std::vector<node_t>             m_pin_nodes;
    std::vector<pin_container_t>    m_node_pins;

    node_t m_next_node_id;
    std::vector<node_t>             m_free_nodes;

    std::array<value_container_t, 2>  m_values;
    std::vector<sim_timestamp_t>      m_node_write_time;
    std::vector<sim_timestamp_t>      m_node_change_time;
    int m_read_idx;
    int m_write_idx;

    sim_timestamp_t                 m_sim_time;

    component_container_t           m_components;
    component_name_lut_t            m_component_name_lut;
};



#endif // LSIM_CIRCUIT_H