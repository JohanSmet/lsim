// sim_component.h - Johan Smet - BSD-3-Clause (see LICENSE)

#ifndef LSIM_SIM_COMPONENT_H
#define LSIM_SIM_COMPONENT_H

#include "sim_types.h"
#include "sim_circuit.h"
#include <memory>

namespace lsim {

class Simulator;

class SimComponent {
public:
	using uptr_t = std::unique_ptr<SimComponent>;
public:
	SimComponent(Simulator* sim, ModelComponent* comp, uint32_t id);
	ModelComponent* description() const { return m_comp_desc; }
	uint32_t id() const { return m_id; }

	void apply_initial_values();

	// pins
	pin_t pin_by_index(uint32_t index) const;
	uint32_t input_pin_index(uint32_t index) const { return index; }
	uint32_t output_pin_index(uint32_t index) const { return m_output_start + index; }
	uint32_t control_pin_index(uint32_t index) const { return m_control_start + index; }
	const pin_container_t& pins() const { return m_pins; }
	pin_container_t input_pins() const;
	pin_container_t output_pins() const;
	pin_container_t control_pins() const;
	size_t num_inputs() const { return m_output_start; }
	size_t num_outputs() const { return m_control_start - m_output_start; }
	size_t num_controls() const { return m_pins.size() - m_control_start; }

	// read/write_pin: read/write the value of the node the specified pin connects to
	Value read_pin(uint32_t index) const;
	void write_pin(uint32_t index, Value value);

	// read/write_checked: convenience functions that make it easy to check if 
	//      all the read nodes (since last bad-read reset) had a valid boolean
	//      value (not undefined or error)
	bool read_pin_checked(uint32_t index);
	void write_pin_checked(uint32_t index, bool value);
	void reset_bad_read_check() { m_read_bad = false; }

	// user_values: input from outside the circuit
	void enable_user_values();
	Value user_value(uint32_t index) const;
	void set_user_value(uint32_t index, Value value);
	bool user_values_enabled() const { return !m_user_values.empty(); }

	// nested circuits
	void set_nested_instance(std::unique_ptr<SimCircuit> instance);
	SimCircuit* nested_instance() const { return m_nested_circuit.get(); }

	// extra-data: component specific data structure
	void set_extra_data_size(size_t size) { m_extra_data.resize(size); };
	uint8_t* extra_data() { return m_extra_data.data(); }

private:
	Simulator* m_sim;
	ModelComponent* m_comp_desc;
	uint32_t m_id;

	pin_container_t m_pins;
	value_container_t m_user_values;
	std::vector<uint8_t> m_extra_data;

	uint32_t m_output_start;
	uint32_t m_control_start;
	bool m_read_bad;

	std::unique_ptr<SimCircuit>    m_nested_circuit;
};

} // namespace lsim

#endif // LSIM_SIM_COMPONENT_H
