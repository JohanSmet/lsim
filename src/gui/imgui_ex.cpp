// imgui_ex.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// QOL extensions for imgui

#include "imgui_ex.h"

namespace ImGuiEx {

void Text(ImVec2 at, TextJustify justify, const char *text) {
    switch(justify) {
        case LEFT: TextLeftJustify(at, text); break;
        case RIGHT: TextRightJustify(at, text); break;
        case CENTER: TextCentered(at, text); break;
    }
}

void TextLeftJustify(ImVec2 at, const char *text) {
    ImGui::SetCursorScreenPos(ImVec2(at.x, at.y));
    ImGui::Text(text);
}

void TextRightJustify(ImVec2 at, const char *text) {
    auto size = ImGui::CalcTextSize(text);
    ImGui::SetCursorScreenPos(ImVec2(at.x - size.x, at.y));
    ImGui::Text(text);
}

void TextCentered(ImVec2 at, const char *text) {
    auto size = ImGui::CalcTextSize(text);
    ImGui::SetCursorScreenPos(ImVec2(at.x - size.x / 2, at.y));
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
