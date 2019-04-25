#include "main_gui.h"
#include "imgui/imgui.h"

#include "component_ui.h"
#include "component_std.h"

#include "simulator.h"
#include "circuit.h"
#include "gate.h"
#include "load_logisim.h"

static UICircuit::uptr_t ui_circuit = nullptr;

void handle_main_circuit_changed(Simulator *sim) {
	auto active_circuit = sim->active_circuit();
	ui_circuit = UICircuitBuilder::create_circuit(active_circuit);
	active_circuit->initialize_interface_pins();
	sim->init();
}

void main_gui_setup(Simulator *sim, const char *circuit_file) {
	component_register_basic();
	component_register_extra();
	component_register_gates();

	// try to load the circuit specified on the command line
	if (circuit_file) {
		load_logisim(sim, circuit_file);
		handle_main_circuit_changed(sim);
	}
}

void main_gui(Simulator *sim)
{
	ImGui::SetNextWindowSize(ImVec2(700,600), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Circuit");

	static bool sim_running = ui_circuit.get() != nullptr;
	ImGui::Checkbox("Run simulation", &sim_running);
	ImGui::SameLine();
	if (ImGui::Button("Reset simulation")) {
		sim->init();
	}
	ImGui::SameLine();
	if (ImGui::Button("Step")) {
		sim->step();
	}

	if (sim_running) {
		sim->step();
	}

	ImGui::BeginChild("left_pane", ImVec2(150, 0), true);
	ImGui::Text("Circuits");
	static size_t selected_circuit = sim->active_circuit_index();
	for (size_t i = 0; i < sim->num_circuits(); ++i) {
		auto circuit = sim->circuit_by_idx(i);
		if (ImGui::Selectable(circuit->name(), selected_circuit == i)) {
			selected_circuit = i;
			sim->set_active_circuit(circuit);
			handle_main_circuit_changed(sim);
		}
	}
	ImGui::EndChild();
	ImGui::SameLine();

	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

	if (ui_circuit) {
		ui_circuit->draw();
	}

	ImGui::EndChild();

	ImGui::End();
}