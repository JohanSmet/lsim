// model_wire.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// describe the wires that connect components

#include "model_wire.h"
#include "model_circuit.h"

#include <cassert>
#include "std_helper.h"

namespace lsim {

///////////////////////////////////////////////////////////////////////////////
//
// ModelWireJunction
//

ModelWireJunction::ModelWireJunction(const Point &p, ModelWireSegment *segment) :
        m_position(p) {
    m_segments.push_back(segment);
}

void ModelWireJunction::add_segment(ModelWireSegment *segment) {
    m_segments.push_back(segment);
}

void ModelWireJunction::remove_segment(ModelWireSegment *segment) {
	remove(m_segments, segment);
}

ModelWireSegment *ModelWireJunction::segment(size_t idx) const {
    assert(idx < m_segments.size());
    return m_segments[idx];
}

void ModelWireJunction::move(const Point& delta) {
	m_position = m_position + delta;
}

///////////////////////////////////////////////////////////////////////////////
//
// ModelWireSegment
//

ModelWireSegment::ModelWireSegment(ModelWire *wire) : 
        m_wire(wire),
        m_ends({nullptr, nullptr}) {
}

void ModelWireSegment::set_junction(size_t idx, ModelWireJunction *junction) {
    assert(idx < 2);
    m_ends[idx] = junction;
}

ModelWireJunction *ModelWireSegment::junction(size_t idx) const {
    assert(idx < 2);
    return m_ends[idx];
}

bool ModelWireSegment::point_on_segment(const Point &p) {
    return point_on_line_segment(m_ends[0]->position(), m_ends[1]->position(), p);
}

void ModelWireSegment::move(const Point& delta) {
	m_ends[0]->move(delta);
	m_ends[1]->move(delta);
}

///////////////////////////////////////////////////////////////////////////////
//
// ModelWire
//

ModelWire::ModelWire(uint32_t id) : m_id(id) {

}

void ModelWire::add_pin(pin_id_t pin) {
    m_pins.push_back(pin);
}

pin_id_t ModelWire::pin(size_t index) const {
    assert(index < m_pins.size());
    return m_pins[index];
}

void ModelWire::remove_component_pins(uint32_t component_id) {
    remove_if(m_pins, [component_id](const auto &pin_id) {
							return component_id_from_pin_id(pin_id) == component_id;
					  }
			 );
}

void ModelWire::remove_pin(pin_id_t pin) {
	remove(m_pins, pin);
}

void ModelWire::clear_pins() {
    m_pins.clear();
}

const Point &ModelWire::segment_point(size_t segment_idx, size_t point_idx) const {
    assert(segment_idx < m_segments.size());
    assert(point_idx < 2);
    return m_segments[segment_idx]->junction(point_idx)->position();
}

ModelWireSegment *ModelWire::segment_by_index(size_t segment_idx) {
    assert(segment_idx < m_segments.size());
    return m_segments[segment_idx].get();
}

size_t ModelWire::junction_segment_count(size_t idx) const {
    assert(idx < m_junctions.size());
    return m_junctions[idx]->num_segments();
}

const Point &ModelWire::junction_position(size_t idx) const {
    assert(idx < m_junctions.size());
    return m_junctions[idx]->position();
}

ModelWireJunction *ModelWire::add_junction(const Point &p, ModelWireSegment *segment) {
    auto found = std::find_if(m_junctions.begin(), m_junctions.end(), [=](auto &j) {return j->position() == p;});
    if (found != m_junctions.end()) {
        (*found)->add_segment(segment);
        return found->get();
    }

	return create_new_junction(p, segment);
}

ModelWireSegment *ModelWire::add_segment(const Point &p0, const Point &p1) {
    m_segments.push_back(std::make_unique<ModelWireSegment>(this));
    auto segment = m_segments.back().get();

    segment->set_junction(0, add_junction(p0, segment));
    segment->set_junction(1, add_junction(p1, segment));
    return segment;
}

ModelWireJunction *ModelWire::create_new_junction(const Point& p, ModelWireSegment* segment) {
    m_junctions.push_back(std::make_unique<ModelWireJunction>(p, segment));
    return m_junctions.back().get();
}

void ModelWire::add_segments(Point *anchors, size_t num_anchors) {
    for(size_t a = 1;a < num_anchors; ++a) {
        if (anchors[a - 1] != anchors[a]) {
            add_segment(anchors[a - 1], anchors[a]);
        }
    }
}

void ModelWire::merge(ModelWire *other) {
    for (const auto &segment : other->m_segments) {
        add_segment(segment->junction(0)->position(), segment->junction(1)->position());
    } 

	std::copy(other->m_pins.begin(), other->m_pins.end(), std::back_inserter(m_pins));
}

void ModelWire::split_at_new_junction(const Point &p) {
    // nothing to do if there is already a junction at the specified point
    if (point_is_junction(p)) {
        return;
    }

    // find segment containing the point
    auto found = std::find_if(m_segments.begin(), m_segments.end(), 
                                [p](const auto &s) {return s->point_on_segment(p);});
    if (found == m_segments.end()) {
        return;
    }
    auto segment = found->get();
	
	auto p_end = segment->junction(1)->position();
	segment->junction(1)->remove_segment(segment);
	segment->set_junction(1, create_new_junction(p, segment));
	add_segment(p, p_end);
}

void ModelWire::move(const Point& delta) {
	for (auto& junction : m_junctions) {
		junction->move(delta);
	}
}

void ModelWire::simplify() {
    // duplicate junctions shouldn't happen because of the way wires are merged. Check not implemented.

    // junctions with just two segments should be removed when the segments are colinear
    std::vector<ModelWireJunction *> to_check;

    for (auto &junction : m_junctions) {
        if (junction->num_segments() == 2) {
            to_check.push_back(junction.get());
        }
    }

    while (!to_check.empty()) {
        auto junction = to_check.back();
        to_check.pop_back();

        ModelWireSegment *segments[2] = {junction->segment(0), junction->segment(1)};

        Point points[3] = {junction->position()};
        points[1] = segments[0]->junction(segments[0]->junction(0) == junction ? 1 : 0)->position();
        points[2] = segments[1]->junction(segments[1]->junction(0) == junction ? 1 : 0)->position();

        if (points_colinear(points[0], points[1], points[2])) {
            add_segment(points[1], points[2]);
            remove_redundant_segment(segments[0]);
            remove_redundant_segment(segments[1]);
        }
    }
}

bool ModelWire::point_is_junction(const Point &p) const {
    return std::any_of(m_junctions.begin(), m_junctions.end(),
                         [p](const auto &j) {return j->position() == p;});
}

bool ModelWire::point_on_wire(const Point &p) const {
    return std::any_of(m_segments.begin(), m_segments.end(),
                         [p](const auto &s) {return s->point_on_segment(p);});
}

ModelWireSegment *ModelWire::segment_at_point(const Point &p) const {
    for (const auto &segment : m_segments) {
        if (segment->point_on_segment(p)) {
            return segment.get();
        }
    }

    return nullptr;
}

void ModelWire::remove_segment(ModelWireSegment *segment) {
    remove_redundant_segment(segment);
}

ModelWire::segment_set_t ModelWire::reachable_segments(ModelWireSegment *from_segment) const {
    std::set<ModelWireSegment *> reachable;
    std::vector<ModelWireSegment *> to_check({from_segment});

    while (!to_check.empty()) {
        auto segment = to_check.back();
        to_check.pop_back();
        reachable.insert(segment);

        for (size_t j = 0; j < 2; ++j) {
            auto junction = segment->junction(j);

            for (size_t s = 0; s < junction->num_segments(); ++s) {
                if (reachable.find(junction->segment(s)) == reachable.end()) {
                    to_check.push_back(junction->segment(s));
                }
            }
        }
    }

    return reachable;
}

bool ModelWire::in_one_piece() const {
// check if the wire isn't split into multiple pieces
    if (!m_segments.empty()) {
       return reachable_segments(m_segments.front().get()).size() == m_segments.size();
    } 
    return false;
}

void ModelWire::remove_junction(ModelWireJunction *junction) {
    assert(junction);
	remove_owner(m_junctions, junction);
}

void ModelWire::remove_segment_from_junction(ModelWireJunction *junction, ModelWireSegment *segment) {
    assert(junction);
    assert(segment);
    junction->remove_segment(segment);
    if (junction->num_segments() == 0) {
        remove_junction(junction);
    }
}

void ModelWire::remove_redundant_segment(ModelWireSegment *segment) {
    assert(segment);
    remove_segment_from_junction(segment->junction(0), segment);
    remove_segment_from_junction(segment->junction(1), segment);
	remove_owner(m_segments, segment);
}



} // namespace lsim