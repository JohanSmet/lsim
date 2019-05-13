// visual_component.cpp - Johan Smet - BSD-3-Clause (see LICENSE)

#include "visual_component.h"
#include "basic.h"
#include "circuit.h"

VisualComponent::VisualComponent(Component *component) :
    m_orientation(EAST),
    m_position({0.0f, 0.0f}),
    m_component(component),
    m_circuit(nullptr) {
    m_type = component->type();
}

VisualComponent::VisualComponent(Circuit *circuit) :
    m_type(COMPONENT_SUB_CIRCUIT),
    m_orientation(EAST),
    m_position({0.0f, 0.0f}),
    m_component(nullptr),
    m_circuit(circuit) {
}

void VisualComponent::set_orientation(VisualComponent::Orientation orientation) {
    m_orientation = orientation;
}

void VisualComponent::set_position(VisualComponent::point_t pos) {
    m_position = pos;
}

Component::pin_container_t VisualComponent::pins() const {
    if (m_component) {
        return m_component->pins();
    } else {
        return m_circuit->ports_pins();
    }
}