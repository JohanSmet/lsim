// serialize.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Serialize / deserialize circuits & components

#ifndef LSIM_SERIALISE_H
#define LSIM_SERIALISE_H

namespace lsim {

class LSimContext;
class ModelCircuitLibrary;

bool serialize_library(LSimContext *context, ModelCircuitLibrary *lib, const char *filename);
bool deserialize_library(LSimContext *context, ModelCircuitLibrary *lib, const char *filename);

} // namespace lsim

#endif // LSIM_SERIALIZE_H