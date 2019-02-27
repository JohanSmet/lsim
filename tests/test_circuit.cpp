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
}

