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

} // unnamed namespace

///////////////////////////////////////////////////////////////////////////////
//
// UIComponent
//

UIComponent::UIComponent(VisualComponent *vis_comp) :
		m_visual_comp(vis_comp),
		m_tooltip(""),
		m_half_size(0.0f, 0.0f),
		m_icon(nullptr),
		m_custom_ui_callback(nullptr) {
	build_transform();
}

void UIComponent::change_tooltip(const char *tooltip) {
	m_tooltip = tooltip;
}

void UIComponent::change_icon(const ComponentIcon *icon) {
	m_icon = icon;
}

void UIComponent::build_transform() {
	m_to_circuit.reset();
	m_to_circuit.rotate(m_visual_comp->get_orientation());
	m_to_circuit.translate(m_visual_comp->get_position());
	recompute_aabb();
}

void UIComponent::change_size(float width, float height) {
	m_half_size = {width / 2.0f, height / 2.0f};
	recompute_aabb();
}

void UIComponent::recompute_aabb() {
	m_aabb_min = m_to_circuit.apply({-m_half_size.x, -m_half_size.y});
	m_aabb_max = m_to_circuit.apply({m_half_size.x, m_half_size.y});

	if (m_aabb_max.x < m_aabb_min.x) {
        std::swap(m_aabb_min.x, m_aabb_max.x);
	}

	if (m_aabb_max.y < m_aabb_min.y) {
        std::swap(m_aabb_min.y, m_aabb_max.y);
	}
}

void UIComponent::set_custom_ui_callback(ui_component_func_t func) {
	m_custom_ui_callback = func;
}

void UIComponent::call_custom_ui_callback(Transform transform) {
	if (m_custom_ui_callback) {
		m_custom_ui_callback(this, transform);
	}
}

void UIComponent::add_endpoint(uint32_t pin, Point location) {
	m_endpoints[pin] = location;
}

void UIComponent::add_pin_line(const uint32_t *pins, size_t pin_count, float size, Point origin, Point inc) {
	if (pin_count == 0) {
		return;
	}

	auto odd  = pin_count % 2;
	auto half = (pin_count - odd) / 2;
	float segment_len = size / (2.0f * (half + 1));
	segment_len = roundf(segment_len / GRID_SIZE) * GRID_SIZE;
	auto segment_delta = inc * segment_len;
	size_t pin_idx = 0;

	// top half
	Point pos = origin - (inc * segment_delta);

	for (size_t idx = 0; idx < half; ++idx) {
		add_endpoint(pins[pin_idx++], pos);
		pos = pos + segment_delta;
	}

	// center
	if (odd > 0) {
		add_endpoint(pins[pin_idx++], origin);
	}

	// bottom half
	pos = origin + segment_delta;

	for (size_t idx = 0; idx < half; ++idx) {
		add_endpoint(pins[pin_idx++], pos);
		pos = pos + segment_delta;
	}
}

void UIComponent::add_pin_line(const uint32_t *pins, size_t pin_count, Point origin, Point delta) {
	if (pin_count == 0) {
		return;
	}

	Point pos = origin;
    for (size_t i = 0; i < pin_count; ++i) {
        add_endpoint(pins[i], pos);
		pos = pos + delta;
	}
}

void UIComponent::dematerialize() {
	m_endpoints.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
// UICircuit
//

UICircuit::UICircuit(Circuit *circuit) : 
			m_circuit(circuit),
			m_name(circuit->name()),
			m_show_grid(true),
			m_scroll_delta(0,0),
			m_state(CS_IDLE),
			m_selected_comp(nullptr) {

}

void UICircuit::add_component(const UIComponent &comp) {
	m_ui_components.push_back(comp);
}

void UICircuit::add_connection(uint32_t node, uint32_t pin_1, uint32_t pin_2) {
	m_ui_connections.push_back({node, pin_1, pin_2});
}

void UICircuit::draw() {

	auto draw_list = ImGui::GetWindowDrawList();

	// grid
	draw_grid(draw_list);

	draw_list->ChannelsSplit(2);

	Point screen_origin = ImGui::GetCursorScreenPos();			// upper-left corner of the window in screen space
    Point offset = m_scroll_delta + screen_origin;				// translation from circuit space to screen space 
	Point mouse_pos_screen = ImGui::GetMousePos();
	Point mouse_pos = mouse_pos_screen - offset;				// position of mouse pointer in circuit space
	Point mouse_grid_point = { 									// position of nearest grid point to the mouse cursor
		roundf(mouse_pos.x / GRID_SIZE) * GRID_SIZE,
		roundf(mouse_pos.y / GRID_SIZE) * GRID_SIZE
	};

	// left mousebutton clicked : reset component selection
	//	if click was inside a component it will be reselectd in the component loop
	if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered()) {
		m_selected_comp = nullptr;
	}

	// components
	m_selected_pin = PIN_UNDEFINED;

	for (auto &comp : m_ui_components) {

		ImGui::PushID(&comp);

		Transform to_screen = comp.to_circuit();
		to_screen.translate(offset);

		if (comp.has_custom_ui_callback()) {
			ImGui::SetCursorScreenPos(comp.aabb_min() + offset);
			draw_list->ChannelsSetCurrent(1);
			comp.call_custom_ui_callback(to_screen);
		}

		draw_list->ChannelsSetCurrent(0);         // background
		ImGui::SetCursorScreenPos(comp.aabb_min() + offset);
		ImGui::InvisibleButton("node", comp.aabb_size());

		// tooltip
		if (comp.has_tooltip() && ImGui::IsItemHovered()) {
			ImGui::SetTooltip(comp.tooltip());
		}

		// draw border + icon
		auto border_color = COLOR_COMPONENT_BORDER;

		if (m_selected_comp == &comp) {
			draw_list->AddRectFilled(comp.aabb_min() + offset, comp.aabb_max() + offset, 
									 COLOR_COMPONENT_SELECTED);
			if (m_state == CS_DRAGGING) {
				border_color = COLOR_COMPONENT_BORDER_DRAGGING;
			}
		}

    	draw_list->AddRect(comp.aabb_min() + offset, comp.aabb_max() + offset, border_color);

		if (comp.icon()) {
    		comp.icon()->draw(to_screen, comp.aabb_size() - Point(10,10), 
							  draw_list, 2, COLOR_COMPONENT_ICON);
		}

		// selection
		if (ImGui::IsItemClicked()) {
			m_selected_comp = &comp;
		}

		// dragging
		if (ImGui::IsItemActive() && m_state == CS_IDLE && m_selected_pin == PIN_UNDEFINED) {
			m_state = CS_DRAGGING;
			m_drag_delta = {0, 0};
		}

		if (m_state == CS_DRAGGING && m_selected_comp == &comp) {
			move_component(&comp, ImGui::GetIO().MouseDelta);
		}

		// end points
		for (const auto &pair : comp.endpoints()) {
			auto endpoint_screen = to_screen.apply(pair.second);
			draw_list->AddCircleFilled(endpoint_screen, 3, COLOR_ENDPOINT);

			if (distance_squared(endpoint_screen, mouse_pos_screen) <= 16) {
				draw_list->AddCircle(endpoint_screen, 8, COLOR_ENDPOINT_HOVER, 12, 2);
				m_selected_pin = pair.first;
				m_segment_start = endpoint_screen - offset;
			}
		}


		ImGui::PopID();
	}

	// connections
	/*for (const auto &conn : m_ui_connections) {
		Point p0 = endpoint_position(conn.m_pin_a) + offset;
		Point p1 = endpoint_position(conn.m_pin_b) + offset;
		Point dp = {(p1.x - p0.x) * 0.25f, 0.0f};
		draw_list->AddBezierCurve(
			p0, p0 + dp, p1 - dp, p1,
			COLOR_CONNECTION[m_circuit->sim()->read_node(conn.m_node)], 
			2.0f
		);
	} */

	// draw wires
	for (const auto &wire : m_circuit->wires()) {
		for (const auto &segment : wire->segments()) {
			draw_list->AddLine(segment[0] + offset, segment[1] + offset, 
							   COLOR_CONNECTION[m_circuit->sim()->read_node(wire->node())], 
							   2.0f);
		}
	}

	if (m_state == CS_CREATE_WIRE) {
		// snap to diagonal (45 degree) / vertical / horizontal
		auto delta = mouse_grid_point - m_segment_start;
		auto abs_delta = Point(fabs(delta.x), fabs(delta.y));
		if (abs_delta.x > 0 && abs_delta.y > 0 && abs(abs_delta.y - abs_delta.x) < 10) {
			m_line_anchors.back() = m_segment_start + Point(delta.x, abs_delta.x * ((delta.y < 0) ? -1.0f : 1.0f));
		} else if (abs_delta.y > abs_delta.x) {
			m_line_anchors.back() = {m_segment_start.x, mouse_grid_point.y};
		} else {
			m_line_anchors.back() = {mouse_grid_point.x, m_segment_start.y};
		}

		// Point p0 = endpoint_position(m_line_origin) + offset;
		Point p0 = m_line_anchors.front() + offset;

		for (const auto &anchor : m_line_anchors) {
			Point p1 = anchor + offset;
			draw_list->AddLine(p0, p1, IM_COL32(0, 0, 255, 255), 4);
			p0 = p1;
		}
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
	if (m_state == CS_CREATE_COMPONENT && m_selected_comp != nullptr) {
		move_component_abs(m_selected_comp, mouse_grid_point);
	}

	// clicking left mouse button while creating ends create state
	if (m_state == CS_CREATE_COMPONENT && ImGui::IsMouseClicked(0)) {
		m_state = CS_IDLE;
	}

	// clicking on a pin (endpoints) activates to CREATE_WIRE mode
	if (m_state == CS_IDLE && m_selected_pin != PIN_UNDEFINED && ImGui::IsMouseClicked(0)) {
		m_state = CS_CREATE_WIRE;
		m_line_origin = m_selected_pin;
		m_line_anchors = {m_segment_start, m_segment_start};
	}

	// right-clicking aborts CREATE_WIRE mode
	if (m_state == CS_CREATE_WIRE && ImGui::IsMouseClicked(1)) {
		m_state = CS_IDLE;
	}

	// clicking while in CREATE_WIRE mode
	if (m_state == CS_CREATE_WIRE && ImGui::IsMouseClicked(0)) {
		if (m_selected_pin != PIN_UNDEFINED && m_selected_pin != m_line_origin) {
			// create wire when another endpoint was clicked
			auto wire = m_circuit->create_wire(m_line_anchors.size(), m_line_anchors.data());
			m_circuit->connect_pins(m_line_origin, m_selected_pin);
			wire->set_node(m_circuit->sim()->pin_node(m_line_origin));
			m_state = CS_IDLE;
		} else {
			// add anchor when clicking in the empty circuit
			m_segment_start = m_line_anchors.back();
			m_line_anchors.push_back(m_segment_start);
		}
	}

	// double-clicking while in CREATE_WIRE mode
	/* disabled for now because each wire needs to have a node associated with it
	if (m_state == CS_CREATE_WIRE && ImGui::IsMouseDoubleClicked(0)) {
		m_circuit->create_wire(m_line_anchors.size(), m_line_anchors.data());
		m_state = CS_IDLE;
	}*/
}

void UICircuit::move_component(UIComponent *ui_comp, Point delta) {
	
	m_drag_delta = m_drag_delta + delta;
	Point cur_pos = ui_comp->visual_comp()->get_position();
	Point new_pos = cur_pos + m_drag_delta;

	// snap to grid
	if (m_drag_delta.x > 0) {
		new_pos.x = floorf(new_pos.x / GRID_SIZE) * GRID_SIZE;
	} else {
		new_pos.x = ceilf(new_pos.x / GRID_SIZE) * GRID_SIZE;
	}

	if (m_drag_delta.y > 0) {
		new_pos.y = floorf(new_pos.y / GRID_SIZE) * GRID_SIZE;
	} else {
		new_pos.y = ceilf(new_pos.y / GRID_SIZE) * GRID_SIZE;
	}

	Point dist = new_pos - cur_pos;

	ui_comp->visual_comp()->set_position({new_pos.x, new_pos.y});
	ui_comp->build_transform();

	if (dist.x != 0.0f) {
		m_drag_delta.x = 0;
	}

	if (dist.y != 0.0f) {
		m_drag_delta.y = 0;
	}
}

void UICircuit::move_component_abs(UIComponent *ui_comp, Point new_pos) {

	ui_comp->visual_comp()->set_position({new_pos.x, new_pos.y});
	ui_comp->build_transform();
}


void UICircuit::create_component(VisualComponent *vis_comp) {
	UICircuitBuilder::materialize_component(this, vis_comp);

	m_state = CS_CREATE_COMPONENT;
	m_selected_comp = &m_ui_components.back();
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
		UIComponent comp(visual_comp);
		mat_func->second(visual_comp->get_component(), &comp, circuit);
		circuit->add_component(comp);
	}

	// connections
/*	typedef std::unordered_map<node_t, pin_t> node_origin_container_t;
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
*/
}

void UICircuitBuilder::rematerialize_component(UICircuit *circuit, UIComponent *ui_component) {
	assert(circuit);
	assert(ui_component);

	ui_component->dematerialize();

	auto mat_func = m_materialize_funcs.find(ui_component->visual_comp()->get_type());
	if (mat_func != m_materialize_funcs.end()) {
		mat_func->second(ui_component->visual_comp()->get_component(), ui_component, circuit);
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