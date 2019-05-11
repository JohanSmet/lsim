// extra.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// various extra components

#include "extra.h"
#include "circuit.h"
#include "simulator.h"

///////////////////////////////////////////////////////////////////////////////
//
// Pull Resistor - pull undefined nodes towards a value
//

Component *PullResistor(Circuit *circuit, Value pull_to) {
    auto resistor = circuit->create_component(0, 1, 0, COMPONENT_PULL_RESISTOR, PRIORITY_DEFERRED);
    resistor->add_property(make_property("pull_to", static_cast<int64_t>(pull_to)));
    resistor->write_pin(0, pull_to);

    resistor->set_check_dirty_func([](Component *resistor) {
        // returning false will cause the previous value to be output again
        //  in this case we're abusing this to write the pull-value when the node is undefined
        return resistor->sim()->read_pin_current_step(resistor->pin(0)) != VALUE_UNDEFINED;
    });

    resistor->set_process_func([](Component *resistor) {});

    return resistor;
}
