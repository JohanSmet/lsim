#ifndef LSIM_GUI_MAIN_GUI_H
#define LSIM_GUI_MAIN_GUI_H

namespace lsim {

class SimCircuit;
class ModelComponent;

namespace gui {

void main_window_setup(const char *circuit_file);
void main_window_update();

} // namespace gui

} // namespace lsim

#endif // LSIM_GUI_MAIN_GUI_H