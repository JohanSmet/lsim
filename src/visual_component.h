// visual_component.h - Johan Smet - BSD-3-Clause (see LICENSE)

#ifndef LSIM_VISUAL_COMPONENT_H
#define LSIM_VISUAL_COMPONENT_H

#include <array>
#include <memory>

class Component;
class Circuit;

class VisualComponent {
public:
    // nested types
    typedef uint32_t Type;

    enum Orientation {
        EAST = 0,
        SOUTH = 90,
        WEST = 180,
        NORTH = 270
    };

    typedef std::array<float, 2> point_t;
    typedef std::unique_ptr<VisualComponent> uptr_t;

public:
    VisualComponent(Component *component);
    VisualComponent(Circuit *circuit);
    void set_orientation(Orientation orientation);
    void set_position(point_t pos);

    uint32_t get_type() const {return m_type;}
    Orientation get_orientation() const {return m_orientation;}
    point_t get_position() const {return m_position;}
    Component *get_component() const {return m_component;}
    Circuit *get_circuit() const {return m_circuit;}

private:    
    Type        m_type;
    Orientation m_orientation;
    point_t     m_position;
    Component * m_component;
    Circuit *   m_circuit;
};



#endif // LSIM_VISUAL_COMPONENT_H