// visual_component.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "visual_component.h"
#include "basic.h"
#include "circuit.h"

#include <cassert>

VisualComponent::VisualComponent(Component *component) :
    m_orientation(EAST),
    m_position(0.0f, 0.0f),
    m_component(component),
    m_circuit(nullptr) {
    m_type = component->type();
}

VisualComponent::VisualComponent(Circuit *circuit) :
    m_type(COMPONENT_SUB_CIRCUIT),
    m_orientation(EAST),
    m_position(0.0f, 0.0f),
    m_component(nullptr),
    m_circuit(circuit) {
}

void VisualComponent::set_orientation(VisualComponent::Orientation orientation) {
    m_orientation = orientation;
}

void VisualComponent::set_position(Point pos) {
    m_position = pos;
}

Component::pin_container_t VisualComponent::pins() const {
    if (m_component) {
        return m_component->pins();
    } else {
        return m_circuit->ports_pins();
    }
}

Wire::Wire() {

}

Wire::Wire(Point *anchors, size_t num_anchors) {
    assert(num_anchors > 1);

    auto p0 = anchors[0];
    segment_t prev_segment = {Point(-1000.0f, -1000.0f), Point(-1000.0f, -1000.0f)};

    for (size_t idx = 1; idx < num_anchors; ++idx) {
        auto p1 = anchors[idx];

        // merge colinear segments
        if ((p1.x == prev_segment[0].x && p1.x == prev_segment[1].x) ||
            (p1.y == prev_segment[0].y && p1.y == prev_segment[1].y)) {
            m_segments.back()[1] = p1;
            prev_segment = m_segments.back();
        } else if (p1 != p0) {
            m_segments.push_back({p0, p1});
            prev_segment = m_segments.back();
        }
        p0 = p1;
    }
}

void Wire::add_segment(const Point &p0, const Point &p1) {
    m_segments.push_back({p0, p1});
}