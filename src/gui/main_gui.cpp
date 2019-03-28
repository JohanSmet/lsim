#include "main_gui.h"
#include "imgui/imgui.h"

#include "component_ui.h"
#include "component_std.h"

#include "simulator.h"
#include "circuit.h"
#include "gate.h"

static UICircuit::uptr_t ui_circuit = nullptr;

void main_gui_setup(Simulator *sim) {
	component_register_basic();
	component_register_gates();

	// create a test circuit for now
    auto circuit = sim->create_circuit("main");
    sim->set_main_circuit(circuit);
    auto buffer = circuit->create_component<Buffer>(8);
    buffer->create_visual({100,100}, VisualComponent::SOUTH);


	const auto &comps = sim->get_main_circuit()->visual_components();
	ui_circuit = UICircuitBuilder::create_circuit(circuit, comps.begin(), comps.end());

	sim->init();
}

void main_gui(Simulator *sim)
{
	ImGui::SetNextWindowSize(ImVec2(700,600), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Circuit");

	static bool sim_running = true;
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
	static size_t selected_circuit = 0;
	for (size_t i = 0; i < sim->num_circuits(); ++i) {
		auto circuit = sim->circuit_by_idx(i);
		if (ImGui::Selectable(circuit->name(), selected_circuit == i)) {
			selected_circuit = i;
			sim->set_main_circuit(circuit);
		}
	}
	ImGui::EndChild();
	ImGui::SameLine();

	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

	ui_circuit->draw();

	ImGui::EndChild();

	ImGui::End();
}