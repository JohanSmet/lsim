// visual_component.h - Johan Smet - BSD-3-Clause (see LICENSE)

#ifndef LSIM_VISUAL_COMPONENT_H
#define LSIM_VISUAL_COMPONENT_H

#include <array>
#include <memory>

class Component;

class VisualComponent {
public:
    // nested types
    enum Type {
        CONNECTOR = 0,
        CONSTANT,
        PULL_RESISTOR,
        BUFFER,
        TRISTATE_BUFFER,
        AND_GATE,
        OR_GATE,
        NOT_GATE,
        NAND_GATE,
        NOR_GATE,
        XOR_GATE,
        XNOR_GATE,
        SUB_CIRCUIT
    };

    enum Orientation {
        EAST = 0,
        SOUTH = 90,
        WEST = 180,
        NORTH = 270
    };

    typedef std::array<float, 2> point_t;
    typedef std::unique_ptr<VisualComponent> uptr_t;

public:
    VisualComponent(Type type, Component *component);
    void set_orientation(Orientation orientation);
    void set_position(point_t pos);

    Type get_type() const {return m_type;}
    Orientation get_orientation() const {return m_orientation;}
    point_t get_position() const {return m_position;}
    Component *get_component() const {return m_component;}




private: 
    Type        m_type;
    Orientation m_orientation;
    point_t     m_position;
    Component * m_component;
};



#endif // LSIM_VISUAL_COMPONENT_H