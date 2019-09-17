// sim_component.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "sim_component.h"
#include "sim_circuit.h"
#include "simulator.h"
#include <cassert>

namespace lsim {

SimComponent::SimComponent(Simulator* sim, ModelComponent* comp, uint32_t id) :
	m_sim(sim),
	m_comp_desc(comp),
	m_id(id),
	m_read_bad(false),
	m_nested_circuit(nullptr) {

	auto num_pins = comp->num_inputs() + comp->num_outputs() + comp->num_controls();
	m_output_start = comp->num_inputs();
	m_control_start = m_output_start + comp->num_outputs();
	for (size_t idx = 0; idx < num_pins; ++idx) {
		m_pins.push_back(sim->assign_pin(this, idx < m_output_start || idx >= m_control_start));
	}
}

void SimComponent::apply_initial_values() {
	auto initial_out = m_comp_desc->property_value("initial_output", VALUE_UNDEFINED);
	if (initial_out != VALUE_UNDEFINED) {
		for (size_t pin = m_output_start; pin < m_control_start; ++pin) {
			m_sim->pin_set_initial_value(m_pins[pin], initial_out);
		}
	}

	if (m_comp_desc->type() == COMPONENT_CONNECTOR_IN &&
		!m_user_values.empty() &&
		!m_comp_desc->property_value("tri_state", false)) {
		for (size_t pin = m_output_start; pin < m_control_start; ++pin) {
			m_user_values[pin] = VALUE_FALSE;
			m_sim->pin_set_initial_value(m_pins[pin], initial_out);
		}
	}
}

pin_t SimComponent::pin_by_index(uint32_t index) const {
	assert(index < m_pins.size());
	return m_pins[index];
}

pin_container_t SimComponent::input_pins() const {
	return pin_container_t(m_pins.begin(), m_pins.begin() + m_output_start);
}

pin_container_t SimComponent::output_pins() const {
	return pin_container_t(m_pins.begin() + m_output_start, m_pins.begin() + m_control_start);
}

pin_container_t SimComponent::control_pins() const {
	return pin_container_t(m_pins.begin() + m_control_start, m_pins.end());
}

Value SimComponent::read_pin(uint32_t index) const {
	assert(index < m_pins.size());
	return m_sim->read_pin(m_pins[index]);
}

void SimComponent::write_pin(uint32_t index, Value value) {
	assert(index < m_pins.size());
	// XXX: is the second test really necessary?
	if (value == VALUE_UNDEFINED && m_sim->pin_output_value(m_pins[index]) == value) {
		return;
	}
	m_sim->write_pin(m_pins[index], value);
}

bool SimComponent::read_pin_checked(uint32_t index) {
	assert(index < m_pins.size());
	auto value = m_sim->read_pin(m_pins[index]);
	m_read_bad |= (value != VALUE_TRUE && value != VALUE_FALSE);
	return static_cast<bool>(value);
}

void SimComponent::write_pin_checked(uint32_t index, bool value) {
	auto output = m_read_bad ? VALUE_ERROR : value;
	write_pin(index, static_cast<Value>(output));
}

void SimComponent::enable_user_values() {
	m_user_values.clear();
	m_user_values.resize(m_pins.size(), VALUE_UNDEFINED);
}

Value SimComponent::user_value(uint32_t index) const {
	if (index < m_user_values.size()) {
		return m_user_values[index];
	}
	else {
		return VALUE_UNDEFINED;
	}
}

void SimComponent::set_user_value(uint32_t index, Value value) {
	assert(index < m_pins.size());
	m_user_values[index] = value;
	m_sim->activate_independent_simulation_func(this);
}

void SimComponent::set_nested_instance(std::unique_ptr<class SimCircuit> instance) {
	m_nested_circuit = std::move(instance);
}

} // namespace lsim
