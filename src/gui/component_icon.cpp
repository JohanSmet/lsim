// component_icon.h - Johan Smet - BSD-3-Clause (see LICENSE)

#include "component_icon.h"

#include "imgui_ex.h"
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

namespace lsim {

namespace gui {

ComponentIcon::icon_lut_t ComponentIcon::m_icon_cache;

ComponentIcon::ComponentIcon(const char* data, size_t len) {
	std::vector<char> dummy(data, data + len);
	auto img = nsvgParse(dummy.data(), "px", 96);

	Point offset(img->width / 2.0f, img->height / 2.0f);
	m_size = { img->width, img->height };

	for (auto shape = img->shapes; shape != nullptr; shape = shape->next) {
		for (auto path = shape->paths; path != nullptr; path = path->next) {
			for (int i = 0; i < path->npts - 1; i += 3) {
				float* p = &path->pts[i * 2];
				m_curves.push_back({
					Point(p[0], p[1]) - offset, Point(p[2], p[3]) - offset,
					Point(p[4], p[5]) - offset, Point(p[6], p[7]) - offset
					});
			}
		}
	}
}

void ComponentIcon::draw(Transform transform, Point draw_size, ImDrawList* draw_list, size_t line_width, uint32_t color) const {

	Point scale_xy = draw_size / m_size;
	float scale = std::min(scale_xy.x, scale_xy.y);

	for (const auto& curve : m_curves) {
		draw_list->AddBezierCurve(
			transform.apply(curve[0] * scale),
			transform.apply(curve[1] * scale),
			transform.apply(curve[2] * scale),
			transform.apply(curve[3] * scale),
			color,
			static_cast<float>(line_width));
	}
}

void ComponentIcon::draw(Point origin, Point draw_size, ImDrawList* draw_list, size_t line_width, uint32_t color) const {
	Transform transform;
	transform.translate(origin);
	draw(transform, draw_size, draw_list, line_width, color);
}

ComponentIcon* ComponentIcon::cache(uint32_t id, const char* data, size_t len) {
	m_icon_cache[id] = std::make_unique<ComponentIcon>(data, len);
	return cached(id);
}

ComponentIcon* ComponentIcon::cached(uint32_t id) {
	auto result = m_icon_cache.find(id);
	if (result == m_icon_cache.end()) {
		return nullptr;
	}
	else {
		return result->second.get();
	}
}

} // namespace lsim

} // namespace gui
