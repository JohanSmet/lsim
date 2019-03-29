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


typedef std::function<void (const UIComponent *)> ui_component_func_t;

struct UIComponent {
    VisualComponent *   m_visual_comp;
    std::string         m_tooltip;
    Transform           m_to_circuit;
    Point               m_circuit_min;
    Point               m_circuit_max;
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
    void add_pin_line(Transform to_circuit, uint32_t *pins, size_t pin_count, float size, Point origin, Point inc);
    void add_connection(uint32_t node, uint32_t pin_1, uint32_t pin_2);

    void draw();

private:
    Point endpoint_position(uint32_t pin);

private:
    typedef std::unordered_map<uint32_t, Point> endpoint_map_t;
    typedef std::vector<UIComponent>            ui_component_container_t;
    typedef std::vector<UIConnection>           ui_connection_container_t;

private:
    class Circuit *          m_circuit;
    std::string              m_name;
    endpoint_map_t           m_endpoints;
    ui_component_container_t  m_ui_components;
    ui_connection_container_t m_ui_connections;
};


class UICircuitBuilder {
public:
    typedef std::function<void(Component *, UIComponent *, UICircuit *)> materialize_func_t;
public:
    static void register_materialize_func(VisualComponent::Type type, materialize_func_t func);

    template <class CompIt>
    static UICircuit::uptr_t create_circuit(Circuit *circuit, CompIt comp_begin, CompIt comp_end) {
        auto ui_circuit = std::make_unique<UICircuit>(circuit);

        for (auto iter = comp_begin; iter != comp_end; ++iter) {
            const auto &visual_comp = *iter;
            materialize_component(ui_circuit.get(), visual_comp.get());
        }

        return std::move(ui_circuit);
    }

private:
    static void materialize_component(UICircuit *circuit, VisualComponent *visual_comp);

private:
    typedef std::unordered_map<VisualComponent::Type, materialize_func_t> materialize_func_map_t;

private:
    static materialize_func_map_t m_materialize_funcs;
    
};

class ComponentIcon {
public:
    ComponentIcon(const char *data, size_t len);
    void draw(Transform transform, ImDrawList *draw_list, size_t line_width, uint32_t color) const;
private:
    typedef std::array<Point, 4> bezier_t;
    typedef std::vector<bezier_t> bezier_container_t;
private:
    bezier_container_t  m_curves;


};


#endif // LSIM_GUI_COMPONENT_UI_H