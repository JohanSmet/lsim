// visual_component.h - Johan Smet - BSD-3-Clause (see LICENSE)

#ifndef LSIM_VISUAL_COMPONENT_H
#define LSIM_VISUAL_COMPONENT_H

#include <vector>
#include <array>
#include <memory>
#include <set>

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
    WireSegment(class Wire *wire);

    void set_junction(size_t idx, WireJunction *junction);
    WireJunction *junction(size_t idx) const;
    Wire *wire() const {return m_wire;}

    bool point_on_segment(const Point &p);

private:
    class Wire *m_wire;
    std::array<WireJunction *, 2>  m_ends;
};

class Wire {
public:
    typedef std::vector<WireJunction::uptr_t> junction_container_t;
    typedef std::vector<WireSegment::uptr_t>  segment_container_t;
    typedef std::set<WireSegment *> segment_set_t;
    typedef std::unique_ptr<Wire> uptr_t;

public:
    Wire(Point *anchors, size_t num_anchors);
    Wire();

    size_t num_segments() const {return m_segments.size();}
    const Point &segment_point(size_t segment_idx, size_t point_idx) const;
    WireSegment *segment_by_index(size_t segment_idx);

    size_t num_junctions() const {return m_junctions.size();}
    size_t junction_segment_count(size_t idx) const;
    const Point &junction_position(size_t idx) const;

    WireJunction *add_junction(const Point &p, WireSegment *segment);
    WireSegment *add_segment(const Point &p0, const Point &p1);
    void add_segments(Point *anchors, size_t num_anchors);
    void merge(Wire *other);
    void split_at_point(const Point &p);
    
    void set_node(node_t node) {m_node = node;};
    node_t node() const {return m_node;}

    void simplify();

    bool point_is_junction(const Point &p) const;
    bool point_on_wire(const Point &p) const;
    WireSegment *segment_at_point(const Point &p) const;

    void remove_segment(WireSegment *segment);
    segment_set_t reachable_segments(WireSegment *from_segment) const;
    bool in_one_piece() const;

    void add_pin(pin_t pin);
    void clear_pins();
    const Component::pin_container_t &pins() const {return m_pins;}

private:
    void remove_junction(WireJunction *junction);
    void remove_segment_from_junction(WireJunction *junction, WireSegment *segment);
    void remove_redundant_segment(WireSegment *segment);

private:
    junction_container_t  m_junctions;
    segment_container_t   m_segments;
    node_t                m_node;
    Component::pin_container_t m_pins;

};

#endif // LSIM_VISUAL_COMPONENT_H