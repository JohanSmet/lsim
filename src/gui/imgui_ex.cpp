// imgui_ex.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// QOL extensions for imgui

#include "imgui_ex.h"

namespace ImGuiEx {

void Text(ImVec2 at, const char *text, TextAlignHor align_hor, TextAlignVer align_ver) {
    auto pos = at;

    if (align_hor != TAH_LEFT || align_ver != TAV_TOP) {
        auto size = ImGui::CalcTextSize(text);

        if (align_hor == TAH_RIGHT) {
            pos.x -= size.x;
        } else if (align_hor == TAH_CENTER) {
            pos.x -= size.x / 2.0f;
        }

        if (align_ver == TAV_BOTTOM) {
            pos.y -= size.y;
        } else  if (align_ver == TAV_CENTER) {
            pos.y -= size.y / 2.0f;
        }
    }

    ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y));
    ImGui::Text(text);
}

void RectFilled(ImVec2 p1, ImVec2 p2, ImU32 col) {
    auto min = p1;
    auto max = p2;

	if (max.x < min.x) {
        std::swap(min.x, max.x);
	}

	if (max.y < min.y) {
        std::swap(min.y, max.y);
	}

    ImGui::GetWindowDrawList()->AddRectFilled(min, max, col);
}

} // namespace ImGuiEx
