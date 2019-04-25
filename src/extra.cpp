// extra.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// various extra components

///////////////////////////////////////////////////////////////////////////////
//
// Pull Resistor - pull undefined nodes towards a value
//

#include "extra.h"
#include "circuit.h"
#include "simulator.h"

PullResistor::PullResistor(Value pull_to) : 
                CloneComponent(1, VisualComponent::PULL_RESISTOR, PRIORITY_DEFERRED),
                m_pull_to(pull_to) {
}

bool PullResistor::is_dirty() const {
    return true;
}

void PullResistor::process() {
    Simulator *sim = get_circuit()->sim();

    if (sim->read_pin_current_step(pin(0)) == VALUE_UNDEFINED) {
        sim->write_pin(pin(0), m_pull_to);
    }
}