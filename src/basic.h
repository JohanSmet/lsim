// basic.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Basic component parts

#ifndef LSIM_BASIC_H
#define LSIM_BASIC_H

#include <vector>
#include <string>
#include <memory>

#include "visual_component.h"

class Circuit;

typedef uint32_t pin_t;
typedef uint32_t node_t;

const pin_t PIN_UNDEFINED = static_cast<pin_t>(-1);
const node_t NOT_CONNECTED = (node_t) -1;

enum Value {
    VALUE_FALSE         = 0,
    VALUE_TRUE          = 1,
    VALUE_UNDEFINED     = 2,
    VALUE_ERROR         = 3,
};

enum Priority {
    PRIORITY_NORMAL     = 0,
    PRIORITY_DEFERRED   = 1
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
    // nested types
    typedef std::vector<pin_t>  pin_container_t;
    typedef std::vector<Value>  value_container_t;
    typedef std::unique_ptr<Component> uptr_t;

public:
    // construction
    Component(size_t pin_count, VisualComponent::Type type, Priority priority = PRIORITY_NORMAL);
    Component(const Component &other);

    Circuit *get_circuit() const {return m_circuit;}
    Priority get_priority() const {return m_priority;}

    // materialize: integrate the component in the specified circuit
    virtual void materialize(Circuit *circuit);

    // clone: create a copy of this component to integrate in another circuit
    virtual Component::uptr_t clone() const = 0;

    // access pins
    pin_t pin(uint32_t index) const;
    size_t num_pins() const {return m_pins.size();}
    const pin_container_t &pins() const {return m_pins;}
    pin_container_t pins(size_t start, size_t end);
    void add_pin(pin_t pin) {m_pins.push_back(pin);}

    // tick: execute one simulation cycle if the value of any of the connected
    //       nodes changed in the previous cycle. If not, no processing is done 
    //       but the cached values are written to the pins
    void tick();
    virtual void process() = 0;

    // read_pin: read the value of the node the specified pin connects to
    Value read_pin(uint32_t index) const;

    // visual component
    VisualComponent *create_visual(VisualComponent::point_t position, VisualComponent::Orientation orientation);

protected:
    // write_pin: set the value of the node the specified pin connects to
    void write_pin(uint32_t index, Value value);

    // read/write_checked: convenience functions that make it easy to check if 
    //      all the read nodes (since last bad-read reset) had a valid boolean
    //      value (not undefined or error)
    bool read_pin_checked(uint32_t index);
    void write_pin_checked(uint32_t index, bool value);
    void reset_bad_read_check() {m_read_bad = false;}

    // is_dirty() : used by tick() to check if processing should occur.
    //      default version checks all input nodes - override if other conditions apply
    virtual bool is_dirty() const;

private:
    Circuit *m_circuit;
    size_t m_pin_count;
    pin_container_t  m_pins;
    value_container_t m_values;
    bool m_read_bad;
    VisualComponent::Type m_visual_type;
    Priority m_priority;
};

// CloneComponent: helper class to prevent every derived class from implementing the same 
//                 clone function
template <class Derived>
class CloneComponent : public Component {
public:
    CloneComponent(size_t pin_count, VisualComponent::Type type) : 
        Component(pin_count, type) {
    }

    CloneComponent(size_t pin_count, VisualComponent::Type type, Priority priority) : 
        Component(pin_count, type, priority) {
    }

    Component::uptr_t clone() const override {
        return std::make_unique<Derived>(static_cast<Derived const &>(*this));
    }
};

// connector - I/O between circuits
class Connector : public CloneComponent<Connector> {
public:
    enum TYPE {
        OUTPUT = false,
        INPUT = true
    };

public:
    Connector(const char *name, size_t data_bits, bool input);
    void materialize(Circuit *circuit) override;

    virtual bool is_dirty() const override;
    virtual void process() override;

    // data
    Value get_data(size_t index) const;

    void change_data(uint64_t data);
    void change_data(Value data);
    void change_data(value_container_t data);
    void change_data(size_t index, Value data);

    // properties
    const char *name() const {return m_name.c_str();}
    bool is_input() const {return m_input;}
    bool is_output() const {return !m_input;}
    bool is_tristate() const {return m_tristate;}

    void set_input(bool input) {m_input = input;}
    void set_tristate(bool tristate) {m_tristate = tristate;}

private: 
    std::string         m_name;
    value_container_t   m_data;
    bool                m_changed;
    bool                m_input;
    bool                m_tristate;
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
