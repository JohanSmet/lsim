// component_std.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// registration of standard components

#include "component_std.h"
#include "component_ui.h"
#include "imgui_ex.h"
#include "shapes.h"
#include "colors.h"

#include <algorithm>

#include "circuit_description.h"
#include "circuit_instance.h"

namespace lsim {

namespace gui {

void materialize_gate(UIComponent *ui_comp,
                      float min_width = 60, float min_height = 60) {

    auto comp = ui_comp->component();

    // size of component box depends on pin count
    const float width = std::max(min_width, 10.0f * std::ceil((comp->num_controls() + 1)/2.0f));
    const float height = std::max(min_height, 20.0f * (std::ceil((std::max(comp->num_inputs(), comp->num_outputs())+1)/2.0f)));
    const float h_width = width / 2.0f;
    const float h_height = height / 2.0f;

    ui_comp->change_size(width, height);

    // pins
    if (comp->num_inputs() > 0) {
        ui_comp->add_pin_line(comp->input_pin_id(0), comp->num_inputs(),
                            height, {-h_width, 0}, {0, 1});
    }
    if (comp->num_outputs() > 0) {
        ui_comp->add_pin_line(comp->output_pin_id(0), comp->num_outputs(),
                            height, {h_width, 0}, {0, 1});
    }
    if (comp->num_controls() > 0) {
        ui_comp->add_pin_line(comp->control_pin_id(0), comp->num_controls(),
                            width, {0, -h_height}, {1, 0});
    }
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
        COMPONENT_CONNECTOR_IN, [=](Component *comp, UIComponent *ui_comp) {
            ui_comp->change_tooltip("Input");

            const float width = 20;
            const float height = 20;
            const float full_height = comp->num_outputs() * height;
            const bool desc = comp->property_value("descending", false);
            ui_comp->change_size(width, full_height);
            ui_comp->add_pin_line(comp->output_pin_id(0), comp->num_outputs(),
                                  {0.5f * width, ((-full_height * 0.5f) + (height * 0.5f)) * (desc ? -1.0f : 1.0f)},
                                  {0, height * (desc ? -1.0f : 1.0f)});

            // custom draw function
            ui_comp->set_custom_ui_callback([=](UICircuit *ui_circuit, const UIComponent *ui_comp, Transform to_window) {

                auto comp = ui_comp->component();
                bool is_tristate = comp->property_value("tri_state", false);
                auto origin = Point(-width * 0.5f, -full_height * 0.5f);
                auto orient = comp->angle();

                ImGui::BeginGroup();
                ImGui::PushID(comp);
                for (size_t i = 0; i < comp->num_outputs(); ++ i)
                {
                    auto cur_val = VALUE_FALSE;
                    if (ui_circuit->is_simulating()) {
                        cur_val = ui_circuit->circuit_inst()->pin_output(comp->output_pin_id(i));
                    }
                    auto center_pos = to_window.apply(Point(0, ((-full_height * 0.5f) + ((i + 0.5f) * height)) * (desc ? -1.0f : 1.0f)));
                    ImGui::SetCursorScreenPos(center_pos - Point(8,8));

                    ImGui::PushID(i);
                    if (ui_circuit->is_simulating() && !ui_circuit->is_view_only_simulation() &&
                        ImGui::InvisibleButton(connector_data_label(cur_val), {16,16})) {
                        ui_circuit->circuit_inst()->write_pin(
                                comp->output_pin_id(i), 
                                static_cast<Value>((cur_val + 1) % (is_tristate ? 3 : 2)));
                    }

                    ImGuiEx::RectFilled(center_pos - Point(8,8), center_pos + Point(8,8), COLOR_CONNECTION[cur_val]);
                    if (ui_circuit->is_simulating()) {
                        ImGuiEx::Text(center_pos, connector_data_label(cur_val), ImGuiEx::TAH_CENTER, ImGuiEx::TAV_CENTER);
                    } else {
                        ImGuiEx::Text(center_pos, std::to_string(i), ImGuiEx::TAH_CENTER, ImGuiEx::TAV_CENTER);
                    }
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
        COMPONENT_CONNECTOR_OUT, [=](Component *comp, UIComponent *ui_comp) {
            ui_comp->change_tooltip("Output");

            const float width = 20;
            const float height = 20;
            const float full_height = comp->num_inputs() * height;
            const bool desc = comp->property_value("descending", false);
            ui_comp->change_size(width, full_height);
            ui_comp->add_pin_line(comp->input_pin_id(0), comp->num_inputs(),
                                  {-0.5f * width, (-full_height * 0.5f + (height * 0.5f)) * (desc ? -1.0f : 1.0f)},
                                  {0, desc ? -height : height});

            // custom draw function
            ui_comp->set_custom_ui_callback([=](UICircuit *ui_circuit, const UIComponent *ui_comp, Transform to_window) {

                auto origin = Point(-width * 0.5f, -full_height * 0.5f);
                auto orient = ui_comp->component()->angle();

                ImGui::BeginGroup();
                ImGui::PushID(comp);
                for (size_t i = 0; i < comp->num_inputs(); ++ i) {
                    auto cur_val = VALUE_FALSE;
                    if (ui_circuit->is_simulating()) {
                        cur_val = ui_circuit->circuit_inst()->read_pin(comp->input_pin_id(i));
                    }
                    auto center_pos = to_window.apply(Point(0, ((-full_height * 0.5f) + ((i + 0.5f) * height)) * (desc ? -1.0f : 1.0f)));

                    ImGui::PushID(i);
                    ImGuiEx::RectFilled(center_pos - Point(8,8), center_pos + Point(8,8), COLOR_CONNECTION[cur_val]);
                    if (ui_circuit->is_simulating()) {
                        ImGuiEx::Text(center_pos, connector_data_label(cur_val), ImGuiEx::TAH_CENTER, ImGuiEx::TAV_CENTER);
                    } else {
                        ImGuiEx::Text(center_pos, std::to_string(i), ImGuiEx::TAH_CENTER, ImGuiEx::TAV_CENTER);
                    }
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
        COMPONENT_CONSTANT, [=](Component *comp, UIComponent *ui_comp) {
            const float width = 20;
            const float height = 20;
            ui_comp->change_size(width, height);
            ui_comp->add_pin_line(comp->output_pin_id(0), comp->num_outputs(), 
                                  {0.5f * width, (-height * 0.5f) + (height * 0.5f)}, 
                                  {0, height});

            // custom draw function
            ui_comp->set_custom_ui_callback([=](UICircuit *ui_circuit, const UIComponent *ui_comp, Transform to_window) {
                auto val = comp->property_value("value", VALUE_FALSE);
                auto center_pos = to_window.apply(Point(0.0f, 0.0f));
                ImGuiEx::RectFilled(center_pos - Point(8,8), center_pos + Point(8,8), COLOR_CONNECTION[val]);
                ImGuiEx::Text(center_pos, connector_data_label(val), ImGuiEx::TAH_CENTER, ImGuiEx::TAV_CENTER);
            });
        }
    );

    // sub circuit
    UICircuitBuilder::register_materialize_func(
        COMPONENT_SUB_CIRCUIT, [=](Component *comp, UIComponent *ui_comp) {
            auto nested = ui_comp->component()->nested_circuit();

            // materialize the sub-circuit
            bool flip = comp->property_value("flip", false);
            float width = 160;
            float height = (std::max(nested->num_input_ports(), nested->num_output_ports()) + 1) * 20 + 20;

            ui_comp->change_size(width, height);
            if (comp->num_inputs() > 0) {
                ui_comp->add_pin_line(comp->input_pin_id(0), comp->num_inputs(),
                                    {(flip ? 1.0f : -1.0f) * width/2.0f, -height/2.0f + 20}, {0.0f, 20.0f});
            }
            if (comp->num_outputs() > 0) {
                ui_comp->add_pin_line(comp->output_pin_id(0), comp->num_outputs(),
                                    {(flip ? -1.0f : 1.0f) * width/2.0f, -height/2.0f + 20}, {0.0f, 20.0f});
            }

            // custom draw function for pin labels
            ui_comp->set_custom_ui_callback([=](UICircuit *ui_circuit, const UIComponent *ui_comp, Transform to_window) {

                ImGuiEx::TransformStart();

                auto pos = [=](bool flipped) {
                    if (!flipped) {
                        return Point((-width * 0.5f) + 10, (-height * 0.5f) + 20);
                    } else {
                        return Point((width * 0.5f) - 10, (-height * 0.5f) + 20);
                    }
                };

                auto align = [](bool flipped) {
                    return !flipped ? ImGuiEx::TAH_LEFT : ImGuiEx::TAH_RIGHT;
                };


                // input pins - labels
                auto cursor = pos(flip);
                for (size_t idx = 0; idx < nested->num_input_ports(); ++idx) {
                    ImGuiEx::TextNoClip(cursor, nested->port_name(true, idx), COLOR_COMPONENT_BORDER, align(flip), ImGuiEx::TAV_CENTER);
                    cursor.y += 20;
                }

                // output pins - labels
                cursor = pos(!flip);
                for (size_t idx = 0; idx < nested->num_output_ports(); ++idx) {
                    ImGuiEx::TextNoClip(cursor, nested->port_name(false, idx), COLOR_COMPONENT_BORDER, align(!flip), ImGuiEx::TAV_CENTER);
                    cursor.y += 20;
                }

                // caption
                ImGuiEx::RectFilled(Point(-width * 0.5f, (height * 0.5) - 20), 
                                    Point(width * 0.5f, (height * 0.5)), 
                                    COLOR_COMPONENT_BORDER);
                cursor = Point(0, (height * 0.5f) - 10);
                ImGuiEx::TextNoClip(cursor, nested->name() + "#" + std::to_string(comp->id()), IM_COL32(0,0,0,255), ImGuiEx::TAH_CENTER, ImGuiEx::TAV_CENTER);

                ImGuiEx::TransformEnd(to_window);
            });
        }
    ); 
}

void component_register_extra() {

    // Pull Resistor
    auto icon_pull_resistor = ComponentIcon::cache(COMPONENT_PULL_RESISTOR, SHAPE_PULL_RESISTOR, sizeof(SHAPE_PULL_RESISTOR));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_PULL_RESISTOR, [=](Component *comp, UIComponent *ui_comp) {
            const float width = 40;
            const float height = 20;
            ui_comp->change_size(width, height);
            ui_comp->add_pin_line(comp->output_pin_id(0), comp->num_outputs(),
                                  {0.5f * width, 0}, {0, 20});

            // custom draw function
            ui_comp->set_custom_ui_callback([=](UICircuit *ui_circuit, const UIComponent *ui_comp, Transform to_window) {
                // icon in right half
                auto icon_pos = to_window.apply(Point(width * 0.25f, 0.0f));
                icon_pull_resistor->draw(icon_pos, Point(16,16), ImGui::GetWindowDrawList(), 1, COLOR_COMPONENT_BORDER);

                // constant visualisation in left half
                auto val = comp->property_value("pull_to", VALUE_FALSE);
                auto label_pos = to_window.apply(Point(-width * 0.25f, 0.0f));
                ImGuiEx::RectFilled(label_pos - Point(8,8), label_pos + Point(8,8), COLOR_CONNECTION[val]);
                ImGuiEx::Text(label_pos, connector_data_label(val), ImGuiEx::TAH_CENTER, ImGuiEx::TAV_CENTER);
            });
        }
    );

    // Via
    auto icon_via = ComponentIcon::cache(COMPONENT_VIA, SHAPE_VIA, sizeof(SHAPE_VIA));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_VIA, [=](Component *comp, UIComponent *ui_comp) {
            ui_comp->change_tooltip("Via");
            ui_comp->change_icon(icon_via);

            const bool right_pin = comp->property_value("right", false);
            const float width = 20;
            const float height = 20;
            const float full_height = comp->num_inputs() * height;
            ui_comp->change_size(width, full_height);
            ui_comp->add_pin_line(comp->input_pin_id(0), comp->num_inputs(),
                                  {(right_pin ? 0.5f : -0.5f) * width, -full_height * 0.5f + (height * 0.5f)},
                                  {0, height});

            // custom draw function
            ui_comp->set_custom_ui_callback([=](UICircuit *ui_circuit, const UIComponent *ui_comp, Transform to_window) {

                auto origin = Point(-width * 0.5f, -full_height * 0.5f);
                auto orient = ui_comp->component()->angle();

                ImGui::BeginGroup();
                ImGui::PushID(comp);
                for (size_t i = 0; i < comp->num_inputs(); ++ i) {
                    auto cur_val = VALUE_FALSE;
                    if (ui_circuit->is_simulating()) {
                        cur_val = ui_circuit->circuit_inst()->read_pin(comp->input_pin_id(i));
                    }
                    auto center_pos = to_window.apply(Point(origin.x + (width * 0.5f), origin.y + (i * 20.0f) + (height * 0.5f)));

                    ImGui::PushID(i);
                    ImGui::GetWindowDrawList()->AddCircleFilled(center_pos, 8, COLOR_CONNECTION[cur_val]);
                    ImGui::PopID();
                }
                ImGui::PopID();

                // label 
                Point anchor = to_window.apply(origin + Point(right_pin ? -5 : (width + 5), (full_height / 2.0f)));

                static const std::pair<ImGuiEx::TextAlignHor, ImGuiEx::TextAlignVer> label_align[] = {
                    {ImGuiEx::TAH_LEFT,     ImGuiEx::TAV_CENTER},   // East - left pin
                    {ImGuiEx::TAH_CENTER,   ImGuiEx::TAV_TOP},      // South - left pin
                    {ImGuiEx::TAH_RIGHT,    ImGuiEx::TAV_CENTER},   // West - left pin
                    {ImGuiEx::TAH_CENTER,   ImGuiEx::TAV_BOTTOM},   // North - left pin
                    {ImGuiEx::TAH_RIGHT,    ImGuiEx::TAV_CENTER},   // East - rigth pin
                    {ImGuiEx::TAH_CENTER,   ImGuiEx::TAV_BOTTOM},   // South - rigth pin
                    {ImGuiEx::TAH_LEFT,     ImGuiEx::TAV_CENTER},   // West - rigth pin
                    {ImGuiEx::TAH_CENTER,   ImGuiEx::TAV_TOP}       // North - rigth pin
                };
                int align_idx = orient / 90 + (right_pin ? 4 : 0);

                ImGuiEx::Text(anchor, comp->property_value("name", "").c_str(), label_align[align_idx].first, label_align[align_idx].second);

                ImGui::EndGroup();
            });
        }
    );

    // oscillator
    auto icon_oscillator = ComponentIcon::cache(COMPONENT_OSCILLATOR, SHAPE_OSCILLATOR, sizeof(SHAPE_OSCILLATOR));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_OSCILLATOR, [=](Component *comp, UIComponent *ui_comp) {
            ui_comp->change_tooltip("Oscillator");
            ui_comp->change_icon(icon_oscillator);
            materialize_gate(ui_comp);
        }
    );


    // Text
    auto icon_text = ComponentIcon::cache(COMPONENT_TEXT, SHAPE_TEXT, sizeof(SHAPE_TEXT));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_TEXT, [icon_text] (Component *comp, UIComponent *ui_comp) {
            ui_comp->show_border(false);
            ui_comp->change_icon(icon_text);
            ui_comp->change_size(0, 0);

            // custom draw function
            ui_comp->set_custom_ui_callback([](UICircuit *ui_circuit, const UIComponent *ui_comp, Transform to_window) {
                auto text = ui_comp->component()->property_value("text", "-");

                if (ui_comp->aabb_size().x == 0) {
                    auto size = ImGui::CalcTextSize(text.c_str());
                    const_cast<UIComponent *>(ui_comp)->change_size(size.x, size.y);
                }

                ImGuiEx::TransformStart();  
                ImGuiEx::TextNoClip(Point(0,0), text.c_str(), COLOR_COMPONENT_BORDER, ImGuiEx::TAH_CENTER, ImGuiEx::TAV_CENTER);
                ImGuiEx::TransformEnd(to_window);
            });
        }
    );
}

void component_register_gates() {

    // buffer
    auto icon_buffer = ComponentIcon::cache(COMPONENT_BUFFER, SHAPE_BUFFER, sizeof(SHAPE_BUFFER));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_BUFFER, [=](Component *comp, UIComponent *ui_comp) {
            ui_comp->change_tooltip("Buffer");
            ui_comp->change_icon(icon_buffer);
            materialize_gate(ui_comp);
        }
    );

    // tristate buffer
	auto icon_tristate_buffer = ComponentIcon::cache(COMPONENT_TRISTATE_BUFFER, SHAPE_TRISTATE_BUFFER, sizeof(SHAPE_TRISTATE_BUFFER));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_TRISTATE_BUFFER, [=](Component *comp, UIComponent *ui_comp) {
            ui_comp->change_tooltip("Tri-state Buffer");
            ui_comp->change_icon(icon_tristate_buffer);
            materialize_gate(ui_comp);
        }
    );

    // AND gate
	auto icon_and = ComponentIcon::cache(COMPONENT_AND_GATE, SHAPE_AND_GATE, sizeof(SHAPE_AND_GATE));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_AND_GATE, [=](Component *comp, UIComponent *ui_comp) {
            ui_comp->change_tooltip("AND");
            ui_comp->change_icon(icon_and);
            materialize_gate(ui_comp);
        }
    );

    // OR gate
	auto icon_or = ComponentIcon::cache(COMPONENT_OR_GATE, SHAPE_OR_GATE, sizeof(SHAPE_OR_GATE));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_OR_GATE, [=](Component *comp, UIComponent *ui_comp) {
            ui_comp->change_tooltip("OR");
            ui_comp->change_icon(icon_or);
            materialize_gate(ui_comp);
        }
    );

    // NOT gate
	auto icon_not = ComponentIcon::cache(COMPONENT_NOT_GATE, SHAPE_NOT_GATE, sizeof(SHAPE_NOT_GATE));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_NOT_GATE, [=](Component *comp, UIComponent *ui_comp) {
            ui_comp->change_tooltip("NOT");
            ui_comp->change_icon(icon_not);
            materialize_gate(ui_comp, 60, 40);
        }
    );

    // NAND gate
	auto icon_nand = ComponentIcon::cache(COMPONENT_NAND_GATE, SHAPE_NAND_GATE, sizeof(SHAPE_NAND_GATE));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_NAND_GATE, [=](Component *comp, UIComponent *ui_comp) {
            ui_comp->change_tooltip("NAND");
            ui_comp->change_icon(icon_nand);
            materialize_gate(ui_comp);
        }
    );

    // NOR gate
	auto icon_nor = ComponentIcon::cache(COMPONENT_NOR_GATE, SHAPE_NOR_GATE, sizeof(SHAPE_NOR_GATE));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_NOR_GATE, [=](Component *comp, UIComponent *ui_comp) {
            ui_comp->change_tooltip("NOR");
            ui_comp->change_icon(icon_nor);
            materialize_gate(ui_comp);
        }
    );

    // XOR gate
	auto icon_xor = ComponentIcon::cache(COMPONENT_XOR_GATE, SHAPE_XOR_GATE, sizeof(SHAPE_XOR_GATE));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_XOR_GATE, [=](Component *comp, UIComponent *ui_comp) {
            ui_comp->change_tooltip("XOR");
            ui_comp->change_icon(icon_xor);
            materialize_gate(ui_comp);
        }
    );

    // XNOR gate
	auto icon_xnor = ComponentIcon::cache(COMPONENT_XNOR_GATE, SHAPE_XNOR_GATE, sizeof(SHAPE_XNOR_GATE));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_XNOR_GATE, [=](Component *comp, UIComponent *ui_comp) {
            ui_comp->change_tooltip("XNOR");
            ui_comp->change_icon(icon_xnor);
            materialize_gate(ui_comp);
        }
    );
}

void component_register_input_output() {

    auto icon_7_segment_led = ComponentIcon::cache(COMPONENT_7_SEGMENT_LED, SHAPE_7_SEGMENT_LED, sizeof(SHAPE_7_SEGMENT_LED));
    UICircuitBuilder::register_materialize_func(
        COMPONENT_7_SEGMENT_LED, [=](Component *comp, UIComponent *ui_comp) {
            ui_comp->change_tooltip("7-segment LED");

            const float width = 60;
            const float height = 80;
            ui_comp->change_size(width, height);
            ui_comp->add_pin_line(comp->input_pin_id(0), 4,
                                  width, {0, height * 0.5f}, {1, 0});
            ui_comp->add_pin_line(comp->input_pin_id(0) + 4, 4,
                                  width, {0, -height * 0.5f}, {1, 0});

            // custom draw function
            ui_comp->set_custom_ui_callback([](UICircuit *ui_circuit, const UIComponent *ui_comp, Transform to_window) {
                auto draw_list = ImGui::GetWindowDrawList();
                auto comp = ui_comp->component();
                const auto slope = 5.0f / 30.f;
                const auto color_off = IM_COL32(100, 0, 0, 255);
                const auto color_on = IM_COL32(255, 0, 0, 255);
                const auto width = 4;

                auto sim_comp = ui_circuit->is_simulating() ? ui_circuit->circuit_inst()->component_by_id(comp->id()) : nullptr;

                const auto led_color = [sim_comp, color_on, color_off](uint32_t idx) -> ImU32 {
                    if (sim_comp) {
                        auto value = sim_comp->read_pin(idx);
                        return value == VALUE_TRUE ? color_on : color_off;
                    } else {
                        return color_off;
                    }
                };

                // LED-pinout
                // pin 0 = E    // pin 1 = D    // pin 2 = C    // pin 3 = DP
                // pin 4 = G    // pin 5 = F    // pin 6 = A    // pin 7 = B

                ImGuiEx::TransformStart();  
                draw_list->AddLine({-15,-30}, {20,-30}, led_color(6), width);                               // A
                draw_list->AddLine({-15 - 5 * slope, -25}, {-15 - 25 * slope, -5}, led_color(5), width);    // F 
                draw_list->AddLine({ 20 - 5 * slope, -25}, { 20 - 25 * slope, -5}, led_color(7), width);    // B 
                draw_list->AddLine({-20,  0}, {15,  0}, led_color(4), width);                               // G
                draw_list->AddLine({-20 - 5 * slope, 5}, {-20 - 25 * slope, 25}, led_color(0), width);      // E 
                draw_list->AddLine({ 15 - 5 * slope, 5}, { 15 - 25 * slope, 25}, led_color(2), width);      // C 
                draw_list->AddLine({-25, 30}, {10, 30}, led_color(1), width);                               // D
                draw_list->AddCircleFilled({20, 30}, 3, led_color(3));                                      // DP
                ImGuiEx::TransformEnd(to_window);

            });
        }
    );
}

} // namespace lsim::gui

} // namespace lsim