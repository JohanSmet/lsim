// model_wire.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// describe the wires that connect components

#ifndef LSIM_MODEL_WIRE_H
#define LSIM_MODEL_WIRE_H

#include <memory>
#include <set>

#include "sim_types.h"
#include "algebra.h"

namespace lsim {

class ModelWireJunction {
public:
    typedef std::unique_ptr<ModelWireJunction> uptr_t;
public:
    ModelWireJunction(const Point &p, class ModelWireSegment *segment);

    const Point &position() const {return m_position;}
    size_t num_segments() const {return m_segments.size();}
    class ModelWireSegment *segment(size_t idx) const;

    void add_segment(ModelWireSegment *segment);
    void remove_segment(ModelWireSegment *segment);

	void move(const Point& delta);

private:
    Point									m_position;
    std::vector<class ModelWireSegment *>   m_segments;
};

class ModelWireSegment {
public:
    typedef std::unique_ptr<ModelWireSegment> uptr_t;
public:
    ModelWireSegment(class ModelWire *wire);

    void set_junction(size_t idx, ModelWireJunction *junction);
    ModelWireJunction *junction(size_t idx) const;
    ModelWire *wire() const {return m_wire;}

    bool point_on_segment(const Point &p);

	void move(const Point& delta);

private:
    class ModelWire *m_wire;
    std::array<ModelWireJunction *, 2>  m_ends;
};

class ModelWire {
public:
    typedef std::unique_ptr<ModelWire>   uptr_t;
    typedef std::vector<ModelWireJunction::uptr_t> junction_container_t;
    typedef std::vector<ModelWireSegment::uptr_t>  segment_container_t;
    typedef std::set<ModelWireSegment *> segment_set_t;

public:
    ModelWire(uint32_t id);
    ModelWire(const ModelWire &) = delete;
    uint32_t id() const {return m_id;}

    // pins
    void add_pin(pin_id_t pin);
    size_t num_pins() const {return m_pins.size();}
    pin_id_t pin(size_t index) const;
    void remove_component_pins(uint32_t component_id);
    void remove_pin(pin_id_t pin);
    void clear_pins();

    // segments & junctions
    size_t num_segments() const {return m_segments.size();}
    const Point &segment_point(size_t segment_idx, size_t point_idx) const;
    ModelWireSegment *segment_by_index(size_t segment_idx);

    size_t num_junctions() const {return m_junctions.size();}
    size_t junction_segment_count(size_t idx) const;
    const Point &junction_position(size_t idx) const;

    ModelWireJunction *add_junction(const Point &p, ModelWireSegment *segment);
    ModelWireSegment *add_segment(const Point &p0, const Point &p1);
    void add_segments(Point *anchors, size_t num_anchors);
    void merge(ModelWire *other);
    void split_at_new_junction(const Point &p);
	void move(const Point& delta);

    void simplify();

    bool point_is_junction(const Point &p) const;
    bool point_on_wire(const Point &p) const;
    ModelWireSegment *segment_at_point(const Point &p) const;

    void remove_segment(ModelWireSegment *segment);
    segment_set_t reachable_segments(ModelWireSegment *from_segment) const;
    bool in_one_piece() const;

private:
    void remove_junction(ModelWireJunction *junction);
    void remove_segment_from_junction(ModelWireJunction *junction, ModelWireSegment *segment);
    void remove_redundant_segment(ModelWireSegment *segment);

private:
    uint32_t m_id;
    pin_id_container_t    m_pins;
    junction_container_t  m_junctions;
    segment_container_t   m_segments;
};


} // namespace lsim


#endif // LSIM_MODEL_WIRE_H