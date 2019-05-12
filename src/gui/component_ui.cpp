// component_ui.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "component_ui.h"
#include "imgui/imgui.h"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

#include "colors.h"
#include "simulator.h"
#include "circuit.h"

///////////////////////////////////////////////////////////////////////////////
//
// UICircuit
//

UICircuit::UICircuit(Circuit *circuit) : 
			m_circuit(circuit),
			m_name(circuit->name()),
			m_scroll_delta(0,0) {

}

void UICircuit::add_component(const UIComponent &comp) {
	m_ui_components.push_back(comp);
}

void UICircuit::add_endpoint(uint32_t pin, Point location) {
	m_endpoints[pin] = location;
}

void UICircuit::add_pin_line(Transform to_circuit, const uint32_t *pins, size_t pin_count, float size, Point origin, Point inc) {
	if (pin_count == 0) {
		return;
	}

    float segment_len = size / (pin_count + 1);
    Point pos = origin + (inc * segment_len);

    for (size_t i = 0; i < pin_count; ++i) {
        add_endpoint(pins[i], to_circuit.apply(pos));
        pos = pos + (inc * segment_len);
	}
}

void UICircuit::add_pin_line(Transform to_circuit, const uint32_t *pins, size_t pin_count, Point origin, Point delta) {
	if (pin_count == 0) {
		return;
	}

	Point pos = origin;
    for (size_t i = 0; i < pin_count; ++i) {
        add_endpoint(pins[i], to_circuit.apply(pos));
		pos = pos + delta;
	}
}

void UICircuit::add_connection(uint32_t node, uint32_t pin_1, uint32_t pin_2) {
	m_ui_connections.push_back({node, pin_1, pin_2});
}

void UICircuit::draw() {

	auto draw_list = ImGui::GetWindowDrawList();
	draw_list->ChannelsSplit(2);

    Point offset = m_scroll_delta + ImGui::GetCursorScreenPos();

	// components
	for (const auto &comp : m_ui_components) {

		Transform to_window = comp.m_to_circuit;
		to_window.translate(offset);

		if (comp.m_custom_ui_callback) {
			ImGui::SetCursorScreenPos(comp.m_circuit_min + offset);
			draw_list->ChannelsSetCurrent(1);
			comp.m_custom_ui_callback(&comp, to_window);
		}

		draw_list->ChannelsSetCurrent(0);         // background
		ImGui::SetCursorScreenPos(comp.m_circuit_min + offset);
		ImGui::InvisibleButton("node", comp.m_circuit_max - comp.m_circuit_min);

		if (!comp.m_tooltip.empty() && ImGui::IsItemHovered()) {
			ImGui::SetTooltip(comp.m_tooltip.c_str());
		}

    	draw_list->AddRect(comp.m_circuit_min + offset, comp.m_circuit_max + offset, 
						   COLOR_COMPONENT_BORDER);

		if (comp.m_icon) {
    		comp.m_icon->draw(to_window, comp.m_circuit_max - comp.m_circuit_min - Point(10,10), 
							  draw_list, 2, COLOR_COMPONENT_ICON);
		}
	}

	// pins
	for (const auto &pair : m_endpoints) {
		draw_list->AddCircleFilled(pair.second + offset, 2, COLOR_ENDPOINT);
	}

	// connections
	for (const auto &conn : m_ui_connections) {
		Point p0 = endpoint_position(conn.m_pin_a) + offset;
		Point p1 = endpoint_position(conn.m_pin_b) + offset;
		Point dp = {(p1.x - p0.x) * 0.25f, 0.0f};
		draw_list->AddBezierCurve(
			p0, p0 + dp, p1 - dp, p1,
			COLOR_CONNECTION[m_circuit->sim()->read_node(conn.m_node)], 
			2.0f
		);
	}

	draw_list->ChannelsMerge();

	// scrolling
    if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f)) {
		m_scroll_delta = m_scroll_delta + ImGui::GetIO().MouseDelta;
		if (m_scroll_delta.x < 0) { 
			m_scroll_delta.x = 0;
		}
		if (m_scroll_delta.y < 0) { 
			m_scroll_delta.y = 0;
		}
	}
}

Point UICircuit::endpoint_position(uint32_t pin) {
	auto res = m_endpoints.find(pin);
	if (res != m_endpoints.end()) {
		return res->second;
	} else {
		return {0, 0};
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// UICircuitBuilder
//

UICircuitBuilder::materialize_func_map_t UICircuitBuilder::m_materialize_funcs;

void UICircuitBuilder::register_materialize_func(ComponentType type, materialize_func_t func) {
	m_materialize_funcs[type] = func;
}

UICircuit::uptr_t UICircuitBuilder::create_circuit(Circuit *circuit) {
	auto ui_circuit = std::make_unique<UICircuit>(circuit);

    for (const auto &visual_comp : circuit->visual_components()) {
		materialize_component(ui_circuit.get(), visual_comp.get());
	}

    return std::move(ui_circuit);
}

void UICircuitBuilder::materialize_component(UICircuit *circuit, VisualComponent *visual_comp) {
	auto mat_func = m_materialize_funcs.find(visual_comp->get_type());
	if (mat_func != m_materialize_funcs.end()) {
		UIComponent comp = {0};
		comp.m_visual_comp = visual_comp;
		mat_func->second(visual_comp->get_component(), &comp, circuit);
		circuit->add_component(comp);
	}

	// connections
	typedef std::unordered_map<node_t, pin_t> node_origin_container_t;
	static node_origin_container_t node_origin_pins;

	auto *sim = circuit->circuit()->sim();
	const auto &pins = visual_comp->get_circuit() ? visual_comp->get_circuit()->ports_pins() : visual_comp->get_component()->pins();

	for (auto pin : pins) {
		auto node = sim->pin_node(pin);
		if (node == NOT_CONNECTED) {
			continue;
		}

		auto res = node_origin_pins.find(node);
		if (res != node_origin_pins.end()) {
			auto origin = res->second;
			circuit->add_connection(node, origin, pin);
		} else {
			node_origin_pins[node] = pin;
		}

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
	m_size = {img->width, img->height};

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

void ComponentIcon::draw(Transform transform, Point draw_size, ImDrawList *draw_list, size_t line_width, uint32_t color) const {

	Point scale_xy = draw_size / m_size;
	float scale = std::min(scale_xy.x, scale_xy.y);

    for (const auto &curve : m_curves) {
		draw_list->AddBezierCurve(
			transform.apply(curve[0] * scale), 
			transform.apply(curve[1] * scale), 
			transform.apply(curve[2] * scale), 
			transform.apply(curve[3] * scale), 
			color, 
			line_width);
	}
}


