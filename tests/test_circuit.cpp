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

TEST_CASE("Test nested circuits", "[circuit]") {

    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    // create a simple sub circuit
    auto circuit_1 = sim->create_circuit();
    REQUIRE(circuit_1);

    auto xor_gate = circuit_1->create_component<XorGate>();
    auto and_gate = circuit_1->create_component<AndGate>(2);
    auto const_1  = circuit_1->create_component<Constant>(VALUE_TRUE);
    auto in_1 = circuit_1->create_component<Connector>("in", 1);
    auto out_1 = circuit_1->create_component<Connector>("out", 1);

    circuit_1->connect_pins(in_1->pin(0), xor_gate->pin(0));
    circuit_1->connect_pins(const_1->pin(0), xor_gate->pin(1));
    circuit_1->connect_pins(xor_gate->pin(2), and_gate->pin(0));
    circuit_1->connect_pins(const_1->pin(0), and_gate->pin(1));
    circuit_1->connect_pins(and_gate->pin(2), out_1->pin(0));

    // create a circuit to hold circuit_1
    auto circuit_2 = sim->create_circuit();
    REQUIRE (circuit_2);

    auto s_in = circuit_2->create_component<Connector>("s_in", 1);
    auto s_out = circuit_2->create_component<Connector>("s_out", 1);
    auto sub = circuit_2->integrate_circuit(circuit_1);

    circuit_2->connect_pins(s_in->pin(0), sub->interface_pin_by_name("in"));
    circuit_2->connect_pins(s_out->pin(0), sub->interface_pin_by_name("out"));

    sim->set_main_circuit(circuit_2);
    sim->init();
    s_in->change_data(VALUE_TRUE);
    sim->run_until_stable(5);
    REQUIRE(circuit_2->read_value(s_out->pin(0)) == VALUE_FALSE);
}