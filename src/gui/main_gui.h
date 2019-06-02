#ifndef LSIM_GUI_MAIN_GUI_H
#define LSIM_GUI_MAIN_GUI_H

namespace lsim {

class LSimContext;

namespace gui {

void main_gui_setup(LSimContext *sim, const char *circuit_file);
void main_gui(LSimContext *sim);

} // namespace gui

} // namespace lsim

#endif // LSIM_GUI_MAIN_GUI_H