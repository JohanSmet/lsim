// gate.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// basic gates

#ifndef LSIM_GATE_H
#define LSIM_GATE_H

#include "basic.h"

Component *Constant(Circuit *circuit, Value value);
Component *Buffer(Circuit *circuit, size_t data_bits);
Component *TriStateBuffer(Circuit *circuit, size_t data_bits);
Component *AndGate(Circuit *circuit, size_t num_inputs);
Component *OrGate(Circuit *circuit, size_t num_inputs);
Component *NotGate(Circuit *circuit);
Component *NandGate(Circuit *circuit, size_t num_inputs);
Component *NorGate(Circuit *circuit, size_t num_inputs);
Component *XorGate(Circuit *circuit);
Component *XnorGate(Circuit *circuit);

#endif // LSIM_GATE_H