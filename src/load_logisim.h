// load_logisim.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Load circuit from a LogiSim circuit

#ifndef LSIM_LOAD_LOGISIM_H
#define  LSIM_LOAD_LOGISIM_H

// includes
#include <vector>

// forward declarations
class Circuit;
class LSimContext;

// interface
Circuit *load_logisim(LSimContext *lsim_context, const char *filename);
Circuit *load_logisim(LSimContext *lsim_context, const char *data, size_t len);

#endif // LSIM_LOAD_LOGISIM_H