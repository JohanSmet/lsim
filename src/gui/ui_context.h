// ui_context.h - Johan Smet - BSD-3-Clause (see LICENSE)

#ifndef LSIM_GUI_UI_CONTEXT_H
#define LSIM_GUI_UI_CONTEXT_H

#include "std_helper.h"
#include <functional>
#include <list>

namespace lsim {

class LSimContext;
class ModelCircuit;
class ModelComponent;
class SimCircuit;

namespace gui {

class CircuitEditor;

class UIContext {
public:
	UIContext(LSimContext* lsim_context);

	// accessors
	LSimContext* lsim_context() const { return m_lsim_context; }
	std::string library_filename() const { return m_lib_filename; }
	int selected_circuit_idx() const { return m_selected_circuit_idx; }
	CircuitEditor* circuit_editor() const { return m_circuit_editor.get(); }
	SimCircuit* sim_circuit() const { return m_sim_circuit.get(); }

	// library management
	void circuit_library_load(const std::string& filename);
	void circuit_library_save(const std::string& filename);
	void circuit_library_close();

	// circuit management
	void change_active_circuit(ModelCircuit* circuit);
	void change_selected_circuit_idx(int delta) { m_selected_circuit_idx += delta; }

	// simulation control
	void simulation_start();
	void simulation_stop();

	// sub-circuit views
	void create_sub_circuit_view(SimCircuit* sim_circuit, ModelComponent *model_comp);
	void foreach_sub_circuit_view(std::function<bool(CircuitEditor*)> callback);

private:
	LSimContext*							m_lsim_context;
	std::string								m_lib_filename = "";
	int										m_selected_circuit_idx = 0;
	unique_ptr<CircuitEditor>				m_circuit_editor = nullptr;
	unique_ptr<SimCircuit>					m_sim_circuit = nullptr;
	std::list<unique_ptr<CircuitEditor>>	m_sub_circuit_views;
};

} // namespace lsim::gui

} // namespace lsim

#endif // LSIM_GUI_UI_CONTEXT_H
