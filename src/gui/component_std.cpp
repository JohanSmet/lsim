// component_std.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// registration of standard components

#include "component_std.h"
#include "component_ui.h"
#include "imgui_ex.h"
#include "shapes.h"
#include "colors.h"

#include "basic.h"
#include "circuit.h"
#include "gate.h"

#include <algorithm>

void materialize_rectangle(UIComponent *ui_comp, UICircuit *ui_circuit, float width, float height) {

    const float h_width = width / 2.0f;
    const float h_height = height / 2.0f;

    // transform coordinates from component space to circuit space
	ui_comp->m_to_circuit.reset();
	ui_comp->m_to_circuit.rotate(ui_comp->m_visual_comp->get_orientation());
	ui_comp->m_to_circuit.translate(ui_comp->m_visual_comp->get_position());

    // compute axis aligned bounding box
	ui_comp->m_circuit_min = ui_comp->m_to_circuit.apply({-h_width, -h_height});
	ui_comp->m_circuit_max = ui_comp->m_to_circuit.apply({h_width, h_height});

	if (ui_comp->m_circuit_max.x < ui_comp->m_circuit_min.x) {
        std::swap(ui_comp->m_circuit_min.x, ui_comp->m_circuit_max.x);
	}

	if (ui_comp->m_circuit_max.y < ui_comp->m_circuit_min.y) {
        std::swap(ui_comp->m_circuit_min.y, ui_comp->m_circuit_max.y);
	}
}

void materialize_gate(UIComponent *ui_comp, UICircuit *ui_circuit, 
                      Component::pin_container_t pins_input,
                      Component::pin_container_t pins_output,
                      Component::pin_container_t pins_control,
                      float min_width = 80, float min_height = 60) {

    // size of component box depends on pin count
    const float width = std::max(min_width, 15.0f * pins_control.size());
    const float height = std::max(min_height, 15.0f * std::max(pins_input.size(), pins_output.size()));
    const float h_width = width / 2.0f;
    const float h_height = height / 2.0f;

    materialize_rectangle(ui_comp, ui_circuit, width, height);

    // pins
    ui_circuit->add_pin_line(ui_comp->m_to_circuit, pins_input.data(), pins_input.size(), 
                             height, {-h_width, -h_height}, {0, 1});
    ui_circuit->add_pin_line(ui_comp->m_to_circuit, pins_output.data(), pins_output.size(), 
                             height, {h_width, -h_height}, {0, 1});
    ui_circuit->add_pin_line(ui_comp->m_to_circuit, pins_control.data(), pins_control.size(), 
                             width, {-h_width, -h_height}, {1, 0});
}

const char *connector_data_label(Value value) {
    switch (value) {
        case VALUE_FALSE:
            return "0";
        case VALUE_TRUE:
            return "1";
        case VALUE_UNDEFINED:
            return "?";
        default:
            return "E";
    }
}

void component_register_basic() {

    // connector
    UICircuitBuilder::register_materialize_func(
        VisualComponent::CONNECTOR, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            auto connector = dynamic_cast<Connector *>(comp);
            ui_comp->m_tooltip = "Connector";

            const float width = 26;
            const float height = comp->num_pins() * 24;
            materialize_rectangle(ui_comp, ui_circuit, width, height);
            ui_circuit->add_pin_line(ui_comp->m_to_circuit, comp->pins().data(), comp->num_pins(), 
                                     {(connector->is_input() ? 0.5f : -0.5f) * width, -height * 0.5f + 12},
                                     {0, 24});

            ui_comp->m_custom_ui_callback = [=](const UIComponent *ui_comp) {

                auto origin = ImGui::GetCursorPos();

                ImGui::BeginGroup();
                ImGui::PushID(connector->name());
                for (size_t i = 0; i < comp->num_pins(); ++ i) {
                    ImGui::PushID(i);
                    auto cur_val = connector->get_data(i);

                    if (connector->is_input()) {
                        ImGui::SetCursorPos(ImVec2(origin.x + 3, origin.y + (i * 24) + 2));
                        if (ImGui::Button(connector_data_label(cur_val), {18,18})) {
                            connector->change_data(i, static_cast<Value>((cur_val + 1) % (connector->is_tristate() ? 3 : 2)));
                        }
                    }

                    if (connector->is_output()) {
                        auto screen_pos = ImGui::GetCursorScreenPos();
                        ImGui::GetWindowDrawList()->AddRectFilled(
                            {screen_pos.x + 3, screen_pos.y + 3}, {screen_pos.x + 22, screen_pos.y + 22},
                            COLOR_CONNECTION[cur_val]);
                        ImGuiEx::TextCentered(ImVec2(origin.x + 13, origin.y + 5 + (i * 24)), connector_data_label(cur_val));
                    }

                    ImGui::PopID();
                }
                ImGui::PopID();
                if (connector->is_input()) {
                    ImGuiEx::TextRightJustify({origin.x - 5, origin.y + (height / 2.0f) - 3}, connector->name());
                } else {
                    ImGuiEx::TextLeftJustify({origin.x + width + 5, origin.y + (height / 2.0f) - 3}, connector->name());
                }
                ImGui::EndGroup();
            };
        }
    );

    // constant
    UICircuitBuilder::register_materialize_func(
        VisualComponent::CONSTANT, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            Constant *constant = dynamic_cast<Constant *>(comp);

            const float width = 26;
            const float height = 26;
            materialize_rectangle(ui_comp, ui_circuit, width, height);
            ui_circuit->add_pin_line(ui_comp->m_to_circuit, comp->pins().data(), comp->num_pins(), 
                                     {0.5f * width, -height * 0.5f + 12}, {0, 24});

            ui_comp->m_custom_ui_callback = [=](const UIComponent *ui_comp) {
                auto origin = ImGui::GetCursorPos();
                auto val = constant->value();

                auto screen_pos = ImGui::GetCursorScreenPos();
                ImGui::GetWindowDrawList()->AddRectFilled(
                    {screen_pos.x + 3, screen_pos.y + 3}, {screen_pos.x + 22, screen_pos.y + 22},
                    COLOR_CONNECTION[val]);
                ImGuiEx::TextCentered(ImVec2(origin.x + 13, origin.y + 5), connector_data_label(val));
            };
        }
    );

    // sub circuit
    UICircuitBuilder::register_materialize_func(
        VisualComponent::SUB_CIRCUIT, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            CircuitComponent *wrapper = dynamic_cast<CircuitComponent *>(comp);
            Circuit *nested = wrapper->nested_circuit();
            ui_comp->m_tooltip = "Circuit";

            // sort sub-circuit pins into inputs (on the left) and outputs (on the right) 
            Component::pin_container_t input_pins, output_pins;
            std::vector<const char *> input_names, output_names;

            for (int i = nested->num_interface_pins() - 1; i >= 0; --i) {
                if (nested->interface_pin_is_input(i)) {
                    input_pins.push_back(nested->interface_pin(i));
                    input_names.push_back(nested->interface_pin_name(i));
                } else {
                    output_pins.push_back(nested->interface_pin(i));
                    output_names.push_back(nested->interface_pin_name(i));
                }
            }

            // materialize the sub-circuit
            float width = 100;
            float height = (std::max(input_pins.size(), output_pins.size()) + 1) * 24 + 24;

            materialize_rectangle(ui_comp, ui_circuit, width, height);
            ui_circuit->add_pin_line(ui_comp->m_to_circuit, input_pins.data(), input_pins.size(),
                                     {-width/2.0f, -height/2.0f + 24}, {0.0f, 24.0f});
            ui_circuit->add_pin_line(ui_comp->m_to_circuit, output_pins.data(), output_pins.size(),
                                     {width/2.0f, -height/2.0f + 24}, {0.0f, 24.0f});

            // custom draw function for pin labels
            ui_comp->m_custom_ui_callback = [=](const UIComponent *ui_comp) {
                auto origin = ImGui::GetCursorPos();

                auto cursor = ImVec2(origin.x + 5, origin.y + 18);
                for (const auto &name : input_names) {
                    ImGuiEx::TextLeftJustify(cursor, name);
                    cursor.y += 24;
                }

                cursor = ImVec2(origin.x + width - 5, origin.y + 18);
                for (const auto &name : output_names) {
                    ImGuiEx::TextRightJustify(cursor, name);
                    cursor.y += 24;
                }

                cursor = ImVec2(origin.x + width/2.0f, origin.y + height - 18);
                ImGuiEx::TextCentered(cursor, nested->name());
            };
        }
    );

}

void component_register_gates() {

    // buffer
    ComponentIcon icon_buffer(SHAPE_BUFFER, sizeof(SHAPE_BUFFER));
    UICircuitBuilder::register_materialize_func(
        VisualComponent::BUFFER, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->m_tooltip = "Buffer";
            ui_comp->m_icon = &icon_buffer;
            int bits = comp->num_pins() / 2;
            materialize_gate(ui_comp, ui_circuit, comp->pins(0, bits-1), comp->pins(bits, bits*2-1), {});
        }
    );

    // tristate buffer
	ComponentIcon icon_tristate_buffer(SHAPE_TRISTATE_BUFFER, sizeof(SHAPE_TRISTATE_BUFFER));
    UICircuitBuilder::register_materialize_func(
        VisualComponent::TRISTATE_BUFFER, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->m_tooltip = "Tri-state Buffer";
            ui_comp->m_icon = &icon_tristate_buffer;
            int bits = (comp->num_pins() - 1) / 2;
            materialize_gate(ui_comp, ui_circuit, comp->pins(0, bits-1), comp->pins(bits+1, bits*2), comp->pins(bits, bits));
        }
    );

    // AND gate
	ComponentIcon icon_and(SHAPE_AND_GATE, sizeof(SHAPE_AND_GATE));
    UICircuitBuilder::register_materialize_func(
        VisualComponent::AND_GATE, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->m_tooltip = "AND";
            ui_comp->m_icon = &icon_and;
            int inputs = comp->num_pins() - 1;
            materialize_gate(ui_comp, ui_circuit, comp->pins(0, inputs-1), comp->pins(inputs, inputs), {});
        }
    );

    // OR gate
	ComponentIcon icon_or(SHAPE_OR_GATE, sizeof(SHAPE_OR_GATE));
    UICircuitBuilder::register_materialize_func(
        VisualComponent::OR_GATE, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->m_tooltip = "OR";
            ui_comp->m_icon = &icon_or;
            int inputs = comp->num_pins() - 1;
            materialize_gate(ui_comp, ui_circuit, comp->pins(0, inputs-1), comp->pins(inputs, inputs), {});
        }
    );

    // NOT gate
	ComponentIcon icon_not(SHAPE_NOT_GATE, sizeof(SHAPE_NOT_GATE));
    UICircuitBuilder::register_materialize_func(
        VisualComponent::NOT_GATE, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->m_tooltip = "NOT";
            ui_comp->m_icon = &icon_not;
            int inputs = comp->num_pins() - 1;
            materialize_gate(ui_comp, ui_circuit, comp->pins(0, inputs-1), comp->pins(inputs, inputs), {});
        }
    );

    // NAND gate
	ComponentIcon icon_nand(SHAPE_NAND_GATE, sizeof(SHAPE_NAND_GATE));
    UICircuitBuilder::register_materialize_func(
        VisualComponent::NAND_GATE, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->m_tooltip = "NAND";
            ui_comp->m_icon = &icon_nand;
            int inputs = comp->num_pins() - 1;
            materialize_gate(ui_comp, ui_circuit, comp->pins(0, inputs-1), comp->pins(inputs, inputs), {});
        }
    );

    // NOR gate
	ComponentIcon icon_nor(SHAPE_NOR_GATE, sizeof(SHAPE_NOR_GATE));
    UICircuitBuilder::register_materialize_func(
        VisualComponent::NOR_GATE, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->m_tooltip = "NOR";
            ui_comp->m_icon = &icon_nor;
            int inputs = comp->num_pins() - 1;
            materialize_gate(ui_comp, ui_circuit, comp->pins(0, inputs-1), comp->pins(inputs, inputs), {});
        }
    );

    // XOR gate
	ComponentIcon icon_xor(SHAPE_XOR_GATE, sizeof(SHAPE_XOR_GATE));
    UICircuitBuilder::register_materialize_func(
        VisualComponent::XOR_GATE, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->m_tooltip = "XOR";
            ui_comp->m_icon = &icon_xor;
            int inputs = comp->num_pins() - 1;
            materialize_gate(ui_comp, ui_circuit, comp->pins(0, inputs-1), comp->pins(inputs, inputs), {});
        }
    );

    // XNOR gate
	ComponentIcon icon_xnor(SHAPE_XNOR_GATE, sizeof(SHAPE_XNOR_GATE));
    UICircuitBuilder::register_materialize_func(
        VisualComponent::XNOR_GATE, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->m_tooltip = "XNOR";
            ui_comp->m_icon = &icon_xnor;
            int inputs = comp->num_pins() - 1;
            materialize_gate(ui_comp, ui_circuit, comp->pins(0, inputs-1), comp->pins(inputs, inputs), {});
        }
    );
}