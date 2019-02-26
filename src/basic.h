// basic.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Basic component parts

#ifndef LSIM_BASIC_H
#define LSIM_BASIC_H

#include <cstdint>
#include <vector>

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

    virtual void process() = 0;

protected:
    Circuit *m_circuit;
    std::vector<pin_t>  m_pins;
};


#if 0

#include <unordered_set>
#include <deque>

class Gate;


enum PinDirection {
    PIN_INPUT,
    PIN_OUTPUT
};

class Pin {
public:
    Pin(Gate *parent, PinDirection direction);

    Value get_value() const {return m_value;}
    PinDirection get_direction() const {return m_direction;}

    void generate_value(Value value);
    void receive_value(Value value);
    void change_direction(PinDirection direction);

private:
    // member variables
    Gate *          m_parent;
    PinDirection    m_direction;
    Value           m_value;
};

class Connection {
public:
    Connection();

    bool is_valid() const {return m_valid;}

    void add_pin(Pin *pin);
    void propagate_output();

private:
    void validate();
    Pin *active_output() const;

private:
    // member variables
    std::unordered_set<Pin *>  m_pins;
    bool m_valid;
};

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
