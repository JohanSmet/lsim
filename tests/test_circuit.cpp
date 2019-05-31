#include "catch.hpp"
#include "lsim_context.h"

#if 0

TEST_CASE("Components are created correctly", "[circuit]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("main");
    REQUIRE(circuit);

    auto and_gate = AndGate(circuit, 2);
    REQUIRE(and_gate);

    auto constant = Constant(circuit, VALUE_TRUE);
    REQUIRE(constant);

    auto connector = ConnectorInput(circuit, "test", 1);
    REQUIRE(connector);
}

TEST_CASE("Test nested circuits", "[circuit]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    // create a simple sub circuit
    auto circuit_1 = lsim_context.user_library()->create_circuit("circuit_1");
    REQUIRE(circuit_1);

    auto xor_gate = XorGate(circuit_1);
    auto and_gate = AndGate(circuit_1, 2);
    auto const_1  = Constant(circuit_1, VALUE_TRUE);
    auto in_1 = ConnectorInput(circuit_1, "in", 1);
    auto out_1 = ConnectorOutput(circuit_1, "out", 1);

    circuit_1->connect_pins(in_1->pin(0), xor_gate->pin(0));
    circuit_1->connect_pins(const_1->pin(0), xor_gate->pin(1));
    circuit_1->connect_pins(xor_gate->pin(2), and_gate->pin(0));
    circuit_1->connect_pins(const_1->pin(0), and_gate->pin(1));
    circuit_1->connect_pins(and_gate->pin(2), out_1->pin(0));

    // create a circuit to hold circuit_1
    auto circuit_2 = lsim_context.user_library()->create_circuit("circuit_2");
    REQUIRE (circuit_2);

    auto s_in = ConnectorInput(circuit_2, "s_in", 1);
    auto s_out = ConnectorOutput(circuit_2, "s_out", 1);
    auto sub = circuit_2->integrate_circuit_clone(circuit_1);

    circuit_2->connect_pins(s_in->pin(0), sub->pin_port_by_name("in"));
    circuit_2->connect_pins(s_out->pin(0), sub->pin_port_by_name("out"));

    sim->init(circuit_2);
    s_in->write_pin(0, VALUE_TRUE);
    sim->run_until_stable(5);
    REQUIRE(circuit_2->read_value(s_out->pin(0)) == VALUE_FALSE);
}

struct AdderIO {
    Circuit *circuit;
    Component *pin_Ci;
    Component *pin_A;
    Component *pin_B;
    Component *pin_O;
    Component *pin_Co;
};

AdderIO create_1bit_adder(LSimContext *lsim_context) {
    AdderIO result = {0};

    result.circuit = lsim_context->user_library()->create_circuit("adder_1bit");

    result.pin_Ci = ConnectorInput(result.circuit, "Ci", 1);
    result.pin_A  = ConnectorInput(result.circuit, "A", 1);
    result.pin_B  = ConnectorInput(result.circuit, "B", 1);
    result.pin_O  = ConnectorInput(result.circuit, "O", 1);
    result.pin_Co = ConnectorOutput(result.circuit, "Co", 1);

    auto xor_1 = XorGate(result.circuit);
    result.circuit->connect_pins(result.pin_A->pin(0), xor_1->pin(0));
    result.circuit->connect_pins(result.pin_B->pin(0), xor_1->pin(1));

    auto xor_2 = XorGate(result.circuit);
    result.circuit->connect_pins(result.pin_Ci->pin(0), xor_2->pin(0));
    result.circuit->connect_pins(xor_1->pin(2), xor_2->pin(1));
    result.circuit->connect_pins(xor_2->pin(2), result.pin_O->pin(0));

    auto and_1 = AndGate(result.circuit, 2);
    result.circuit->connect_pins(xor_1->pin(2), and_1->pin(0));
    result.circuit->connect_pins(result.pin_Ci->pin(0), and_1->pin(1));

    auto and_2 = AndGate(result.circuit, 2);
    result.circuit->connect_pins(result.pin_A->pin(0), and_2->pin(0));
    result.circuit->connect_pins(result.pin_B->pin(0), and_2->pin(1));

    auto or_1 = OrGate(result.circuit, 2);
    result.circuit->connect_pins(and_1->pin(2), or_1->pin(0));
    result.circuit->connect_pins(and_2->pin(2), or_1->pin(1));
    result.circuit->connect_pins(or_1->pin(2), result.pin_Co->pin(0));

    return result;
}

TEST_CASE("1bit Adder", "[circuit]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto adder_1bit = create_1bit_adder(&lsim_context);
    sim->init(adder_1bit.circuit);

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

    for (auto test_idx = 0u; test_idx < num_tests; ++test_idx) {
        adder_1bit.pin_Ci->write_pin(0, truth_table[test_idx][0]);
        adder_1bit.pin_A->write_pin(0, truth_table[test_idx][1]);
        adder_1bit.pin_B->write_pin(0, truth_table[test_idx][2]);
        sim->run_until_stable(5);
        REQUIRE(adder_1bit.circuit->read_value(adder_1bit.pin_Co->pin(0)) == truth_table[test_idx][3]);
        REQUIRE(adder_1bit.circuit->read_value(adder_1bit.pin_O->pin(0)) == truth_table[test_idx][4]);
    }
}

AdderIO create_4bit_adder(LSimContext *lsim_context) {
    AdderIO result = {0};

    // create 1bit adder circuit
    auto adder_1bit = create_1bit_adder(lsim_context);

    // create a 4bit adder circuit
    result.circuit = lsim_context->user_library()->create_circuit("adder_4bit");

    result.pin_Ci = ConnectorInput(result.circuit, "Ci", 1);
    result.pin_A  = ConnectorInput(result.circuit, "A", 4);
    result.pin_B  = ConnectorInput(result.circuit, "B", 4);
    result.pin_O  = ConnectorInput(result.circuit, "O", 4);
    result.pin_Co = ConnectorOutput(result.circuit, "Co", 1);

    auto add1_0 = result.circuit->integrate_circuit_clone(adder_1bit.circuit);
    result.circuit->connect_pins(result.pin_Ci->pin(0), add1_0->pin_port_by_name("Ci"));
    result.circuit->connect_pins(result.pin_A->pin(0), add1_0->pin_port_by_name("A"));
    result.circuit->connect_pins(result.pin_B->pin(0), add1_0->pin_port_by_name("B"));
    result.circuit->connect_pins(result.pin_O->pin(0), add1_0->pin_port_by_name("O"));

    auto add1_1 = result.circuit->integrate_circuit_clone(adder_1bit.circuit);
    result.circuit->connect_pins(add1_0->pin_port_by_name("Co"), add1_1->pin_port_by_name("Ci"));
    result.circuit->connect_pins(result.pin_A->pin(1), add1_1->pin_port_by_name("A"));
    result.circuit->connect_pins(result.pin_B->pin(1), add1_1->pin_port_by_name("B"));
    result.circuit->connect_pins(result.pin_O->pin(1), add1_1->pin_port_by_name("O"));

    auto add1_2 = result.circuit->integrate_circuit_clone(adder_1bit.circuit);
    result.circuit->connect_pins(add1_1->pin_port_by_name("Co"), add1_2->pin_port_by_name("Ci"));
    result.circuit->connect_pins(result.pin_A->pin(2), add1_2->pin_port_by_name("A"));
    result.circuit->connect_pins(result.pin_B->pin(2), add1_2->pin_port_by_name("B"));
    result.circuit->connect_pins(result.pin_O->pin(2), add1_2->pin_port_by_name("O"));

    auto add1_3 = result.circuit->integrate_circuit_clone(adder_1bit.circuit);
    result.circuit->connect_pins(add1_2->pin_port_by_name("Co"), add1_3->pin_port_by_name("Ci"));
    result.circuit->connect_pins(result.pin_A->pin(3), add1_3->pin_port_by_name("A"));
    result.circuit->connect_pins(result.pin_B->pin(3), add1_3->pin_port_by_name("B"));
    result.circuit->connect_pins(result.pin_O->pin(3), add1_3->pin_port_by_name("O"));
    result.circuit->connect_pins(add1_3->pin_port_by_name("Co"), result.pin_Co->pin(0));

    return result;
}

TEST_CASE("4bit adder (circuit cloning)", "[circuit]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto adder_4bit = create_4bit_adder(&lsim_context);
    sim->init(adder_4bit.circuit);

    for (int ci = 0; ci < 2; ++ci) {
        adder_4bit.pin_Ci->write_pin(0, static_cast<Value>(ci));

        for (int a = 0; a < 16; ++a) {
            for (int b = 0; b < 16; ++b) {
                int req_O = (a + b + ci) & 0xf;
                int req_Co = ((a + b + ci) >> 4) & 0x1;

                adder_4bit.pin_A->write_output_pins(a);
                adder_4bit.pin_B->write_output_pins(b);
                sim->run_until_stable(5);

                REQUIRE(sim->read_nibble(adder_4bit.pin_O->pins()) == req_O);
                REQUIRE(sim->read_pin(adder_4bit.pin_Co->pin(0)) == req_Co);
            }
        }
    }
}

TEST_CASE("8bit adder (multi-level cloning)", "[circuit]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("adder_8bit");
    auto pin_Ci = ConnectorInput(circuit, "Ci", 1);
    auto pin_A  = ConnectorInput(circuit, "A", 8);
    auto pin_B  = ConnectorInput(circuit, "B", 8);
    auto pin_O  = ConnectorInput(circuit, "O", 8);
    auto pin_Co = ConnectorOutput(circuit, "Co", 1);

    auto adder_4bit = create_4bit_adder(&lsim_context);

    auto add4_0 = circuit->integrate_circuit_clone(adder_4bit.circuit);
    circuit->connect_pins(pin_Ci->pin(0), add4_0->pin_port_by_name("Ci"));
    circuit->connect_pins(pin_A->pin(0), add4_0->pin_port_by_name("A[0]"));
    circuit->connect_pins(pin_A->pin(1), add4_0->pin_port_by_name("A[1]"));
    circuit->connect_pins(pin_A->pin(2), add4_0->pin_port_by_name("A[2]"));
    circuit->connect_pins(pin_A->pin(3), add4_0->pin_port_by_name("A[3]"));
    circuit->connect_pins(pin_B->pin(0), add4_0->pin_port_by_name("B[0]"));
    circuit->connect_pins(pin_B->pin(1), add4_0->pin_port_by_name("B[1]"));
    circuit->connect_pins(pin_B->pin(2), add4_0->pin_port_by_name("B[2]"));
    circuit->connect_pins(pin_B->pin(3), add4_0->pin_port_by_name("B[3]"));
    circuit->connect_pins(pin_O->pin(0), add4_0->pin_port_by_name("O[0]"));
    circuit->connect_pins(pin_O->pin(1), add4_0->pin_port_by_name("O[1]"));
    circuit->connect_pins(pin_O->pin(2), add4_0->pin_port_by_name("O[2]"));
    circuit->connect_pins(pin_O->pin(3), add4_0->pin_port_by_name("O[3]"));

    auto add4_1 = circuit->integrate_circuit_clone(adder_4bit.circuit);
    circuit->connect_pins(add4_0->pin_port_by_name("Co"), add4_1->pin_port_by_name("Ci"));
    circuit->connect_pins(pin_A->pin(4), add4_1->pin_port_by_name("A[0]"));
    circuit->connect_pins(pin_A->pin(5), add4_1->pin_port_by_name("A[1]"));
    circuit->connect_pins(pin_A->pin(6), add4_1->pin_port_by_name("A[2]"));
    circuit->connect_pins(pin_A->pin(7), add4_1->pin_port_by_name("A[3]"));
    circuit->connect_pins(pin_B->pin(4), add4_1->pin_port_by_name("B[0]"));
    circuit->connect_pins(pin_B->pin(5), add4_1->pin_port_by_name("B[1]"));
    circuit->connect_pins(pin_B->pin(6), add4_1->pin_port_by_name("B[2]"));
    circuit->connect_pins(pin_B->pin(7), add4_1->pin_port_by_name("B[3]"));
    circuit->connect_pins(pin_O->pin(4), add4_1->pin_port_by_name("O[0]"));
    circuit->connect_pins(pin_O->pin(5), add4_1->pin_port_by_name("O[1]"));
    circuit->connect_pins(pin_O->pin(6), add4_1->pin_port_by_name("O[2]"));
    circuit->connect_pins(pin_O->pin(7), add4_1->pin_port_by_name("O[3]"));
    circuit->connect_pins(add4_1->pin_port_by_name("Co"), pin_Co->pin(0)); 

    sim->init(circuit);

    for (int ci = 0; ci < 2; ++ci) {
        pin_Ci->write_output_pins(ci);

        for (int a = 0; a < 256; a += 12) {
            for (int b = 0; b < 256; b += 9) {
                int req_O = (a + b + ci) & 0xff;
                int req_Co = ((a + b + ci) >> 8) & 0x1;

                pin_A->write_output_pins(a);
                pin_B->write_output_pins(b);
                sim->run_until_stable(5);

                REQUIRE(sim->read_byte(pin_O->pins()) == req_O);
                REQUIRE(sim->read_pin(pin_Co->pin(0)) == req_Co);
            }
        }
    }
}

#endif 