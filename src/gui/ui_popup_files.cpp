// ui_popup_files.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// file related popups

#include "ui_popup_files.h"
#include "imgui/imgui.h"
#include "lsim_context.h"

#include "cute_files.h"

#include <vector>
#include <algorithm>

namespace {

constexpr const char* POPUP_FILE_SELECTOR = "Select file";
constexpr const char* POPUP_FILENAME_ENTRY = "Filename entry";

// famous last words: there should only ever by one file selection popup open at a time, so why bother.
std::vector<std::string>  file_list;
lsim::gui::on_select_func_t on_select_callback = nullptr;
lsim::gui::on_select_func_t on_close_callback = nullptr;

void r_scan_dir(const char *dir_path) {
    std::vector<cf_file_t>  entries;
    cf_dir_t dir;

    for (cf_dir_open(&dir, dir_path); dir.has_next != 0; cf_dir_next(&dir)) {
        cf_file_t file;
        cf_read_file(&dir, &file);

        if (file.is_dir != 0 && file.name[0] == '.')
            continue;

        if (file.is_reg != 0 && cf_match_ext(&file, ".lsim") == 0)
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

        if (entry.is_dir != 0) {
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

    on_select_callback = move(on_select);

	ImGui::OpenPopup(POPUP_FILE_SELECTOR);
}

void ui_file_selector_define() {

    ImGui::SetNextWindowPos(ImGui::GetMousePos(), ImGuiCond_Appearing);
    if (ImGui::BeginPopupModal(POPUP_FILE_SELECTOR)) {
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

void ui_filename_entry_open(LSimContext* context, on_select_func_t on_close) {
	on_close_callback = move(on_close);
	ImGui::OpenPopup(POPUP_FILENAME_ENTRY);
}

void ui_filename_entry_define() {
	static char buffer[512] = "";
	
	ImGui::SetNextWindowContentWidth(300);
	if (ImGui::BeginPopupModal(POPUP_FILENAME_ENTRY)) {
		ImGui::InputText("Filename", buffer, sizeof(buffer) / sizeof(buffer[0]));

		auto close_popup = []() {
			buffer[0] = '\0';
			ImGui::CloseCurrentPopup();
		};

		if (ImGui::Button("Ok")) {
			if (on_close_callback) {
				on_close_callback(buffer);
			}
			close_popup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			close_popup();
		}
		ImGui::EndPopup();
	}

}

} // namespace lsim::gui

} // namespace lsim
