// circuit_editor.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "circuit_editor.h"
#include "component_icon.h"
#include "component_widget.h"
#include "configuration.h"
#include "imgui_ex.h"

#include "std_helper.h"
#include <set>

#include "colors.h"
#include "lsim_context.h"
#include "ui_context.h"

namespace {

constexpr const char *POPUP_EMBED_CIRCUIT = "embed_circuit";
constexpr const char *POPUP_SUB_CIRCUIT = "sub_circuit";
constexpr const char* POPUP_EDIT_SEGMENT = "edit_segment";

} // unnamed namespace

namespace lsim {

namespace gui {

size_t CircuitEditor::PointHash::operator() (const Point &p) const {
		// abuse the fact that positions will always be aligned to the grid
		auto x = (int32_t) p.x;
		auto y = (int32_t) p.y;
		return ((int64_t) y) << 32 | (size_t) x;
}


CircuitEditor::CircuitEditor(ModelCircuit *model_circuit) : 
			m_model_circuit(model_circuit),
			m_sim_circuit(nullptr),
			m_view_only(false),
			m_show_grid(true),
			m_scroll_delta(0,0),
			m_state(CS_IDLE),
			m_hovered_pin(PIN_ID_INVALID),
			m_hovered_widget(nullptr),
			m_hovered_wire(nullptr),
			m_popup_component(nullptr) {
}

void CircuitEditor::refresh(UIContext *ui_context) {

	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse);
	init_ui_refresh();
	draw_ui(ui_context);
	user_interaction();
	ImGui::EndChild();	// scrolling region
}

void CircuitEditor::init_ui_refresh() {

	Point screen_origin = ImGui::GetCursorScreenPos();			// upper-left corner of the window in screen space
	m_screen_offset = m_scroll_delta + screen_origin;			// translation from circuit space to screen space 
	Point mouse_pos_screen = ImGui::GetMousePos();				// current position of mouse pointer in screen space
	Point mouse_pos = mouse_pos_screen - m_screen_offset;		// position of mouse pointer in circuit space
	
	// snap mouse position to grid
	m_mouse_grid_point = { 										
		roundf(mouse_pos.x / GRID_SIZE) * GRID_SIZE,
		roundf(mouse_pos.y / GRID_SIZE) * GRID_SIZE
	};

	// restrain mouse position even further while creating a wire
	if (m_state == CS_CREATE_WIRE) {
		// snap to diagonal (45 degree) / vertical / horizontal when creating wires
		auto delta = m_mouse_grid_point - m_segment_start;
		auto abs_delta = Point(fabsf(delta.x), fabsf(delta.y));
		if (abs_delta.x > 0 && abs_delta.y > 0 && fabsf(abs_delta.y - abs_delta.x) < 10) {
			m_mouse_grid_point = m_segment_start + Point(delta.x, abs_delta.x * ((delta.y < 0) ? -1.0f : 1.0f));
		} else if (abs_delta.y > abs_delta.x) {
			m_mouse_grid_point.x = m_segment_start.x;
		} else {
			m_mouse_grid_point.y = m_segment_start.y;
		}
	}

	// screenspace point -> pin lookup is recreated every frame 
	m_point_pin_lut.clear();

	// reset hovered items
	m_hovered_pin = PIN_ID_INVALID;
	m_hovered_wire = nullptr;
	m_hovered_widget = nullptr;
}

void CircuitEditor::draw_ui(UIContext *ui_context) {

	auto draw_list = ImGui::GetWindowDrawList();
	
	// clipping
	ImGui::PushClipRect(ImGui::GetCursorScreenPos(), Point(ImGui::GetCursorScreenPos()) + ImGui::GetContentRegionMax(), true);
	draw_list->PushClipRect(ImGui::GetCursorScreenPos(), Point(ImGui::GetCursorScreenPos()) + ImGui::GetContentRegionMax(), true);

	// grid
	if (!is_simulating()) {
		draw_grid(draw_list);
	}

	// potential popup windows
	ui_popup_embed_circuit();
	ui_popup_sub_circuit(ui_context);
	ui_popup_edit_segment();
	
	// create two layers to draw the background and the widgets
	draw_list->ChannelsSplit(2);

	// draw all widgets
	for (auto &widget : m_widgets) {

		ImGui::PushID(&widget);
		
		// setup
		Transform widget_to_screen = widget->to_circuit();
		widget_to_screen.translate(m_screen_offset);

		const auto widget_aabb_min = widget->aabb_min() + m_screen_offset;
		const auto widget_aabb_max = widget->aabb_max() + m_screen_offset;

		// custom draw routine
		if (widget->has_draw_callback()) {
			ImGui::SetCursorScreenPos(widget_aabb_min);
			draw_list->ChannelsSetCurrent(1);
			widget->run_draw_callback(this, widget_to_screen);
		}
	
		// invisible button used for widget selection
		draw_list->ChannelsSetCurrent(0);         // background
		ImGui::SetCursorScreenPos(widget_aabb_min);
		ImGui::InvisibleButton("widget", widget->aabb_size());

		// tooltip
		if (widget->has_tooltip() && ImGui::IsItemHovered()) {
			ImGui::SetTooltip("%s", widget->tooltip());
		}

		// draw border
		auto border_color = COLOR_COMPONENT_BORDER;

		if (is_selected(widget.get())) {
			draw_list->AddRectFilled(widget_aabb_min, widget_aabb_max, COLOR_COMPONENT_SELECTED);
			if (m_state == CS_DRAGGING) {
				border_color = COLOR_COMPONENT_BORDER_DRAGGING;
			}
		}

		if (widget->has_border()) {
  		  	draw_list->AddRect(widget_aabb_min, widget_aabb_max, border_color);
		}

		// draw icon
		if (widget->has_border() && widget->icon() != nullptr) {
    		widget->icon()->draw(widget_to_screen, widget->aabb_size() - Point(10,10), 
							   draw_list, 2, COLOR_COMPONENT_ICON);
		}

		// check for mouse hover
		if (ImGui::IsItemHovered()) {
			m_hovered_widget = widget.get();
		}

		// end points
		for (const auto &pair : widget->endpoints()) {
			// pair.first = pin-id ; pair.second = position
			auto endpoint_screen = widget_to_screen.apply(pair.second);
			auto endpoint_circuit = endpoint_screen - m_screen_offset;

			auto pin_color = COLOR_ENDPOINT;
			if (is_simulating()) {
				pin_color = COLOR_CONNECTION[m_sim_circuit->pin_output(pair.first)];
			}

			m_point_pin_lut[endpoint_circuit] = pair.first;

			draw_list->AddCircleFilled(endpoint_screen, 3, pin_color);

			if (distance_squared(m_mouse_grid_point, endpoint_circuit) <= 2) {
				draw_list->AddCircle(endpoint_screen, 8, COLOR_ENDPOINT_HOVER, 12, 2);
				m_hovered_pin = pair.first;
			}
		}

		ImGui::PopID();
	}

	// draw wires
	for (const auto &wire_it : m_model_circuit->wires()) {
		auto wire = wire_it.second.get();
		auto wire_color = COLOR_CONNECTION_UNDEFINED;
		bool dirty_node = false;

		// check color & highlight when simulation is running
		if (is_simulating() && wire->num_pins() > 0) {
			auto value = m_sim_circuit->read_pin(wire->pin(0));
			wire_color = COLOR_CONNECTION[value];

			auto node_id = m_sim_circuit->pin_node(wire->pin(0));
			dirty_node = m_sim_circuit->node_dirty(node_id);
		}

		// segments
		for (size_t idx = 0; idx < wire->num_segments(); ++idx) {
			const auto p0 = wire->segment_point(idx, 0) + m_screen_offset;
			const auto p1 = wire->segment_point(idx, 1) + m_screen_offset;

			if (dirty_node) {
				draw_list->AddLine(p0, p1, COLOR_CONNECTION_DIRTY, 4.0f);
			}

			auto segment_color = is_selected(wire->segment_by_index(idx)) ? COLOR_WIRE_SELECTED : wire_color;
			draw_list->AddLine(p0, p1, segment_color, 2.0f);
		}

		// draw junctions with more than 2 segments
		for (size_t idx = 0; idx < wire->num_junctions(); ++idx) {
			if (wire->junction_segment_count(idx) > 2) {
				draw_list->AddCircleFilled(wire->junction_position(idx) + m_screen_offset, 4, wire_color);
			}
		}
		
		// check for hovered wire
		if (m_hovered_wire == nullptr && wire->point_on_wire(m_mouse_grid_point)) {
			draw_list->AddCircle(m_mouse_grid_point + m_screen_offset, 8, COLOR_ENDPOINT_HOVER, 12, 2);
			m_hovered_wire = wire;
		}
	} 

	// draw area selection box
	if (m_state == CS_AREA_SELECT) {
		ImGuiEx::RectFilled(m_area_sel_a + m_screen_offset, m_mouse_grid_point + m_screen_offset, 
							IM_COL32(0, 0, 128, 128));
	}

	// draw wire under construction
	if (m_state == CS_CREATE_WIRE) {
		m_line_anchors.back() = m_mouse_grid_point;
		Point p0 = m_line_anchors.front() + m_screen_offset;

		for (const auto &anchor : m_line_anchors) {
			Point p1 = anchor + m_screen_offset;
			draw_list->AddLine(p0, p1, IM_COL32(0, 0, 255, 255), 4);
			p0 = p1;
		}
	}

	draw_list->ChannelsMerge();
	ImGui::PopClipRect();
	draw_list->PopClipRect();
}

void CircuitEditor::user_interaction() {

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
			m_wire_start = { m_mouse_grid_point, m_hovered_pin, nullptr };
			m_line_anchors = { m_mouse_grid_point, m_mouse_grid_point };
			m_segment_start = m_mouse_grid_point;
		} else if (m_state == CS_IDLE &&
			m_hovered_pin == PIN_ID_INVALID &&
			m_hovered_wire == nullptr &&
			m_hovered_widget == nullptr) {
			// clicking on the circuit background clears the selection ...
			clear_selection();

			// ... and starts area select mode
			m_state = CS_AREA_SELECT;
			m_area_sel_a = m_mouse_grid_point;
		} else if (m_state == CS_CREATE_WIRE && m_mouse_grid_point != m_wire_start.m_position) {
			// clicking while in CREATE_WIRE mode
			bool sw_create = false;

			if (m_hovered_pin != PIN_ID_INVALID) {
				m_wire_end = { m_mouse_grid_point, m_hovered_pin, nullptr };
				sw_create = true;
			} else if (m_hovered_wire != nullptr) {
				m_wire_end = { m_mouse_grid_point, PIN_ID_INVALID, m_hovered_wire };
				sw_create = true;
			}

			if (sw_create) {
				add_wire();
				m_state = CS_IDLE;
			} else {
				// add anchor when clicking in the empty circuit
				m_segment_start = m_line_anchors.back();
				m_line_anchors.push_back(m_segment_start);
			}
		}
	}

	// -> edit-mode: left mouse button up
	if (!is_simulating() && mouse_in_window && ImGui::IsMouseReleased(0) && !ImGui::GetIO().MouseDownWasDoubleClick[0]) {
		if (m_state == CS_IDLE && m_hovered_wire != nullptr) {
			// wire selection
			m_state = CS_IDLE;
			auto segment = m_hovered_wire->segment_at_point(m_mouse_grid_point);

			if (!ImGui::GetIO().KeyShift) {
				clear_selection();
				select_wire_segment(segment);
			} else {
				toggle_selection(segment);
			}
		} else if (m_state == CS_IDLE && m_hovered_widget != nullptr) {
			// component selection
			if (!ImGui::GetIO().KeyShift) {
				clear_selection();
				select_widget(m_hovered_widget);
			} else {
				toggle_selection(m_hovered_widget);
			}
		} else if (m_state == CS_AREA_SELECT) {
			select_by_area(m_area_sel_a, m_mouse_grid_point);
			m_state = CS_IDLE;
		}
	}

	// -> edit-mode: right mouse button released
	if (!is_simulating() && mouse_in_window && ImGui::IsMouseReleased(1)) {
		if (m_state == CS_IDLE && m_hovered_wire != nullptr) {
			// popup menu to edit the wire segment
			m_wire_start = { m_mouse_grid_point, PIN_ID_INVALID, m_hovered_wire };
			ui_popup_edit_segment_open();
		} else if (m_state == CS_IDLE && m_hovered_widget == nullptr && m_hovered_wire == nullptr) {
			// popup menu to insert a sub circuit
			ui_popup_embed_circuit_open();
		} else if (m_state == CS_CREATE_WIRE) {
			// end CREATE_WIRE state
			m_state = CS_IDLE;
		}
	}

	// -> simulation-mode: right mouse button released
	if (is_simulating() && mouse_in_window && ImGui::IsMouseReleased(1)) {
		if (m_hovered_widget != nullptr && m_hovered_widget->component_model()->type() == COMPONENT_SUB_CIRCUIT) {
			ui_popup_sub_circuit_open();
		}
	}

	// -> edit-mode: double-clicking
	if (!is_simulating() && mouse_in_window && ImGui::IsMouseDoubleClicked(0)) {
		if (m_state == CS_CREATE_WIRE) {
			// end CREATE_WIRE state without actually connecting to something
			m_wire_end = { m_mouse_grid_point, PIN_ID_INVALID, nullptr };
			add_wire();
			m_state = CS_IDLE;
		} else if (m_hovered_wire != nullptr) {
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
			delete_selected_items();
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
	if (mouse_in_window && m_state == CS_IDLE && ImGui::IsMouseDragging(0) &&
	    (m_hovered_widget != nullptr || m_hovered_wire != nullptr)) {
		m_state = CS_DRAGGING;
		prepare_move_selected_items();
	}

	// move selected items while dragging
	if (m_state == CS_DRAGGING) {
		move_selected_items();
	}

	// handle end of dragging
	if (m_state == CS_DRAGGING && !ImGui::IsMouseDragging(0)) {
		reconnect_selected_items();
		m_state = CS_IDLE;
	}

	// snap component to mouse cursor when creating
	if (m_state == CS_CREATE_COMPONENT && selected_widget() != nullptr) {
		reposition_widget(selected_widget(), m_mouse_grid_point);
	}
}

Point CircuitEditor::circuit_dimensions() const {
	Point result = {0, 0};

	for (const auto &ui_comp : m_widgets) {
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

ComponentWidget* CircuitEditor::create_widget_for_component(ModelComponent* component_model) {
	m_widgets.push_back(std::make_unique<ComponentWidget>(component_model));
	auto widget = m_widgets.back().get();
	CircuitEditorFactory::materialize_component(widget);
	return widget;
}

void CircuitEditor::remove_widget(ComponentWidget* widget) {
	remove_owner(m_widgets, widget);
}

void CircuitEditor::reposition_widget(ComponentWidget *ui_comp, Point new_pos) {
	ui_comp->component_model()->set_position({new_pos.x, new_pos.y});
	ui_comp->build_transform();
}

void CircuitEditor::prepare_move_selected_items() {

	m_dragging_last_point = m_mouse_grid_point;

	// move selected line-segments to separate wires so they can be moved freely
	std::set<ModelWire *>	touched_wires;

	for (auto& item : m_selection) {
		if (item.m_segment != nullptr) {
			auto new_wire = m_model_circuit->create_wire();
			auto old_wire = item.m_segment->wire();
			new_wire->add_segment(item.m_segment->junction(0)->position(), item.m_segment->junction(1)->position());

			old_wire->remove_segment(item.m_segment);
			item.m_segment = new_wire->segment_by_index(0);

			touched_wires.insert(old_wire);
		}
	}

	for (auto wire : touched_wires) {
		ui_fix_wire(wire);
	}
}

void CircuitEditor::move_selected_items() {

	if (m_dragging_last_point == m_mouse_grid_point) {
		return;
	}

	// compute how much widgets should be moved, snapping to the grid
	Point delta = m_mouse_grid_point - m_dragging_last_point;
	m_dragging_last_point = m_mouse_grid_point;

	for (auto &item : m_selection) {
		if (item.m_widget != nullptr) {
			auto model = item.m_widget->component_model();
			model->set_position(model->position() + delta);
			item.m_widget->build_transform();
		} else if (item.m_segment != nullptr) {
			item.m_segment->move(delta);
		}
	}
}

void CircuitEditor::reconnect_selected_items() {

	for (auto &item : m_selection) {
		if (item.m_widget != nullptr) {
			ui_fix_widget_connections(item.m_widget);
		} else if (item.m_segment != nullptr) {
			auto wire = ui_try_merge_wire_segment(item.m_segment);
			ui_fix_wire(wire);
		}
	}

	clear_selection();
}

void CircuitEditor::delete_selected_items() {

	std::set<ModelWire *>	touched_wires;
	
	// phase 1: remove components + wire segments
	for (auto &item : m_selection) {
		if (item.m_widget != nullptr) {
			auto model = item.m_widget->component_model();
			m_model_circuit->remove_component(model->id());
			remove_widget(item.m_widget);
		} else if (item.m_segment != nullptr) {
			auto wire = item.m_segment->wire();
			wire->remove_segment(item.m_segment);
			touched_wires.insert(wire);
		}
	}
	
	// phase 2: split broken wires / remove broken connections / make newly-formed connections
	for (auto wire : touched_wires) {
		ui_fix_wire(wire);
	}

	clear_selection();
}

void CircuitEditor::add_wire() {

	// nothing to do if drawn wire ends in the same place it begins
	if (m_wire_start.m_position == m_wire_end.m_position) {
		return;
	}

	if (m_wire_start.m_pin != PIN_ID_INVALID && m_wire_end.m_pin != PIN_ID_INVALID) {
		// a new wire between two pins
		auto wire = m_model_circuit->create_wire();
		wire->add_segments(m_line_anchors.data(), m_line_anchors.size());
		wire->add_pin(m_wire_start.m_pin);
		wire->add_pin(m_wire_end.m_pin);
	} else if (m_wire_end.m_pin == PIN_ID_INVALID && m_wire_end.m_wire == nullptr) {
		// wire without an explicit endpoint
		ModelWire *wire = m_wire_start.m_wire;
		if (wire == nullptr) {
			wire = m_model_circuit->create_wire();
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
			m_model_circuit->remove_wire(m_wire_end.m_wire->id());
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

		if (wire != nullptr) {
			wire->split_at_new_junction(junction);
			wire->add_segments(m_line_anchors.data(), m_line_anchors.size());
			wire->simplify();
			wire->add_pin(pin);
		}
	}
}

void CircuitEditor::ui_create_component(ModelComponent* comp) {
	m_state = CS_CREATE_COMPONENT;
	clear_selection();
	select_widget(create_widget_for_component(comp));
}

void CircuitEditor::ui_embed_circuit(const char* name) {
	auto comp = m_model_circuit->add_sub_circuit(name, 0, 0);
	comp->sync_nested_circuit(m_model_circuit->context());
	comp->set_position(m_mouse_grid_point);
	create_widget_for_component(comp);
}

void CircuitEditor::ui_fix_widget_connections(ComponentWidget* widget) {
	assert(widget);
	auto comp = widget->component_model();

	// disconnect the component
	m_model_circuit->disconnect_component(comp->id());

	// iterate over the endpoints of the component and check if it should be connected to a wire
	for (const auto& c_pair : widget->endpoints()) {
		auto pin_id = c_pair.first;
		auto position = widget->to_circuit().apply(c_pair.second);

		for (auto& w_pair : m_model_circuit->wires()) {
			auto wire = w_pair.second.get();
			if (wire->point_on_wire(position)) {
				wire->split_at_new_junction(position);
				wire->add_pin(pin_id);
			}
		}
	}
}

void CircuitEditor::ui_fix_wire(ModelWire* wire) {
	wire->clear_pins();

	if (!wire->in_one_piece()) {
		while (wire->num_segments() > 0) {
			auto reachable_segments = wire->reachable_segments(wire->segment_by_index(0));
			ModelWire* new_wire = m_model_circuit->create_wire();

			for (auto segment : reachable_segments) {
				new_wire->add_segment(segment->junction(0)->position(), segment->junction(1)->position());
				wire->remove_segment(segment);
			}

			new_wire->simplify();
			wire_make_connections(new_wire);
		}

		m_model_circuit->remove_wire(wire->id());
	} else {
		wire->simplify();
		wire_make_connections(wire);
	}
}

ModelWire* CircuitEditor::ui_try_merge_wire_segment(ModelWireSegment* segment) {

	ModelWire* target_wires[2] = { nullptr, nullptr };
	Point end_points[2] = { segment->junction(0)->position(), segment->junction(1)->position() };

	for (auto& iter : m_model_circuit->wires()) {
		auto wire = iter.second.get();
		if (wire->id() >= segment->wire()->id()) {
			continue;
		}
			
		if (target_wires[0] == nullptr && wire->point_on_wire(end_points[0])) {
			wire->split_at_new_junction(end_points[0]);
			target_wires[0] = wire;
		} else if (target_wires[1] == nullptr && wire->point_on_wire(end_points[1])) {
			wire->split_at_new_junction(end_points[1]);
			target_wires[1] = wire;
		}

		if (target_wires[0] != nullptr && target_wires[1] != nullptr) {
			break;
		}
	}

	auto wire_to_merge = segment->wire();
	bool merged = false;
	for (int w = 1; w >= 0; --w) {
		if (target_wires[w] != nullptr) {
			target_wires[w]->merge(wire_to_merge);
			m_model_circuit->remove_wire(wire_to_merge->id());
			wire_to_merge = target_wires[w];
			merged = true;
		}
	}

	return (merged) ? wire_to_merge : segment->wire();
}

void CircuitEditor::clear_selection() {
	m_selection.clear();
}

void CircuitEditor::select_widget(ComponentWidget *widget) {
	if (is_selected(widget)) {
		return;
	}

	m_selection.push_back({widget, nullptr});
}

void CircuitEditor::deselect_widget(ComponentWidget *widget) {
	remove_if(m_selection, [widget](const auto &s) {return s.m_widget == widget;});
}

void CircuitEditor::select_wire_segment(ModelWireSegment *segment) {
	if (is_selected(segment)) {
		return;
	}

	m_selection.push_back({nullptr, segment});
}

void CircuitEditor::deselect_wire_segment(ModelWireSegment *segment) {
	remove_if(m_selection, [segment](const auto &s) {return s.m_segment == segment;});
}

void CircuitEditor::select_by_area(Point p0, Point p1) {
	auto area_min = p0;
	auto area_max = p1;
	if (area_min.x > area_max.x) {
		std::swap(area_min.x, area_max.x);
	}
	if (area_min.y > area_max.y) {
		std::swap(area_min.y, area_max.y);
	}

	clear_selection();
	for (const auto &widget : m_widgets) {
		if (widget->aabb_min().x >= area_min.x && widget->aabb_max().x <= area_max.x &&
		    widget->aabb_min().y >= area_min.y && widget->aabb_max().y <= area_max.y) {
			select_widget(widget.get());
		}
	}

	for (const auto &iter : m_model_circuit->wires()) {
		auto wire = iter.second.get();

		for (int s = 0; s < wire->num_segments(); ++s) {
			bool inside = true;

			for (int j = 0; j < 2; ++j) {
				auto p = wire->segment_point(s, j);
				if (p.x < area_min.x || p.x > area_max.x ||
					p.y < area_min.y || p.y > area_max.y) {
					inside = false;
					break;
				}
			}

			if (inside) {
				select_wire_segment(wire->segment_by_index(s));
			}
		}
	}
}

void CircuitEditor::toggle_selection(ComponentWidget *widget) {
	if (is_selected(widget)) {
		deselect_widget(widget);
	} else {
		select_widget(widget);
	}
}

void CircuitEditor::toggle_selection(ModelWireSegment *segment) {
	if (is_selected(segment)) {
		deselect_wire_segment(segment);
	} else {
		select_wire_segment(segment);
	}
}

bool CircuitEditor::is_selected(ComponentWidget *widget) {
	return std::any_of(m_selection.begin(), m_selection.end(),
						[widget](const auto &s) {return s.m_widget == widget;});
}

bool CircuitEditor::is_selected(ModelWireSegment *segment) {
	return std::any_of(m_selection.begin(), m_selection.end(),
						[segment](const auto &s) {return s.m_segment == segment;});
}

ComponentWidget *CircuitEditor::selected_widget() const {
	if (m_selection.size() == 1) {
		return m_selection.front().m_widget;
	}
	return nullptr;
}

void CircuitEditor::set_simulation_instance(SimCircuit *sim_circuit, bool view_only) {

	if (sim_circuit != nullptr && m_sim_circuit == nullptr) {
		m_state = CS_SIMULATING;
		m_view_only = view_only;
		clear_selection();
	}

	if (sim_circuit == nullptr && m_sim_circuit != nullptr) {
		m_state = CS_IDLE;
		m_view_only = false;
	}

	m_sim_circuit = sim_circuit;
}

bool CircuitEditor::is_simulating() const {
	return m_state == CS_SIMULATING;
}

void CircuitEditor::copy_selected_components() {
	m_copy_components.clear();
	m_copy_center = {0, 0};

	for (auto &item : m_selection) {
		if (item.m_widget != nullptr) {
			auto comp = item.m_widget->component_model();
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

void CircuitEditor::paste_components() {
	clear_selection();

	for (const auto &comp : m_copy_components) {
		auto pasted = m_model_circuit->paste_component(comp.get());
		pasted->set_position(m_mouse_grid_point + comp->position() - m_copy_center);
		auto ui_comp = create_widget_for_component(pasted);
		select_widget(ui_comp);
	}
}

void CircuitEditor::wire_make_connections(ModelWire *wire) {
	for (size_t j = 0; j < wire->num_junctions(); ++j) {
		auto junction = wire->junction_position(j);	
		auto found = m_point_pin_lut.find(junction);
		if (found != m_point_pin_lut.end()) {
			wire->add_pin(found->second);
		}
	}
}

void CircuitEditor::draw_grid(ImDrawList *draw_list) {
	if (!m_show_grid) {
		return;
	}

	Point win_pos = ImGui::GetCursorScreenPos();
	Point win_size = ImGui::GetWindowSize();

	for (float x = fmodf(m_scroll_delta.x, GRID_SIZE); x < win_size.x; x += GRID_SIZE) {		// NOLINT
		draw_list->AddLine(Point(x, 0.0f) + win_pos, Point(x, win_size.y) + win_pos, COLOR_GRID_LINE);
	}

	for (float y = fmodf(m_scroll_delta.y, GRID_SIZE); y < win_size.y; y += GRID_SIZE) {		// NOLINT
		draw_list->AddLine(Point(0.0f, y) + win_pos, Point(win_size.x, y) + win_pos, COLOR_GRID_LINE);
	}
}

void CircuitEditor::ui_popup_embed_circuit() {
	if (ImGui::BeginPopup(POPUP_EMBED_CIRCUIT)) {
		auto lib = m_model_circuit->context()->user_library();
		auto max = lib->num_circuits();

		ImGui::Text("Choose circuit to embed:");

		for (size_t i = 0; i < max; ++i) {
			auto sub = lib->circuit_by_idx(i);
			if (ImGui::Selectable(sub->name().c_str())) {
				ui_embed_circuit(sub->name().c_str());
			}
		}
	
		if (max <= 0) {
			ImGui::Text("No circuits available to embed.");
		}

		ImGui::EndPopup();
	}
}

void CircuitEditor::ui_popup_embed_circuit_open() {
	ImGui::OpenPopup(POPUP_EMBED_CIRCUIT);
}

void CircuitEditor::ui_popup_sub_circuit(UIContext *ui_context) {
	if (ImGui::BeginPopup(POPUP_SUB_CIRCUIT)) {
		if (ImGui::Selectable("Drill down ...")) {
			ui_context->create_sub_circuit_view(m_sim_circuit, m_popup_component->component_model());
		}
		ImGui::EndPopup();
	} 
}

void CircuitEditor::ui_popup_sub_circuit_open() {
	m_popup_component = m_hovered_widget;
	ImGui::OpenPopup(POPUP_SUB_CIRCUIT);
}

void CircuitEditor::ui_popup_edit_segment() {
	if (ImGui::BeginPopup(POPUP_EDIT_SEGMENT)) {
		if (ImGui::Selectable("Attach wire")) {
			m_state = CS_CREATE_WIRE;
			m_line_anchors = { m_wire_start.m_position, m_wire_start.m_position };
			m_segment_start = m_wire_start.m_position;
		}
		ImGui::EndPopup();
	}
}

void CircuitEditor::ui_popup_edit_segment_open() {
	ImGui::OpenPopup(POPUP_EDIT_SEGMENT);
}

///////////////////////////////////////////////////////////////////////////////
//
// CircuitEditorFactory
//

CircuitEditorFactory::materialize_func_map_t CircuitEditorFactory::m_materialize_funcs;

void CircuitEditorFactory::register_materialize_func(ComponentType type, materialize_func_t func) {
	m_materialize_funcs[type] = move(func);
}

CircuitEditor::uptr_t CircuitEditorFactory::create_circuit(ModelCircuit *circuit) {
	auto editor = std::make_unique<CircuitEditor>(circuit);

	auto comp_ids = circuit->component_ids();
    for (const auto &comp_id : comp_ids) {
		auto comp = circuit->component_by_id(comp_id);
		editor->create_widget_for_component(comp);
	}

    return std::move(editor);
}

void CircuitEditorFactory::materialize_component(ComponentWidget *ui_component) {
	auto mat_func = m_materialize_funcs.find(ui_component->component_model()->type());
	if (mat_func != m_materialize_funcs.end()) {
		mat_func->second(ui_component->component_model(), ui_component);
	}
}

void CircuitEditorFactory::rematerialize_component(CircuitEditor *circuit, ComponentWidget *ui_component) {
	assert(circuit);
	assert(ui_component);

	ui_component->dematerialize();
	materialize_component(ui_component);
}

} // namespace lsim::gui

} // namespace lsim
