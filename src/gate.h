// gate.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// base for gates

#ifndef LSIM_GATE_H
#define LSIM_GATE_H

#include "basic.h"

class Constant : public CloneComponent<Constant> {
public:
    Constant(Value value);
    virtual bool is_dirty() const override;
    virtual void process();
private:
    Value m_value;
};

class Buffer : public CloneComponent<Buffer> {
public:
    Buffer(size_t data_bits);
    virtual void process();
};

class TriStateBuffer : public CloneComponent<TriStateBuffer> {
public:
    TriStateBuffer(size_t data_bits);
    virtual void process();
    pin_t enable_pin() const {return m_pins[m_enable_idx];}
private:
    size_t  m_enable_idx;
};

class AndGate : public CloneComponent<AndGate> {
public:
    AndGate(size_t num_inputs);
    virtual void process();
};

class OrGate : public CloneComponent<OrGate> {
public:
    OrGate(size_t num_inputs);
    virtual void process();
};

class NotGate : public CloneComponent<NotGate> {
public:
    NotGate();
    virtual void process();
};

class NandGate : public CloneComponent<NandGate> {
public:
    NandGate(size_t num_inputs);
    virtual void process();
};

class NorGate : public CloneComponent<NorGate> {
public:
    NorGate(size_t num_inputs);
    virtual void process();
};

class XorGate : public CloneComponent<XorGate> {
public:
    XorGate();
    virtual void process();
};

class XnorGate : public CloneComponent<XnorGate> {
public:
    XnorGate();
    virtual void process();
};

#endif // LSIM_GATE_H