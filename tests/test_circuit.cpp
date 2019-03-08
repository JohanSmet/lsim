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

struct Adder1bitIO {
    Circuit *circuit;
    Connector *pin_Ci;
    Connector *pin_A;
    Connector *pin_B;
    Connector *pin_O;
    Connector *pin_Co;
};

Adder1bitIO create_1bit_adder(Simulator *sim) {
    Adder1bitIO result = {0};

    result.circuit = sim->create_circuit();

    result.pin_Ci = result.circuit->create_component<Connector>("Ci", 1);
    result.pin_A  = result.circuit->create_component<Connector>("A", 1);
    result.pin_B  = result.circuit->create_component<Connector>("B", 1);
    result.pin_O  = result.circuit->create_component<Connector>("O", 1);
    result.pin_Co = result.circuit->create_component<Connector>("Co", 1);

    auto xor_1 = result.circuit->create_component<XorGate>();
    result.circuit->connect_pins(result.pin_A->pin(0), xor_1->pin(0));
    result.circuit->connect_pins(result.pin_B->pin(0), xor_1->pin(1));

    auto xor_2 = result.circuit->create_component<XorGate>();
    result.circuit->connect_pins(result.pin_Ci->pin(0), xor_2->pin(0));
    result.circuit->connect_pins(xor_1->pin(2), xor_2->pin(1));
    result.circuit->connect_pins(xor_2->pin(2), result.pin_O->pin(0));

    auto and_1 = result.circuit->create_component<AndGate>(2);
    result.circuit->connect_pins(xor_1->pin(2), and_1->pin(0));
    result.circuit->connect_pins(result.pin_Ci->pin(0), and_1->pin(1));

    auto and_2 = result.circuit->create_component<AndGate>(2);
    result.circuit->connect_pins(result.pin_A->pin(0), and_2->pin(0));
    result.circuit->connect_pins(result.pin_B->pin(0), and_2->pin(1));

    auto or_1 = result.circuit->create_component<OrGate>(2);
    result.circuit->connect_pins(and_1->pin(2), or_1->pin(0));
    result.circuit->connect_pins(and_2->pin(2), or_1->pin(1));
    result.circuit->connect_pins(or_1->pin(2), result.pin_Co->pin(0));

    return result;
}

TEST_CASE("1bit Adder", "[circuit]") {

    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    auto adder_1bit = create_1bit_adder(sim.get());

    sim->set_main_circuit(adder_1bit.circuit);
    sim->init();

    Value truth_table[][5] = {
        //  Ci           A            B            Co          O
        {VALUE_FALSE, VALUE_FALSE, VALUE_FALSE, VALUE_FALSE, VALUE_FALSE},
        {VALUE_FALSE, VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE, VALUE_TRUE},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE},

        {VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE, VALUE_FALSE, VALUE_TRUE},
        {VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE}
    };

    size_t num_tests = sizeof(truth_table) / sizeof(truth_table[0]);

    sim->init();

    for (auto test_idx = 0u; test_idx < num_tests; ++test_idx) {
        adder_1bit.pin_Ci->change_data(truth_table[test_idx][0]);
        adder_1bit.pin_A->change_data(truth_table[test_idx][1]);
        adder_1bit.pin_B->change_data(truth_table[test_idx][2]);
        sim->run_until_stable(5);
        REQUIRE(adder_1bit.circuit->read_value(adder_1bit.pin_Co->pin(0)) == truth_table[test_idx][3]);
        REQUIRE(adder_1bit.circuit->read_value(adder_1bit.pin_O->pin(0)) == truth_table[test_idx][4]);
    }
}

TEST_CASE("4bit adder (circuit cloning)", "[circuit]") {

    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    // create 1bit adder circuit
    auto adder_1bit = create_1bit_adder(sim.get());

    // create a 4bit adder circuit
    auto adder_4bit = sim->create_circuit();

    auto s_pin_Ci = adder_4bit->create_component<Connector>("Ci", 1);
    auto s_pin_A = adder_4bit->create_component<Connector>("A", 4);
    auto s_pin_B = adder_4bit->create_component<Connector>("B", 4);
    auto s_pin_O = adder_4bit->create_component<Connector>("O", 4);
    auto s_pin_Co = adder_4bit->create_component<Connector>("Co", 1);

    auto add1_0 = adder_4bit->integrate_circuit(adder_1bit.circuit);
    adder_4bit->connect_pins(s_pin_Ci->pin(0), add1_0->interface_pin_by_name("Ci"));
    adder_4bit->connect_pins(s_pin_A->pin(0), add1_0->interface_pin_by_name("A"));
    adder_4bit->connect_pins(s_pin_B->pin(0), add1_0->interface_pin_by_name("B"));
    adder_4bit->connect_pins(s_pin_O->pin(0), add1_0->interface_pin_by_name("O"));

    auto add1_1 = adder_4bit->integrate_circuit(sim->clone_circuit(adder_1bit.circuit));
    adder_4bit->connect_pins(add1_0->interface_pin_by_name("Co"), add1_1->interface_pin_by_name("Ci"));
    adder_4bit->connect_pins(s_pin_A->pin(1), add1_1->interface_pin_by_name("A"));
    adder_4bit->connect_pins(s_pin_B->pin(1), add1_1->interface_pin_by_name("B"));
    adder_4bit->connect_pins(s_pin_O->pin(1), add1_1->interface_pin_by_name("O"));

    auto add1_2 = adder_4bit->integrate_circuit(sim->clone_circuit(adder_1bit.circuit));
    adder_4bit->connect_pins(add1_1->interface_pin_by_name("Co"), add1_2->interface_pin_by_name("Ci"));
    adder_4bit->connect_pins(s_pin_A->pin(2), add1_2->interface_pin_by_name("A"));
    adder_4bit->connect_pins(s_pin_B->pin(2), add1_2->interface_pin_by_name("B"));
    adder_4bit->connect_pins(s_pin_O->pin(2), add1_2->interface_pin_by_name("O"));

    auto add1_3 = adder_4bit->integrate_circuit(sim->clone_circuit(adder_1bit.circuit));
    adder_4bit->connect_pins(add1_2->interface_pin_by_name("Co"), add1_3->interface_pin_by_name("Ci"));
    adder_4bit->connect_pins(s_pin_A->pin(3), add1_3->interface_pin_by_name("A"));
    adder_4bit->connect_pins(s_pin_B->pin(3), add1_3->interface_pin_by_name("B"));
    adder_4bit->connect_pins(s_pin_O->pin(3), add1_3->interface_pin_by_name("O"));
    adder_4bit->connect_pins(add1_3->interface_pin_by_name("Co"), s_pin_Co->pin(0));

    sim->set_main_circuit(adder_4bit);

    for (int ci = 0; ci < 2; ++ci) {
        s_pin_Ci->change_data(ci);

        for (int a = 0; a < 16; ++a) {
            for (int b = 0; b < 16; ++b) {
                int req_O = (a + b + ci) & 0xf;
                int req_Co = ((a + b + ci) >> 4) & 0x1;

                s_pin_A->change_data(a);
                s_pin_B->change_data(b);
                sim->run_until_stable(5);

                int val = (int) adder_4bit->read_value(s_pin_O->pin(0));  
                val |= ((int) adder_4bit->read_value(s_pin_O->pin(1))) << 1;
                val |= ((int) adder_4bit->read_value(s_pin_O->pin(2))) << 2;
                val |= ((int) adder_4bit->read_value(s_pin_O->pin(3))) << 3;

                REQUIRE(val == req_O);
                REQUIRE(sim->read_pin(s_pin_Co->pin(0)) == req_Co);
            }
        }
    }
}