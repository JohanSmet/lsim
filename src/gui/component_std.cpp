// component_std.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// registration of standard components

#include "component_std.h"
#include "component_ui.h"
#include "shapes.h"

#include "basic.h"

#include <algorithm>

void materialize_gate(UIComponent *ui_comp, UICircuit *ui_circuit, 
                      Component::pin_container_t pins_input,
                      Component::pin_container_t pins_output,
                      Component::pin_container_t pins_control) {

    // size of component box depends on pin count
    const float width = std::max(80ul, 15 * pins_control.size());
    const float height = std::max(60ul, 15 * std::max(pins_input.size(), pins_output.size()));
    const float h_width = width / 2.0f;
    const float h_height = height / 2.0f;

    // transform coordinates from component space to circuit space
	ui_comp->m_to_circuit.reset();
	ui_comp->m_to_circuit.rotate(ui_comp->m_visual_comp->get_orientation());
	ui_comp->m_to_circuit.translate(ui_comp->m_visual_comp->get_position());

    // compute axis aligned bounding box
	ui_comp->m_circuit_min = ui_comp->m_to_circuit.apply({-h_width, -h_height});
	ui_comp->m_circuit_max = ui_comp->m_to_circuit.apply({h_width, h_height});

	if (ui_comp->m_circuit_max.x < ui_comp->m_circuit_min.x) {
        std::swap(ui_comp->m_circuit_min.x, ui_comp->m_circuit_max.x);
	}

	if (ui_comp->m_circuit_max.y < ui_comp->m_circuit_min.y) {
        std::swap(ui_comp->m_circuit_min.y, ui_comp->m_circuit_max.y);
	}

    // pins
    ui_circuit->add_pin_line(ui_comp->m_to_circuit, pins_input.data(), pins_input.size(), 
                             height, {-h_width, -h_height}, {0, 1});
    ui_circuit->add_pin_line(ui_comp->m_to_circuit, pins_output.data(), pins_output.size(), 
                             height, {h_width, -h_height}, {0, 1});
    ui_circuit->add_pin_line(ui_comp->m_to_circuit, pins_control.data(), pins_control.size(), 
                             width, {-h_width, -h_height}, {1, 0});
}

void component_register_basic() {

    // connector

    // constant

}

void component_register_gates() {

    // buffer
    ComponentIcon icon_buffer(SHAPE_BUFFER, sizeof(SHAPE_BUFFER));

    UICircuitBuilder::register_materialize_func(
        VisualComponent::BUFFER, [=](Component *comp, UIComponent *ui_comp, UICircuit *ui_circuit) {
            ui_comp->m_tooltip = "Buffer";
            ui_comp->m_icon = &icon_buffer;
            int bits = comp->num_pins() / 2;
            materialize_gate(ui_comp, ui_circuit, comp->pins(0, bits-1), comp->pins(bits, bits*2-1), {});
        }
    );

}