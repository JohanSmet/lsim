// component_ui.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "component_ui.h"
#include "imgui/imgui.h"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

#include "colors.h"
#include "simulator.h"
#include "circuit.h"

namespace {

static const float GRID_SIZE = 10.0f;

static void compute_comp_aabb(Transform to_window, Point h_size, Point *min, Point *max) {
	assert(min);
	assert(max);

	*min = to_window.apply({-h_size.x, -h_size.y});
	*max = to_window.apply({h_size.x, h_size.y});

	if (max->x < min->x) {
        std::swap(min->x, max->x);
	}

	if (max->y < min->y) {
        std::swap(min->y, max->y);
	}
}

} // unnamed namespace

///////////////////////////////////////////////////////////////////////////////
//
// UICircuit
//

UICircuit::UICircuit(Circuit *circuit) : 
			m_circuit(circuit),
			m_name(circuit->name()),
			m_show_grid(true),
			m_scroll_delta(0,0),
			m_selected_comp(nullptr) {

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

	// grid
	draw_grid(draw_list);

	draw_list->ChannelsSplit(2);

	Point screen_origin = ImGui::GetCursorScreenPos();		// upper-left corner of the window in screen space
    Point offset = m_scroll_delta + screen_origin;			// translation from circuit space to screen space 

	// position of nearest grid point to the mouse cursor
	Point mouse_grid_point = Point(ImGui::GetMousePos()) - offset;
	mouse_grid_point.x = roundf(mouse_grid_point.x / GRID_SIZE) * GRID_SIZE;
	mouse_grid_point.y = roundf(mouse_grid_point.y / GRID_SIZE) * GRID_SIZE;


	// left mousebutton clicked : reset component selection
	//	if click was inside a component it will be reselectd in the component loop
	if (ImGui::IsMouseClicked(0)) {
		m_selected_comp = nullptr;
	}

	// components
	for (auto &comp : m_ui_components) {

		ImGui::PushID(&comp);

		Transform to_window = comp.m_to_circuit;
		to_window.translate(offset);

		Point comp_min, comp_max;
		compute_comp_aabb(to_window, comp.m_half_size, &comp_min, &comp_max);

		if (comp.m_custom_ui_callback) {
			ImGui::SetCursorScreenPos(comp_min);
			draw_list->ChannelsSetCurrent(1);
			comp.m_custom_ui_callback(&comp, to_window);
		}

		draw_list->ChannelsSetCurrent(0);         // background
		ImGui::SetCursorScreenPos(comp_min);
		ImGui::InvisibleButton("node", comp_max - comp_min);

		// tooltip
		if (!comp.m_tooltip.empty() && ImGui::IsItemHovered()) {
			ImGui::SetTooltip(comp.m_tooltip.c_str());
		}

		// draw border + icon
		auto border_color = COLOR_COMPONENT_BORDER;

		if (m_selected_comp == &comp) {
			draw_list->AddRectFilled(comp_min, comp_max, COLOR_COMPONENT_SELECTED);
			if (m_state == CS_DRAGGING) {
				border_color = COLOR_COMPONENT_BORDER_DRAGGING;
			}
		}

    	draw_list->AddRect(comp_min, comp_max, border_color);

		if (comp.m_icon) {
    		comp.m_icon->draw(to_window, comp_max - comp_min - Point(10,10), 
							  draw_list, 2, COLOR_COMPONENT_ICON);
		}

		// selection
		if (ImGui::IsItemClicked()) {
			m_selected_comp = &comp;
		}

		// dragging
		if (ImGui::IsItemActive()) {
			if (m_state == CS_IDLE) {
				m_state = CS_DRAGGING;
				comp.m_drag_delta = {0, 0};
			}
			move_component(&comp, ImGui::GetIO().MouseDelta);
		}


		ImGui::PopID();
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
		if (m_scroll_delta.x > 0) { 
			m_scroll_delta.x = 0;
		}
		if (m_scroll_delta.y > 0) { 
			m_scroll_delta.y = 0;
		}
	}

	// handle end of dragging
	if (m_state == CS_DRAGGING && !ImGui::IsMouseDragging(0, 0.0f)) {
		m_state = CS_IDLE;
	}

	// snap component to mouse cursor when creating
	if (m_state == CS_CREATING && m_selected_comp != nullptr) {
		move_component_abs(m_selected_comp, mouse_grid_point);
	}

	// clicking left mouse button while creating ends create state
	if (m_state == CS_CREATING && ImGui::IsMouseClicked(0)) {
		m_state = CS_IDLE;
	}

}

void UICircuit::move_component(UIComponent *ui_comp, Point delta) {
	
	ui_comp->m_drag_delta = ui_comp->m_drag_delta + delta;
	Point cur_pos = ui_comp->m_visual_comp->get_position();
	Point new_pos = cur_pos + ui_comp->m_drag_delta;

	// snap to grid
	if (ui_comp->m_drag_delta.x > 0) {
		new_pos.x = floorf(new_pos.x / GRID_SIZE) * GRID_SIZE;
	} else {
		new_pos.x = ceilf(new_pos.x / GRID_SIZE) * GRID_SIZE;
	}

	if (ui_comp->m_drag_delta.y > 0) {
		new_pos.y = floorf(new_pos.y / GRID_SIZE) * GRID_SIZE;
	} else {
		new_pos.y = ceilf(new_pos.y / GRID_SIZE) * GRID_SIZE;
	}

	Point dist = new_pos - cur_pos;

	ui_comp->m_to_circuit.translate(dist);

	for (const auto &pin : ui_comp->m_visual_comp->pins()) {
		auto &pos = m_endpoints[pin];
		pos = pos + dist;
	}

	ui_comp->m_visual_comp->set_position({new_pos.x, new_pos.y});

	if (dist.x != 0.0f) {
		ui_comp->m_drag_delta.x = 0;
	}

	if (dist.y != 0.0f) {
		ui_comp->m_drag_delta.y = 0;
	}
}

void UICircuit::move_component_abs(UIComponent *ui_comp, Point new_pos) {

	Point cur_pos = ui_comp->m_visual_comp->get_position();

	Point dist = new_pos - cur_pos;
	ui_comp->m_to_circuit.translate(dist);

	for (const auto &pin : ui_comp->m_visual_comp->pins()) {
		auto &pos = m_endpoints[pin];
		pos = pos + dist;
	}

	ui_comp->m_visual_comp->set_position({new_pos.x, new_pos.y});
}


void UICircuit::create_component(VisualComponent *vis_comp) {
	UICircuitBuilder::materialize_component(this, vis_comp);

	m_state = CS_CREATING;
	m_selected_comp = &m_ui_components.back();
}

Point UICircuit::endpoint_position(uint32_t pin) {
	auto res = m_endpoints.find(pin);
	if (res != m_endpoints.end()) {
		return res->second;
	} else {
		return {0, 0};
	}
}

void UICircuit::draw_grid(ImDrawList *draw_list) {
	if (!m_show_grid) {
		return;
	}

	Point win_pos = ImGui::GetCursorScreenPos();
	Point win_size = ImGui::GetWindowSize();

	for (float x = fmodf(m_scroll_delta.x, GRID_SIZE); x < win_size.x; x += GRID_SIZE) {
		draw_list->AddLine(Point(x, 0.0f) + win_pos, Point(x, win_size.y) + win_pos, COLOR_GRID_LINE);
	}

	for (float y = fmodf(m_scroll_delta.y, GRID_SIZE); y < win_size.y; y += GRID_SIZE) {
		draw_list->AddLine(Point(0.0f, y) + win_pos, Point(win_size.x, y) + win_pos, COLOR_GRID_LINE);
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

ComponentIcon::icon_lut_t ComponentIcon::m_icon_cache;

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

void ComponentIcon::draw(Point origin, Point draw_size, ImDrawList *draw_list, size_t line_width, uint32_t color) const {
	Transform transform;
	transform.translate(origin);
	draw(transform, draw_size, draw_list, line_width, color);
}

ComponentIcon *ComponentIcon::cache(uint32_t id, const char *data, size_t len) {
	m_icon_cache[id] = std::make_unique<ComponentIcon>(data, len);
	return cached(id);
}

ComponentIcon *ComponentIcon::cached(uint32_t id) {
	auto result = m_icon_cache.find(id);
	if (result == m_icon_cache.end()) {
		return nullptr;
	} else {
		return result->second.get();
	}
}