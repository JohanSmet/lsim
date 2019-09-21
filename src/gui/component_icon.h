// component_icon.h - Johan Smet - BSD-3-Clause (see LICENSE)

#ifndef LSIM_GUI_COMPONENT_ICON_H
#define LSIM_GUI_COMPONENT_ICON_H

#include "algebra.h"
#include <unordered_map>
#include <vector>

struct ImDrawList;

namespace lsim {

namespace gui {

class ComponentIcon {
public:
	ComponentIcon(const char* data, size_t len);
	void draw(Transform transform, Point draw_size, ImDrawList* draw_list, size_t line_width, uint32_t color) const;
	void draw(Point origin, Point draw_size, ImDrawList* draw_list, size_t line_width, uint32_t color) const;
	static ComponentIcon* cache(uint32_t id, const char* data, size_t len);
	static ComponentIcon* cached(uint32_t id);
private:
	typedef std::array<Point, 4> bezier_t;
	typedef std::vector<bezier_t> bezier_container_t;
	typedef std::unordered_map<uint32_t, std::unique_ptr<ComponentIcon> > icon_lut_t;
private:
	bezier_container_t  m_curves;
	Point               m_size;

	static icon_lut_t   m_icon_cache;
};

} // namespace gui

} // namespace lsim

#endif // LSIM_GUI_COMPONENT_ICON_H
