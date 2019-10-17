// ui_panel_library.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "imgui_ex.h"

#include "lsim_context.h"
#include "ui_context.h"
#include "ui_popup_files.h"

namespace lsim {

namespace gui {

void ui_panel_library(UIContext* ui_context) {

	// popups
	ui_file_selector_define();
	ui_filename_entry_define();

	if (ImGui::Button("New")) {
		ui_context->simulation_stop();
		ui_context->circuit_library_load("");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load")) {
		ui_context->simulation_stop();
		ui_file_selector_open(ui_context->lsim_context(), [ui_context](const auto& selection) {
			ui_context->circuit_library_load(selection);
		});
	}
	ImGui::SameLine();
	if (ImGui::Button("Save")) {
		if (!ui_context->library_filename().empty()) {
			ui_context->circuit_library_save(ui_context->library_filename());
		} else {
			ui_filename_entry_open([ui_context](const auto& filename) {
				ui_context->circuit_library_save(filename);
			});
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Add Library")) {
		ui_file_selector_open(ui_context->lsim_context(), [ui_context](const std::string& selection) {
			auto name_begin = selection.find_last_of('/') + 1;
			auto name_end = selection.find_last_of('.');
			auto name = selection.substr(name_begin, name_end - name_begin);
			auto context = ui_context->lsim_context();
			context->load_reference_library(name.c_str(), context->relative_file_path(selection).c_str());
			context->user_library()->add_reference(name.c_str());
		});
	}
}

} // namespace lsim::gui

} // namespace lsim
