// component_draw.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// functions to materialize components and define custom draw functions

#ifndef LSIM_GUI_COMPONENT_DRAW_H
#define LSIM_GUI_COMPONENT_DRAW_H

namespace lsim {

namespace gui {

void component_register_basic();
void component_register_extra();
void component_register_gates();
void component_register_input_output();

} // namespace lsim::gui

} // namespace lsim

#endif // LSIM_GUI_COMPONENT_DRAW_H