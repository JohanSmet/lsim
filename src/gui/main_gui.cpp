#include "main_gui.h"
#include "imgui_ex.h"

#include "component_ui.h"
#include "component_std.h"

#include "lsim_context.h"
#include "load_logisim.h"
#include "colors.h"

#include "serialize.h"

#include <set>

namespace {

using namespace lsim;
using namespace lsim::gui;

static UICircuit::uptr_t ui_circuit = nullptr;
static std::string ui_filename = "";
static int selected_circuit_idx = 0;
static const char *value_labels[] = {"False", "True", "Undefined", "Error"};

static void change_active_circuit(Simulator *sim, CircuitDescription *active_circuit) {
	if (ui_circuit != nullptr && ui_circuit->circuit_desc() == active_circuit) {
		return;
	}

	if (active_circuit) {
		// XXX sim->change_active_circuit(active_circuit);
		ui_circuit = UICircuitBuilder::create_circuit(active_circuit);
		// XXX active_circuit->initialize_input_ports();
	}
}

static void init_input_connectors(CircuitDescription *desc, CircuitInstance *inst) {
	auto connector_ids = desc->component_ids_of_type(COMPONENT_CONNECTOR_IN);

	for (const auto &id : connector_ids) {
		auto connector = desc->component_by_id(id);
		auto pin = connector->output_pin_id(0);
		for (size_t idx = 0; idx < connector->num_outputs(); ++idx) {
			inst->write_pin(pin++, VALUE_FALSE);
		}
	}
}

} // unnamed namespace

namespace lsim {

namespace gui {

void debug_window(LSimContext *lsim_context) {
	ImGui::SetNextWindowSize(ImVec2(250,100), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Debug");

	/*auto sim = lsim_context->sim();

	if (ImGui::CollapsingHeader("Nodes", ImGuiTreeNodeFlags_DefaultOpen)) {
		
		std::set<node_t> free_nodes(sim->free_nodes().begin(), sim->free_nodes().end());

		for (size_t node_id = 0; node_id < sim->num_nodes(); ++node_id) {
			if (free_nodes.find(node_id) != free_nodes.end()) {
				continue;
			}

			auto value = sim->read_node(node_id);
			if (ImGui::TreeNode(reinterpret_cast<void *>(node_id), "Node %.2d\tValue = %s", node_id, value_labels[value])) {
				ImGui::Text("Time last written : %d", sim->node_last_written(node_id));
				ImGui::Text("Time last changed : %d", sim->node_last_changed(node_id));
				ImGui::TreePop();			
			}
		}
	}*/

	ImGui::End();
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
			// auto circuit = load_logisim(lsim_context, circuit_file);
			// ui_filename.replace(ui_filename.begin() + ui_filename.find_last_of('.'), ui_filename.end(), ".lsim");
		} else {
			deserialize_library(lsim_context, lsim_context->user_library(), circuit_file);
		}

		auto main_circuit = lsim_context->user_library()->main_circuit();
		selected_circuit_idx = lsim_context->user_library()->circuit_idx(main_circuit);
		change_active_circuit(lsim_context->sim(), main_circuit);
	}
}

void main_gui(LSimContext *lsim_context)
{
	auto sim = lsim_context->sim();
	auto lib = lsim_context->user_library();

	static bool sim_running = false;

	ImGui::SetNextWindowSize(ImVec2(700,600), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Circuit", nullptr, ImGuiWindowFlags_NoScrollWithMouse);

	if (ImGui::Button("Save")) {
		serialize_library(lsim_context, lib, ui_filename.c_str());
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Editor", !ui_circuit->is_simulating())) {
		ui_circuit->change_simulation_status(false, sim);
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Simulation", ui_circuit->is_simulating())) {
		ui_circuit->change_simulation_status(true, sim);
		init_input_connectors(ui_circuit->circuit_desc(), ui_circuit->circuit_inst());
		sim_running = true;
		sim->init();
	}
	if (ui_circuit->is_simulating()) {
		ImGui::SameLine();
		ImGui::Checkbox("Run simulation", &sim_running);
		ImGui::SameLine();
		if (ImGui::Button("Reset simulation")) {
			sim->init();
		}
		ImGui::SameLine();
		if (ImGui::Button("Step")) {
			sim->step();
		}
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
			if (selected_circuit_idx < lib->num_circuits() && selected_circuit_idx > 0) {
				lib->swap_circuits(selected_circuit_idx, selected_circuit_idx - 1);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Down")) {
			if (selected_circuit_idx < lib->num_circuits() - 1) {
				lib->swap_circuits(selected_circuit_idx, selected_circuit_idx + 1);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Add")) {
			auto circuit = lib->create_circuit((std::string("circuit#") + std::to_string(lib->num_circuits()+1)).c_str(), lsim_context);
			if (!lib->main_circuit()) {
				lib->change_main_circuit(circuit->name().c_str());
			}
			change_active_circuit(sim, circuit);
		}
		if (lib->num_circuits() > 1 && lib->main_circuit() != lib->circuit_by_idx(selected_circuit_idx)) {
			ImGui::SameLine();
			if (ImGui::Button("Delete")) {
				lib->delete_circuit(lib->circuit_by_idx(selected_circuit_idx));
				change_active_circuit(sim, lib->main_circuit());
			}
		}

		for (size_t i = 0; i < lib->num_circuits(); ++i) {
			auto circuit = lib->circuit_by_idx(i);
			if (ImGui::Selectable(circuit->name().c_str(), selected_circuit_idx == i)) {
				selected_circuit_idx = i;
				change_active_circuit(sim, circuit);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////
	//
	// Component creation
	//

	auto draw_list = ImGui::GetWindowDrawList();

	auto add_component_button = [=](uint32_t component, const char *caption, Component * (create_func)(CircuitDescription *circuit)) {
		Point pos = ImGui::GetCursorScreenPos();
		ImGui::PushID(caption);
		if (ImGui::Button("", {40, 40})) {
			if (!ui_circuit->is_simulating()) {
				auto component = create_func(ui_circuit->circuit_desc());
				component->set_position({-200, -200});
				ui_circuit->ui_create_component(component);
			}
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
		add_component_button(COMPONENT_AND_GATE, "AND", [](CircuitDescription *circuit) {return circuit->add_and_gate(2);});
		add_component_button(COMPONENT_OR_GATE, "OR", [](CircuitDescription *circuit) {return circuit->add_or_gate(2);});
		add_component_button(COMPONENT_NOT_GATE, "NOT", [](CircuitDescription *circuit) {return circuit->add_not_gate();});
		add_component_button(COMPONENT_NAND_GATE, "NAND", [](CircuitDescription *circuit) {return circuit->add_nand_gate(2);});
		add_component_button(COMPONENT_NOR_GATE, "NOR", [](CircuitDescription *circuit) {return circuit->add_nor_gate(2);});
		add_component_button(COMPONENT_XOR_GATE, "XOR", [](CircuitDescription *circuit) {return circuit->add_xor_gate();});
		add_component_button(COMPONENT_XNOR_GATE, "XNOR", [](CircuitDescription *circuit) {return circuit->add_xnor_gate();});
		add_component_button(COMPONENT_BUFFER, "Buffer", [](CircuitDescription *circuit) {return circuit->add_buffer(1);});
		add_component_button(COMPONENT_TRISTATE_BUFFER, "TriState Buffer", [](CircuitDescription *circuit) {return circuit->add_tristate_buffer(1);});
	}

	ImGui::Spacing();
	if (ImGui::CollapsingHeader("Various", ImGuiTreeNodeFlags_DefaultOpen)) {
		add_component_button(COMPONENT_CONNECTOR_IN, "Input", [](CircuitDescription *circuit) {return circuit->add_connector_in("in", 1);});
		add_component_button(COMPONENT_CONNECTOR_OUT, "Output", [](CircuitDescription *circuit) {return circuit->add_connector_out("out", 1);});
		add_component_button(COMPONENT_CONSTANT, "Constant", [](CircuitDescription *circuit) {return circuit->add_constant(VALUE_TRUE);});
		add_component_button(COMPONENT_PULL_RESISTOR, "PullResistor", [](CircuitDescription *circuit) {return circuit->add_pull_resistor(VALUE_TRUE);});
	}

	ImGui::EndChild();	// left_pane
	ImGui::SameLine();

	/////////////////////////////////////////////////////////////////////////////////
	//
	// circuit editor 
	//

	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

	if (ui_circuit) {
		ui_circuit->draw();
	}

	if (ImGui::BeginPopupContextWindow("circuit_context")) {
		ImGui::Text("Choose circuit to embed:");

		// for now only allow to embed circuits that are define before the current circuit
		//	- there's no circular dependency checking (yet)
		//	- the serializer/deserialiser isn't smart enough to handle other case 
		for (size_t i = 0; i < selected_circuit_idx; ++i) {
			auto sub = lib->circuit_by_idx(i);
			if (ImGui::Selectable(sub->name().c_str())) {
				ui_circuit->embed_circuit(sub->name().c_str());
			}
		}

		ImGui::EndPopup();
	}

	ImGui::EndChild();	// scrolling region

	ImGui::End();

	/////////////////////////////////////////////////////////////////////////////////
	//
	// property window
	//

	ImGui::SetNextWindowSize(ImVec2(250,100), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Properties");
	if (ui_circuit != nullptr && ui_circuit->selected_component()) {
		auto sel_ui_comp = ui_circuit->selected_component();
		auto component = sel_ui_comp->component();

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
			if (ImGui::Combo(caption, &cur_value, value_labels, 4)) {
				property->value(static_cast<int64_t>(cur_value));
			}
			return static_cast<Value>(cur_value);
		};

		// orientation - present for all components
		int cur_orientation = component->angle() / 90;
		const char *orientations[] = {"East", "South", "West", "North"};
		if (ImGui::Combo("Orientation", &cur_orientation, orientations, sizeof(orientations) / sizeof(orientations[0]))) {
			component->set_angle(cur_orientation * 90);
			sel_ui_comp->build_transform();
		}

		// component specific fields
		if (component->type() == COMPONENT_CONNECTOR_IN || component->type() == COMPONENT_CONNECTOR_OUT) {
			text_property("Name", component->property("name"));
			boolean_property("TriState", component->property("tri_state"));

			int data_bits = component->num_inputs() + component->num_outputs();
			if (ImGui::InputInt("Data Bits", &data_bits)) {
				if (component->type() == COMPONENT_CONNECTOR_IN) {
					// XXX component->change_output_pins(data_bits);
				} else {
					// XXX component->change_input_pins(data_bits);
				}
				UICircuitBuilder::rematerialize_component(ui_circuit.get(), sel_ui_comp);
				// XXX ui_circuit->circuit()->add_ports(component->property_value("name",""), component);
				// XXX ui_circuit->circuit()->initialize_input_ports();
			}
		}

		if (component->type() == COMPONENT_CONSTANT) {
			auto new_value = value_property("Value", component->property("value"));
			// XXX component->write_pin(0, new_value);
		}

		if (component->type() == COMPONENT_PULL_RESISTOR) {
			auto new_value = value_property("Value", component->property("pull_to"));
			// XXX component->write_pin(0, new_value);
		}

		if (component->type() == COMPONENT_BUFFER || component->type() == COMPONENT_TRISTATE_BUFFER) {
			int data_bits = component->num_inputs();

			if (ImGui::InputInt("Data Bits", &data_bits)) {
				if (data_bits <= 0) {
					data_bits = 1;
				}
				// XXX component->change_input_pins(data_bits);
				// XXX component->change_output_pins(data_bits);
				UICircuitBuilder::rematerialize_component(ui_circuit.get(), sel_ui_comp);
			}
		}

		if (component->type() == COMPONENT_AND_GATE ||
		    component->type() == COMPONENT_OR_GATE ||
			component->type() == COMPONENT_NAND_GATE ||
			component->type() == COMPONENT_NOR_GATE) {
			int num_inputs = component->num_inputs();
			if (ImGui::SliderInt("Inputs", &num_inputs, 2, 8)) {
				// XXX component->change_input_pins(num_inputs);
				UICircuitBuilder::rematerialize_component(ui_circuit.get(), sel_ui_comp);
			}
		}

	} else if (ui_circuit != nullptr) {
		// no component selected - edit circuit properties
		std::vector<char> name(ui_circuit->circuit_desc()->name().begin(), ui_circuit->circuit_desc()->name().end());
		name.resize(256);

		if (ImGui::InputText("Name", name.data(), name.size(), ImGuiInputTextFlags_EnterReturnsTrue)) {
			// FIXME: name does not get changed in the library
			ui_circuit->circuit_desc()->change_name(name.data());
		}

		bool main_circuit = lsim_context->user_library()->main_circuit() == ui_circuit->circuit_desc();
		if (main_circuit) {
			ImGui::Text("This is the main circuit");
		} else {
			if (ImGui::Button("Set as main circuit")) {
				lsim_context->user_library()->change_main_circuit(ui_circuit->circuit_desc()->name().c_str());
			}
		}
	}
	ImGui::End();

	debug_window(lsim_context);
}

} // namespace lsim

} // namespace gui