// visual_component.h - Johan Smet - BSD-3-Clause (see LICENSE)

#ifndef LSIM_VISUAL_COMPONENT_H
#define LSIM_VISUAL_COMPONENT_H

#include <vector>
#include <array>
#include <memory>

#include "basic.h"
#include "algebra.h"

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

    typedef std::unique_ptr<VisualComponent> uptr_t;

public:
    VisualComponent(Component *component);
    VisualComponent(Circuit *circuit);
    void set_orientation(Orientation orientation);
    void set_position(Point pos);

    uint32_t get_type() const {return m_type;}
    Orientation get_orientation() const {return m_orientation;}
    Point get_position() const {return m_position;}
    Component *get_component() const {return m_component;}
    Circuit *get_circuit() const {return m_circuit;}

    Component::pin_container_t pins() const;

private:    
    Type        m_type;
    Orientation m_orientation;
    Point       m_position;
    Component * m_component;
    Circuit *   m_circuit;
};

class Wire {
public:
    typedef std::array<Point, 2> segment_t;
    typedef std::vector<segment_t> segment_container_t;
    typedef std::unique_ptr<Wire> uptr_t;

public:
    Wire(Point *anchors, size_t num_anchors);
    Wire();

    size_t num_segments() const {return m_segments.size();}
    const segment_container_t &segments() const {return m_segments;}

    void add_segment(const Point &p0, const Point &p1);
    
    void set_node(node_t node) {m_node = node;};
    node_t node() const {return m_node;}

private:
    segment_container_t   m_segments;
    node_t                m_node;

};

#endif // LSIM_VISUAL_COMPONENT_H