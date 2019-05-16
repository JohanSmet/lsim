// component_ui.h - Johan Smet - BSD-3-Clause (see LICENSE)

#ifndef LSIM_GUI_COMPONENT_UI_H
#define LSIM_GUI_COMPONENT_UI_H

#include "visual_component.h"
#include "algebra.h"

#include <unordered_map>
#include <functional>
#include <array>
#include <vector>

class ImDrawList;
class ComponentIcon;
class UIComponent;

typedef uint32_t ComponentType;

typedef std::function<void (const UIComponent *, Transform)> ui_component_func_t;

enum UICircuitState {
    CS_IDLE = 0,
    CS_DRAGGING,
    CS_CREATE_COMPONENT,
    CS_CREATE_WIRE
};

class UIComponent {
public:
    UIComponent(VisualComponent *vis_comp);

    VisualComponent *visual_comp() const {return m_visual_comp;}
    bool has_tooltip() const {return !m_tooltip.empty();}
    const char *tooltip() const {return m_tooltip.c_str();}
    const ComponentIcon *icon() const {return m_icon;}
    const Transform &to_circuit() const {return m_to_circuit;}
    const Point &aabb_min() const {return m_aabb_min;}
    const Point &aabb_max() const {return m_aabb_max;}
    Point aabb_size() const {return m_aabb_max - m_aabb_min;}

    void change_tooltip(const char *tooltip);
    void change_icon(const ComponentIcon *icon);
    void change_size(float width, float height);
    void build_transform();

    void set_custom_ui_callback(ui_component_func_t func);
    bool has_custom_ui_callback() const {return m_custom_ui_callback != nullptr;}
    void call_custom_ui_callback(Transform transform);

private:
    void recompute_aabb();

private:
    VisualComponent *   m_visual_comp;
    std::string         m_tooltip;
    Transform           m_to_circuit;
    Point               m_half_size;
    Point               m_aabb_min;
    Point               m_aabb_max;
    const ComponentIcon *m_icon;
    ui_component_func_t m_custom_ui_callback;
};

struct UIConnection {
    uint32_t m_node;
    uint32_t m_pin_a;
    uint32_t m_pin_b;
};

class UICircuit {
public:
    typedef std::unique_ptr<UICircuit>  uptr_t;

public:
    UICircuit(class Circuit *circuit);

    void add_component(const UIComponent &comp);
    void add_endpoint(uint32_t pin, Point location);
    void add_pin_line(Transform to_circuit, const uint32_t *pins, size_t pin_count, float size, Point origin, Point inc);
    void add_pin_line(Transform to_circuit, const uint32_t *pins, size_t pin_count, Point origin, Point delta);
    void add_connection(uint32_t node, uint32_t pin_1, uint32_t pin_2);

    void draw();

    void move_component(UIComponent *comp, Point delta);
    void move_component_abs(UIComponent *comp, Point pos);
    void create_component(VisualComponent *vis_comp);

    class Circuit *circuit() {return m_circuit;}

    UIComponent *selected_component() const {return m_selected_comp;}

private:
    Point endpoint_position(uint32_t pin);
    void draw_grid(ImDrawList *draw_list);

private:
    typedef std::unordered_map<uint32_t, Point> endpoint_map_t;
    typedef std::vector<UIComponent>            ui_component_container_t;
    typedef std::vector<UIConnection>           ui_connection_container_t;
    typedef std::vector<Point>                  point_container_t;

private:
    class Circuit *          m_circuit;
    std::string              m_name;
    endpoint_map_t           m_endpoints;
    ui_component_container_t  m_ui_components;
    ui_connection_container_t m_ui_connections;

    UICircuitState           m_state;
    UIComponent *            m_selected_comp;
    pin_t                    m_selected_pin;

    Point               m_drag_delta;

    pin_t               m_line_origin;
    point_container_t   m_line_anchors;
    Point               m_segment_start;

    Point   m_scroll_delta;
    bool    m_show_grid;
};


class UICircuitBuilder {
public:
    typedef std::function<void(Component *, UIComponent *, UICircuit *)> materialize_func_t;
public:
    static void register_materialize_func(ComponentType type, materialize_func_t func);
    static UICircuit::uptr_t create_circuit(Circuit *circuit);
    static void materialize_component(UICircuit *circuit, VisualComponent *visual_comp);

private:
    typedef std::unordered_map<ComponentType, materialize_func_t> materialize_func_map_t;

private:
    static materialize_func_map_t m_materialize_funcs;
    
};

class ComponentIcon {
public:
    ComponentIcon(const char *data, size_t len);
    void draw(Transform transform, Point draw_size, ImDrawList *draw_list, size_t line_width, uint32_t color) const;
    void draw(Point origin, Point draw_size, ImDrawList *draw_list, size_t line_width, uint32_t color) const;
    static ComponentIcon *cache(uint32_t id, const char *data, size_t len);
    static ComponentIcon *cached(uint32_t id);
private:
    typedef std::array<Point, 4> bezier_t;
    typedef std::vector<bezier_t> bezier_container_t;
    typedef std::unordered_map<uint32_t, std::unique_ptr<ComponentIcon> > icon_lut_t;
private:
    bezier_container_t  m_curves;
    Point               m_size;

    static icon_lut_t   m_icon_cache;
};


#endif // LSIM_GUI_COMPONENT_UI_H