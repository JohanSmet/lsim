#include "catch.hpp"
#include "circuit.h"
#include "gate.h"

TEST_CASE("Components are created correctly", "[circuit]") {
    auto circuit = std::make_unique<Circuit>();
    REQUIRE(circuit);

    auto and_gate = circuit->create_component<AndGate>(circuit.get(), 2);
    REQUIRE(and_gate);

    auto constant = circuit->create_component<Constant>(circuit.get(), 1, VALUE_TRUE);
    REQUIRE(constant);

    circuit->connect_pins(constant->pin(0), and_gate->pin(0));
    circuit->connect_pins(constant->pin(0), and_gate->pin(1));
    circuit->connect_pins(and_gate->pin(2), and_gate->pin(2));

    circuit->simulation_init();
    circuit->simulation_tick();
    
    REQUIRE(circuit->read_value(and_gate->pin(2)) == VALUE_TRUE);
}

