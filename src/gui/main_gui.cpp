#include "main_gui.h"
#include "imgui/imgui.h"

#include "component_ui.h"
#include "component_std.h"

#include "lsim_context.h"
#include "gate.h"
#include "load_logisim.h"

#include "serialize.h"

static UICircuit::uptr_t ui_circuit = nullptr;
static std::string ui_filename = "";

void handle_main_circuit_changed(Simulator *sim) {
	auto active_circuit = sim->active_circuit();
	if (active_circuit) {
		ui_circuit = UICircuitBuilder::create_circuit(active_circuit);
		active_circuit->initialize_input_ports();
	}
}

void main_gui_setup(LSimContext *lsim_context, const char *circuit_file) {
	component_register_basic();
	component_register_extra();
	component_register_gates();

	// try to load the circuit specified on the command line
	if (circuit_file) {
		ui_filename = circuit_file;
		auto ext = ui_filename.substr(ui_filename.find_last_of('.') + 1);

		if (ext == "circ") {
			auto circuit = load_logisim(lsim_context, circuit_file);
			ui_filename.replace(ui_filename.begin() + ui_filename.find_last_of('.'), ui_filename.end(), ".lsim");
		} else {
			deserialize_library(lsim_context, lsim_context->user_library(), circuit_file);
		}

		if (lsim_context->user_library()->main_circuit()) {
			lsim_context->sim()->change_active_circuit(lsim_context->user_library()->main_circuit());
			handle_main_circuit_changed(lsim_context->sim());
		}
	}
}

void main_gui(LSimContext *lsim_context)
{
	auto sim = lsim_context->sim();
	auto lib = lsim_context->user_library();

	ImGui::SetNextWindowSize(ImVec2(700,600), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Circuit", nullptr, ImGuiWindowFlags_NoScrollWithMouse);

	static bool sim_running = ui_circuit.get() != nullptr;
	ImGui::Checkbox("Run simulation", &sim_running);
	ImGui::SameLine();
	if (ImGui::Button("Reset simulation")) {
		sim->init(sim->active_circuit());
	}
	ImGui::SameLine();
	if (ImGui::Button("Step")) {
		sim->step();
	}
	ImGui::SameLine();
	if (ImGui::Button("Save")) {
		serialize_library(lsim_context, lsim_context->user_library(), ui_filename.c_str());
	}

	if (sim_running) {
		sim->step();
	}

	ImGui::BeginChild("left_pane", ImVec2(150, 0), true);
	ImGui::Text("Circuits");
	static size_t selected_circuit = (sim->active_circuit() == nullptr) ? -1 : lib->circuit_idx(sim->active_circuit());
	for (size_t i = 0; i < lib->num_circuits(); ++i) {
		auto circuit = lib->circuit_by_idx(i);
		if (ImGui::Selectable(circuit->name(), selected_circuit == i)) {
			selected_circuit = i;
			sim->change_active_circuit(circuit);
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