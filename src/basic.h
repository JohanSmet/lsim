// basic.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Basic component parts

#ifndef LSIM_BASIC_H
#define LSIM_BASIC_H

#include <cstdint>
#include <vector>
#include <array>
#include <string>
#include <memory>

class Circuit;

typedef uint32_t pin_t;
typedef uint32_t node_t;

const pin_t PIN_UNDEFINED = static_cast<pin_t>(-1);
const node_t NOT_CONNECTED = (node_t) -1;

enum Value {
    VALUE_FALSE         = 0,
    VALUE_TRUE          = 1,
    VALUE_UNDEFINED     = 2
};

inline Value negate_value(Value input) {
    if (input == VALUE_TRUE) {
        return VALUE_FALSE; 
    } else if (input == VALUE_FALSE) {
        return VALUE_TRUE;
    } else {
        return VALUE_UNDEFINED;
    }
}


class Component {
public:
    typedef std::vector<pin_t>  pin_container_t;
    typedef std::vector<Value>  value_container_t;

public:
    Component(size_t pin_count);
    Component(const Component &other);

    virtual void materialize(Circuit *circuit);

    pin_t pin(uint32_t index);
    size_t num_pins() const {return m_pins.size();}
    const pin_container_t &pins() const {return m_pins;}
    pin_container_t pins(size_t start, size_t end);

    void write_pin(uint32_t index, Value value);

    virtual std::unique_ptr<Component> clone() const = 0;

    virtual void tick();
    virtual void process() = 0;

private:
    bool is_dirty() const;

protected:
    Circuit *m_circuit;
    size_t m_pin_count;
    pin_container_t  m_pins;
    value_container_t m_values;
};

template <class Derived>
class CloneComponent : public Component {
public:
    CloneComponent(size_t pin_count) : Component(pin_count) {
    }

    std::unique_ptr<Component> clone() const override {
        return std::unique_ptr<Component>(new Derived(static_cast<Derived const &>(*this)));
    }
};

// connector - I/O between circuits
class Connector : public CloneComponent<Connector> {
public:
    Connector(const char *name, size_t data_bits);
    void materialize(Circuit *circuit) override;

    virtual void tick();
    virtual void process();

    void change_data(uint64_t data);

private: 
    uint64_t    m_data;
    bool        m_changed;
    std::string m_name;
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
