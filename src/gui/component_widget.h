// component_widget.h - Johan Smet - BSD-3-Clause (see LICENSE)

#ifndef LSIM_GUI_COMPONENT_WIDGET_H
#define LSIM_GUI_COMPONENT_WIDGET_H

#include <functional>
#include "std_helper.h"

#include "algebra.h"
#include "sim_types.h"

namespace lsim {

namespace gui {

class ComponentIcon;
class UICircuit;

class ComponentWidget {
public:
	typedef std::unordered_map<pin_id_t, Point> endpoint_map_t;
	typedef std::unique_ptr<ComponentWidget> uptr_t;
	using draw_func_t = std::function<void(UICircuit *, const ComponentWidget *, Transform)>;

public:
	ComponentWidget(ModelComponent* comp);

	// access properties
	ModelComponent* component_model() const { return m_component; }
	bool has_border() const { return m_border; }
	bool has_tooltip() const { return !m_tooltip.empty(); }
	const char* tooltip() const { return m_tooltip.c_str(); }
	const ComponentIcon* icon() const { return m_icon; }
	const Transform& to_circuit() const { return m_to_circuit; }
	const Point& aabb_min() const { return m_aabb_min; }
	const Point& aabb_max() const { return m_aabb_max; }
	Point aabb_size() const { return m_aabb_max - m_aabb_min; }

	// change properties
	void show_border(bool show) { m_border = show; }
	void change_tooltip(const char* tooltip);
	void change_icon(const ComponentIcon* icon);
	void change_size(float width, float height);
	void build_transform();

	// custom draw function
	void set_draw_callback(draw_func_t func);
	bool has_draw_callback() const { return m_draw_callback != nullptr; }
	void run_draw_callback(UICircuit* circuit, Transform transform);

	// endpoints
	void add_endpoint(pin_id_t pin, Point location);
	void add_pin_line(pin_id_t pin_start, size_t pin_count, float size, Point origin, Point inc);
	void add_pin_line(pin_id_t pin_start, size_t pin_count, Point origin, Point delta);
	const endpoint_map_t& endpoints() const { return m_endpoints; }

	void dematerialize();

private:
	void recompute_aabb();

private:
	ModelComponent*		m_component;
	bool                m_border;
	string         		m_tooltip;
	Transform           m_to_circuit;
	Point               m_half_size;
	Point               m_aabb_min;
	Point               m_aabb_max;
	const ComponentIcon* m_icon;
	draw_func_t			m_draw_callback;

	endpoint_map_t      m_endpoints;
};

} // namespace lsim::gui

} // namespace lsim

#endif // LSIM_GUI_COMPONENT_WIDGET_H
