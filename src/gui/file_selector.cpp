// file_selector.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// file selection GUI

#include "file_selector.h"
#include "imgui/imgui.h"
#include "lsim_context.h"

#include "cute_files.h"

#include <vector>
#include <algorithm>

namespace {

// famous last words: there should only ever by one file selection popup open at a time, so why bother.
std::vector<std::string>  file_list;
lsim::gui::on_select_func_t on_select_callback = nullptr;

void r_scan_dir(const char *dir_path) {
    std::vector<cf_file_t>  entries;
    cf_dir_t dir;

    for (cf_dir_open(&dir, dir_path); dir.has_next; cf_dir_next(&dir)) {
        cf_file_t file;
        cf_read_file(&dir, &file);

        if (file.is_dir && file.name[0] == '.')
            continue;

        if (file.is_reg && !cf_match_ext(&file, ".lsim"))
            continue;

        entries.push_back(file);
    }

    std::sort(entries.begin(), entries.end(), [](const auto &a, const auto &b) {
        if (a.is_dir && !b.is_dir) {
            return true;
        } else if (!a.is_dir && b.is_dir) {
            return false;
        } else {
            return strcmp(a.name, b.name) < 0;
        }
    });

    for (const auto &entry : entries) {
        std::string name = entry.path;
        //name += '/' + entry.name;

        if (entry.is_dir) {
            r_scan_dir(name.c_str());
        } else {
           file_list.push_back(name);
        }
    }

    cf_dir_close(&dir);
}

} // unnamed namespace

namespace lsim {

namespace gui {

void ui_file_selector_open(LSimContext *context, on_select_func_t on_select) {
    file_list.clear();
    for (size_t idx = 0; idx < context->num_folders(); ++idx) {
        r_scan_dir(context->folder_path(idx).c_str());
    }

    on_select_callback = on_select;

    ImGui::OpenPopup("Select file");
}

void ui_file_selector_define() {

    ImGui::SetNextWindowPos(ImGui::GetMousePos(), ImGuiCond_Appearing);
    if (ImGui::BeginPopupModal("Select file")) {
        ImGui::Text("Click to select library");
        ImGui::Separator();

        for (const auto &entry : file_list) {
            if (ImGui::Selectable(entry.c_str())) {
                if (on_select_callback) {
                    on_select_callback(entry);
                }
                break;
            }
        }

        ImGui::Separator();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

} // namespace lsim::gui

} // namespace lsim
