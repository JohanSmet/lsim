// component_ui.h - Johan Smet - BSD-3-Clause (see LICENSE)

#ifndef LSIM_GUI_COMPONENT_UI_H
#define LSIM_GUI_COMPONENT_UI_H

#include "component_widget.h"
#include "model_circuit.h"
#include "algebra.h"

#include <unordered_map>
#include <functional>
#include <array>
#include <vector>

struct ImDrawList;

namespace lsim {

class SimCircuit;

namespace gui {

class UICircuit;


enum UICircuitState {
    CS_IDLE = 0,
    CS_DRAGGING,
    CS_AREA_SELECT,
    CS_CREATE_COMPONENT,
    CS_CREATE_WIRE,
    CS_SIMULATING
};


class UICircuit {
public:
    typedef std::unique_ptr<UICircuit>  uptr_t;

public:
    UICircuit(ModelCircuit *circuit_desc);

    ComponentWidget *create_component(ModelComponent *component_model);
    void remove_component(ComponentWidget *ui_comp);

    void draw();
    Point circuit_dimensions() const;

    void move_selected_components();
    void delete_selected_components();
    void move_component_abs(ComponentWidget *comp, Point pos);
    void ui_create_component(ModelComponent *component_model);
    void embed_circuit(const char *name);
    void create_wire();

    ModelCircuit *circuit_desc() const {return m_circuit_desc;}
    SimCircuit *circuit_inst() const {return m_circuit_inst;}

    // selection
    size_t num_selected_items() const {return m_selection.size();}
    void clear_selection();
    void select_component(ComponentWidget *widget);
    void deselect_component(ComponentWidget *widget);
    void select_wire_segment(ModelWireSegment *segment);
    void deselect_wire_segment(ModelWireSegment *segment);
    void select_by_area(Point p0, Point p1);
    void toggle_selection(ComponentWidget *widget);
    void toggle_selection(ModelWireSegment *segment);
    bool is_selected(ComponentWidget *widget);
    bool is_selected(ModelWireSegment *segment);
    ComponentWidget *selected_component() const;

    // simulation
    void set_simulation_instance(SimCircuit *circuit_inst, bool view_only = false);
    bool is_simulating() const;
    bool is_view_only_simulation() const {return m_view_only;};

    // connections
    void fix_component_connections(ComponentWidget *ui_comp);

    // copy & paste
    void copy_selected_components();
    void paste_components();

private:
    void wire_make_connections(ModelWire *wire);
    void draw_grid(ImDrawList *draw_list);
    void ui_popup_embed_circuit();
    void ui_popup_embed_circuit_open();
    void ui_popup_sub_circuit();
    void ui_popup_sub_circuit_open();

private:
    struct PointHash {
        size_t operator() (const Point &p) const;
    };

    typedef std::vector<ModelComponent::uptr_t>      component_container_t;
    typedef std::vector<ComponentWidget::uptr_t>    ui_component_container_t;
    typedef std::vector<Point>                  point_container_t;
    typedef std::unordered_map<Point, pin_id_t, PointHash>  point_pin_lut_t;

    struct WireEndPoint {
        Point    m_position;
        pin_id_t m_pin;
        ModelWire *   m_wire;
    };

    struct SelectedItem {
        ComponentWidget *m_component;
        ModelWireSegment *m_segment;
    };
    typedef std::vector<SelectedItem>   selection_container_t;

private:
    ModelCircuit *      m_circuit_desc;
    std::string               m_name;
 
    ui_component_container_t  m_ui_components;
    point_pin_lut_t           m_point_pin_lut;
    selection_container_t     m_selection;

    component_container_t     m_copy_components;
    Point                     m_copy_center;

    Point                     m_area_sel_a;

    UICircuitState      m_state;

    ComponentWidget *       m_hovered_component;
    pin_id_t            m_hovered_pin;
    ModelWire *              m_hovered_wire;

    Point               m_mouse_grid_point;
    Point               m_drag_last;

    WireEndPoint        m_wire_start;
    WireEndPoint        m_wire_end;
    Point               m_segment_start;
    point_container_t   m_line_anchors;

    SimCircuit *   m_circuit_inst;
    bool                m_view_only;
    ComponentWidget *       m_popup_component;

    Point   m_scroll_delta;
    bool    m_show_grid;
};


class UICircuitBuilder {
public:
    typedef std::function<void(ModelComponent *, ComponentWidget *)> materialize_func_t;
public:
    static void register_materialize_func(ComponentType type, materialize_func_t func);
    static UICircuit::uptr_t create_circuit(ModelCircuit *circuit);
    static void materialize_component(ComponentWidget *ui_component);
    static void rematerialize_component(UICircuit *circuit, ComponentWidget *ui_component);

private:
    typedef std::unordered_map<ComponentType, materialize_func_t> materialize_func_map_t;

private:
    static materialize_func_map_t m_materialize_funcs;
    
};

} // namespace gui

} // namespace lsim

#endif // LSIM_GUI_COMPONENT_UI_H