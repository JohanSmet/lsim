#include "catch.hpp"
#include "circuit.h"
#include "gate.h"

TEST_CASE("Components are created correctly", "[circuit]") {
    auto circuit = std::make_unique<Circuit>();
    REQUIRE(circuit);

    auto and_gate = circuit->create_component<AndGate>(2);
    REQUIRE(and_gate);

    auto constant = circuit->create_component<Constant>(1, VALUE_TRUE);
    REQUIRE(constant);

    auto connector = circuit->create_component<Connector>(1);
    REQUIRE(connector);

    circuit->connect_pins(constant->pin(0), and_gate->pin(0));
    circuit->connect_pins(constant->pin(0), and_gate->pin(1));
    circuit->connect_pins(and_gate->pin(2), connector->pin(0));

    circuit->simulation_init();
    circuit->simulation_tick();
    
    REQUIRE(circuit->read_value(connector->pin(0)) == VALUE_TRUE);
}

