#ifndef LSIM_GUI_MAIN_GUI_H
#define LSIM_GUI_MAIN_GUI_H

namespace lsim {

class LSimContext;
class SimCircuit;
class ModelComponent;

namespace gui {

void main_gui_setup(LSimContext *sim, const char *circuit_file);
void main_gui(LSimContext *sim);
void main_gui_drill_down_sub_circuit(SimCircuit *parent_inst, ModelComponent *comp);

} // namespace gui

} // namespace lsim

#endif // LSIM_GUI_MAIN_GUI_H