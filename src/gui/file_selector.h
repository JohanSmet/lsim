// file_selector.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// file selection GUI

#ifndef LSIM_GUI_FILE_SELECTOR_H
#define LSIM_GUI_FILE_SELECTOR_H

#include <string>

namespace lsim {

namespace gui {

void ui_file_selector_init(const char *base_dir);
bool ui_file_selector(std::string *selected);

} // namespace gui

} // namespace lsim



#endif // LSIM_GUI_FILE_SELECTOR_H