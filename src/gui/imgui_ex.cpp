// imgui_ex.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// QOL extensions for imgui

#include "imgui_ex.h"

namespace ImGuiEx {

void TextLeftJustify(ImVec2 at, const char *text) {
    ImGui::SetCursorPos(ImVec2(at.x, at.y));
    ImGui::Text(text);
}

void TextRightJustify(ImVec2 at, const char *text) {
    auto size = ImGui::CalcTextSize(text);
    ImGui::SetCursorPos(ImVec2(at.x - size.x, at.y));
    ImGui::Text(text);
}

void TextCentered(ImVec2 at, const char *text) {
    auto size = ImGui::CalcTextSize(text);
    ImGui::SetCursorPos(ImVec2(at.x - size.x / 2, at.y));
    ImGui::Text(text);
}

} // namespace ImGuiEx
