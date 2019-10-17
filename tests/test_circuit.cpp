#include "catch.hpp"
#include "lsim_context.h"
#include "sim_circuit.h"

using namespace lsim;

TEST_CASE("Components are created correctly", "[circuit]") {

    LSimContext lsim_context;

    auto circuit_desc = lsim_context.create_user_circuit("main");
    REQUIRE(circuit_desc);

    auto and_gate = circuit_desc->add_and_gate(2);
    REQUIRE(and_gate);

    auto constant = circuit_desc->add_constant(VALUE_TRUE);
    REQUIRE(constant);

    auto connector = circuit_desc->add_connector_out("test", 1);
    REQUIRE(connector);
}

TEST_CASE("Test nested circuits", "[circuit]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    // create a simple sub circuit
    auto circuit_1_desc = lsim_context.create_user_circuit("circuit_1");
    REQUIRE(circuit_1_desc);

    auto xor_gate = circuit_1_desc->add_xor_gate();
    auto and_gate = circuit_1_desc->add_and_gate(2);
    auto const_1  = circuit_1_desc->add_constant(VALUE_TRUE);
    auto in_1 = circuit_1_desc->add_connector_in("in", 1);
    auto out_1 = circuit_1_desc->add_connector_out("out", 1);

    circuit_1_desc->connect(in_1->pin_id(0), xor_gate->pin_id(0));
    circuit_1_desc->connect(const_1->pin_id(0), xor_gate->pin_id(1));
    circuit_1_desc->connect(xor_gate->pin_id(2), and_gate->pin_id(0));
    circuit_1_desc->connect(const_1->pin_id(0), and_gate->pin_id(1));
    circuit_1_desc->connect(and_gate->pin_id(2), out_1->pin_id(0));

    // create a circuit to hold circuit_1
    auto circuit_2_desc = lsim_context.create_user_circuit("circuit_2");
    REQUIRE (circuit_2_desc);

    auto s_in = circuit_2_desc->add_connector_in("s_in", 1);
    auto s_out = circuit_2_desc->add_connector_out("s_out", 1);
    auto sub = circuit_2_desc->add_sub_circuit("circuit_1");

    circuit_2_desc->connect(s_in->pin_id(0), sub->port_by_name("in"));
    circuit_2_desc->connect(s_out->pin_id(0), sub->port_by_name("out"));

    auto circuit_2 = circuit_2_desc->instantiate(sim);
    REQUIRE(circuit_2);

    sim->init();
    circuit_2->write_pin(s_in->pin_id(0), VALUE_TRUE);
    sim->run_until_stable(5);
    REQUIRE(circuit_2->read_pin(s_out->pin_id(0)) == VALUE_FALSE);
}

struct AdderIO {
    ModelCircuit *circuit;
    ModelComponent *pin_Ci;
    ModelComponent *pin_A;
    ModelComponent *pin_B;
    ModelComponent *pin_O;
    ModelComponent *pin_Co;
};

AdderIO create_1bit_adder(LSimContext *lsim_context) {
    AdderIO result = {};

    result.circuit = lsim_context->create_user_circuit("adder_1bit");

    result.pin_Ci = result.circuit->add_connector_in("Ci", 1);
    result.pin_A  = result.circuit->add_connector_in("A", 1);
    result.pin_B  = result.circuit->add_connector_in("B", 1);
    result.pin_O  = result.circuit->add_connector_out("O", 1);
    result.pin_Co = result.circuit->add_connector_out("Co", 1);

    auto xor_1 = result.circuit->add_xor_gate();
    result.circuit->connect(result.pin_A->pin_id(0), xor_1->pin_id(0));
    result.circuit->connect(result.pin_B->pin_id(0), xor_1->pin_id(1));

    auto xor_2 = result.circuit->add_xor_gate();
    result.circuit->connect(result.pin_Ci->pin_id(0), xor_2->pin_id(0));
    result.circuit->connect(xor_1->pin_id(2), xor_2->pin_id(1));
    result.circuit->connect(xor_2->pin_id(2), result.pin_O->pin_id(0));

    auto and_1 = result.circuit->add_and_gate(2);
    result.circuit->connect(xor_1->pin_id(2), and_1->pin_id(0));
    result.circuit->connect(result.pin_Ci->pin_id(0), and_1->pin_id(1));

    auto and_2 = result.circuit->add_and_gate(2);
    result.circuit->connect(result.pin_A->pin_id(0), and_2->pin_id(0));
    result.circuit->connect(result.pin_B->pin_id(0), and_2->pin_id(1));

    auto or_1 = result.circuit->add_or_gate(2);
    result.circuit->connect(and_1->pin_id(2), or_1->pin_id(0));
    result.circuit->connect(and_2->pin_id(2), or_1->pin_id(1));
    result.circuit->connect(or_1->pin_id(2), result.pin_Co->pin_id(0));

    return result;
}

TEST_CASE("1bit Adder", "[circuit]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto adder_1bit_desc = create_1bit_adder(&lsim_context);

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

    auto circuit = adder_1bit_desc.circuit->instantiate(sim);
    REQUIRE(circuit);

    sim->init();

    for (const auto &test : truth_table) {
        circuit->write_pin(adder_1bit_desc.pin_Ci->pin_id(0), test[0]);
        circuit->write_pin(adder_1bit_desc.pin_A->pin_id(0), test[1]);
        circuit->write_pin(adder_1bit_desc.pin_B->pin_id(0), test[2]);
        sim->run_until_stable(5);
        REQUIRE(circuit->read_pin(adder_1bit_desc.pin_Co->pin_id(0)) == test[3]);
        REQUIRE(circuit->read_pin(adder_1bit_desc.pin_O->pin_id(0)) == test[4]);
    }
}

AdderIO create_4bit_adder(LSimContext *lsim_context) {
    AdderIO result = {};

    // create 1bit adder circuit
    auto adder_1bit = create_1bit_adder(lsim_context);

    // create a 4bit adder circuit
    result.circuit = lsim_context->create_user_circuit("adder_4bit");

    result.pin_Ci = result.circuit->add_connector_in("Ci", 1);
    result.pin_A  = result.circuit->add_connector_in("A", 4);
    result.pin_B  = result.circuit->add_connector_in("B", 4);
    result.pin_O  = result.circuit->add_connector_out("O", 4);
    result.pin_Co = result.circuit->add_connector_out("Co", 1);

    auto add1_0 = result.circuit->add_sub_circuit("adder_1bit");
    result.circuit->connect(result.pin_Ci->pin_id(0), add1_0->port_by_name("Ci"));
    result.circuit->connect(result.pin_A->pin_id(0), add1_0->port_by_name("A"));
    result.circuit->connect(result.pin_B->pin_id(0), add1_0->port_by_name("B"));
    result.circuit->connect(result.pin_O->pin_id(0), add1_0->port_by_name("O"));

    auto add1_1 = result.circuit->add_sub_circuit("adder_1bit");
    result.circuit->connect(add1_0->port_by_name("Co"), add1_1->port_by_name("Ci"));
    result.circuit->connect(result.pin_A->pin_id(1), add1_1->port_by_name("A"));
    result.circuit->connect(result.pin_B->pin_id(1), add1_1->port_by_name("B"));
    result.circuit->connect(result.pin_O->pin_id(1), add1_1->port_by_name("O"));

    auto add1_2 = result.circuit->add_sub_circuit("adder_1bit");
    result.circuit->connect(add1_1->port_by_name("Co"), add1_2->port_by_name("Ci"));
    result.circuit->connect(result.pin_A->pin_id(2), add1_2->port_by_name("A"));
    result.circuit->connect(result.pin_B->pin_id(2), add1_2->port_by_name("B"));
    result.circuit->connect(result.pin_O->pin_id(2), add1_2->port_by_name("O"));

    auto add1_3 = result.circuit->add_sub_circuit("adder_1bit");
    result.circuit->connect(add1_2->port_by_name("Co"), add1_3->port_by_name("Ci"));
    result.circuit->connect(result.pin_A->pin_id(3), add1_3->port_by_name("A"));
    result.circuit->connect(result.pin_B->pin_id(3), add1_3->port_by_name("B"));
    result.circuit->connect(result.pin_O->pin_id(3), add1_3->port_by_name("O"));
    result.circuit->connect(add1_3->port_by_name("Co"), result.pin_Co->pin_id(0));

    return result;
}

TEST_CASE("4bit adder (circuit cloning)", "[circuit]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto adder_4bit_desc = create_4bit_adder(&lsim_context);
    auto circuit = adder_4bit_desc.circuit->instantiate(sim);
    REQUIRE(circuit);

    sim->init();

    for (int ci = 0; ci < 2; ++ci) {
        circuit->write_pin(adder_4bit_desc.pin_Ci->pin_id(0), static_cast<Value>(ci));

        for (int a = 0; a < 16; ++a) {
            for (int b = 0; b < 16; ++b) {
                int req_O = (a + b + ci) & 0xf;
                int req_Co = ((a + b + ci) >> 4) & 0x1;

                circuit->write_output_pins(adder_4bit_desc.pin_A->id(), a);
                circuit->write_output_pins(adder_4bit_desc.pin_B->id(), b);
                sim->run_until_stable(5);

                REQUIRE(circuit->read_nibble(adder_4bit_desc.pin_O->id()) == req_O);
                REQUIRE(circuit->read_pin(adder_4bit_desc.pin_Co->pin_id(0)) == req_Co);
            }
        }
    }
}


TEST_CASE("8bit adder (multi-level cloning)", "[circuit]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit_desc = lsim_context.create_user_circuit("adder_8bit");
    auto pin_Ci = circuit_desc->add_connector_in("Ci", 1);
    auto pin_A  = circuit_desc->add_connector_in("A", 8);
    auto pin_B  = circuit_desc->add_connector_in("B", 8);
    auto pin_O  = circuit_desc->add_connector_out("O", 8);
    auto pin_Co = circuit_desc->add_connector_out("Co", 1);

    auto adder_4bit = create_4bit_adder(&lsim_context);

    auto add4_0 = circuit_desc->add_sub_circuit("adder_4bit");
    circuit_desc->connect(pin_Ci->pin_id(0), add4_0->port_by_name("Ci"));
    circuit_desc->connect(pin_A->pin_id(0), add4_0->port_by_name("A[0]"));
    circuit_desc->connect(pin_A->pin_id(1), add4_0->port_by_name("A[1]"));
    circuit_desc->connect(pin_A->pin_id(2), add4_0->port_by_name("A[2]"));
    circuit_desc->connect(pin_A->pin_id(3), add4_0->port_by_name("A[3]"));
    circuit_desc->connect(pin_B->pin_id(0), add4_0->port_by_name("B[0]"));
    circuit_desc->connect(pin_B->pin_id(1), add4_0->port_by_name("B[1]"));
    circuit_desc->connect(pin_B->pin_id(2), add4_0->port_by_name("B[2]"));
    circuit_desc->connect(pin_B->pin_id(3), add4_0->port_by_name("B[3]"));
    circuit_desc->connect(pin_O->pin_id(0), add4_0->port_by_name("O[0]"));
    circuit_desc->connect(pin_O->pin_id(1), add4_0->port_by_name("O[1]"));
    circuit_desc->connect(pin_O->pin_id(2), add4_0->port_by_name("O[2]"));
    circuit_desc->connect(pin_O->pin_id(3), add4_0->port_by_name("O[3]"));

    auto add4_1 = circuit_desc->add_sub_circuit("adder_4bit");
    circuit_desc->connect(add4_0->port_by_name("Co"), add4_1->port_by_name("Ci"));
    circuit_desc->connect(pin_A->pin_id(4), add4_1->port_by_name("A[0]"));
    circuit_desc->connect(pin_A->pin_id(5), add4_1->port_by_name("A[1]"));
    circuit_desc->connect(pin_A->pin_id(6), add4_1->port_by_name("A[2]"));
    circuit_desc->connect(pin_A->pin_id(7), add4_1->port_by_name("A[3]"));
    circuit_desc->connect(pin_B->pin_id(4), add4_1->port_by_name("B[0]"));
    circuit_desc->connect(pin_B->pin_id(5), add4_1->port_by_name("B[1]"));
    circuit_desc->connect(pin_B->pin_id(6), add4_1->port_by_name("B[2]"));
    circuit_desc->connect(pin_B->pin_id(7), add4_1->port_by_name("B[3]"));
    circuit_desc->connect(pin_O->pin_id(4), add4_1->port_by_name("O[0]"));
    circuit_desc->connect(pin_O->pin_id(5), add4_1->port_by_name("O[1]"));
    circuit_desc->connect(pin_O->pin_id(6), add4_1->port_by_name("O[2]"));
    circuit_desc->connect(pin_O->pin_id(7), add4_1->port_by_name("O[3]"));
    circuit_desc->connect(add4_1->port_by_name("Co"), pin_Co->pin_id(0)); 

    auto circuit = circuit_desc->instantiate(sim);
    REQUIRE(circuit);

    sim->init();

    for (int ci = 0; ci < 2; ++ci) {
        circuit->write_pin(pin_Ci->pin_id(0), static_cast<Value>(ci));

        for (int a = 0; a < 256; a += 12) {
            for (int b = 0; b < 256; b += 9) {
                int req_O = (a + b + ci) & 0xff;
                int req_Co = ((a + b + ci) >> 8) & 0x1;

                circuit->write_output_pins(pin_A->id(), a);
                circuit->write_output_pins(pin_B->id(), b);
                sim->run_until_stable(5);

                REQUIRE(circuit->read_byte(pin_O->id()) == req_O);
                REQUIRE(circuit->read_pin(pin_Co->pin_id(0)) == req_Co);
            }
        }
    }
}