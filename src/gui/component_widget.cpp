// component_widget.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "component_widget.h"

#include "configuration.h"
#include "model_component.h"

namespace lsim {

namespace gui {

ComponentWidget::ComponentWidget(ModelComponent* component_model) :
	m_component(component_model),
	m_border(true),
	m_tooltip(""),
	m_half_size(0.0f, 0.0f),
	m_icon(nullptr),
	m_draw_callback(nullptr) {
	build_transform();
}

void ComponentWidget::change_tooltip(const char* tooltip) {
	m_tooltip = tooltip;
}

void ComponentWidget::change_icon(const ComponentIcon* icon) {
	m_icon = icon;
}

void ComponentWidget::build_transform() {
	m_to_circuit.reset();
	m_to_circuit.rotate(static_cast<float>(m_component->angle()));
	m_to_circuit.translate(m_component->position());
	recompute_aabb();
}

void ComponentWidget::change_size(float width, float height) {
	m_half_size = { width / 2.0f, height / 2.0f };
	recompute_aabb();
}

void ComponentWidget::recompute_aabb() {
	m_aabb_min = m_to_circuit.apply({ -m_half_size.x, -m_half_size.y });
	m_aabb_max = m_to_circuit.apply({ m_half_size.x, m_half_size.y });
	if (m_aabb_max.x < m_aabb_min.x) {
		std::swap(m_aabb_min.x, m_aabb_max.x);
	}

	if (m_aabb_max.y < m_aabb_min.y) {
		std::swap(m_aabb_min.y, m_aabb_max.y);
	}
}

void ComponentWidget::set_draw_callback(ComponentWidget::draw_func_t func) {
	m_draw_callback = func;
}

void ComponentWidget::run_draw_callback(CircuitEditor* circuit, Transform transform) {
	if (m_draw_callback) {
		m_draw_callback(circuit, this, transform);
	}
}

void ComponentWidget::add_endpoint(pin_id_t pin, Point location) {
	m_endpoints[pin] = location;
}

void ComponentWidget::add_pin_line(pin_id_t pin_start, size_t pin_count, float size, Point origin, Point inc) {
	if (pin_count == 0) {
		return;
	}

	auto odd = pin_count % 2;
	float half = (pin_count - odd) * 0.5f;
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

void ComponentWidget::add_pin_line(pin_id_t pin_start, size_t pin_count, Point origin, Point delta) {
	if (pin_count == 0) {
		return;
	}

	auto pos = origin;
	for (size_t i = 0; i < pin_count; ++i) {
		add_endpoint(pin_start + i, pos);
		pos = pos + delta;
	}
}

void ComponentWidget::dematerialize() {
	m_endpoints.clear();
}

} // namespace lsim::gui

} // namespace lsim
