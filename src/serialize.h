// serialize.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Serialize / deserialize circuits & components

#ifndef LSIM_SERIALISE_H
#define LSIM_SERIALISE_H

#include <string>

namespace lsim {

class LSimContext;
class CircuitLibrary;

bool serialize_library(LSimContext *context, CircuitLibrary *lib, const char *filename);
bool deserialize_library(LSimContext *context, CircuitLibrary *lib, const char *filename);

} // namespace lsim

#endif // LSIM_SERIALISE_H