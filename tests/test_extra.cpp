#include "catch.hpp"
#include "lsim_context.h"
#include "circuit_instance.h"

using namespace lsim;

TEST_CASE("PullResistor", "[extra]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit_desc = lsim_context.create_user_circuit("main");
    REQUIRE(circuit_desc);

    auto in = circuit_desc->add_connector_in("in", 1);
    REQUIRE(in);

    auto out = circuit_desc->add_connector_out("out", 1);
    REQUIRE(out);

    auto pull = circuit_desc->add_pull_resistor(VALUE_TRUE);
    REQUIRE(pull);

    circuit_desc->connect(in->pin_id(0), out->pin_id(0));
    circuit_desc->connect(pull->pin_id(0), out->pin_id(0));

    Value truth_table[][2] = {
        // in               out
        {VALUE_FALSE,     VALUE_FALSE},
        {VALUE_TRUE,      VALUE_TRUE},
        {VALUE_UNDEFINED, VALUE_TRUE}
    };

    auto circuit = circuit_desc->instantiate(sim);
    REQUIRE(circuit);

    sim->init();

    for (const auto &test : truth_table) {
        circuit->write_pin(in->pin_id(0), test[0]);
        sim->run_until_stable(5);
        REQUIRE(circuit->read_pin(out->pin_id(0)) == test[1]);
    }
}

TEST_CASE("Via", "[extra]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit_desc = lsim_context.create_user_circuit("main");
    REQUIRE(circuit_desc);

    auto in = circuit_desc->add_connector_in("in", 3);
    REQUIRE(in);

    auto via_1 = circuit_desc->add_via("via", 2);
    REQUIRE(via_1);
    auto via_2 = circuit_desc->add_via("via", 2);
    REQUIRE(via_2);
    auto via_3 = circuit_desc->add_via("via", 2);
    REQUIRE(via_3);
    auto via_a = circuit_desc->add_via("via2", 1);
    REQUIRE(via_a);
    auto via_b = circuit_desc->add_via("via2", 1);
    REQUIRE(via_b);

    auto out_a = circuit_desc->add_connector_out("out_a", 2);
    REQUIRE(out_a);
    auto out_b = circuit_desc->add_connector_out("out_b", 2);
    REQUIRE(out_b);
    auto out_c = circuit_desc->add_connector_out("out_c", 1);
    REQUIRE(out_c);

    auto not_1 = circuit_desc->add_not_gate();
    REQUIRE(not_1);
    auto not_2 = circuit_desc->add_not_gate();
    REQUIRE(not_2);

    circuit_desc->connect(in->pin_id(0), via_1->pin_id(0));
    circuit_desc->connect(in->pin_id(1), via_1->pin_id(1));
    circuit_desc->connect(in->pin_id(2), via_a->pin_id(0));

    circuit_desc->connect(via_2->pin_id(0), out_a->pin_id(0));
    circuit_desc->connect(via_2->pin_id(1), out_a->pin_id(1));

    circuit_desc->connect(via_3->pin_id(0), not_1->input_pin_id(0));
    circuit_desc->connect(via_3->pin_id(1), not_2->input_pin_id(0));

    circuit_desc->connect(not_1->output_pin_id(0), out_b->pin_id(0));
    circuit_desc->connect(not_2->output_pin_id(0), out_b->pin_id(1));

    circuit_desc->connect(via_b->pin_id(0), out_c->pin_id(0));

    Value truth_table[][8] = {
        // in[0]        in[1]        in[2]        out_a[0]     out_a[1]    out_b[0]      out_b[1]
        {VALUE_FALSE, VALUE_FALSE, VALUE_FALSE, VALUE_FALSE, VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE},
        {VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE, VALUE_FALSE},
        {VALUE_FALSE, VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE},
        {VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE, VALUE_TRUE}
    };

    auto circuit = circuit_desc->instantiate(sim);
    REQUIRE(circuit);

    sim->init();

    for (const auto &test : truth_table) {
        circuit->write_pin(in->pin_id(0), test[0]);
        circuit->write_pin(in->pin_id(1), test[1]);
        circuit->write_pin(in->pin_id(2), test[2]);
        sim->run_until_stable(5);
        REQUIRE(circuit->read_pin(out_a->pin_id(0)) == test[3]);
        REQUIRE(circuit->read_pin(out_a->pin_id(1)) == test[4]);
        REQUIRE(circuit->read_pin(out_b->pin_id(0)) == test[5]);
        REQUIRE(circuit->read_pin(out_b->pin_id(1)) == test[6]);
        REQUIRE(circuit->read_pin(out_c->pin_id(0)) == test[7]);
    }
}

TEST_CASE("Oscillator", "[extra]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit_desc = lsim_context.create_user_circuit("main");
    REQUIRE(circuit_desc);

    auto clock = circuit_desc->add_oscillator(3, 2);
    REQUIRE(clock);

    auto out = circuit_desc->add_connector_out("out", 1);
    REQUIRE(out);

    circuit_desc->connect(clock->output_pin_id(0), out->pin_id(0));

    auto circuit = circuit_desc->instantiate(sim);
    REQUIRE(circuit);

    sim->init();

    for (int i = 0; i < 3; ++i) {
        REQUIRE(circuit->read_pin(out->pin_id(0)) == VALUE_FALSE);
        sim->step();
        REQUIRE(circuit->read_pin(out->pin_id(0)) == VALUE_FALSE);
        sim->step();
        REQUIRE(circuit->read_pin(out->pin_id(0)) == VALUE_FALSE);
        sim->step();
        REQUIRE(circuit->read_pin(out->pin_id(0)) == VALUE_TRUE);
        sim->step();
        REQUIRE(circuit->read_pin(out->pin_id(0)) == VALUE_TRUE);
        sim->step();
    }
}

TEST_CASE("Oscillator Long", "[extra]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit_desc = lsim_context.create_user_circuit("main");
    REQUIRE(circuit_desc);

    auto clock = circuit_desc->add_oscillator(40, 40);
    REQUIRE(clock);

    auto out = circuit_desc->add_connector_out("out", 1);
    REQUIRE(out);

    circuit_desc->connect(clock->output_pin_id(0), out->pin_id(0));

    auto circuit = circuit_desc->instantiate(sim);
    REQUIRE(circuit);

    sim->init();

    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 40; ++j) {
            REQUIRE(circuit->read_pin(out->pin_id(0)) == VALUE_FALSE);
            sim->step();
        }

        for (int j = 0; j < 40; ++j) {
            REQUIRE(circuit->read_pin(out->pin_id(0)) == VALUE_TRUE);
            sim->step();
        }
    }
}