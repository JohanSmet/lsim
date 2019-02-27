// basic.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Basic component parts

#ifndef LSIM_BASIC_H
#define LSIM_BASIC_H

#include <cstdint>
#include <vector>
#include <array>

class Circuit;

typedef uint32_t pin_t;
typedef uint32_t node_t;

enum Value {
    VALUE_FALSE         = 0,
    VALUE_TRUE          = 1,
    VALUE_UNDEFINED     = 2
};

class Component {
public:
    Component(Circuit *circuit, size_t pin_count);

    pin_t pin(uint32_t index);

    virtual void prepare();
    virtual void tick();
    virtual void process() = 0;

    void set_dirty() {m_dirty[1] = true;}

protected:
    Circuit *m_circuit;
    std::vector<pin_t>  m_pins;
    std::array<bool, 2> m_dirty;
};

// connector - I/O between circuits
class Connector : public Component {
public:
    Connector(Circuit *circuit, size_t data_bits);
    virtual void process();

    void change_data(uint64_t data);

private: 
    uint64_t m_data;
    bool     m_changed;
};


#if 0

class Delay {
public:
    Delay(size_t delay);

    Value push_value(Value new_value);
    Value push_last();

private:
    // member variables
    std::deque<Value>  m_values;
};

#endif


#endif // LSIM_BASIC_H
