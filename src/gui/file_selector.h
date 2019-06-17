// file_selector.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// file selection GUI

#ifndef LSIM_GUI_FILE_SELECTOR_H
#define LSIM_GUI_FILE_SELECTOR_H

#include <string>
#include <functional>

namespace lsim {

class LSimContext;

namespace gui {

typedef std::function<void(const std::string &)> on_select_func_t;

void ui_file_selector_open(LSimContext *context, on_select_func_t on_select);
void ui_file_selector_define();

} // namespace gui

} // namespace lsim



#endif // LSIM_GUI_FILE_SELECTOR_H