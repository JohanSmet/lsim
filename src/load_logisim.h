// load_logisim.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Load circuit from a LogiSim circuit

#ifndef LSIM_LOAD_LOGISIM_H
#define  LSIM_LOAD_LOGISIM_H

// includes
#include <vector>

namespace lsim {

// forward declarations
class Circuit;
class LSimContext;

// interface
bool load_logisim(LSimContext *lsim_context, const char *filename);
bool load_logisim(LSimContext *lsim_context, const char *data, size_t len);

} // namespace lsim

#endif // LSIM_LOAD_LOGISIM_H