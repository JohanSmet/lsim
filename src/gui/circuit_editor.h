// component_ui.h - Johan Smet - BSD-3-Clause (see LICENSE)

#ifndef LSIM_GUI_COMPONENT_UI_H
#define LSIM_GUI_COMPONENT_UI_H

#include "algebra.h"
#include "sim_types.h"
#include "component_widget.h"

#include "std_helper.h"
#include <functional>

struct ImDrawList;

namespace lsim {

class ModelCircuit;
class ModelWireSegment;
class ModelWire;
class SimCircuit;

namespace gui {

class UIContext;

enum CircuitEditorState {
    CS_IDLE = 0,
    CS_DRAGGING,
    CS_AREA_SELECT,
    CS_CREATE_COMPONENT,
    CS_CREATE_WIRE,
    CS_SIMULATING
};

class CircuitEditor {
public:
    typedef unique_ptr<CircuitEditor>  uptr_t;

public:
    CircuitEditor(ModelCircuit *model_circuit);
	
	// display frame and interaction
	void refresh(UIContext *ui_context);
private:
	void init_ui_refresh();
    void draw_ui(UIContext *ui_context);
	void user_interaction();

public:	
	// properties
    Point circuit_dimensions() const;
    ModelCircuit *model_circuit() const {return m_model_circuit;}
    SimCircuit *sim_circuit() const {return m_sim_circuit;}

	// widget management
    ComponentWidget *create_widget_for_component(ModelComponent *component_model);
    void remove_widget(ComponentWidget *widget);
    void reposition_widget(ComponentWidget *comp, Point pos);

	// circuit modification
	void prepare_move_selected_items();
    void move_selected_items();
	void reconnect_selected_items();
    void delete_selected_items();
    void add_wire();

    void ui_create_component(ModelComponent *component_model);
    void ui_embed_circuit(const char *name);
    void ui_fix_widget_connections(ComponentWidget *widget);
	void ui_fix_wire(ModelWire * wire);
	ModelWire* ui_try_merge_wire_segment(ModelWireSegment* segment);


    // selection
    size_t num_selected_items() const {return m_selection.size();}
    void clear_selection();
    void select_widget(ComponentWidget *widget);
    void deselect_widget(ComponentWidget *widget);
    void select_wire_segment(ModelWireSegment *segment);
    void deselect_wire_segment(ModelWireSegment *segment);
    void select_by_area(Point p0, Point p1);
    void toggle_selection(ComponentWidget *widget);
    void toggle_selection(ModelWireSegment *segment);
    bool is_selected(ComponentWidget *widget);
    bool is_selected(ModelWireSegment *segment);
    ComponentWidget *selected_widget() const;

    // simulation
    void set_simulation_instance(SimCircuit *sim_circuit, bool view_only = false);
    bool is_simulating() const;
    bool is_view_only_simulation() const {return m_view_only;};

    // copy & paste
    void copy_selected_components();
    void paste_components();

private:
    void wire_make_connections(ModelWire *wire);
    void draw_grid(ImDrawList *draw_list);
    void ui_popup_embed_circuit();
    void ui_popup_embed_circuit_open();
    void ui_popup_sub_circuit(UIContext *ui_context);
    void ui_popup_sub_circuit_open();
	void ui_popup_edit_segment();
	void ui_popup_edit_segment_open();

private:
    struct PointHash {
        size_t operator() (const Point &p) const;
    };

    typedef std::vector<unique_ptr<ModelComponent>>		component_container_t;
    typedef std::vector<unique_ptr<ComponentWidget>>    widget_container_t;
    typedef std::vector<Point>							point_container_t;
    typedef std::unordered_map<Point, pin_id_t, PointHash>  point_pin_lut_t;

    struct WireEndPoint {
        Point    m_position;
        pin_id_t m_pin;
        ModelWire *   m_wire;
    };

    struct SelectedItem {
        ComponentWidget *m_widget;
        ModelWireSegment *m_segment;
    };
    typedef std::vector<SelectedItem>   selection_container_t;

private:
    ModelCircuit *				m_model_circuit;		// the circuit being shown/edited
	
	// circuit elements
    widget_container_t			m_widgets;				// widgets for the components
    point_pin_lut_t				m_point_pin_lut;		// Point -> pin lookup

	// selection
    selection_container_t		m_selection;			// selected items
    Point						m_area_sel_a;			// start point of area selection
	
	// copy & paste (fixme: move to higher level to allow copy&paste between circuits)
    component_container_t		m_copy_components;
    Point						m_copy_center;

	// editor state
    CircuitEditorState			m_state;				// current state of the editor state machine
    ComponentWidget *			m_hovered_widget;		// currently hovered widget
    pin_id_t					m_hovered_pin;			// currently hovered pin
    ModelWire *					m_hovered_wire;			// currently hovered wire
    Point						m_scroll_delta;			// distance the origin of circuit has been scrolled from origin of editor window
	Point						m_screen_offset;		// translation from circuit space to screen space

    Point						m_mouse_grid_point;		// grid point nearest to the mouse cursor
    Point						m_dragging_last_point;	// last point items were moved to while dragging

	// wire creation
    WireEndPoint				m_wire_start;			// starting anchor point of the wire
    WireEndPoint				m_wire_end;				// ending anchor point of the wire
    Point						m_segment_start;		// start point of current segment
    point_container_t			m_line_anchors;			// all anchor points of the line being created

	// simulation specific variables
    SimCircuit *				m_sim_circuit;			// simulation circuit (nullptr == not simulating atm)
    bool						m_view_only;			// view only simulation (user cannot change connector values)
    ComponentWidget *			m_popup_component;		// drill-down target component

	// drawing properties
    bool						m_show_grid;			// draw grid?
};


class CircuitEditorFactory {
public:
    typedef std::function<void(ModelComponent *, ComponentWidget *)> materialize_func_t;
public:
    static void register_materialize_func(ComponentType type, materialize_func_t func);
    static CircuitEditor::uptr_t create_circuit(ModelCircuit *circuit);
    static void materialize_component(ComponentWidget *ui_component);
    static void rematerialize_component(CircuitEditor *circuit, ComponentWidget *ui_component);

private:
    typedef std::unordered_map<ComponentType, materialize_func_t> materialize_func_map_t;

private:
    static materialize_func_map_t m_materialize_funcs;
    
};

} // namespace lsim::gui

} // namespace lsim

#endif // LSIM_GUI_COMPONENT_UI_H