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
    auto circuit = sim->create_circuit();
    sim->set_main_circuit(circuit);
    auto buffer = circuit->create_component<Buffer>(8);
    buffer->create_visual({100,100}, VisualComponent::SOUTH);


	const auto &comps = sim->get_main_circuit()->visual_components();
	ui_circuit = UICircuitBuilder::create_circuit("main", comps.begin(), comps.end());
}

void main_gui(Simulator *sim)
{
	ImGui::SetNextWindowSize(ImVec2(700,600), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Circuit");

	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

	ui_circuit->draw();

	ImGui::EndChild();

	ImGui::End();
}