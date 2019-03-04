// gate.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// base for gates

#ifndef LSIM_GATE_H
#define LSIM_GATE_H

#include "basic.h"

class Constant : public Component {
public:
    Constant(Circuit *circuit, Value value);
    virtual void tick();
    virtual void process();
private:
    Value m_value;
};

class Buffer : public Component {
public:
    Buffer(Circuit *circuit, size_t data_bits);
    virtual void process();
};

class TriStateBuffer : public Component {
public:
    TriStateBuffer(Circuit *circuit, size_t data_bits);
    virtual void process();
    pin_t enable_pin() const {return m_pins[m_enable_idx];}
private:
    size_t  m_enable_idx;
};

class AndGate : public Component {
public:
    AndGate(Circuit *circuit, size_t num_inputs);
    virtual void process();
};

class OrGate : public Component {
public:
    OrGate(Circuit *circuit, size_t num_inputs);
    virtual void process();
};

class NotGate : public Component {
public:
    NotGate(Circuit *circuit);
    virtual void process();
};

class NandGate : public Component {
public:
    NandGate(Circuit *circuit, size_t num_inputs);
    virtual void process();
};

class NorGate : public Component {
public:
    NorGate(Circuit *circuit, size_t num_inputs);
    virtual void process();
};

class XorGate : public Component {
public:
    XorGate(Circuit *circuit);
    virtual void process();
};

class XnorGate : public Component {
public:
    XnorGate(Circuit *circuit);
    virtual void process();
};

#endif // LSIM_GATE_H