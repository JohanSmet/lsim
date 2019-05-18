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
#include "extra.h"
#include "gate.h"

#include <algorithm>

void materialize_gate(UIComponent *ui_comp, UICircuit *ui_circuit, 
                      Component::pin_container_t pins_input,
                      Component::pin_container_t pins_output,
                      Component::pin_container_t pins_control,
                      float min_width = 60, float min_height = 60) {

    // size of component box depends on pin count
    const float width = std::max(min_width, 10.0f * pins_control.size());
    const float height = std::max(min_height, 10.0f * std::max(pins_input.size(), pins_output.size()));
    const float h_width = width / 2.0f;
    const float h_height = height / 2.0f;

    ui_comp->change_size(width, height);

    // pins
    ui_comp->add_pin_line(pins_input.data(), pins_input.size(), 
                          height, {-h_width, -h_height}, {0, 1});
    ui_comp->add_pin_line(pins_output.data(), pins_output.size(), 
                          height, {h_width, -h_height}, {0, 1});
    ui_comp->add_pin_line(pins_control.data(), pins_control.size(), 
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

    // connector input
    UICircuitBuilder::register_materialize_func(
        COMPONENT_CONNECTOR_IN, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->change_tooltip("Input");

            const float width = 26;
            const float height = comp->num_pins() * 24;
            ui_comp->change_size(width, height);
            ui_comp->add_pin_line(comp->pins().data(), comp->num_pins(), 
                                  {0.5f * width, -height * 0.5f + 12},
                                  {0, 24});

            // custor draw function
            ui_comp->set_custom_ui_callback([=](const UIComponent *ui_comp, Transform to_window) {

                bool is_tristate = comp->property_value("tri_state", false);
                auto origin = Point(-width / 2.0f, -height / 2.0f);

                // hack to get ImGui::Button to display at the right position
                auto orient = ui_comp->visual_comp()->get_orientation();
                Point button_offset = 
                    (orient == VisualComponent::WEST) ? Point(18, 18) : 
                    (orient == VisualComponent::NORTH) ? Point(0, 18) :
                    (orient == VisualComponent::SOUTH) ? Point(18, 0) :
                    Point(0, 0);

                ImGui::BeginGroup();
                ImGui::PushID(comp);
                for (size_t i = 0; i < comp->num_pins(); ++ i) {
                    ImGui::PushID(i);
                    auto cur_val = comp->read_value(comp->output_pin_index(i)); 
                    auto screen_pos = to_window.apply(Point(origin.x + 4, origin.y + (i * 24) + 2));
                    ImGui::SetCursorScreenPos(screen_pos - button_offset);

                    if (ImGui::Button(connector_data_label(cur_val), {18,18})) {
                        comp->write_pin(comp->output_pin_index(i), 
                                        static_cast<Value>((cur_val + 1) % (is_tristate ? 3 : 2)));
                    }
                    ImGui::PopID();
                }
                ImGui::PopID();

                // label for connector
                Point anchor = to_window.apply(Point(origin.x - 5, origin.y + (height / 2.0f) - 4));
                ImGuiEx::TextJustify justify = ImGuiEx::CENTER;
                if (orient == VisualComponent::EAST) {
                    justify = ImGuiEx::RIGHT;
                } else if (orient == VisualComponent::WEST) {
                    justify = ImGuiEx::LEFT;
                } else if (orient == VisualComponent::SOUTH) {
                    anchor.y -= 10;
                }

                ImGuiEx::Text(anchor, justify, comp->property_value("name","").c_str());

                ImGui::EndGroup();
            });
        }
    );

    // connector output
    UICircuitBuilder::register_materialize_func(
        COMPONENT_CONNECTOR_OUT, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->change_tooltip("Output");

            const float width = 26;
            const float height = comp->num_pins() * 24;
            ui_comp->change_size(width, height);
            ui_comp->add_pin_line(comp->pins().data(), comp->num_pins(), 
                                  {-0.5f * width, -height * 0.5f + 12},
                                  {0, 24});

            // custor draw function
            ui_comp->set_custom_ui_callback([=](const UIComponent *ui_comp, Transform to_window) {

                auto origin = Point(-width / 2.0f, -height / 2.0f);
                auto orient = ui_comp->visual_comp()->get_orientation();

                ImGui::BeginGroup();
                ImGui::PushID(comp);
                for (size_t i = 0; i < comp->num_pins(); ++ i) {
                    ImGui::PushID(i);
                    auto cur_val = comp->read_pin(comp->input_pin_index(i));
                    auto screen_pos = to_window.apply(Point(origin.x, origin.y + (i * 24)));

                    ImGui::GetWindowDrawList()->AddRectFilled(
                        {screen_pos.x + 3, screen_pos.y + 3}, {screen_pos.x + 22, screen_pos.y + 22},
                        COLOR_CONNECTION[cur_val]);
                    ImGuiEx::TextCentered(screen_pos + Point(width / 2, 5), connector_data_label(cur_val));

                    ImGui::PopID();
                }
                ImGui::PopID();

                // label for connector
                Point anchor = to_window.apply(Point(
                    origin.x + width + 5,
                    origin.y + (height / 2.0f) - 4
                ));
                ImGuiEx::TextJustify justify = ImGuiEx::CENTER;
                if (orient == VisualComponent::EAST) {
                    justify = ImGuiEx::LEFT;
                } else if (orient == VisualComponent::WEST) {
                    justify = ImGuiEx::RIGHT;
                } else if (orient == VisualComponent::SOUTH) {
                    anchor.y -= 10;
                }

                ImGuiEx::Text(anchor, justify, comp->property_value("name", "").c_str());

                ImGui::EndGroup();
            });
        }
    );


    // constant
    UICircuitBuilder::register_materialize_func(
        COMPONENT_CONSTANT, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            const float width = 26;
            const float height = 26;
            ui_comp->change_size(width, height);
            ui_comp->add_pin_line(comp->pins().data(), comp->num_pins(), 
                                  {0.5f * width, -height * 0.5f + 12}, {0, 24});

            // custor draw function
            ui_comp->set_custom_ui_callback([=](const UIComponent *ui_comp, Transform to_window) {
                auto val = comp->read_value(comp->output_pin_index(0));
                auto screen_pos = to_window.apply(Point(-width / 2.0f, -height / 2.0f));
                ImGui::GetWindowDrawList()->AddRectFilled(
                    {screen_pos.x + 3, screen_pos.y + 3}, {screen_pos.x + 22, screen_pos.y + 22},
                    COLOR_CONNECTION[val]);
                ImGuiEx::Text(screen_pos + Point(13, 5), connector_data_label(val), ImGuiEx::TAH_CENTER);
            });
        }
    );

    // sub circuit
    UICircuitBuilder::register_materialize_func(
        COMPONENT_SUB_CIRCUIT, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            auto nested = ui_comp->visual_comp()->get_circuit();
            ui_comp->change_tooltip("Circuit");

            // materialize the sub-circuit
            float width = 200;
            float height = (std::max(nested->num_input_ports(), nested->num_output_ports()) + 1) * 20 + 20;

            ui_comp->change_size(width, height);
            ui_comp->add_pin_line(nested->input_ports_pins().data(), nested->num_input_ports(),
                                  {-width/2.0f, -height/2.0f + 20}, {0.0f, 20.0f});
            ui_comp->add_pin_line(nested->output_ports_pins().data(), nested->num_output_ports(),
                                  {width/2.0f, -height/2.0f + 20}, {0.0f, 20.0f});

            // custom draw function for pin labels
            ui_comp->set_custom_ui_callback([=](const UIComponent *ui_comp, Transform to_window) {
                auto cursor = to_window.apply(Point((-width / 2.0f) + 5, (-height / 2.0f) + 12));
                auto delta = Point(0, 20);
            
                for (size_t idx = 0; idx < nested->num_input_ports(); ++idx) {
                    ImGuiEx::Text(cursor, nested->input_port_name(idx));
                    cursor = cursor + delta;
                }

                cursor = to_window.apply(Point((width / 2.0f) - 5, (-height / 2.0f) + 12));
                for (size_t idx = 0; idx < nested->num_output_ports(); ++idx) {
                    ImGuiEx::Text(cursor, nested->output_port_name(idx), ImGuiEx::TAH_RIGHT);
                    cursor = cursor + delta;
                }

                cursor = to_window.apply(Point(0, (height / 2.0f) - 18));
                ImGuiEx::Text(cursor, nested->name(), ImGuiEx::TAH_CENTER);
            });
        }
    ); 
}

void component_register_extra() {

    // Pull Resistor
    UICircuitBuilder::register_materialize_func(
        COMPONENT_PULL_RESISTOR, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            const float width = 26;
            const float height = 26;
            ui_comp->change_size(width, height);
            ui_comp->add_pin_line(comp->pins().data(), comp->num_pins(), 
                                  {0.5f * width, -height * 0.5f + 12}, {0, 24});

            // custor draw function
            ui_comp->set_custom_ui_callback([=](const UIComponent *ui_comp, Transform to_window) {
                auto val = comp->read_value(comp->output_pin_index(0));
                auto screen_origin = to_window.apply(Point(-width / 2.0f, -height / 2.0f));
                ImGuiEx::RectFilled(
                    screen_origin + to_window.apply_to_vector({3, 3}),
                    screen_origin + to_window.apply_to_vector({22, 22}),
                    COLOR_CONNECTION[val]);
                ImGuiEx::Text(screen_origin + to_window.apply_to_vector({13, 5}), connector_data_label(val), ImGuiEx::TAH_CENTER);
            });
        }
    );
}

void component_register_gates() {

    // buffer
    auto icon_buffer = ComponentIcon::cache(COMPONENT_BUFFER, SHAPE_BUFFER, sizeof(SHAPE_BUFFER));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_BUFFER, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->change_tooltip("Buffer");
            ui_comp->change_icon(icon_buffer);
            int bits = comp->num_pins() / 2;
            materialize_gate(ui_comp, ui_circuit, comp->pins(0, bits-1), comp->pins(bits, bits*2-1), {});
        }
    );

    // tristate buffer
	auto icon_tristate_buffer = ComponentIcon::cache(COMPONENT_TRISTATE_BUFFER, SHAPE_TRISTATE_BUFFER, sizeof(SHAPE_TRISTATE_BUFFER));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_TRISTATE_BUFFER, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->change_tooltip("Tri-state Buffer");
            ui_comp->change_icon(icon_tristate_buffer);
            int bits = (comp->num_pins() - 1) / 2;
            materialize_gate(ui_comp, ui_circuit, comp->pins(0, bits-1), comp->pins(bits+1, bits*2), comp->pins(bits, bits));
        }
    );

    // AND gate
	auto icon_and = ComponentIcon::cache(COMPONENT_AND_GATE, SHAPE_AND_GATE, sizeof(SHAPE_AND_GATE));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_AND_GATE, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->change_tooltip("AND");
            ui_comp->change_icon(icon_and);
            int inputs = comp->num_pins() - 1;
            materialize_gate(ui_comp, ui_circuit, comp->pins(0, inputs-1), comp->pins(inputs, inputs), {});
        }
    );

    // OR gate
	auto icon_or = ComponentIcon::cache(COMPONENT_OR_GATE, SHAPE_OR_GATE, sizeof(SHAPE_OR_GATE));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_OR_GATE, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->change_tooltip("OR");
            ui_comp->change_icon(icon_or);
            int inputs = comp->num_pins() - 1;
            materialize_gate(ui_comp, ui_circuit, comp->pins(0, inputs-1), comp->pins(inputs, inputs), {});
        }
    );

    // NOT gate
	auto icon_not = ComponentIcon::cache(COMPONENT_NOT_GATE, SHAPE_NOT_GATE, sizeof(SHAPE_NOT_GATE));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_NOT_GATE, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->change_tooltip("NOT");
            ui_comp->change_icon(icon_not);
            int inputs = comp->num_pins() - 1;
            materialize_gate(ui_comp, ui_circuit, comp->pins(0, inputs-1), comp->pins(inputs, inputs), {}, 60, 40);
        }
    );

    // NAND gate
	auto icon_nand = ComponentIcon::cache(COMPONENT_NAND_GATE, SHAPE_NAND_GATE, sizeof(SHAPE_NAND_GATE));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_NAND_GATE, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->change_tooltip("NAND");
            ui_comp->change_icon(icon_nand);
            int inputs = comp->num_pins() - 1;
            materialize_gate(ui_comp, ui_circuit, comp->pins(0, inputs-1), comp->pins(inputs, inputs), {});
        }
    );

    // NOR gate
	auto icon_nor = ComponentIcon::cache(COMPONENT_NOR_GATE, SHAPE_NOR_GATE, sizeof(SHAPE_NOR_GATE));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_NOR_GATE, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->change_tooltip("NOR");
            ui_comp->change_icon(icon_nor);
            int inputs = comp->num_pins() - 1;
            materialize_gate(ui_comp, ui_circuit, comp->pins(0, inputs-1), comp->pins(inputs, inputs), {});
        }
    );

    // XOR gate
	auto icon_xor = ComponentIcon::cache(COMPONENT_XOR_GATE, SHAPE_XOR_GATE, sizeof(SHAPE_XOR_GATE));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_XOR_GATE, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->change_tooltip("XOR");
            ui_comp->change_icon(icon_xor);
            int inputs = comp->num_pins() - 1;
            materialize_gate(ui_comp, ui_circuit, comp->pins(0, inputs-1), comp->pins(inputs, inputs), {});
        }
    );

    // XNOR gate
	auto icon_xnor = ComponentIcon::cache(COMPONENT_XNOR_GATE, SHAPE_XNOR_GATE, sizeof(SHAPE_XNOR_GATE));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_XNOR_GATE, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->change_tooltip("XNOR");
            ui_comp->change_icon(icon_xnor);
            int inputs = comp->num_pins() - 1;
            materialize_gate(ui_comp, ui_circuit, comp->pins(0, inputs-1), comp->pins(inputs, inputs), {});
        }
    );
}