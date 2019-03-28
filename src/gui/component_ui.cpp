// component_ui.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "component_ui.h"
#include "imgui/imgui.h"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

#include "colors.h"

constexpr const ImVec2 &imvec2(const Point &p) {
    return *reinterpret_cast<const ImVec2 *>(&p);
}

constexpr const Point &point(const ImVec2 &v) {
    return *reinterpret_cast<const Point *>(&v);
}

///////////////////////////////////////////////////////////////////////////////
//
// UICircuit
//

UICircuit::UICircuit(const char *name) : m_name(name) {

}

void UICircuit::add_component(const UIComponent &comp) {
	m_ui_components.push_back(comp);
}

void UICircuit::add_pin_location(uint32_t pin, Point location) {
	m_pin_locations[pin] = location;
}

void UICircuit::add_pin_line(Transform to_circuit, uint32_t *pins, size_t pin_count, float size, Point origin, Point inc) {
	if (pin_count == 0) {
		return;
	}

    float segment_len = size / (pin_count + 1);
    Point pos = origin + (inc * segment_len);

    for (size_t i = 0; i < pin_count; ++i) {
        add_pin_location(pins[i], to_circuit.apply(pos));
        pos = pos + (inc * segment_len);
	}
}

void UICircuit::draw() {

	auto draw_list = ImGui::GetWindowDrawList();
    auto offset = point(ImGui::GetCursorScreenPos() /* + scrolling */);

	for (const auto &comp : m_ui_components) {

		draw_list->ChannelsSetCurrent(0);         // background
		ImGui::SetCursorScreenPos(imvec2(comp.m_circuit_min + offset));
		ImGui::InvisibleButton("node", imvec2(comp.m_circuit_max - comp.m_circuit_min));

		if (!comp.m_tooltip.empty() && ImGui::IsItemHovered()) {
			ImGui::SetTooltip(comp.m_tooltip.c_str());
		}

    	draw_list->AddRect(imvec2(comp.m_circuit_min + offset), imvec2(comp.m_circuit_max + offset), 
						   COLOR_COMPONENT_BORDER);

		if (comp.m_icon) {
			Transform to_window = comp.m_to_circuit;
			to_window.translate(offset);
    		comp.m_icon->draw(to_window, draw_list, 2, COLOR_COMPONENT_ICON);
		}
	}

	for (const auto &pair : m_pin_locations) {
		draw_list->AddCircleFilled(imvec2(pair.second + offset), 2, COLOR_ENDPOINT);
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// UICircuitBuilder
//

UICircuitBuilder::materialize_func_map_t UICircuitBuilder::m_materialize_funcs;

void UICircuitBuilder::register_materialize_func(VisualComponent::Type type, materialize_func_t func) {
	m_materialize_funcs[type] = func;
}

void UICircuitBuilder::materialize_component(UICircuit *circuit, VisualComponent *visual_comp) {
	auto mat_func = m_materialize_funcs.find(visual_comp->get_type());
	if (mat_func != m_materialize_funcs.end()) {
		UIComponent comp = {0};
		comp.m_visual_comp = visual_comp;
		mat_func->second(visual_comp->get_component(), &comp, circuit);
		circuit->add_component(comp);
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// ComponentIcon
//


ComponentIcon::ComponentIcon(const char *data, size_t len) {
    std::vector<char> dummy(data, data + len);
	auto img = nsvgParse(dummy.data(), "px", 96);

	Point offset(img->width / 2.0f, img->height / 2.0f);

	for (auto shape = img->shapes; shape != nullptr; shape = shape->next) {
		for (auto path = shape->paths; path != nullptr; path = path->next) {
			for (int i = 0; i < path->npts-1; i += 3) {
				float* p = &path->pts[i*2];
				m_curves.push_back({
					Point(p[0], p[1]) - offset, Point(p[2], p[3]) - offset, 
                    Point(p[4], p[5]) - offset, Point(p[6], p[7]) - offset
				});
			}
		}
	}
}

void ComponentIcon::draw(Transform transform, ImDrawList *draw_list, size_t line_width, uint32_t color) const {
    for (const auto &curve : m_curves) {
		draw_list->AddBezierCurve(
			imvec2(transform.apply(curve[0])), 
			imvec2(transform.apply(curve[1])), 
			imvec2(transform.apply(curve[2])), 
			imvec2(transform.apply(curve[3])), 
			color, 
			line_width);
	}
}


