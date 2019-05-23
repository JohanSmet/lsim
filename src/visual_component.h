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

class WireJunction {
public:
    typedef std::unique_ptr<WireJunction> uptr_t;
public:
    WireJunction(const Point &p, class WireSegment *segment);

    const Point &position() const {return m_position;}
    size_t num_segments() const {return m_segments.size();}
    class WireSegment *segment(size_t idx) const;

    void add_segment(WireSegment *segment);
    void remove_segment(WireSegment *segment);

private:
    Point                               m_position;
    std::vector<class WireSegment *>    m_segments;
};

class WireSegment {
public:
    typedef std::unique_ptr<WireSegment> uptr_t;
public:
    WireSegment();

    void set_junction(size_t idx, WireJunction *junction);
    WireJunction *junction(size_t idx) const;

    bool point_on_segment(const Point &p);

private:
    std::array<WireJunction *, 2>  m_ends;
};

class Wire {
public:
    typedef std::vector<WireJunction::uptr_t> junction_container_t;
    typedef std::vector<WireSegment::uptr_t>  segment_container_t;
    typedef std::unique_ptr<Wire> uptr_t;

public:
    Wire(Point *anchors, size_t num_anchors);
    Wire();

    size_t num_segments() const {return m_segments.size();}
    const Point &segment_point(size_t segment_idx, size_t point_idx) const;

    size_t num_junctions() const {return m_junctions.size();}
    size_t junction_segment_count(size_t idx) const;
    const Point &junction_position(size_t idx) const;

    WireJunction *add_junction(const Point &p, WireSegment *segment);
    WireSegment *add_segment(const Point &p0, const Point &p1);
    
    void set_node(node_t node) {m_node = node;};
    node_t node() const {return m_node;}

    void simplify();

    bool point_is_junction(const Point &p) const;
    bool point_on_wire(const Point &p) const;
private:
    void remove_junction(WireJunction *junction);
    void remove_segment_from_junction(WireJunction *junction, WireSegment *segment);
    void remove_redundant_segment(WireSegment *segment);

private:
    junction_container_t  m_junctions;
    segment_container_t   m_segments;
    node_t                m_node;

};

#endif // LSIM_VISUAL_COMPONENT_H