// wire_description.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// describe the wires that connect components

#include "wire_description.h"
#include "circuit_description.h"

#include <cassert>
#include <algorithm>

namespace lsim {

///////////////////////////////////////////////////////////////////////////////
//
// WireJunction
//

WireJunction::WireJunction(const Point &p, WireSegment *segment) :
        m_position(p) {
    m_segments.push_back(segment);
}

void WireJunction::add_segment(WireSegment *segment) {
    m_segments.push_back(segment);
}

void WireJunction::remove_segment(WireSegment *segment) {
    m_segments.erase(std::remove(m_segments.begin(), m_segments.end(), segment));
}

WireSegment *WireJunction::segment(size_t idx) const {
    assert(idx < m_segments.size());
    return m_segments[idx];
}

///////////////////////////////////////////////////////////////////////////////
//
// WireSegment
//

WireSegment::WireSegment(Wire *wire) : 
        m_wire(wire),
        m_ends({nullptr, nullptr}) {
}

void WireSegment::set_junction(size_t idx, WireJunction *junction) {
    assert(idx < 2);
    m_ends[idx] = junction;
}

WireJunction *WireSegment::junction(size_t idx) const {
    assert(idx < 2);
    return m_ends[idx];
}

bool WireSegment::point_on_segment(const Point &p) {
    return point_on_line_segment(m_ends[0]->position(), m_ends[1]->position(), p);
}

///////////////////////////////////////////////////////////////////////////////
//
// Wire
//

Wire::Wire(uint32_t id) : m_id(id) {

}

void Wire::add_pin(pin_id_t pin) {
    m_pins.push_back(pin);
}

pin_id_t Wire::pin(size_t index) const {
    assert(index < m_pins.size());
    return m_pins[index];
}

const Point &Wire::segment_point(size_t segment_idx, size_t point_idx) const {
    assert(segment_idx < m_segments.size());
    assert(point_idx < 2);
    return m_segments[segment_idx]->junction(point_idx)->position();
}

WireSegment *Wire::segment_by_index(size_t segment_idx) {
    assert(segment_idx < m_segments.size());
    return m_segments[segment_idx].get();
}

size_t Wire::junction_segment_count(size_t idx) const {
    assert(idx < m_junctions.size());
    return m_junctions[idx]->num_segments();
}

const Point &Wire::junction_position(size_t idx) const {
    assert(idx < m_junctions.size());
    return m_junctions[idx]->position();
}

WireJunction *Wire::add_junction(const Point &p, WireSegment *segment) {
    auto found = std::find_if(m_junctions.begin(), m_junctions.end(), [=](auto &j) {return j->position() == p;});
    if (found != m_junctions.end()) {
        (*found)->add_segment(segment);
        return found->get();
    }

    m_junctions.push_back(std::make_unique<WireJunction>(p, segment));
    return m_junctions.back().get();
}

WireSegment *Wire::add_segment(const Point &p0, const Point &p1) {
    m_segments.push_back(std::make_unique<WireSegment>(this));
    auto segment = m_segments.back().get();

    segment->set_junction(0, add_junction(p0, segment));
    segment->set_junction(1, add_junction(p1, segment));
    return segment;
}

void Wire::add_segments(Point *anchors, size_t num_anchors) {
    for(size_t a = 1;a < num_anchors; ++a) {
        if (anchors[a - 1] != anchors[a]) {
            add_segment(anchors[a - 1], anchors[a]);
        }
    }
}

void Wire::merge(Wire *other) {
    for (const auto &segment : other->m_segments) {
        add_segment(segment->junction(0)->position(), segment->junction(1)->position());
    } 

    std::copy(other->m_pins.begin(), other->m_pins.end(), m_pins.end());
}

void Wire::split_at_new_junction(const Point &p) {
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

    add_segment(segment->junction(0)->position(), p);
    add_segment(p, segment->junction(1)->position());
    remove_redundant_segment(segment);
}

void Wire::simplify() {
    // checking for duplicate junctions isn't necessary until junctions can be moved by the user

    // junctions with just two segments should be removed when the segments are colinear
    std::vector<WireJunction *> to_check;

    for (auto &junction : m_junctions) {
        if (junction->num_segments() == 2) {
            to_check.push_back(junction.get());
        }
    }

    while (!to_check.empty()) {
        auto junction = to_check.back();
        to_check.pop_back();

        WireSegment *segments[2] = {junction->segment(0), junction->segment(1)};

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

bool Wire::point_is_junction(const Point &p) const {
    return std::any_of(m_junctions.begin(), m_junctions.end(),
                         [p](const auto &j) {return j->position() == p;});
}

bool Wire::point_on_wire(const Point &p) const {
    return std::any_of(m_segments.begin(), m_segments.end(),
                         [p](const auto &s) {return s->point_on_segment(p);});
}

WireSegment *Wire::segment_at_point(const Point &p) const {
    for (const auto &segment : m_segments) {
        if (segment->point_on_segment(p)) {
            return segment.get();
        }
    }

    return nullptr;
}

void Wire::remove_segment(WireSegment *segment) {
    remove_redundant_segment(segment);
}

Wire::segment_set_t Wire::reachable_segments(WireSegment *from_segment) const {
    std::set<WireSegment *> reachable;
    std::vector<WireSegment *> to_check({from_segment});

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

bool Wire::in_one_piece() const {
// check if the wire isn't split into multiple pieces
    if (!m_segments.empty()) {
       return reachable_segments(m_segments.front().get()).size() == m_segments.size();
    } else {
        return false;
    }
}

void Wire::remove_junction(WireJunction *junction) {
    assert(junction);
    m_junctions.erase(std::remove_if(m_junctions.begin(), m_junctions.end(), 
                        [junction](const auto &j) {return j.get() == junction;}
    ));
}

void Wire::remove_segment_from_junction(WireJunction *junction, WireSegment *segment) {
    assert(junction);
    assert(segment);
    junction->remove_segment(segment);
    if (junction->num_segments() == 0) {
        remove_junction(junction);
    }
}

void Wire::remove_redundant_segment(WireSegment *segment) {
    assert(segment);
    remove_segment_from_junction(segment->junction(0), segment);
    remove_segment_from_junction(segment->junction(1), segment);
    m_segments.erase(std::remove_if(m_segments.begin(), m_segments.end(),
                        [segment](const auto &s) {return s.get() == segment;}
    ));
}



} // namespace lsim