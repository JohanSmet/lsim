// ui_context.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "ui_context.h"

#include "lsim_context.h"
#include "model_circuit.h"
#include "component_widget.h"

#include "serialize.h"

namespace lsim {

namespace gui {

UIContext::UIContext(LSimContext* lsim_context) :
	m_lsim_context(lsim_context) { 
}

void UIContext::circuit_library_load(const std::string& filename) {
	if (m_circuit_editor != nullptr) {
		circuit_library_close();
	}

	m_lib_filename = filename;
	
	// fixme: decent error handling would be nice (*cough*)
	deserialize_library(m_lsim_context, m_lsim_context->user_library(), filename.c_str());

	// make sure there is at least one circuit
	if (m_lsim_context->user_library()->num_circuits() <= 0) {
		m_lsim_context->user_library()->create_circuit("main", m_lsim_context);
		m_lsim_context->user_library()->change_main_circuit("main");
	}

	// select main circuit
	auto main_circuit = m_lsim_context->user_library()->main_circuit();
	change_active_circuit(main_circuit);
}

void UIContext::circuit_library_save(const std::string& filename) {
	serialize_library(m_lsim_context, m_lsim_context->user_library(), filename.c_str());
}

void UIContext::circuit_library_close() {
	simulation_stop();
	change_active_circuit(nullptr);
	m_lsim_context->user_library()->clear_circuits();
	m_lsim_context->user_library()->clear_references();
	m_lsim_context->clear_reference_libraries();
}

void UIContext::change_active_circuit(ModelCircuit* circuit) {
	if (m_circuit_editor != nullptr && m_circuit_editor->model_circuit() == circuit) {
		return;
	}

	if (circuit) {
		circuit->sync_sub_circuit_components();
		m_circuit_editor = CircuitEditorFactory::create_circuit(circuit);
		m_selected_circuit_idx = m_lsim_context->user_library()->circuit_idx(circuit);
	}
	else {
		m_circuit_editor = nullptr;
	}
}

void UIContext::simulation_start() {
	auto sim = m_lsim_context->sim();

	m_sim_circuit = m_circuit_editor->model_circuit()->instantiate(sim);
	m_circuit_editor->set_simulation_instance(m_sim_circuit.get());
	sim->init();
}

void UIContext::simulation_stop() {
	m_sim_circuit = nullptr;
	m_circuit_editor->set_simulation_instance(nullptr);
	m_sub_circuit_views.clear();
	m_lsim_context->sim()->clear_components();
}

void UIContext::create_sub_circuit_view(SimCircuit* parent_sim, ModelComponent *model_comp) {
	auto nested_model = model_comp->nested_circuit();
	auto nested_sim = parent_sim->component_by_id(model_comp->id())->nested_instance();

	auto sub_circuit = CircuitEditorFactory::create_circuit(nested_model);
	sub_circuit->set_simulation_instance(nested_sim, true);

	m_sub_circuit_views.push_back(move(sub_circuit));
}

void UIContext::foreach_sub_circuit_view(const std::function<bool(CircuitEditor*)> &callback) {
	for (auto iter = m_sub_circuit_views.begin(); iter != m_sub_circuit_views.end();) {
		auto keep_open = callback(iter->get());
		
		if (!keep_open) {
			iter = m_sub_circuit_views.erase(iter);
		} else {
			++iter;
		}
	}
}

} // namespace lsim::gui

} // namespace lsim
