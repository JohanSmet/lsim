// component_std.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// registration of standard components

#ifndef LSIM_GUI_COMPONENT_STD_H
#define LSIM_GUI_COMPONENT_STD_H

namespace lsim {

namespace gui {

class ComponentUI;

void component_register_basic();
void component_register_extra();
void component_register_gates();
void component_register_input_output();

} // namespace lsim::gui

} // namespace lsim

#endif // LSIM_GUI_COMPONENT_STD_H