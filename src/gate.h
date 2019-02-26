// gate.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// base for gates

#ifndef LSIM_GATE_H
#define LSIM_GATE_H

#include "basic.h"

class AndGate : Component {
public:
    AndGate(Circuit *circuit, size_t num_inputs);

    virtual void process();
    
};

#if 0
#include <vector>
#include <memory>

class Gate {
public:
    Gate(size_t delay);

    void set_dirty();
    void tick();

protected:
    void add_pin(PinDirection direction);
    virtual void process() = 0;

protected:
    std::vector<std::unique_ptr<Pin>> m_pins;
    Delay m_delay;
    bool m_dirty;
};

class AndGate : public Gate {
public:
    AndGate(size_t num_inputs);

private:
    size_t m_num_inputs;
};

#endif


#endif // LSIM_GATE_H