// gate.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// base for gates

#ifndef LSIM_GATE_H
#define LSIM_GATE_H

#include "basic.h"

class Constant : public Component {
public:
    Constant(Circuit *circuit, size_t outputs, Value value);
    virtual void process();
private:
    Value m_value;
};

class Buffer : public Component {
public:
    Buffer(Circuit *circuit, size_t data_bits);
    virtual void process();
};

class AndGate : public Component {
public:
    AndGate(Circuit *circuit, size_t num_inputs);
    virtual void process();
};

#endif // LSIM_GATE_H