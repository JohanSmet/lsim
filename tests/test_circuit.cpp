#include "catch.hpp"
#include "circuit.h"
#include "simulator.h"
#include "gate.h"

TEST_CASE("Components are created correctly", "[circuit]") {

    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    auto circuit = sim->create_circuit();
    REQUIRE(circuit);

    auto and_gate = circuit->create_component<AndGate>(2);
    REQUIRE(and_gate);

    auto constant = circuit->create_component<Constant>(VALUE_TRUE);
    REQUIRE(constant);

    auto connector = circuit->create_component<Connector>("Test", 1);
    REQUIRE(connector);
}

