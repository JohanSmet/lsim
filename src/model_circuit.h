// model_circuit.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// describe the composition of a logic circuit

#ifndef LSIM_MODEL_CIRCUIT_H
#define LSIM_MODEL_CIRCUIT_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

#include "model_component.h"
#include "model_wire.h"

namespace lsim {

class ModelCircuit {
public:
    typedef std::unique_ptr<ModelCircuit> uptr_t;
    typedef std::unordered_map<uint32_t, ModelWire::uptr_t> wire_lut_t;
public:
    ModelCircuit(const char *name, class LSimContext *context, class ModelCircuitLibrary *ref_lib);
    ModelCircuit(const ModelCircuit &) = delete;

    class LSimContext *context() const {return m_context;}
    class ModelCircuitLibrary *lib() const {return m_lib;}

    // name
    const std::string &name() const {return m_name;}
    std::string qualified_name() const;
    void change_name(const char *name);

    // components
protected:
    ModelComponent *create_component(ComponentType type, uint32_t input_pins, uint32_t output_pins = 1, uint32_t control_pins = 0);
    ModelComponent *create_component(const char *circuit_name, uint32_t input_pins, uint32_t output_pins);
public:
    ModelComponent *component_by_id(uint32_t id);
    std::vector<uint32_t> component_ids() const;
    std::vector<uint32_t> component_ids_of_type(ComponentType type) const;
    void disconnect_component(uint32_t id);
    void remove_component(uint32_t id);
    void sync_sub_circuit_components();
    ModelComponent *paste_component(ModelComponent *comp);

    // connections
    ModelWire *create_wire();
    ModelWire *connect(pin_id_t pin_a, pin_id_t pin_b);
    void disconnect_pin(pin_id_t pin);
    std::vector<uint32_t> wire_ids() const;
    ModelWire *wire_by_id(uint32_t id) const;
    const wire_lut_t &wires() const {return m_wires;}
    void remove_wire(uint32_t id);

    // ports
    void rebuild_port_list();
    void change_port_pin_count(uint32_t comp_id, uint32_t new_count);
    pin_id_t port_by_name(const char *name) const;
    pin_id_t port_by_index(bool input, size_t index) const;
    const std::string &port_name(bool input, size_t index) const;
    uint32_t num_input_ports() const {return static_cast<uint32_t>(m_input_ports.size());}
    uint32_t num_output_ports() const {return static_cast<uint32_t>(m_output_ports.size());}

    // specialized component creation functions
    ModelComponent *add_connector_in(const char *name, uint32_t data_bits, bool tri_state = false);
    ModelComponent *add_connector_out(const char *name, uint32_t data_bits, bool tri_state = false);
    ModelComponent *add_constant(Value value);
    ModelComponent *add_pull_resistor(Value pull_to);
    ModelComponent *add_buffer(uint32_t data_bits);
    ModelComponent *add_tristate_buffer(uint32_t data_bits);
    ModelComponent *add_and_gate(uint32_t num_inputs);
    ModelComponent *add_or_gate(uint32_t num_inputs);
    ModelComponent *add_not_gate();
    ModelComponent *add_nand_gate(uint32_t num_inputs);
    ModelComponent *add_nor_gate(uint32_t num_inputs);
    ModelComponent *add_xor_gate();
    ModelComponent *add_xnor_gate();
    ModelComponent *add_via(const char *name, uint32_t data_bits);
    ModelComponent *add_oscillator(uint32_t low_duration, uint32_t high_duration);
    ModelComponent *add_7_segment_led();
    ModelComponent *add_sub_circuit(const char *circuit, uint32_t num_inputs, uint32_t num_outputs);
    ModelComponent *add_sub_circuit(const char *circuit);
    ModelComponent *add_text(const char *text);

    // instantiate into a simulator
    std::unique_ptr<class CircuitInstance> instantiate(class Simulator *sim, bool top_level = true);

private:
    typedef std::unordered_map<uint32_t, ModelComponent::uptr_t> component_lut_t;
    typedef std::vector<std::string> port_container_t;

private:
    class LSimContext *m_context;
    class ModelCircuitLibrary *m_lib;

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

#endif // LSIM_MODEL_CIRCUIT_H