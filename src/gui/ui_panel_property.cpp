// ui_panel_property.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "imgui_ex.h"

#include "component_widget.h"
#include "circuit_editor.h"
#include "lsim_context.h"
#include "ui_context.h"

namespace lsim {

namespace gui {

void ui_panel_property(UIContext* ui_context) {

	auto circuit_editor = ui_context->circuit_editor();
	auto context = ui_context->lsim_context();

	if (circuit_editor != nullptr && circuit_editor->selected_widget()) {
		auto ui_comp = circuit_editor->selected_widget();
		auto component = ui_comp->component_model();

		// helper functions
		auto text_property = [](const char* caption, Property* property) -> bool {
			std::string value = property->value_as_string();
			std::vector<char> buffer(value.begin(), value.end());
			buffer.resize(256);

			if (ImGui::InputText(caption, buffer.data(), buffer.size(), ImGuiInputTextFlags_EnterReturnsTrue)) {
				property->value(buffer.data());
				return true;
			}
			else {
				return false;
			}
		};

		auto boolean_property = [](const char* caption, Property* property) {
			bool value = property->value_as_boolean();
			if (ImGui::Checkbox(caption, &value)) {
				property->value(value);
				return true;
			}
			else {
				return false;
			}
		};

		auto integer_property = [](const char* caption, Property* property) {
			int value = static_cast<int> (property->value_as_integer());
			if (ImGui::InputInt(caption, &value)) {
				property->value(static_cast<int64_t>(value));
				return true;
			}
			else {
				return false;
			}
		};

		auto value_property = [](const char* caption, Property* property) -> Value {
			int cur_value = static_cast<int> (property->value_as_integer());
			static const char *value_labels[] = {"False", "True", "Undefined", "Error"};

			if (ImGui::Combo(caption, &cur_value, value_labels, 4)) {
				property->value(static_cast<int64_t>(cur_value));
			}
			return property->value_as_lsim_value();
		};

		// orientation - present for all components
		int cur_orientation = component->angle() / 90;
		const char* orientations[] = { "East", "South", "West", "North" };
		if (ImGui::Combo("Orientation", &cur_orientation, orientations, sizeof(orientations) / sizeof(orientations[0]))) {
			component->set_angle(cur_orientation * 90);
			ui_comp->build_transform();
		}

		// component specific fields
		if (component->type() == COMPONENT_CONNECTOR_IN || component->type() == COMPONENT_CONNECTOR_OUT || component->type() == COMPONENT_VIA) {
			if (text_property("Name", component->property("name"))) {
				circuit_editor->model_circuit()->rebuild_port_list();
			}
			if (component->type() != COMPONENT_VIA) {
				boolean_property("TriState", component->property("tri_state"));

				if (boolean_property("Descending", component->property("descending"))) {
					CircuitEditorFactory::rematerialize_component(circuit_editor, ui_comp);
					circuit_editor->ui_fix_widget_connections(ui_comp);
				}
			}
			else {
				if (boolean_property("Right pin", component->property("right"))) {
					CircuitEditorFactory::rematerialize_component(circuit_editor, ui_comp);
					circuit_editor->ui_fix_widget_connections(ui_comp);
				}
			}

			int data_bits = component->num_inputs() + component->num_outputs();
			int org_bits = data_bits;
			if (ImGui::InputInt("Data Bits", &data_bits)) {
				if (data_bits <= 0) {
					data_bits = 1;
				}
				circuit_editor->model_circuit()->change_port_pin_count(component->id(), data_bits);

				// update the position of the connector so the endpoints do not move
				auto dir = (component->angle() == 0 || component->angle() == 180) ? Point(0, 10) : Point(10, 0);
				component->set_position(component->position() + (dir * (data_bits - org_bits)));
				ui_comp->build_transform();

				CircuitEditorFactory::rematerialize_component(circuit_editor, ui_comp);
				circuit_editor->ui_fix_widget_connections(ui_comp);
			}
		}

		if (component->type() == COMPONENT_CONSTANT) {
			value_property("Value", component->property("value"));
		}

		if (component->type() == COMPONENT_PULL_RESISTOR) {
			value_property("Value", component->property("pull_to"));
		}

		if (component->type() == COMPONENT_BUFFER || component->type() == COMPONENT_TRISTATE_BUFFER) {
			int data_bits = component->num_inputs();

			if (ImGui::InputInt("Data Bits", &data_bits)) {
				if (data_bits <= 0) {
					data_bits = 1;
				}
				component->change_input_pins(data_bits);
				component->change_output_pins(data_bits);
				CircuitEditorFactory::rematerialize_component(circuit_editor, ui_comp);
			}
		}

		if (component->type() == COMPONENT_AND_GATE ||
			component->type() == COMPONENT_OR_GATE ||
			component->type() == COMPONENT_NAND_GATE ||
			component->type() == COMPONENT_NOR_GATE) {
			int num_inputs = component->num_inputs();
			if (ImGui::SliderInt("Inputs", &num_inputs, 2, 8)) {
				component->change_input_pins(num_inputs);
				CircuitEditorFactory::rematerialize_component(circuit_editor, ui_comp);
			}
		}

		if (component->type() == COMPONENT_OSCILLATOR) {
			auto low = component->property("low_duration");
			int64_t min_value = 1;
			if (integer_property("Low Duration", low)) {
				if (low->value_as_integer() < min_value) {
					low->value(min_value);
				}
			};
			auto high = component->property("high_duration");
			if (integer_property("High Duration", high)) {
				if (high->value_as_integer() < min_value) {
					high->value(min_value);
				}
			}
		}

		if (component->type() == COMPONENT_SUB_CIRCUIT) {
			text_property("Caption", component->property("caption"));

			if (boolean_property("Flip", component->property("flip"))) {
				CircuitEditorFactory::rematerialize_component(circuit_editor, ui_comp);
				circuit_editor->ui_fix_widget_connections(ui_comp);
			}
		}

		if (component->type() == COMPONENT_TEXT) {
			if (text_property("Value", component->property("text"))) {
				CircuitEditorFactory::rematerialize_component(circuit_editor, ui_comp);
			}
		}

		// initial output state
		if (component->property("initial_output")) {
			value_property("Initial Output", component->property("initial_output"));
		}


	}
	else if (circuit_editor != nullptr) {
		// no component selected - edit circuit properties
		std::vector<char> name(circuit_editor->model_circuit()->name().begin(), circuit_editor->model_circuit()->name().end());
		name.resize(256);

		if (ImGui::InputText("Name", name.data(), name.size(), ImGuiInputTextFlags_EnterReturnsTrue)) {
			context->user_library()->rename_circuit(circuit_editor->model_circuit(), name.data());
		}

		bool main_circuit = context->user_library()->main_circuit() == circuit_editor->model_circuit();
		if (main_circuit) {
			ImGui::Text("This is the main circuit");
		}
		else {
			if (ImGui::Button("Set as main circuit")) {
				context->user_library()->change_main_circuit(circuit_editor->model_circuit()->name().c_str());
			}
		}
	}
}

} // namespace lsim::gui

} // namespace lsim
