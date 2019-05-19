// imgui_ex.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// QOL extensions for imgui

#include "imgui/imgui.h"

namespace ImGuiEx {

enum TextAlignHor {
    TAH_LEFT,
    TAH_RIGHT,
    TAH_CENTER
};

enum TextAlignVer {
    TAV_TOP,
    TAV_BOTTOM,
    TAV_CENTER
};

void Text(ImVec2 at, const char *text, TextAlignHor align_hor = TAH_LEFT, TextAlignVer align_ver = TAV_TOP);
inline void Text(ImVec2 at, const std::string &text, TextAlignHor align_hor = TAH_LEFT, TextAlignVer align_ver = TAV_TOP) {
    Text(at, text.c_str(), align_hor, align_ver);
}
void RectFilled(ImVec2 p1, ImVec2 p2, ImU32 col);

} // namespace ImGuiEx