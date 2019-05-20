#include "main_gui.h"
#include "imgui_ex.h"

#include "component_ui.h"
#include "component_std.h"

#include "lsim_context.h"
#include "gate.h"
#include "extra.h"
#include "load_logisim.h"
#include "colors.h"

#include "serialize.h"

static UICircuit::uptr_t ui_circuit = nullptr;
static std::string ui_filename = "";

void change_active_circuit(Simulator *sim, Circuit *active_circuit) {
	if (active_circuit && active_circuit != sim->active_circuit()) {
		sim->change_active_circuit(active_circuit);
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

		if (lsim_context->user_library()->num_circuits() <= 0) {
			lsim_context->user_library()->create_circuit("main");
		}

		if (lsim_context->user_library()->main_circuit()) {
			change_active_circuit(lsim_context->sim(), lsim_context->user_library()->main_circuit());
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
		serialize_library(lsim_context, lib, ui_filename.c_str());
	}

	if (sim_running) {
		sim->step();
	}

	ImGui::BeginChild("left_pane", ImVec2(150, 0), true);

	/////////////////////////////////////////////////////////////////////////////////
	//
	// Circuit interaction (select / create / delete)
	//

	if (ImGui::CollapsingHeader("Circuits", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::Button("Up")) {
			size_t selected_circuit = lib->circuit_idx(sim->active_circuit());
			if (selected_circuit < lib->num_circuits() && selected_circuit > 0) {
				lib->swap_circuits(selected_circuit, selected_circuit - 1);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Down")) {
			size_t selected_circuit = lib->circuit_idx(sim->active_circuit());
			if (selected_circuit < lib->num_circuits() - 1) {
				lib->swap_circuits(selected_circuit, selected_circuit + 1);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Add")) {
			auto circuit = lib->create_circuit((std::string("circuit#") + std::to_string(lib->num_circuits()+1)).c_str());
			change_active_circuit(sim, circuit);
		}
		if (lib->num_circuits() > 1 && lib->main_circuit() != sim->active_circuit()) {
			ImGui::SameLine();
			if (ImGui::Button("Delete")) {
				lib->delete_circuit(sim->active_circuit());
				change_active_circuit(sim, lib->main_circuit());
			}
		}

		size_t selected_circuit = (sim->active_circuit() == nullptr) ? -1 : lib->circuit_idx(sim->active_circuit());
		for (size_t i = 0; i < lib->num_circuits(); ++i) {
			auto circuit = lib->circuit_by_idx(i);
			if (ImGui::Selectable(circuit->name().c_str(), selected_circuit == i)) {
				selected_circuit = i;
				change_active_circuit(sim, circuit);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////
	//
	// Component creation
	//

	auto draw_list = ImGui::GetWindowDrawList();

	auto create_component = [=](Component *component) {
		auto vis_comp = sim->active_circuit()->create_visual_component(component);
		vis_comp->set_position({-200, -200});
		ui_circuit->create_component(vis_comp);
	};

	auto add_component_button = [=](uint32_t component, const char *caption, Component * (create_func)(Circuit *circuit)) {
		Point pos = ImGui::GetCursorScreenPos();
		ImGui::PushID(caption);
		if (ImGui::Button("", {40, 40})) {
			create_component(create_func(sim->active_circuit()));
		}
		auto icon = ComponentIcon::cached(component);
		if (icon) {
			icon->draw(pos + Point(20,20), {34, 34}, draw_list, 1, COLOR_COMPONENT_BORDER);
		}
		ImGuiEx::Text(pos + Point(50,20), caption, ImGuiEx::TAH_LEFT, ImGuiEx::TAV_CENTER);
		ImGui::SetCursorScreenPos(pos + Point(0,42));
		ImGui::PopID();
	};

	ImGui::Spacing();
	if (ImGui::CollapsingHeader("Gates", ImGuiTreeNodeFlags_DefaultOpen)) {
		add_component_button(COMPONENT_AND_GATE, "AND", [](Circuit *circuit) {return AndGate(circuit, 2);});
		add_component_button(COMPONENT_OR_GATE, "OR", [](Circuit *circuit) {return OrGate(circuit, 2);});
		add_component_button(COMPONENT_NOT_GATE, "NOT", [](Circuit *circuit) {return NotGate(circuit);});
		add_component_button(COMPONENT_NAND_GATE, "NAND", [](Circuit *circuit) {return NandGate(circuit, 2);});
		add_component_button(COMPONENT_NOR_GATE, "NOR", [](Circuit *circuit) {return NorGate(circuit, 2);});
		add_component_button(COMPONENT_XOR_GATE, "XOR", [](Circuit *circuit) {return XorGate(circuit);});
		add_component_button(COMPONENT_XNOR_GATE, "XNOR", [](Circuit *circuit) {return XnorGate(circuit);});
		add_component_button(COMPONENT_BUFFER, "Buffer", [](Circuit *circuit) {return Buffer(circuit, 1);});
		add_component_button(COMPONENT_TRISTATE_BUFFER, "TriState Buffer", [](Circuit *circuit) {return TriStateBuffer(circuit, 1);});
	}

	ImGui::Spacing();
	if (ImGui::CollapsingHeader("Various", ImGuiTreeNodeFlags_DefaultOpen)) {
		add_component_button(COMPONENT_CONNECTOR_IN, "Input", [](Circuit *circuit) {return ConnectorInput(circuit, "in", 1);});
		add_component_button(COMPONENT_CONNECTOR_OUT, "Output", [](Circuit *circuit) {return ConnectorOutput(circuit, "out", 1);});
		add_component_button(COMPONENT_CONSTANT, "Constant", [](Circuit *circuit) {return Constant(circuit, VALUE_TRUE);});
		add_component_button(COMPONENT_PULL_RESISTOR, "PullResistor", [](Circuit *circuit) {return PullResistor(circuit, VALUE_TRUE);});
	}

	ImGui::EndChild();	// left_pane
	ImGui::SameLine();


	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

	if (ui_circuit) {
		ui_circuit->draw();
	}

	ImGui::EndChild();	// scrolling region

	ImGui::End();

	/////////////////////////////////////////////////////////////////////////////////
	//
	// property window
	//

	ImGui::SetNextWindowSize(ImVec2(250,100), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Properties");
	auto sel_ui_comp = ui_circuit->selected_component();
	if (sel_ui_comp) {
		auto visual_comp = sel_ui_comp->visual_comp();
		auto component = visual_comp->get_component();

		// helper functions
		auto text_property = [](const char *caption, Property *property) {
			std::string value = property->value_as_string();
			std::vector<char> buffer(value.begin(), value.end());
			buffer.resize(256);

			if (ImGui::InputText(caption, buffer.data(), buffer.size(), ImGuiInputTextFlags_EnterReturnsTrue)) {
				property->value(buffer.data());
			}
		};

		auto boolean_property = [](const char *caption, Property *property) {
			bool value = property->value_as_boolean();
			if (ImGui::Checkbox(caption, &value)) {
				property->value(value);
			}
		};

		auto value_property = [](const char *caption, Property *property) -> Value {
			int cur_value = property->value_as_integer();
			const char *value_labels[] = {"False", "True", "Undefined", "Error"};
			if (ImGui::Combo(caption, &cur_value, value_labels, 4)) {
				property->value(static_cast<int64_t>(cur_value));
			}
			return static_cast<Value>(cur_value);
		};

		// orientation - present for all components
		int cur_orientation = visual_comp->get_orientation() / 90;
		const char *orientations[] = {"East", "South", "West", "North"};
		if (ImGui::Combo("Orientation", &cur_orientation, orientations, sizeof(orientations) / sizeof(orientations[0]))) {
			visual_comp->set_orientation(static_cast<VisualComponent::Orientation>(cur_orientation * 90));
			sel_ui_comp->build_transform();
		}

		// component specific fields
		if (component->type() == COMPONENT_CONNECTOR_IN || component->type() == COMPONENT_CONNECTOR_OUT) {
			text_property("Name", component->property("name"));
			boolean_property("TriState", component->property("tri_state"));

			int data_bits = component->num_pins();
			if (ImGui::InputInt("Data Bits", &data_bits)) {
				if (component->type() == COMPONENT_CONNECTOR_IN) {
					component->change_output_pins(data_bits);
				} else {
					component->change_input_pins(data_bits);
				}
				UICircuitBuilder::rematerialize_component(ui_circuit.get(), sel_ui_comp);
				ui_circuit->circuit()->add_ports(component->property_value("name",""), component);
				ui_circuit->circuit()->initialize_input_ports();
			}
		}

		if (component->type() == COMPONENT_CONSTANT) {
			auto new_value = value_property("Value", component->property("value"));
			component->write_pin(0, new_value);
		}

		if (component->type() == COMPONENT_PULL_RESISTOR) {
			auto new_value = value_property("Value", component->property("pull_to"));
			component->write_pin(0, new_value);
		}

		if (component->type() == COMPONENT_BUFFER || component->type() == COMPONENT_TRISTATE_BUFFER) {
			int data_bits = component->num_input_pins();

			if (ImGui::InputInt("Data Bits", &data_bits)) {
				if (data_bits <= 0) {
					data_bits = 1;
				}
				component->change_input_pins(data_bits);
				component->change_output_pins(data_bits);
				UICircuitBuilder::rematerialize_component(ui_circuit.get(), sel_ui_comp);
			}
		}

		if (component->type() == COMPONENT_AND_GATE ||
		    component->type() == COMPONENT_OR_GATE ||
			component->type() == COMPONENT_NAND_GATE ||
			component->type() == COMPONENT_NOR_GATE) {
			int num_inputs = component->num_input_pins();
			if (ImGui::SliderInt("Inputs", &num_inputs, 2, 8)) {
				component->change_input_pins(num_inputs);
				UICircuitBuilder::rematerialize_component(ui_circuit.get(), sel_ui_comp);
			}
		}
	} else {
		// no component selected - edit circuit properties
		std::vector<char> name(ui_circuit->circuit()->name().begin(), ui_circuit->circuit()->name().end());
		name.resize(256);

		if (ImGui::InputText("Name", name.data(), name.size(), ImGuiInputTextFlags_EnterReturnsTrue)) {
			ui_circuit->circuit()->change_name(name.data());
		}

		bool main_circuit = lsim_context->user_library()->main_circuit() == ui_circuit->circuit();
		if (main_circuit) {
			ImGui::Text("This is the main circuit");
		} else {
			if (ImGui::Button("Set as main circuit")) {
				lsim_context->user_library()->set_main_circuit(ui_circuit->circuit());
			}
		}
	}
	ImGui::End();
}