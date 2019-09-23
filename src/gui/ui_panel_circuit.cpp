// ui_panel_circuit.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "imgui_ex.h"

#include "circuit_editor.h"
#include "lsim_context.h"
#include "ui_context.h"

namespace lsim {

namespace gui {

void ui_panel_circuit(UIContext* ui_context) {

	auto lib = ui_context->lsim_context()->user_library();
	const auto selected_circuit_idx = ui_context->selected_circuit_idx();

	// move up
	if (ImGui::Button("Up")) {
		if (selected_circuit_idx < lib->num_circuits() && selected_circuit_idx > 0) {
			lib->swap_circuits(selected_circuit_idx, selected_circuit_idx - 1);
			ui_context->change_selected_circuit_idx(-1);
		}
	}

	// move down
	ImGui::SameLine();
	if (ImGui::Button("Down")) {
		if (selected_circuit_idx < lib->num_circuits() - 1) {
			lib->swap_circuits(selected_circuit_idx, selected_circuit_idx + 1);
			ui_context->change_selected_circuit_idx(+1);
		}
	}

	// add a new circuit to the library
	ImGui::SameLine();
	if (ImGui::Button("Add")) {
		auto new_name = std::string("circuit#") + std::to_string(lib->num_circuits() + 1);
		auto circuit = lib->create_circuit(new_name.c_str(), ui_context->lsim_context());
		if (!lib->main_circuit()) {
			lib->change_main_circuit(circuit->name().c_str());
		}
		ui_context->change_active_circuit(circuit);
	}

	// remove circuit from library if allowed
	if (lib->num_circuits() > 1 && lib->main_circuit() != lib->circuit_by_idx(selected_circuit_idx)) {
		ImGui::SameLine();
		if (ImGui::Button("Delete")) {
			lib->delete_circuit(lib->circuit_by_idx(selected_circuit_idx));
			ui_context->change_active_circuit(lib->main_circuit());
		}
	}

	// list of circuits in the user library
	ImGui::Separator();

	for (size_t i = 0; i < lib->num_circuits(); ++i) {
		auto circuit = lib->circuit_by_idx(i);
		if (ImGui::Selectable(circuit->name().c_str(), selected_circuit_idx == i)) {
			ui_context->change_active_circuit(circuit);
		}
	}

	ImGui::Separator();
}

} // namespace lsim::gui

} // namespace lsim

