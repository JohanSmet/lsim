// ui_panel_component_.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "imgui_ex.h"

#include "component_icon.h"
#include "colors.h"
#include "circuit_editor.h"
#include "lsim_context.h"
#include "ui_context.h"

namespace lsim {

namespace gui {

void ui_panel_component(UIContext* ui_context) {

	auto circuit_editor = ui_context->circuit_editor();
	auto context = ui_context->lsim_context();

	auto add_component_button = [circuit_editor](uint32_t component, const char* caption, std::function<ModelComponent* (ModelCircuit*)> create_func) {
		Point pos = ImGui::GetCursorScreenPos();
		auto draw_list = ImGui::GetWindowDrawList();

		ImGui::PushID((std::string("lsim_") + caption).c_str());
		if (ImGui::Button("", { 40, 40 })) {
			if (!circuit_editor->is_simulating()) {
				auto component = create_func(circuit_editor->model_circuit());
				component->set_position({ -200, -200 });
				circuit_editor->ui_create_component(component);
			}
		}
		auto icon = ComponentIcon::cached(component);
		if (icon) {
			icon->draw(pos + Point(20, 20), { 34, 34 }, draw_list, 1, COLOR_COMPONENT_BORDER);
		}
		ImGuiEx::Text(pos + Point(50, 20), caption, ImGuiEx::TAH_LEFT, ImGuiEx::TAV_CENTER);
		ImGui::SetCursorScreenPos(pos + Point(0, 42));
		ImGui::PopID();
	};

	ImGui::Spacing();
	if (ImGui::TreeNodeEx("Gates", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
		ImGui::BeginGroup();
		ImGui::Indent();
		add_component_button(COMPONENT_AND_GATE, "AND", [](ModelCircuit* circuit) {return circuit->add_and_gate(2); });
		add_component_button(COMPONENT_OR_GATE, "OR", [](ModelCircuit* circuit) {return circuit->add_or_gate(2); });
		add_component_button(COMPONENT_NOT_GATE, "NOT", [](ModelCircuit* circuit) {return circuit->add_not_gate(); });
		add_component_button(COMPONENT_NAND_GATE, "NAND", [](ModelCircuit* circuit) {return circuit->add_nand_gate(2); });
		add_component_button(COMPONENT_NOR_GATE, "NOR", [](ModelCircuit* circuit) {return circuit->add_nor_gate(2); });
		add_component_button(COMPONENT_XOR_GATE, "XOR", [](ModelCircuit* circuit) {return circuit->add_xor_gate(); });
		add_component_button(COMPONENT_XNOR_GATE, "XNOR", [](ModelCircuit* circuit) {return circuit->add_xnor_gate(); });
		add_component_button(COMPONENT_BUFFER, "Buffer", [](ModelCircuit* circuit) {return circuit->add_buffer(1); });
		add_component_button(COMPONENT_TRISTATE_BUFFER, "TriState Buffer", [](ModelCircuit* circuit) {return circuit->add_tristate_buffer(1); });
		ImGui::EndGroup();
	}

	ImGui::Spacing();
	if (ImGui::TreeNodeEx("Various", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
		ImGui::BeginGroup();
		ImGui::Indent();
		add_component_button(COMPONENT_CONNECTOR_IN, "Input", [](ModelCircuit* circuit) {return circuit->add_connector_in("in", 1); });
		add_component_button(COMPONENT_CONNECTOR_OUT, "Output", [](ModelCircuit* circuit) {return circuit->add_connector_out("out", 1); });
		add_component_button(COMPONENT_CONSTANT, "Constant", [](ModelCircuit* circuit) {return circuit->add_constant(VALUE_TRUE); });
		add_component_button(COMPONENT_PULL_RESISTOR, "PullResistor", [](ModelCircuit* circuit) {return circuit->add_pull_resistor(VALUE_TRUE); });
		add_component_button(COMPONENT_VIA, "Via", [](ModelCircuit* circuit) {return circuit->add_via("via", 1); });
		add_component_button(COMPONENT_OSCILLATOR, "Oscillator", [](ModelCircuit* circuit) {return circuit->add_oscillator(5, 5); });
		add_component_button(COMPONENT_TEXT, "Text", [](ModelCircuit* circuit) {return circuit->add_text("text"); });
		ImGui::EndGroup();
	}

	ImGui::Spacing();
	if (ImGui::TreeNodeEx("I/O", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
		ImGui::BeginGroup();
		ImGui::Indent();
		add_component_button(COMPONENT_7_SEGMENT_LED, "7-segment LED", [](ModelCircuit* circuit) {return circuit->add_7_segment_led(); });
		ImGui::EndGroup();
	}

	for (const auto& ref : context->user_library()->references()) {
		ImGui::Spacing();
		if (ImGui::TreeNodeEx(ref.c_str(), ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
			auto ref_lib = context->library_by_name(ref.c_str());

			ImGui::BeginGroup();
			ImGui::Indent();
			for (size_t idx = 0; idx < ref_lib->num_circuits(); ++idx) {
				auto sub_name = ref_lib->circuit_by_idx(idx)->name();
				auto full_name = ref + ".";
				full_name += sub_name;
				add_component_button(COMPONENT_SUB_CIRCUIT, sub_name.c_str(),
					[full_name](ModelCircuit* circuit) {
						return circuit->add_sub_circuit(full_name.c_str());
					});
			}
			ImGui::EndGroup();
		}
	}
}

} // namespace lsim::gui

} // namespace lsim
