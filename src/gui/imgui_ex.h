// imgui_ex.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// QOL extensions for imgui

#include "imgui/imgui.h"

namespace ImGuiEx {

enum TextJustify {
    LEFT,
    RIGHT,
    CENTER
};

void Text(ImVec2 at, TextJustify justify, const char *text);

void TextLeftJustify(ImVec2 at, const char *text);
void TextRightJustify(ImVec2 at, const char *text);
void TextCentered(ImVec2 at, const char *text);

void RectFilled(ImVec2 p1, ImVec2 p2, ImU32 col);

} // namespace ImGuiEx