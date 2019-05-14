// colors.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Commonly used colors 

#ifndef LSIM_GUI_COLORS_H
#define LSIM_GUI_COLORS_H

#include "imgui/imgui.h"

constexpr const auto COLOR_COMPONENT_BORDER = IM_COL32(200, 200, 200, 255);
constexpr const auto COLOR_COMPONENT_BORDER_DRAGGING = IM_COL32(100, 100, 100, 255);
constexpr const auto COLOR_COMPONENT_ICON   = COLOR_COMPONENT_BORDER;

constexpr const auto COLOR_COMPONENT_SELECTED = IM_COL32(100, 100, 0, 255);

constexpr const auto COLOR_CONNECTION_FALSE = IM_COL32(0, 75, 0, 255);
constexpr const auto COLOR_CONNECTION_TRUE  = IM_COL32(0, 175, 0, 255);
constexpr const auto COLOR_CONNECTION_UNDEFINED = IM_COL32(120, 120, 120, 255);
constexpr const auto COLOR_CONNECTION_ERROR = IM_COL32(200, 0, 0, 255);

constexpr const auto COLOR_ENDPOINT = IM_COL32(150, 150, 150, 255);
constexpr const auto COLOR_ENDPOINT_HOVER = IM_COL32(255, 255, 0, 200);

const ImU32 COLOR_CONNECTION[] = {
    COLOR_CONNECTION_FALSE,
    COLOR_CONNECTION_TRUE,
    COLOR_CONNECTION_UNDEFINED,
    COLOR_CONNECTION_ERROR
};

constexpr const auto COLOR_GRID_LINE = IM_COL32(200, 200, 200, 40);

#endif // LSIM_GUI_COLORS_H