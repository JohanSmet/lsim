// imgui_ex.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// QOL extensions for imgui

#include "imgui_ex.h"
#include "imgui/imgui_internal.h"

namespace {

static int transform_start_index = 0;

inline ImVec2 aligned_position(ImVec2 at, const char *text, ImGuiEx::TextAlignHor align_hor, ImGuiEx::TextAlignVer align_ver) {

    if (align_hor != ImGuiEx::TAH_LEFT || align_ver != ImGuiEx::TAV_TOP) {
        auto pos = at;
        auto size = ImGui::CalcTextSize(text);

        if (align_hor == ImGuiEx::TAH_RIGHT) {
            pos.x -= size.x;
        } else if (align_hor == ImGuiEx::TAH_CENTER) {
            pos.x -= size.x / 2.0f;
        }

        if (align_ver == ImGuiEx::TAV_BOTTOM) {
            pos.y -= size.y;
        } else  if (align_ver == ImGuiEx::TAV_CENTER) {
            pos.y -= size.y / 2.0f;
        }
        return pos;
    } else {
        return at;
    }
}

} // unnamed namespace

namespace ImGuiEx {

void Text(ImVec2 at, const char *text, TextAlignHor align_hor, TextAlignVer align_ver) {
    auto pos = aligned_position(at, text, align_hor, align_ver);
    ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y));
    ImGui::Text("%s", text);
}

void TextNoClip(ImVec2 at, const char *text, ImU32 col, TextAlignHor align_hor, TextAlignVer align_ver) {
// reimplementation of ImDrawList::AddText which does not do clipping
//  particullary to be used with TransformStart and TransformEnd

    auto pos = aligned_position(at, text, align_hor, align_ver);

    if ((col & IM_COL32_A_MASK) == 0)
        return;

    auto text_begin = text;
    auto text_end = text_begin + strlen(text_begin);
    if (text_begin == text_end)
        return;

    // Pull default font/size from the shared ImDrawListSharedData instance
    auto draw_list = ImGui::GetWindowDrawList();
    const auto font = draw_list->_Data->Font;
    const auto font_size = draw_list->_Data->FontSize;
    const auto no_clipping = ImVec4(-8192.0f, -8192.0f, +8192.0f, +8192.0f);

    font->RenderText(draw_list, font_size, pos, col, no_clipping, text_begin, text_end);
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

// TransformStart & TransformEnd based on ideas from https://gist.github.com/carasuca/e72aacadcf6cf8139de46f97158f790f
void TransformStart() {
    transform_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
}

void TransformEnd(lsim::Transform transform) {
    auto &vtx_buf = ImGui::GetWindowDrawList()->VtxBuffer;

    for (auto i = transform_start_index; i < vtx_buf.Size; ++i) {
        vtx_buf[i].pos = transform.apply(vtx_buf[i].pos);
    }
}

} // namespace ImGuiEx
