// load_logisim.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Load circuit from a LogiSim circuit

#ifndef LSIM_LOAD_LOGISIM_H
#define  LSIM_LOAD_LOGISIM_H

// includes
#include <vector>

// forward declarations
class Circuit;

// interface
bool load_logisim(Circuit *circuit, const char *filename);
bool load_logisim(Circuit *circuit, const char *data, size_t len);

#endif // LSIM_LOAD_LOGISIM_H