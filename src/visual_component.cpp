// visual_component.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "visual_component.h"

VisualComponent::VisualComponent(Type type, Component *component) :
    m_type(type),
    m_orientation(EAST),
    m_position({0.0f, 0.0f}),
    m_component(component) {
}

void VisualComponent::set_orientation(VisualComponent::Orientation orientation) {
    m_orientation = orientation;
}

void VisualComponent::set_position(VisualComponent::point_t pos) {
    m_position = pos;
}