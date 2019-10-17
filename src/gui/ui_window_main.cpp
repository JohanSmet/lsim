// ui_window_main.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "ui_window_main.h"
#include "imgui_ex.h"

#include "component_draw.h"
#include "component_widget.h"
#include "circuit_editor.h"

#include "lsim_context.h"
#include "sim_circuit.h"
#include "ui_context.h"

namespace {

lsim::LSimContext lsim_context;
lsim::gui::UIContext ui_context(&lsim_context);

} // unnamed namespace

namespace lsim {

namespace gui {

void ui_panel_component(UIContext* ui_context);
void ui_panel_circuit(UIContext* ui_context);
void ui_panel_library(UIContext* ui_context);
void ui_panel_property(UIContext* ui_context);

void main_window_setup(const char *circuit_file) {
	component_register_basic();
	component_register_extra();
	component_register_gates();
	component_register_input_output();

	ui_context.lsim_context()->add_folder("examples", "./examples");

	// try to load the circuit specified on the command line
	if (circuit_file != nullptr) {
		ui_context.circuit_library_load(circuit_file);
	}
}

void main_window_update() {
	auto sim = ui_context.lsim_context()->sim();

	///////////////////////////////////////////////////////////////////////////
	//
	// control window
	//

	ImGui::SetNextWindowPos({0,0}, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize({268, ImGui::GetIO().DisplaySize.y}, ImGuiCond_FirstUseEver);


	ImGui::Begin("Control");

		// Library management
		ui_panel_library(&ui_context);
	
		// Circuit management
		ImGui::Spacing();
		if (ImGui::CollapsingHeader("Circuits", ImGuiTreeNodeFlags_DefaultOpen)) {
			ui_panel_circuit(&ui_context);
		}

		// Component pallette
		ImGui::Spacing();
		if (ImGui::CollapsingHeader("Components", ImGuiTreeNodeFlags_DefaultOpen)) {
			ui_panel_component(&ui_context);
		}

		// Properties
		ImGui::Spacing();
		if (ImGui::CollapsingHeader("Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
			ui_panel_property(&ui_context);
		}

	ImGui::End();

	///////////////////////////////////////////////////////////////////////////
	//
	// main circuit window
	//

	static bool sim_running = false;
	static bool sim_single_step = false;
	static int cycles_per_frame = 5;

	ImGui::SetNextWindowPos({268, 0}, ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize({ImGui::GetIO().DisplaySize.x-268, ImGui::GetIO().DisplaySize.y}, ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Circuit", nullptr, ImGuiWindowFlags_NoScrollWithMouse);

		if (ImGui::RadioButton("Editor", !ui_context.circuit_editor()->is_simulating())) {
			ui_context.simulation_stop();
		}
		ImGui::SameLine();

		if (ImGui::RadioButton("Simulation", ui_context.circuit_editor()->is_simulating())) {
			ui_context.simulation_start();
		}

		if (ui_context.circuit_editor()->is_simulating()) {
			ImGui::SameLine();
			ImGui::Checkbox("Run simulation", &sim_running);
			ImGui::SameLine();
			if (ImGui::Button("Reset simulation")) {
				sim->init();
			}
			ImGui::SameLine();
			sim_single_step = ImGui::Button("Step");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80);
			if (ImGui::InputInt("Cycles per frame", &cycles_per_frame)) {
				if (cycles_per_frame <= 0) {
					cycles_per_frame = 1;
				}
			}
		}

		if (sim_single_step) {
			sim->step();
			sim_single_step = false;
		} else if (sim_running && ui_context.sim_circuit() != nullptr) {
			for (int i = 0; i < cycles_per_frame; ++i) {
				sim->step();
			}
		}

		if (ui_context.circuit_editor() != nullptr) {
			ui_context.circuit_editor()->refresh(&ui_context);
		}

	ImGui::End();

	// display windows for drill-downs
	ui_context.foreach_sub_circuit_view([](auto* drill_down) {
		bool keep_open = true;

		ImGui::SetNextWindowSize(drill_down->circuit_dimensions() + Point(50,50), ImGuiCond_Appearing);
		ImGui::Begin(drill_down->sim_circuit()->name(), &keep_open, ImGuiWindowFlags_NoScrollWithMouse);
			drill_down->refresh(&ui_context);
		ImGui::End();

		return keep_open;
	});
}

} // namespace lsim::gui

} // namespace lsim