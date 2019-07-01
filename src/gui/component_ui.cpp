// component_ui.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "component_ui.h"
#include "imgui/imgui.h"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

#include <algorithm>
#include <set>

#include "colors.h"
#include "simulator.h"
#include "circuit_description.h"
#include "circuit_instance.h"
#include "circuit_library.h"
#include "lsim_context.h"
#include "main_gui.h"

namespace {

static const float GRID_SIZE = 10.0f;

static const char *POPUP_EMBED_CIRCUIT = "embed_circuit";
static const char *POPUP_SUB_CIRCUIT = "sub_circuit";

} // unnamed namespace

namespace lsim {

namespace gui {

size_t UICircuit::PointHash::operator() (const Point &p) const {
		// abuse the fact that positions will always be aligned to the grid
		int32_t x = (int32_t) p.x;
		int32_t y = (int32_t) p.y;
		return ((int64_t) y) << 32 | (size_t) x;
}


///////////////////////////////////////////////////////////////////////////////
//
// UIComponent
//

UIComponent::UIComponent(Component *component) :
		m_component(component),
		m_border(true),
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
	m_to_circuit.rotate(m_component->angle());
	m_to_circuit.translate(m_component->position());
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

void UIComponent::call_custom_ui_callback(UICircuit *circuit, Transform transform) {
	if (m_custom_ui_callback) {
		m_custom_ui_callback(circuit, this, transform);
	}
}

void UIComponent::add_endpoint(pin_id_t pin, Point location) {
	m_endpoints[pin] = location;
}

void UIComponent::add_pin_line(pin_id_t pin_start, size_t pin_count, float size, Point origin, Point inc) {
	if (pin_count == 0) {
		return;
	}

	auto odd  = pin_count % 2;
	auto half = (pin_count - odd) / 2;
	float segment_len = size / (2.0f * (half + 1));
	segment_len = roundf(segment_len / GRID_SIZE) * GRID_SIZE;
	auto segment_delta = inc * segment_len;
	auto pin = pin_start;

	// top half
	Point pos = origin - (segment_delta * half);

	for (size_t idx = 0; idx < half; ++idx) {
		add_endpoint(pin++, pos);
		pos = pos + segment_delta;
	}

	// center
	if (odd > 0) {
		add_endpoint(pin++, origin);
	}

	// bottom half
	pos = origin + segment_delta;

	for (size_t idx = 0; idx < half; ++idx) {
		add_endpoint(pin++, pos);
		pos = pos + segment_delta;
	}
}

void UIComponent::add_pin_line(pin_id_t pin_start, size_t pin_count, Point origin, Point delta) {
	if (pin_count == 0) {
		return;
	}

	auto pos = origin;
    for (size_t i = 0; i < pin_count; ++i) {
        add_endpoint(pin_start+i, pos);
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

UICircuit::UICircuit(CircuitDescription *circuit_desc) : 
			m_circuit_desc(circuit_desc),
			m_circuit_inst(nullptr),
			m_name(circuit_desc->name()),
			m_show_grid(true),
			m_scroll_delta(0,0),
			m_state(CS_IDLE),
			m_popup_component(nullptr) {
}

UIComponent *UICircuit::create_component(Component *component) {
	m_ui_components.push_back(std::make_unique<UIComponent>(component));
	auto comp = m_ui_components.back().get();
	UICircuitBuilder::materialize_component(comp);
	return comp;
}

void UICircuit::remove_component(UIComponent *ui_comp) {
	m_ui_components.erase(std::remove_if(m_ui_components.begin(), m_ui_components.end(),
							[ui_comp](const auto &uc) {return uc.get() == ui_comp;}));
}

void UICircuit::draw() {

	auto draw_list = ImGui::GetWindowDrawList();

	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

	ImGui::PushClipRect(ImGui::GetCursorScreenPos(), Point(ImGui::GetCursorScreenPos()) + ImGui::GetContentRegionMax(), true);
	draw_list->PushClipRect(ImGui::GetCursorScreenPos(), Point(ImGui::GetCursorScreenPos()) + ImGui::GetContentRegionMax(), true);

	// grid
	if (!is_simulating()) {
		draw_grid(draw_list);
	}

	// potential popups
	ui_popup_embed_circuit();
	ui_popup_sub_circuit();

	draw_list->ChannelsSplit(2);

	Point screen_origin = ImGui::GetCursorScreenPos();			// upper-left corner of the window in screen space
    Point offset = m_scroll_delta + screen_origin;				// translation from circuit space to screen space 
	Point mouse_pos_screen = ImGui::GetMousePos();
	Point mouse_pos = mouse_pos_screen - offset;				// position of mouse pointer in circuit space
	m_mouse_grid_point = { 										// position of nearest grid point to the mouse cursor
		roundf(mouse_pos.x / GRID_SIZE) * GRID_SIZE,
		roundf(mouse_pos.y / GRID_SIZE) * GRID_SIZE
	};

	if (m_state == CS_CREATE_WIRE) {
		// snap to diagonal (45 degree) / vertical / horizontal when creating wires
		auto delta = m_mouse_grid_point - m_segment_start;
		auto abs_delta = Point(fabs(delta.x), fabs(delta.y));
		if (abs_delta.x > 0 && abs_delta.y > 0 && abs(abs_delta.y - abs_delta.x) < 10) {
			m_mouse_grid_point = m_segment_start + Point(delta.x, abs_delta.x * ((delta.y < 0) ? -1.0f : 1.0f));
		} else if (abs_delta.y > abs_delta.x) {
			m_mouse_grid_point.x = m_segment_start.x;
		} else {
			m_mouse_grid_point.y = m_segment_start.y;
		}
	}

	// components
	m_hovered_pin = PIN_ID_INVALID;
	m_hovered_wire = nullptr;
	m_hovered_component = nullptr;

	for (auto &comp : m_ui_components) {

		ImGui::PushID(&comp);

		Transform to_screen = comp->to_circuit();
		to_screen.translate(offset);

		if (comp->has_custom_ui_callback()) {
			ImGui::SetCursorScreenPos(comp->aabb_min() + offset);
			draw_list->ChannelsSetCurrent(1);
			comp->call_custom_ui_callback(this, to_screen);
		}

		draw_list->ChannelsSetCurrent(0);         // background
		ImGui::SetCursorScreenPos(comp->aabb_min() + offset);
		ImGui::InvisibleButton("node", comp->aabb_size());

		// tooltip
		if (comp->has_tooltip() && ImGui::IsItemHovered()) {
			ImGui::SetTooltip("%s", comp->tooltip());
		}

		// draw border + icon
		auto border_color = COLOR_COMPONENT_BORDER;

		if (is_selected(comp.get())) {
			draw_list->AddRectFilled(comp->aabb_min() + offset, comp->aabb_max() + offset, 
									 COLOR_COMPONENT_SELECTED);
			if (m_state == CS_DRAGGING) {
				border_color = COLOR_COMPONENT_BORDER_DRAGGING;
			}
		}

		if (comp->has_border()) {
  		  	draw_list->AddRect(comp->aabb_min() + offset, comp->aabb_max() + offset, border_color);
		}

		if (comp->icon()) {
    		comp->icon()->draw(to_screen, comp->aabb_size() - Point(10,10), 
							   draw_list, 2, COLOR_COMPONENT_ICON);
		}

		if (ImGui::IsItemHovered()) {
			m_hovered_component = comp.get();
		}

		// end points
		for (const auto &pair : comp->endpoints()) {
			auto endpoint_screen = to_screen.apply(pair.second);
			auto endpoint_circuit = endpoint_screen - offset;

			auto pin_color = COLOR_ENDPOINT;
			if (is_simulating()) {
				pin_color = COLOR_CONNECTION[m_circuit_inst->pin_output(pair.first)];
			}

			draw_list->AddCircleFilled(endpoint_screen, 3, pin_color);
			m_point_pin_lut[endpoint_circuit] = pair.first;

			if (distance_squared(m_mouse_grid_point, endpoint_circuit) <= 2) {
				draw_list->AddCircle(endpoint_screen, 8, COLOR_ENDPOINT_HOVER, 12, 2);
				m_hovered_pin = pair.first;
				// XXX ImGui::SetTooltip("Pin = %d\nNode = %d", m_hovered_pin, m_circuit_desc->sim()->pin_node(m_hovered_pin));
			}
		}

		ImGui::PopID();
	}

	// draw wires
	for (const auto &wire_it : m_circuit_desc->wires()) {
		auto wire = wire_it.second.get();
		auto wire_color = COLOR_CONNECTION_UNDEFINED;

		if (is_simulating() && wire->num_pins() > 0) {
			auto value = m_circuit_inst->read_pin(wire->pin(0));
			wire_color = COLOR_CONNECTION[value];
		}

		// segments
		for (size_t idx = 0; idx < wire->num_segments(); ++idx) {
			auto segment_color = is_selected(wire->segment_by_index(idx)) ? COLOR_WIRE_SELECTED : wire_color;
			draw_list->AddLine(wire->segment_point(idx, 0) + offset, wire->segment_point(idx, 1) + offset, 
							   segment_color, 2.0f);
		}

		// draw junctions with more than 2 segments
		for (size_t idx = 0; idx < wire->num_junctions(); ++idx) {
			if (wire->junction_segment_count(idx) > 2) {
				draw_list->AddCircleFilled(wire->junction_position(idx) + offset, 4, wire_color);
			}
		}

		if (m_hovered_wire == nullptr && wire->point_on_wire(m_mouse_grid_point)) {
			draw_list->AddCircle(m_mouse_grid_point + offset, 8, COLOR_ENDPOINT_HOVER, 12, 2);
			m_hovered_wire = wire;
			if (m_hovered_pin == PIN_ID_INVALID) {
				// ImGui::SetTooltip("Node = %d", wire->node());
			}
		}
	} 

	if (m_state == CS_CREATE_WIRE) {
		m_line_anchors.back() = m_mouse_grid_point;
		Point p0 = m_line_anchors.front() + offset;

		for (const auto &anchor : m_line_anchors) {
			Point p1 = anchor + offset;
			draw_list->AddLine(p0, p1, IM_COL32(0, 0, 255, 255), 4);
			p0 = p1;
		}
	}

	// handle input
	bool mouse_in_window = ImGui::IsMouseHoveringWindow();

	// -> edit-mode: left mouse button down
	if (!is_simulating() && mouse_in_window && ImGui::IsMouseClicked(0)) {

		if (m_state == CS_CREATE_COMPONENT) {
			// ends CREATE_COMPONENT state
			m_state = CS_IDLE;
		} else if (m_state == CS_IDLE && m_hovered_pin != PIN_ID_INVALID) {
			// clicking on an endpoint activates CREATE_WIRE state
			m_state = CS_CREATE_WIRE;
			m_wire_start = {m_mouse_grid_point, m_hovered_pin, nullptr};
			m_line_anchors = {m_mouse_grid_point, m_mouse_grid_point};
			m_segment_start = m_mouse_grid_point;
		} else if (m_state == CS_IDLE && m_hovered_wire != nullptr) {
			// clicking on a wire activates CREATE_WIRE state
			m_state = CS_CREATE_WIRE;
			m_wire_start = {m_mouse_grid_point, PIN_ID_INVALID, m_hovered_wire};
			m_line_anchors = {m_mouse_grid_point, m_mouse_grid_point};
			m_segment_start = m_mouse_grid_point;
		} else if (m_state == CS_IDLE && 
				   m_hovered_pin == PIN_ID_INVALID &&
				   m_hovered_wire == nullptr &&
				   m_hovered_component == nullptr) {
			// clicking on the circuit background clears the selection
			clear_selection();
		} else if (m_state == CS_CREATE_WIRE && m_mouse_grid_point != m_wire_start.m_position) {
			// clicking while in CREATE_WIRE mode
			bool sw_create = false;

			if (m_hovered_pin != PIN_ID_INVALID) {
				m_wire_end = {m_mouse_grid_point, m_hovered_pin, nullptr};
				sw_create = true;
			} else if (m_hovered_wire != nullptr) {
				m_wire_end = {m_mouse_grid_point, PIN_ID_INVALID, m_hovered_wire};
				sw_create = true;
			}

			if (sw_create) {
				create_wire();
				m_state = CS_IDLE;
			} else {
				// add anchor when clicking in the empty circuit
				m_segment_start = m_line_anchors.back();
				m_line_anchors.push_back(m_segment_start);
			}
		}
	}

	// -> edit-mode: left mouse button up
	if (!is_simulating() && mouse_in_window && ImGui::IsMouseReleased(0)) {
		if (m_state == CS_IDLE && m_hovered_component != nullptr) {
			// component selection
			if (!ImGui::GetIO().KeyShift) {
				clear_selection();
				select_component(m_hovered_component);
			} else {
				toggle_selection(m_hovered_component);
			}
		} else if (m_state == CS_CREATE_WIRE && m_hovered_wire != nullptr &&
				   m_hovered_wire == m_wire_start.m_wire) {
			// wire selection
			m_state = CS_IDLE;
			auto segment = m_hovered_wire->segment_at_point(m_mouse_grid_point);

			if (!ImGui::GetIO().KeyShift) {
				clear_selection();
				select_wire_segment(segment);
			} else {
				toggle_selection(segment);
			}
		}
	}

	// -> edit-mode: right mouse button down
	if (!is_simulating() && mouse_in_window && ImGui::IsMouseClicked(1)) {
		if (m_state == CS_CREATE_WIRE) {
			// end CREATE_WIRE state
			m_state = CS_IDLE;
		}
	}

	// -> edit-mode: right mouse button released
	if (!is_simulating() && mouse_in_window && ImGui::IsMouseReleased(1)) {
		if (m_state == CS_IDLE && !m_hovered_component) {
			// popup menu to insert a sub circuit
			ui_popup_embed_circuit_open();
		}
	}

	// -> simulation-mode: right mouse button released
	if (is_simulating() && mouse_in_window && ImGui::IsMouseReleased(1)) {
		if (m_hovered_component && m_hovered_component->component()->type() == COMPONENT_SUB_CIRCUIT) {
			ui_popup_sub_circuit_open();
		}
	}

	// -> edit-mode: double-clicking
	if (!is_simulating() && mouse_in_window && ImGui::IsMouseDoubleClicked(0)) {
		if (m_state == CS_CREATE_WIRE) {
			// end CREATE_WIRE state without actually connecting to something
			m_wire_end = {m_mouse_grid_point, PIN_ID_INVALID, nullptr};
			create_wire();
			m_state = CS_IDLE;
		} else if (m_hovered_wire) {
			// select the entire wire
			for (size_t idx = 0; idx < m_hovered_wire->num_segments(); ++idx) {
				auto segment = m_hovered_wire->segment_by_index(idx);
				if (!is_selected(segment)) {
					select_wire_segment(segment);
				}
			}
		}
	}

	// -> edit-mode: keyboard
	if (!is_simulating() && ImGui::IsWindowFocused()) {
		if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete))) {
			delete_selected_components();
		}

		if (ImGui::GetIO().KeyCtrl) {
			if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_C))) {
				copy_selected_components();
			}
			if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_V))) {
				paste_components();
			}
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

	// start dragging
	if (mouse_in_window && m_state == CS_IDLE && ImGui::IsMouseDragging(0)) {
		m_state = CS_DRAGGING;
		m_drag_last = m_mouse_grid_point;
	}

	// handle end of dragging
	if (m_state == CS_DRAGGING && !ImGui::IsMouseDragging(0)) {
		m_state = CS_IDLE;
	}

	// move selected items while dragging
	if (m_state == CS_DRAGGING) {
		move_selected_components();
	}

	// snap component to mouse cursor when creating
	if (m_state == CS_CREATE_COMPONENT && selected_component() != nullptr) {
		move_component_abs(selected_component(), m_mouse_grid_point);
	}
	
	ImGui::PopClipRect();
	draw_list->PopClipRect();
	ImGui::EndChild();	// scrolling region
}

Point UICircuit::circuit_dimensions() const {
	Point result = {0, 0};

	for (const auto &ui_comp : m_ui_components) {
		auto comp_max = ui_comp->aabb_max();
		if (comp_max.x > result.x) {
			result.x = comp_max.x;
		}
		if (comp_max.y > result.y) {
			result.y = comp_max.y;
		}
	}

	return result;
}

void UICircuit::move_selected_components() {

	if (m_drag_last == m_mouse_grid_point) {
		return;
	}

	Point delta = m_mouse_grid_point - m_drag_last;
	m_drag_last = m_mouse_grid_point;

	for (auto &item : m_selection) {
		if (item.m_component) {
			auto comp = item.m_component->component();
			comp->set_position(comp->position() + delta);
			item.m_component->build_transform();
		}
	}

	for (auto &item : m_selection) {
		if (item.m_component) {
			fix_component_connections(item.m_component);
		}
	}
}

void UICircuit::delete_selected_components() {

	std::set<Wire *>	touched_wires;

	for (auto &item : m_selection) {
		if (item.m_component) {
			auto comp = item.m_component->component();
			m_circuit_desc->remove_component(comp->id());
			remove_component(item.m_component);
		} else if (item.m_segment) {
			auto wire = item.m_segment->wire();
			wire->remove_segment(item.m_segment);
			touched_wires.insert(wire);
		}
	}

	for (auto wire : touched_wires) {
		wire->clear_pins();

		if (!wire->in_one_piece()) {
			std::vector<Wire *> new_wires;

			while (wire->num_segments() > 0) {
				auto reachable_segments = wire->reachable_segments(wire->segment_by_index(0));
				Wire *new_wire = m_circuit_desc->create_wire();

				for (auto segment : reachable_segments) {
					new_wire->add_segment(segment->junction(0)->position(), segment->junction(1)->position());
					wire->remove_segment(segment);
				}

				wire_make_connections(new_wire);
				new_wires.push_back(new_wire);
			}

			m_circuit_desc->remove_wire(wire->id());
		} else {
			wire->simplify();
			wire_make_connections(wire);
		}
	}

	clear_selection();
}

void UICircuit::move_component_abs(UIComponent *ui_comp, Point new_pos) {
	ui_comp->component()->set_position({new_pos.x, new_pos.y});
	ui_comp->build_transform();
}


void UICircuit::ui_create_component(Component *comp) {
	m_state = CS_CREATE_COMPONENT;
	clear_selection();
	select_component(create_component(comp));
}

void UICircuit::embed_circuit(const char *name) {
	auto comp = m_circuit_desc->add_sub_circuit(name, 0, 0);
	comp->sync_nested_circuit(m_circuit_desc->context());
	comp->set_position(m_mouse_grid_point);
	create_component(comp);
}

void UICircuit::create_wire() {

	// nothing to do if drawn wire ends in the same place it begins
	if (m_wire_start.m_position == m_wire_end.m_position) {
		return;
	}

	if (m_wire_start.m_pin != PIN_ID_INVALID && m_wire_end.m_pin != PIN_ID_INVALID) {
		// a new wire between two pins
		auto wire = m_circuit_desc->create_wire();
		wire->add_segments(m_line_anchors.data(), m_line_anchors.size());
		wire->add_pin(m_wire_start.m_pin);
		wire->add_pin(m_wire_end.m_pin);
	} else if (m_wire_end.m_pin == PIN_ID_INVALID && m_wire_end.m_wire == nullptr) {
		// wire without an explicit endpoint
		Wire *wire = m_wire_start.m_wire;
		if (!wire) {
			wire = m_circuit_desc->create_wire();
			wire->add_pin(m_wire_start.m_pin);
		} else {
			wire->split_at_new_junction(m_wire_start.m_position);
		}
		wire->add_segments(m_line_anchors.data(), m_line_anchors.size());
		wire->simplify();
	} else if (m_wire_start.m_wire != nullptr && m_wire_end.m_wire != nullptr) {
		// the newly drawn wire merges two wires
		if (m_wire_start.m_wire != m_wire_end.m_wire) {
			m_wire_start.m_wire->merge(m_wire_end.m_wire);
			m_circuit_desc->remove_wire(m_wire_end.m_wire->id());
		}
		auto wire = m_wire_start.m_wire;
		wire->split_at_new_junction(m_wire_start.m_position);
		wire->split_at_new_junction(m_wire_end.m_position);
		wire->add_segments(m_line_anchors.data(), m_line_anchors.size());
	} else {
		// join a pin to an existing wire
		auto pin = (m_wire_start.m_pin != PIN_ID_INVALID) ? m_wire_start.m_pin : m_wire_end.m_pin;
		auto wire = (m_wire_start.m_wire != nullptr) ? m_wire_start.m_wire : m_wire_end.m_wire;
		auto junction = (m_wire_start.m_wire != nullptr) ? m_wire_start.m_position : m_wire_end.m_position;

		wire->split_at_new_junction(junction);
		wire->add_segments(m_line_anchors.data(), m_line_anchors.size());
		wire->simplify();
		wire->add_pin(pin);
	}
}

void UICircuit::clear_selection() {
	m_selection.clear();
}

void UICircuit::select_component(UIComponent *component) {
	if (is_selected(component)) {
		return;
	}

	m_selection.push_back({component, nullptr});
}

void UICircuit::deselect_component(UIComponent *component) {
	m_selection.erase(std::remove_if(m_selection.begin(), m_selection.end(),
						[component](const auto &s) {return s.m_component == component;}));
}

void UICircuit::select_wire_segment(WireSegment *segment) {
	if (is_selected(segment)) {
		return;
	}

	m_selection.push_back({nullptr, segment});
}

void UICircuit::deselect_wire_segment(WireSegment *segment) {
	m_selection.erase(std::remove_if(m_selection.begin(), m_selection.end(),
						[segment](const auto &s) {return s.m_segment == segment;}));
}

void UICircuit::toggle_selection(UIComponent *component) {
	if (is_selected(component)) {
		deselect_component(component);
	} else {
		select_component(component);
	}
}

void UICircuit::toggle_selection(WireSegment *segment) {
	if (is_selected(segment)) {
		deselect_wire_segment(segment);
	} else {
		select_wire_segment(segment);
	}
}

bool UICircuit::is_selected(UIComponent *component) {
	return std::any_of(m_selection.begin(), m_selection.end(),
						[component](const auto &s) {return s.m_component == component;});
}

bool UICircuit::is_selected(WireSegment *segment) {
	return std::any_of(m_selection.begin(), m_selection.end(),
						[segment](const auto &s) {return s.m_segment == segment;});
}

UIComponent *UICircuit::selected_component() const {
	if (m_selection.size() == 1) {
		return m_selection.front().m_component;
	} else {
		return nullptr;
	}
}

void UICircuit::set_simulation_instance(CircuitInstance *circuit_inst, bool view_only) {

	if (circuit_inst && m_circuit_inst == nullptr) {
		m_state = CS_SIMULATING;
		m_view_only = view_only;
		clear_selection();
	}

	if (!circuit_inst && m_circuit_inst != nullptr) {
		m_state = CS_IDLE;
		m_view_only = false;
	}

	m_circuit_inst = circuit_inst;
}

bool UICircuit::is_simulating() const {
	return m_state == CS_SIMULATING;
}

void UICircuit::fix_component_connections(UIComponent *ui_comp) {
	assert(ui_comp);
	auto comp = ui_comp->component();

	// disconnect the component
	m_circuit_desc->disconnect_component(comp->id());

	// iterate over the endpoints of the component and check if it should be connected to a wire
	for (const auto &c_pair : ui_comp->endpoints()) {
		auto pin_id = c_pair.first;
		auto position = ui_comp->to_circuit().apply(c_pair.second);

		for (auto &w_pair : m_circuit_desc->wires()) {
			auto wire = w_pair.second.get();
			if (wire->point_on_wire(position)) {
				wire->split_at_new_junction(position);
				wire->add_pin(pin_id);
			}
		}
	}
}

void UICircuit::copy_selected_components() {
	m_copy_components.clear();
	m_copy_center = {0, 0};

	for (auto &item : m_selection) {
		if (item.m_component) {
			auto comp = item.m_component->component();
			m_copy_components.push_back(comp->copy());
			m_copy_center = m_copy_center + comp->position();
		}
	}

	if (!m_copy_components.empty()) {
		m_copy_center = m_copy_center * (1.0f / m_copy_components.size());
		m_copy_center = {
			roundf(m_copy_center.x / GRID_SIZE) * GRID_SIZE,
			roundf(m_copy_center.y / GRID_SIZE) * GRID_SIZE
		};
	}
}

void UICircuit::paste_components() {
	clear_selection();

	for (const auto &comp : m_copy_components) {
		auto pasted = m_circuit_desc->paste_component(comp.get());
		pasted->set_position(m_mouse_grid_point + comp->position() - m_copy_center);
		auto ui_comp = create_component(pasted);
		select_component(ui_comp);
	}
}

void UICircuit::wire_make_connections(Wire *wire) {
	for (size_t j = 0; j < wire->num_junctions(); ++j) {
		auto junction = wire->junction_position(j);	
		auto found = m_point_pin_lut.find(junction);
		if (found != m_point_pin_lut.end()) {
			wire->add_pin(found->second);
		}
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

void UICircuit::ui_popup_embed_circuit() {
	if (ImGui::BeginPopup(POPUP_EMBED_CIRCUIT)) {
		auto lib = m_circuit_desc->context()->user_library();
		auto max = lib->num_circuits();

		ImGui::Text("Choose circuit to embed:");

		for (size_t i = 0; i < max; ++i) {
			auto sub = lib->circuit_by_idx(i);
			if (ImGui::Selectable(sub->name().c_str())) {
				embed_circuit(sub->name().c_str());
			}
		}
	
		if (max <= 0) {
			ImGui::Text("No circuits available to embed.");
		}

		ImGui::EndPopup();
	}
}

void UICircuit::ui_popup_embed_circuit_open() {
	ImGui::OpenPopup(POPUP_EMBED_CIRCUIT);
}

void UICircuit::ui_popup_sub_circuit() {
	if (ImGui::BeginPopup(POPUP_SUB_CIRCUIT)) {
		if (ImGui::Selectable("Drill down ...")) {
			main_gui_drill_down_sub_circuit(m_circuit_inst, m_popup_component->component());
		}
		ImGui::EndPopup();
	} 
}

void UICircuit::ui_popup_sub_circuit_open() {
	m_popup_component = m_hovered_component;
	ImGui::OpenPopup(POPUP_SUB_CIRCUIT);
}

///////////////////////////////////////////////////////////////////////////////
//
// UICircuitBuilder
//

UICircuitBuilder::materialize_func_map_t UICircuitBuilder::m_materialize_funcs;

void UICircuitBuilder::register_materialize_func(ComponentType type, materialize_func_t func) {
	m_materialize_funcs[type] = func;
}

UICircuit::uptr_t UICircuitBuilder::create_circuit(CircuitDescription *circuit) {
	auto ui_circuit = std::make_unique<UICircuit>(circuit);

	auto comp_ids = circuit->component_ids();
    for (const auto &comp_id : comp_ids) {
		auto comp = circuit->component_by_id(comp_id);
		ui_circuit->create_component(comp);
	}

    return std::move(ui_circuit);
}

void UICircuitBuilder::materialize_component(UIComponent *ui_component) {
	auto mat_func = m_materialize_funcs.find(ui_component->component()->type());
	if (mat_func != m_materialize_funcs.end()) {
		mat_func->second(ui_component->component(), ui_component);
	}
}

void UICircuitBuilder::rematerialize_component(UICircuit *circuit, UIComponent *ui_component) {
	assert(circuit);
	assert(ui_component);

	ui_component->dematerialize();
	materialize_component(ui_component);
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

} // namespace lsim

} // namespace gui