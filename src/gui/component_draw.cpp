// component_draw.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// functions to materialize components and define custom draw functions

#include "component_draw.h"
#include "component_icon.h"
#include "component_widget.h"
#include "circuit_editor.h"
#include "imgui_ex.h"
#include "shapes.h"
#include "colors.h"

#include <algorithm>

#include "model_circuit.h"
#include "sim_circuit.h"
#include "sim_component.h"

namespace {

using namespace lsim;
using namespace lsim::gui;

void materialize_gate(ComponentWidget *widget, float min_width = 60, float min_height = 60) {

	auto model = widget->component_model();

	// size of component box depends on pin count
	const float width = std::max(min_width, 10.0f * std::ceil((model->num_controls() + 1) / 2.0f));
	const float height = std::max(min_height, 20.0f * (std::ceil((std::max(model->num_inputs(), model->num_outputs()) + 1) / 2.0f)));
	const float h_width = width / 2.0f;
	const float h_height = height / 2.0f;

	widget->change_size(width, height);

	// pins
	if (model->num_inputs() > 0) {
		widget->add_pin_line(model->input_pin_id(0), model->num_inputs(),
			height, { -h_width, 0 }, { 0, 1 });
	}
	if (model->num_outputs() > 0) {
		widget->add_pin_line(model->output_pin_id(0), model->num_outputs(),
			height, { h_width, 0 }, { 0, 1 });
	}
	if (model->num_controls() > 0) {
		widget->add_pin_line(model->control_pin_id(0), model->num_controls(),
			width, { 0, -h_height }, { 1, 0 });
	}
}

const char *value_label(Value value) {
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

} // unnamed namespace

namespace lsim {

namespace gui {

///////////////////////////////////////////////////////////////////////////////
//
// basic components: connectors, constant, sub-curcuits
//

void component_register_basic() {

    // connector input
    auto icon_connector_in = ComponentIcon::cache(COMPONENT_CONNECTOR_IN, SHAPE_CONNECTOR_IN, sizeof(SHAPE_CONNECTOR_IN));
    CircuitEditorFactory::register_materialize_func(
        COMPONENT_CONNECTOR_IN, [](ModelComponent *comp, ComponentWidget *widget) {
            widget->change_tooltip("Input");

            const float width = 20;
            const float pin_spacing = 20;
            const float height = comp->num_outputs() * pin_spacing;
            const bool desc = comp->property_value("descending", false);
            widget->change_size(width, height);
            widget->add_pin_line(comp->output_pin_id(0), comp->num_outputs(),
                                  {0.5f * width, ((-height * 0.5f) + (pin_spacing * 0.5f)) * (desc ? -1.0f : 1.0f)},
                                  {0, pin_spacing * (desc ? -1.0f : 1.0f)});

            // custom draw function
            widget->set_draw_callback([=](CircuitEditor *circuit_editor, const ComponentWidget *widget, Transform to_window) {

                auto model = widget->component_model();

                bool is_tristate = model->property_value("tri_state", false);
                auto origin = Point(-width * 0.5f, -height * 0.5f);
                auto orient = model->angle();
				const Point button_half_size = {8,8};
				const Point button_size = {16,16};

                ImGui::BeginGroup();
                ImGui::PushID(model);

				// connector 'buttons'
                for (auto i = 0u; i < model->num_outputs(); ++ i)
                {
                    auto cur_val = VALUE_FALSE;
                    if (circuit_editor->is_simulating()) {
                        cur_val = circuit_editor->sim_circuit()->user_value(model->output_pin_id(i));
                    }
                    auto center_pos = to_window.apply(Point(0, ((-height * 0.5f) + ((i + 0.5f) * pin_spacing)) * (desc ? -1.0f : 1.0f)));
                    ImGui::SetCursorScreenPos(center_pos - button_half_size);

                    ImGui::PushID(i);
                    if (circuit_editor->is_simulating() && !circuit_editor->is_view_only_simulation() &&
                        ImGui::InvisibleButton(value_label(cur_val), button_size)) {
                        cur_val = static_cast<Value>((cur_val + 1) % (is_tristate ? 3 : 2));
                        circuit_editor->sim_circuit()->write_pin(model->output_pin_id(i), cur_val);
                    }

                    ImGuiEx::RectFilled(center_pos - button_half_size, center_pos + button_half_size, COLOR_CONNECTION[cur_val]);
                    if (circuit_editor->is_simulating()) {
                        ImGuiEx::Text(center_pos, value_label(cur_val), ImGuiEx::TAH_CENTER, ImGuiEx::TAV_CENTER);
                    } else {
                        ImGuiEx::Text(center_pos, std::to_string(i), ImGuiEx::TAH_CENTER, ImGuiEx::TAV_CENTER);
                    }
                    ImGui::PopID();
                }
                ImGui::PopID();

                // label for connector
                Point anchor = to_window.apply(Point(origin.x - 5, origin.y + (height / 2.0f)));

                static const std::pair<ImGuiEx::TextAlignHor, ImGuiEx::TextAlignVer> label_align[] = {
                    {ImGuiEx::TAH_RIGHT,    ImGuiEx::TAV_CENTER},   // East
                    {ImGuiEx::TAH_CENTER,   ImGuiEx::TAV_BOTTOM},   // South
                    {ImGuiEx::TAH_LEFT,     ImGuiEx::TAV_CENTER},   // West
                    {ImGuiEx::TAH_CENTER,   ImGuiEx::TAV_TOP}       // North
                };

                ImGuiEx::Text(anchor, model->property_value("name","").c_str(), label_align[orient / 90].first, label_align[orient / 90].second);

                ImGui::EndGroup();
            });
        }
    );

    // connector output
    auto icon_connector_out = ComponentIcon::cache(COMPONENT_CONNECTOR_OUT, SHAPE_CONNECTOR_OUT, sizeof(SHAPE_CONNECTOR_OUT));
    CircuitEditorFactory::register_materialize_func(
        COMPONENT_CONNECTOR_OUT, [=](ModelComponent *comp, ComponentWidget *widget) {
            widget->change_tooltip("Output");

            const float width = 20;
            const float pin_spacing = 20;
            const float height = comp->num_inputs() * pin_spacing;
            const bool desc = comp->property_value("descending", false);
            widget->change_size(width, height);
            widget->add_pin_line(comp->input_pin_id(0), comp->num_inputs(),
                                  {-0.5f * width, (-height * 0.5f + (pin_spacing * 0.5f)) * (desc ? -1.0f : 1.0f)},
                                  {0, desc ? -pin_spacing : pin_spacing});

            // custom draw function
            widget->set_draw_callback([=](CircuitEditor *circuit_editor, const ComponentWidget *widget, Transform to_window) {

                auto origin = Point(-width * 0.5f, -height * 0.5f);
                auto orient = widget->component_model()->angle();
				const Point button_half_size = {8,8};

                ImGui::BeginGroup();
                ImGui::PushID(comp);
                for (auto i = 0u; i < comp->num_inputs(); ++ i) {
                    auto cur_val = VALUE_FALSE;
                    if (circuit_editor->is_simulating()) {
                        cur_val = circuit_editor->sim_circuit()->read_pin(comp->input_pin_id(i));
                    }
                    auto center_pos = to_window.apply(Point(0, ((-height * 0.5f) + ((i + 0.5f) * pin_spacing)) * (desc ? -1.0f : 1.0f)));

                    ImGui::PushID(i);
                    ImGuiEx::RectFilled(center_pos - button_half_size, center_pos + button_half_size, COLOR_CONNECTION[cur_val]);
                    if (circuit_editor->is_simulating()) {
                        ImGuiEx::Text(center_pos, value_label(cur_val), ImGuiEx::TAH_CENTER, ImGuiEx::TAV_CENTER);
                    } else {
                        ImGuiEx::Text(center_pos, std::to_string(i), ImGuiEx::TAH_CENTER, ImGuiEx::TAV_CENTER);
                    }
                    ImGui::PopID();
                }
                ImGui::PopID();

                // label for connector
                Point anchor = to_window.apply(origin + Point(width + 5, (height / 2.0f)));

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
    CircuitEditorFactory::register_materialize_func(
        COMPONENT_CONSTANT, [=](ModelComponent *comp, ComponentWidget *widget) {
            const float width = 20;
            const float height = 20;
            widget->change_size(width, height);
            widget->add_pin_line(comp->output_pin_id(0), comp->num_outputs(), 
                                  {0.5f * width, (-height * 0.5f) + (height * 0.5f)}, 
                                  {0, height});

            // custom draw function
            widget->set_draw_callback([=](CircuitEditor *circuit_editor, const ComponentWidget *widget, Transform to_window) {
                auto val = comp->property_value("value", VALUE_FALSE);
                auto center_pos = to_window.apply(Point(0.0f, 0.0f));
                ImGuiEx::RectFilled(center_pos - Point(8,8), center_pos + Point(8,8), COLOR_CONNECTION[val]);
                ImGuiEx::Text(center_pos, value_label(val), ImGuiEx::TAH_CENTER, ImGuiEx::TAV_CENTER);
            });
        }
    );

    // sub circuit
    CircuitEditorFactory::register_materialize_func(
        COMPONENT_SUB_CIRCUIT, [=](ModelComponent *comp, ComponentWidget *widget) {
            auto nested = widget->component_model()->nested_circuit();

            // materialize the sub-circuit
            bool flip = comp->property_value("flip", false);
            float width = 160;
            float height = (std::max(nested->num_input_ports(), nested->num_output_ports()) + 1) * 20.0f + 20.0f;

            widget->change_size(width, height);
            if (comp->num_inputs() > 0) {
                widget->add_pin_line(comp->input_pin_id(0), comp->num_inputs(),
                                    {(flip ? 1.0f : -1.0f) * width/2.0f, -height/2.0f + 20}, {0.0f, 20.0f});
            }
            if (comp->num_outputs() > 0) {
                widget->add_pin_line(comp->output_pin_id(0), comp->num_outputs(),
                                    {(flip ? -1.0f : 1.0f) * width/2.0f, -height/2.0f + 20}, {0.0f, 20.0f});
            }

            // custom draw function for pin labels
            widget->set_draw_callback([=](CircuitEditor *circuit_editor, const ComponentWidget *widget, Transform to_window) {

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
                for (auto idx = 0u; idx < nested->num_input_ports(); ++idx) {
                    ImGuiEx::TextNoClip(cursor, nested->port_name(true, idx), COLOR_COMPONENT_BORDER, align(flip), ImGuiEx::TAV_CENTER);
                    cursor.y += 20;
                }

                // output pins - labels
                cursor = pos(!flip);
                for (auto idx = 0u; idx < nested->num_output_ports(); ++idx) {
                    ImGuiEx::TextNoClip(cursor, nested->port_name(false, idx), COLOR_COMPONENT_BORDER, align(!flip), ImGuiEx::TAV_CENTER);
                    cursor.y += 20;
                }

                // caption
                ImGuiEx::RectFilled(Point(-width * 0.5f, (height * 0.5f) - 20), 
                                    Point(width * 0.5f, (height * 0.5f)), 
                                    COLOR_COMPONENT_BORDER);
                cursor = Point(0, (height * 0.5f) - 10);
                ImGuiEx::TextNoClip(cursor, comp->property_value("caption", "unknown"), IM_COL32(0,0,0,255), ImGuiEx::TAH_CENTER, ImGuiEx::TAV_CENTER);

                ImGuiEx::TransformEnd(to_window);
            });
        }
    ); 
}

///////////////////////////////////////////////////////////////////////////////
//
// extra components: pull-resistor, via, oscillator, text
//

void component_register_extra() {

    // Pull Resistor
    auto icon_pull_resistor = ComponentIcon::cache(COMPONENT_PULL_RESISTOR, SHAPE_PULL_RESISTOR, sizeof(SHAPE_PULL_RESISTOR));
    CircuitEditorFactory::register_materialize_func(
        COMPONENT_PULL_RESISTOR, [=](ModelComponent *comp, ComponentWidget *widget) {
            const float width = 40;
            const float height = 20;
            widget->change_size(width, height);
            widget->add_pin_line(comp->output_pin_id(0), comp->num_outputs(),
                                  {0.5f * width, 0}, {0, 20});

            // custom draw function
            widget->set_draw_callback([=](CircuitEditor *circuit_editor, const ComponentWidget *widget, Transform to_window) {
                // icon in right half
                auto icon_pos = to_window.apply(Point(width * 0.25f, 0.0f));
                icon_pull_resistor->draw(icon_pos, Point(16,16), ImGui::GetWindowDrawList(), 1, COLOR_COMPONENT_BORDER);

                // constant visualisation in left half
                auto val = comp->property_value("pull_to", VALUE_FALSE);
                auto label_pos = to_window.apply(Point(-width * 0.25f, 0.0f));
                ImGuiEx::RectFilled(label_pos - Point(8,8), label_pos + Point(8,8), COLOR_CONNECTION[val]);
                ImGuiEx::Text(label_pos, value_label(val), ImGuiEx::TAH_CENTER, ImGuiEx::TAV_CENTER);
            });
        }
    );

    // Via
    auto icon_via = ComponentIcon::cache(COMPONENT_VIA, SHAPE_VIA, sizeof(SHAPE_VIA));
    CircuitEditorFactory::register_materialize_func(
        COMPONENT_VIA, [=](ModelComponent *comp, ComponentWidget *widget) {
            widget->change_tooltip("Via");
            widget->change_icon(icon_via);

            const bool right_pin = comp->property_value("right", false);
            const float width = 20;
            const float pin_spacing = 20;
            const float height = comp->num_inputs() * pin_spacing;
            widget->change_size(width, height);
            widget->add_pin_line(comp->input_pin_id(0), comp->num_inputs(),
                                  {(right_pin ? 0.5f : -0.5f) * width, -height * 0.5f + (pin_spacing * 0.5f)},
                                  {0, pin_spacing});

            // custom draw function
            widget->set_draw_callback([=](CircuitEditor *circuit_editor, const ComponentWidget *widget, Transform to_window) {

                auto origin = Point(-width * 0.5f, -height * 0.5f);
                auto orient = widget->component_model()->angle();

                ImGui::BeginGroup();

				// connection points
                ImGui::PushID(comp);
                for (auto i = 0u; i < comp->num_inputs(); ++ i) {
                    auto cur_val = VALUE_FALSE;
                    if (circuit_editor->is_simulating()) {
                        cur_val = circuit_editor->sim_circuit()->read_pin(comp->input_pin_id(i));
                    }
                    auto center_pos = to_window.apply(Point(origin.x + (width * 0.5f), origin.y + (i * pin_spacing) + (pin_spacing * 0.5f)));

                    ImGui::PushID(i);
                    ImGui::GetWindowDrawList()->AddCircleFilled(center_pos, 8, COLOR_CONNECTION[cur_val]);
                    ImGui::PopID();
                }
                ImGui::PopID();

                // label 
                Point anchor = to_window.apply(origin + Point(right_pin ? -5 : (width + 5), (height / 2.0f)));

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
    CircuitEditorFactory::register_materialize_func(
        COMPONENT_OSCILLATOR, [=](ModelComponent *comp, ComponentWidget *widget) {
            widget->change_tooltip("Oscillator");
            widget->change_icon(icon_oscillator);
            materialize_gate(widget);
        }
    );


    // Text
    auto icon_text = ComponentIcon::cache(COMPONENT_TEXT, SHAPE_TEXT, sizeof(SHAPE_TEXT));
    CircuitEditorFactory::register_materialize_func(
        COMPONENT_TEXT, [icon_text] (ModelComponent *comp, ComponentWidget *widget) {
            widget->show_border(false);
            widget->change_icon(icon_text);
            widget->change_size(0, 0);

            // custom draw function
            widget->set_draw_callback([](CircuitEditor *circuit_editor, const ComponentWidget *widget, Transform to_window) {
                auto text = widget->component_model()->property_value("text", "-");

                if (widget->aabb_size().x == 0) {
                    auto size = ImGui::CalcTextSize(text.c_str());
                    const_cast<ComponentWidget *>(widget)->change_size(size.x, size.y);
                }

                ImGuiEx::TransformStart();  
                ImGuiEx::TextNoClip(Point(0,0), text.c_str(), COLOR_COMPONENT_BORDER, ImGuiEx::TAH_CENTER, ImGuiEx::TAV_CENTER);
                ImGuiEx::TransformEnd(to_window);
            });
        }
    );
}

///////////////////////////////////////////////////////////////////////////////
//
// logic gates
//

void component_register_gates() {

    // buffer
    auto icon_buffer = ComponentIcon::cache(COMPONENT_BUFFER, SHAPE_BUFFER, sizeof(SHAPE_BUFFER));
    CircuitEditorFactory::register_materialize_func(
        COMPONENT_BUFFER, [=](ModelComponent *comp, ComponentWidget *widget) {
            widget->change_tooltip("Buffer");
            widget->change_icon(icon_buffer);
            materialize_gate(widget, 60, 40);
        }
    );

    // tristate buffer
	auto icon_tristate_buffer = ComponentIcon::cache(COMPONENT_TRISTATE_BUFFER, SHAPE_TRISTATE_BUFFER, sizeof(SHAPE_TRISTATE_BUFFER));
    CircuitEditorFactory::register_materialize_func(
        COMPONENT_TRISTATE_BUFFER, [=](ModelComponent *comp, ComponentWidget *widget) {
            widget->change_tooltip("Tri-state Buffer");
            widget->change_icon(icon_tristate_buffer);
            materialize_gate(widget);
        }
    );

    // AND gate
	auto icon_and = ComponentIcon::cache(COMPONENT_AND_GATE, SHAPE_AND_GATE, sizeof(SHAPE_AND_GATE));
    CircuitEditorFactory::register_materialize_func(
        COMPONENT_AND_GATE, [=](ModelComponent *comp, ComponentWidget *widget) {
            widget->change_tooltip("AND");
            widget->change_icon(icon_and);
            materialize_gate(widget);
        }
    );

    // OR gate
	auto icon_or = ComponentIcon::cache(COMPONENT_OR_GATE, SHAPE_OR_GATE, sizeof(SHAPE_OR_GATE));
    CircuitEditorFactory::register_materialize_func(
        COMPONENT_OR_GATE, [=](ModelComponent *comp, ComponentWidget *widget) {
            widget->change_tooltip("OR");
            widget->change_icon(icon_or);
            materialize_gate(widget);
        }
    );

    // NOT gate
	auto icon_not = ComponentIcon::cache(COMPONENT_NOT_GATE, SHAPE_NOT_GATE, sizeof(SHAPE_NOT_GATE));
    CircuitEditorFactory::register_materialize_func(
        COMPONENT_NOT_GATE, [=](ModelComponent *comp, ComponentWidget *widget) {
            widget->change_tooltip("NOT");
            widget->change_icon(icon_not);
            materialize_gate(widget, 60, 40);
        }
    );

    // NAND gate
	auto icon_nand = ComponentIcon::cache(COMPONENT_NAND_GATE, SHAPE_NAND_GATE, sizeof(SHAPE_NAND_GATE));
    CircuitEditorFactory::register_materialize_func(
        COMPONENT_NAND_GATE, [=](ModelComponent *comp, ComponentWidget *widget) {
            widget->change_tooltip("NAND");
            widget->change_icon(icon_nand);
            materialize_gate(widget);
        }
    );

    // NOR gate
	auto icon_nor = ComponentIcon::cache(COMPONENT_NOR_GATE, SHAPE_NOR_GATE, sizeof(SHAPE_NOR_GATE));
    CircuitEditorFactory::register_materialize_func(
        COMPONENT_NOR_GATE, [=](ModelComponent *comp, ComponentWidget *widget) {
            widget->change_tooltip("NOR");
            widget->change_icon(icon_nor);
            materialize_gate(widget);
        }
    );

    // XOR gate
	auto icon_xor = ComponentIcon::cache(COMPONENT_XOR_GATE, SHAPE_XOR_GATE, sizeof(SHAPE_XOR_GATE));
    CircuitEditorFactory::register_materialize_func(
        COMPONENT_XOR_GATE, [=](ModelComponent *comp, ComponentWidget *widget) {
            widget->change_tooltip("XOR");
            widget->change_icon(icon_xor);
            materialize_gate(widget);
        }
    );

    // XNOR gate
	auto icon_xnor = ComponentIcon::cache(COMPONENT_XNOR_GATE, SHAPE_XNOR_GATE, sizeof(SHAPE_XNOR_GATE));
    CircuitEditorFactory::register_materialize_func(
        COMPONENT_XNOR_GATE, [=](ModelComponent *comp, ComponentWidget *widget) {
            widget->change_tooltip("XNOR");
            widget->change_icon(icon_xnor);
            materialize_gate(widget);
        }
    );
}

///////////////////////////////////////////////////////////////////////////////
//
// I/O components
//

void component_register_input_output() {

    auto icon_7_segment_led = ComponentIcon::cache(COMPONENT_7_SEGMENT_LED, SHAPE_7_SEGMENT_LED, sizeof(SHAPE_7_SEGMENT_LED));
    CircuitEditorFactory::register_materialize_func(
        COMPONENT_7_SEGMENT_LED, [=](ModelComponent *comp, ComponentWidget *widget) {
            widget->change_tooltip("7-segment LED");

            const float width = 60;
            const float height = 80;
            widget->change_size(width, height);
            widget->add_pin_line(comp->input_pin_id(0), 4,
                                  width, {0, -height * 0.5f}, {1, 0});
            widget->add_pin_line(comp->input_pin_id(0) + 4, 4,
                                  width, {0, height * 0.5f}, {1, 0});

            widget->add_pin_line(comp->control_pin_id(0), comp->num_controls(),
                                  width, {0, height * 0.5f}, {1, 0});

            // custom draw function
            widget->set_draw_callback([](CircuitEditor *circuit_editor, const ComponentWidget *widget, Transform to_window) {
                auto draw_list = ImGui::GetWindowDrawList();
                auto comp = widget->component_model();
                const auto slope = 5.0f / 30.f;
                const auto color_off = IM_COL32(100, 0, 0, 255);
                const auto color_on = IM_COL32(255, 0, 0, 255);
                const auto width = 4;

                auto sim_comp = circuit_editor->is_simulating() ? circuit_editor->sim_circuit()->component_by_id(comp->id()) : nullptr;
                auto enabled  = sim_comp ? sim_comp->read_pin(sim_comp->control_pin_index(0)) == VALUE_TRUE : false;

                ImU32 led_colors[8] = {color_off, color_off, color_off, color_off,color_off, color_off, color_off, color_off};

                if (sim_comp) {
                    auto *extra = reinterpret_cast<ExtraData7SegmentLED *>(sim_comp->extra_data());

                    if (extra->m_num_samples > 0) {
                        for (size_t idx = 0; idx < 8; ++idx) {
                            float fraction = (extra->m_samples[idx] * 3.0f) / extra->m_num_samples;
                            extra->m_samples[idx] = 0;
                            led_colors[idx] = IM_COL32(std::min(255, 50 + (int) (205 * fraction)), 0, 0, 255);
                        } 
                        extra->m_num_samples = 0;
                    } else if (enabled) {
                        for (auto idx = 0u; idx < 8; ++idx) {
                            led_colors[idx] = sim_comp->read_pin(idx) == VALUE_TRUE ? color_on : color_off;
                        }
                    }
                } 

                // LED-pinout: the pinout does not emulate the pinout of real 7-segment LED components.
                //  This was done on purpose to make it easier and simpler to make nice circuits.
                ImGuiEx::TransformStart();  
                draw_list->AddLine({-15,-30}, {20,-30}, led_colors[0], width);                               // A
                draw_list->AddLine({-15 - 5 * slope, -25}, {-15 - 25 * slope, -5}, led_colors[5], width);    // F 
                draw_list->AddLine({ 20 - 5 * slope, -25}, { 20 - 25 * slope, -5}, led_colors[1], width);    // B 
                draw_list->AddLine({-20,  0}, {15,  0}, led_colors[6], width);                               // G
                draw_list->AddLine({-20 - 5 * slope, 5}, {-20 - 25 * slope, 25}, led_colors[4], width);      // E 
                draw_list->AddLine({ 15 - 5 * slope, 5}, { 15 - 25 * slope, 25}, led_colors[2], width);      // C 
                draw_list->AddLine({-25, 30}, {10, 30}, led_colors[3], width);                               // D
                draw_list->AddCircleFilled({20, 30}, 3, led_colors[7]);                                      // DP
                ImGuiEx::TransformEnd(to_window);
            });
        }
    );
}

} // namespace lsim::gui

} // namespace lsim