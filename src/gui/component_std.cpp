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
    const float width = std::max(min_width, 10.0f * (pins_control.size() + 1));
    const float height = std::max(min_height, 10.0f * (std::max(pins_input.size(), pins_output.size()) + 1));
    const float h_width = width / 2.0f;
    const float h_height = height / 2.0f;

    ui_comp->change_size(width, height);

    // pins
    ui_comp->add_pin_line(pins_input.data(), pins_input.size(), 
                          height, {-h_width, 0}, {0, 1});
    ui_comp->add_pin_line(pins_output.data(), pins_output.size(), 
                          height, {h_width, 0}, {0, 1});
    ui_comp->add_pin_line(pins_control.data(), pins_control.size(), 
                          width, {0, -h_height}, {1, 0});
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
    auto icon_connector_in = ComponentIcon::cache(COMPONENT_CONNECTOR_IN, SHAPE_CONNECTOR_IN, sizeof(SHAPE_CONNECTOR_IN));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_CONNECTOR_IN, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->change_tooltip("Input");

            const float width = 20;
            const float height = 20;
            const float full_height = comp->num_pins() * height;
            ui_comp->change_size(width, full_height);
            ui_comp->add_pin_line(comp->pins().data(), comp->num_pins(), 
                                  {0.5f * width, (-full_height * 0.5f) + (height * 0.5f)},
                                  {0, height});

            // custom draw function
            ui_comp->set_custom_ui_callback([=](const UIComponent *ui_comp, Transform to_window) {

                bool is_tristate = comp->property_value("tri_state", false);
                auto origin = Point(-width * 0.5f, -full_height * 0.5f);
                auto orient = ui_comp->visual_comp()->get_orientation();

                ImGui::BeginGroup();
                ImGui::PushID(comp);
                for (size_t i = 0; i < comp->num_pins(); ++ i) {
                    auto cur_val = comp->read_value(comp->output_pin_index(i)); 
                    auto center_pos = to_window.apply(Point(0, (-full_height * 0.5f) + ((i + 0.5f) * height)));
                    ImGui::SetCursorScreenPos(center_pos - Point(8,8));

                    ImGui::PushID(i);
                    if (ImGui::InvisibleButton(connector_data_label(cur_val), {16,16})) {
                        comp->write_pin(comp->output_pin_index(i), 
                                        static_cast<Value>((cur_val + 1) % (is_tristate ? 3 : 2)));
                    }

                    ImGuiEx::RectFilled(center_pos - Point(8,8), center_pos + Point(8,8), COLOR_CONNECTION[cur_val]);
                    ImGuiEx::Text(center_pos, connector_data_label(cur_val), ImGuiEx::TAH_CENTER, ImGuiEx::TAV_CENTER);
                    ImGui::PopID();
                }
                ImGui::PopID();

                // label for connector
                Point anchor = to_window.apply(Point(origin.x - 5, origin.y + (full_height / 2.0f)));

                static const std::pair<ImGuiEx::TextAlignHor, ImGuiEx::TextAlignVer> label_align[] = {
                    {ImGuiEx::TAH_RIGHT,    ImGuiEx::TAV_CENTER},   // East
                    {ImGuiEx::TAH_CENTER,   ImGuiEx::TAV_BOTTOM},   // South
                    {ImGuiEx::TAH_LEFT,     ImGuiEx::TAV_CENTER},   // West
                    {ImGuiEx::TAH_CENTER,   ImGuiEx::TAV_TOP}       // North
                };

                ImGuiEx::Text(anchor, comp->property_value("name","").c_str(), label_align[orient / 90].first, label_align[orient / 90].second);

                ImGui::EndGroup();
            });
        }
    );

    // connector output
    auto icon_connector_out = ComponentIcon::cache(COMPONENT_CONNECTOR_OUT, SHAPE_CONNECTOR_OUT, sizeof(SHAPE_CONNECTOR_OUT));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_CONNECTOR_OUT, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->change_tooltip("Output");

            const float width = 20;
            const float height = 20;
            const float full_height = comp->num_pins() * height;
            ui_comp->change_size(width, full_height);
            ui_comp->add_pin_line(comp->pins().data(), comp->num_pins(), 
                                  {-0.5f * width, -full_height * 0.5f + (height * 0.5f)},
                                  {0, height});

            // custom draw function
            ui_comp->set_custom_ui_callback([=](const UIComponent *ui_comp, Transform to_window) {

                auto origin = Point(-width * 0.5f, -full_height * 0.5f);
                auto orient = ui_comp->visual_comp()->get_orientation();

                ImGui::BeginGroup();
                ImGui::PushID(comp);
                for (size_t i = 0; i < comp->num_pins(); ++ i) {
                    auto cur_val = comp->read_pin(comp->input_pin_index(i));
                    auto center_pos = to_window.apply(Point(origin.x + (width * 0.5f), origin.y + (i * 20.0f) + (height * 0.5f)));

                    ImGui::PushID(i);
                    ImGuiEx::RectFilled(center_pos - Point(8,8), center_pos + Point(8,8), COLOR_CONNECTION[cur_val]);
                    ImGuiEx::Text(center_pos, connector_data_label(cur_val), ImGuiEx::TAH_CENTER, ImGuiEx::TAV_CENTER);
                    ImGui::PopID();
                }
                ImGui::PopID();

                // label for connector
                Point anchor = to_window.apply(origin + Point(width + 5, (full_height / 2.0f)));

                static const std::pair<ImGuiEx::TextAlignHor, ImGuiEx::TextAlignVer> label_align[] = {
                    {ImGuiEx::TAH_LEFT,     ImGuiEx::TAV_CENTER},   // East
                    {ImGuiEx::TAH_CENTER,   ImGuiEx::TAV_TOP},      // South
                    {ImGuiEx::TAH_RIGHT,    ImGuiEx::TAV_CENTER},   // West
                    {ImGuiEx::TAH_CENTER,   ImGuiEx::TAV_BOTTOM}    // North
                };
                ImGuiEx::Text(anchor, comp->property_value("name", "").c_str(), label_align[orient / 90].first, label_align[orient / 90].second);

                ImGui::EndGroup();
            });
        }
    );


    // constant
    auto icon_constant = ComponentIcon::cache(COMPONENT_CONSTANT, SHAPE_CONSTANT, sizeof(SHAPE_CONSTANT));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_CONSTANT, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            const float width = 20;
            const float height = 20;
            ui_comp->change_size(width, height);
            ui_comp->add_pin_line(comp->pins().data(), comp->num_pins(), 
                                  {0.5f * width, (-height * 0.5f) + (height * 0.5f)}, 
                                  {0, height});

            // custom draw function
            ui_comp->set_custom_ui_callback([=](const UIComponent *ui_comp, Transform to_window) {
                auto val = comp->read_value(comp->output_pin_index(0));
                auto center_pos = to_window.apply(Point(0.0f, 0.0f));
                ImGuiEx::RectFilled(center_pos - Point(8,8), center_pos + Point(8,8), COLOR_CONNECTION[val]);
                ImGuiEx::Text(center_pos, connector_data_label(val), ImGuiEx::TAH_CENTER, ImGuiEx::TAV_CENTER);
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
    auto icon_pull_resistor = ComponentIcon::cache(COMPONENT_PULL_RESISTOR, SHAPE_PULL_RESISTOR, sizeof(SHAPE_PULL_RESISTOR));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_PULL_RESISTOR, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            const float width = 40;
            const float height = 20;
            ui_comp->change_size(width, height);
            ui_comp->add_pin_line(comp->pins().data(), comp->num_pins(), 
                                  {0.5f * width, 0}, {0, 20});

            // custor draw function
            ui_comp->set_custom_ui_callback([=](const UIComponent *ui_comp, Transform to_window) {
                // icon in right half
                auto icon_pos = to_window.apply(Point(width * 0.25f, 0.0f));
                icon_pull_resistor->draw(icon_pos, Point(16,16), ImGui::GetWindowDrawList(), 1, COLOR_COMPONENT_BORDER);

                // constant visualisation in left half
                auto val = comp->read_value(comp->output_pin_index(0));
                auto label_pos = to_window.apply(Point(-width * 0.25f, 0.0f));
                ImGuiEx::RectFilled(label_pos - Point(8,8), label_pos + Point(8,8), COLOR_CONNECTION[val]);
                ImGuiEx::Text(label_pos, connector_data_label(val), ImGuiEx::TAH_CENTER, ImGuiEx::TAV_CENTER);
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