// wire_description.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// describe the wires that connect components

#ifndef LSIM_WIRE_DESCRIPTION_H
#define LSIM_WIRE_DESCRIPTION_H

#include <memory>
#include <set>

#include "sim_types.h"
#include "algebra.h"

namespace lsim {

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
    typedef std::unique_ptr<Wire>   uptr_t;
    typedef std::vector<WireJunction::uptr_t> junction_container_t;
    typedef std::vector<WireSegment::uptr_t>  segment_container_t;
    typedef std::set<WireSegment *> segment_set_t;

public:
    Wire(uint32_t id);
    Wire(const Wire &) = delete;
    uint32_t id() const {return m_id;}

    // pins
    void add_pin(pin_id_t pin);
    size_t num_pins() const {return m_pins.size();}
    pin_id_t pin(size_t index) const;
    void remove_component_pins(uint32_t component_id);
    void clear_pins();

    // segments & junctions
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
    void split_at_new_junction(const Point &p);

    void simplify();

    bool point_is_junction(const Point &p) const;
    bool point_on_wire(const Point &p) const;
    WireSegment *segment_at_point(const Point &p) const;

    void remove_segment(WireSegment *segment);
    segment_set_t reachable_segments(WireSegment *from_segment) const;
    bool in_one_piece() const;

private:
    void remove_junction(WireJunction *junction);
    void remove_segment_from_junction(WireJunction *junction, WireSegment *segment);
    void remove_redundant_segment(WireSegment *segment);

private:
    uint32_t m_id;
    pin_id_container_t    m_pins;
    junction_container_t  m_junctions;
    segment_container_t   m_segments;
};


} // namespace lsim


#endif // LSIM_WIRE_DESCRIPTION_H