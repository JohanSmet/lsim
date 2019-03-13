#include "catch.hpp"
#include "simulator.h"
#include "circuit.h"
#include "gate.h"

TEST_CASE("Buffer", "[gate]") {

    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    auto circuit = sim->create_circuit();
    REQUIRE(circuit);

    auto in = circuit->create_component<Connector>("in", 4);
    REQUIRE(in);

    auto out = circuit->create_component<Connector>("out", 4);
    REQUIRE(out);

    auto buffer = circuit->create_component<Buffer>(4);
    REQUIRE(buffer);

    for (int idx = 0; idx < 4; ++idx) {
        circuit->connect_pins(in->pin(idx), buffer->pin(idx));
        circuit->connect_pins(buffer->pin(idx + 4), out->pin(idx));
    }

    sim->init();
    REQUIRE(circuit->read_value(out->pin(0)) == VALUE_UNDEFINED);

    // buffer takes a cycle to change output
    in->change_data({VALUE_TRUE, VALUE_FALSE, VALUE_FALSE, VALUE_FALSE});
    sim->step();
    REQUIRE(circuit->read_value(out->pin(0)) == VALUE_UNDEFINED);
    sim->step();
    REQUIRE(circuit->read_value(out->pin(0)) == VALUE_TRUE);
    REQUIRE(circuit->read_value(out->pin(1)) == VALUE_FALSE);
    REQUIRE(circuit->read_value(out->pin(2)) == VALUE_FALSE);

    in->change_data({VALUE_FALSE, VALUE_FALSE, VALUE_TRUE, VALUE_FALSE});
    sim->step();
    REQUIRE(circuit->read_value(out->pin(0)) == VALUE_TRUE);
    REQUIRE(circuit->read_value(out->pin(1)) == VALUE_FALSE);
    REQUIRE(circuit->read_value(out->pin(2)) == VALUE_FALSE);
    sim->step();
    REQUIRE(circuit->read_value(out->pin(0)) == VALUE_FALSE);
    REQUIRE(circuit->read_value(out->pin(1)) == VALUE_FALSE);
    REQUIRE(circuit->read_value(out->pin(2)) == VALUE_TRUE);
}

TEST_CASE("TriStateBuffer", "[gate]") {

    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    auto circuit = sim->create_circuit();
    REQUIRE(circuit);

    auto in = circuit->create_component<Connector>("in", 2);
    REQUIRE(in);

    auto out = circuit->create_component<Connector>("out", 2);
    REQUIRE(out);

    auto en = circuit->create_component<Connector>("en", 1);
    REQUIRE(en);

    auto buffer = circuit->create_component<TriStateBuffer>(2);
    REQUIRE(buffer);

    circuit->connect_pins(en->pin(0), buffer->enable_pin());
    for (int idx = 0; idx < 2; ++idx) {
        circuit->connect_pins(in->pin(idx), buffer->pin(idx));
        circuit->connect_pins(buffer->pin(idx + 3), out->pin(idx));
    }

    sim->init();

    Value truth_table[][5] = {
        // in[0]        in[1]         en          out[0]           out[1]
        {VALUE_FALSE, VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE,     VALUE_FALSE},
        {VALUE_FALSE, VALUE_FALSE, VALUE_FALSE, VALUE_UNDEFINED, VALUE_UNDEFINED},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE,     VALUE_TRUE},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE, VALUE_UNDEFINED, VALUE_UNDEFINED},

        {VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE,      VALUE_FALSE},
        {VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE, VALUE_UNDEFINED, VALUE_UNDEFINED},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE,      VALUE_TRUE},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE, VALUE_UNDEFINED, VALUE_UNDEFINED}
    };

    size_t num_tests = sizeof(truth_table) / sizeof(truth_table[0]);

    for (auto test_idx = 0u; test_idx < num_tests; ++test_idx) {
        in->change_data({truth_table[test_idx][0], truth_table[test_idx][1]});
        en->change_data(truth_table[test_idx][2]);
        sim->run_until_stable(5);
        REQUIRE(circuit->read_value(out->pin(0)) == truth_table[test_idx][3]);
        REQUIRE(circuit->read_value(out->pin(1)) == truth_table[test_idx][4]);
    }
}


TEST_CASE("AndGate", "[gate]") {

    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    auto circuit = sim->create_circuit();
    REQUIRE(circuit);

    auto in_0 = circuit->create_component<Connector>("i0", 1);
    REQUIRE(in_0);
    auto in_1 = circuit->create_component<Connector>("i1", 1);
    REQUIRE(in_1);

    auto and_gate = circuit->create_component<AndGate>(2);
    REQUIRE(and_gate);

    auto out = circuit->create_component<Connector>("out", 1);
    REQUIRE(out);

    circuit->connect_pins(and_gate->pin(0), in_0->pin(0));
    circuit->connect_pins(and_gate->pin(1), in_1->pin(0));
    circuit->connect_pins(and_gate->pin(2), out->pin(0));

    Value truth_table[][3] = {
        // in_0         in_1         out
        {VALUE_FALSE, VALUE_FALSE, VALUE_FALSE},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE},
        {VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE},

        {VALUE_UNDEFINED, VALUE_FALSE,     VALUE_ERROR},
        {VALUE_UNDEFINED, VALUE_TRUE,      VALUE_ERROR},
        {VALUE_FALSE,     VALUE_UNDEFINED, VALUE_ERROR},
        {VALUE_TRUE,      VALUE_UNDEFINED, VALUE_ERROR}
    };

    size_t num_tests = sizeof(truth_table) / sizeof(truth_table[0]);

    sim->init();

    for (auto test_idx = 0u; test_idx < num_tests; ++test_idx) {
        in_0->change_data(truth_table[test_idx][0]);
        in_1->change_data(truth_table[test_idx][1]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == truth_table[test_idx][2]);
    }
}

TEST_CASE("OrGate", "[gate]") {

    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    auto circuit = sim->create_circuit();
    REQUIRE(circuit);

    auto in_0 = circuit->create_component<Connector>("i0", 1);
    REQUIRE(in_0);
    auto in_1 = circuit->create_component<Connector>("i1", 1);
    REQUIRE(in_1);

    auto or_gate = circuit->create_component<OrGate>(2);
    REQUIRE(or_gate);

    auto out = circuit->create_component<Connector>("out", 1);
    REQUIRE(out);

    circuit->connect_pins(or_gate->pin(0), in_0->pin(0));
    circuit->connect_pins(or_gate->pin(1), in_1->pin(0));
    circuit->connect_pins(or_gate->pin(2), out->pin(0));

    Value truth_table[][3] = {
        // in_0         in_1         out
        {VALUE_FALSE, VALUE_FALSE, VALUE_FALSE},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE},
        {VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE},

        {VALUE_UNDEFINED, VALUE_FALSE,     VALUE_ERROR},
        {VALUE_UNDEFINED, VALUE_TRUE,      VALUE_ERROR},
        {VALUE_FALSE,     VALUE_UNDEFINED, VALUE_ERROR},
        {VALUE_TRUE,      VALUE_UNDEFINED, VALUE_ERROR}
    };

    size_t num_tests = sizeof(truth_table) / sizeof(truth_table[0]);

    sim->init();

    for (auto test_idx = 0u; test_idx < num_tests; ++test_idx) {
        in_0->change_data(truth_table[test_idx][0]);
        in_1->change_data(truth_table[test_idx][1]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == truth_table[test_idx][2]);
    }
}

TEST_CASE("NotGate", "[gate]") {

    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    auto circuit = sim->create_circuit();
    REQUIRE(circuit);

    auto in = circuit->create_component<Connector>("in", 1);
    REQUIRE(in);

    auto not_gate = circuit->create_component<NotGate>();
    REQUIRE(not_gate);

    auto out = circuit->create_component<Connector>("out", 1);
    REQUIRE(out);

    circuit->connect_pins(not_gate->pin(0), in->pin(0));
    circuit->connect_pins(not_gate->pin(1), out->pin(0));

    Value truth_table[][2] = {
        // in               out
        {VALUE_FALSE,     VALUE_TRUE},
        {VALUE_TRUE,      VALUE_FALSE},
        {VALUE_UNDEFINED, VALUE_ERROR}
    };

    sim->init();

    for (auto test : truth_table) {
        in->change_data(test[0]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == test[1]);
    }
}

TEST_CASE("NandGate", "[gate]") {

    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    auto circuit = sim->create_circuit();
    REQUIRE(circuit);

    auto in_0 = circuit->create_component<Connector>("in_0", 1);
    REQUIRE(in_0);
    auto in_1 = circuit->create_component<Connector>("in_1", 1);
    REQUIRE(in_1);

    auto nand_gate = circuit->create_component<NandGate>(2);
    REQUIRE(nand_gate);

    auto out = circuit->create_component<Connector>("out", 1);
    REQUIRE(out);

    circuit->connect_pins(in_0->pin(0), nand_gate->pin(0));
    circuit->connect_pins(in_1->pin(0), nand_gate->pin(1));
    circuit->connect_pins(nand_gate->pin(2), out->pin(0));

     Value truth_table[][3] = {
        // in_0         in_1         out
        {VALUE_FALSE, VALUE_FALSE, VALUE_TRUE},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE},
        {VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE},

        {VALUE_UNDEFINED, VALUE_FALSE,     VALUE_ERROR},
        {VALUE_UNDEFINED, VALUE_TRUE,      VALUE_ERROR},
        {VALUE_FALSE,     VALUE_UNDEFINED, VALUE_ERROR},
        {VALUE_TRUE,      VALUE_UNDEFINED, VALUE_ERROR}
    };

    sim->init();

    for (auto test : truth_table) {
        in_0->change_data(test[0]);
        in_1->change_data(test[1]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == test[2]);
    }
}

TEST_CASE("NorGate", "[gate]") {

    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    auto circuit = sim->create_circuit();
    REQUIRE(circuit);

    auto in_0 = circuit->create_component<Connector>("in_0", 1);
    REQUIRE(in_0);
    auto in_1 = circuit->create_component<Connector>("in_1", 1);
    REQUIRE(in_1);

    auto nor_gate = circuit->create_component<NorGate>(2);
    REQUIRE(nor_gate);

    auto out = circuit->create_component<Connector>("out", 1);
    REQUIRE(out);

    circuit->connect_pins(in_0->pin(0), nor_gate->pin(0));
    circuit->connect_pins(in_1->pin(0), nor_gate->pin(1));
    circuit->connect_pins(nor_gate->pin(2), out->pin(0));

     Value truth_table[][3] = {
        // in_0         in_1         out
        {VALUE_FALSE, VALUE_FALSE, VALUE_TRUE},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE},
        {VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE},

        {VALUE_UNDEFINED, VALUE_FALSE,     VALUE_ERROR},
        {VALUE_UNDEFINED, VALUE_TRUE,      VALUE_ERROR},
        {VALUE_FALSE,     VALUE_UNDEFINED, VALUE_ERROR},
        {VALUE_TRUE,      VALUE_UNDEFINED, VALUE_ERROR}
    };

    sim->init();

    for (auto test : truth_table) {
        in_0->change_data(test[0]);
        in_1->change_data(test[1]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == test[2]);
    }
}

TEST_CASE("XorGate", "[gate]") {

    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    auto circuit = sim->create_circuit();
    REQUIRE(circuit);

    auto in_0 = circuit->create_component<Connector>("in_0", 1);
    REQUIRE(in_0);
    auto in_1 = circuit->create_component<Connector>("in_1", 1);
    REQUIRE(in_1);

    auto xor_gate = circuit->create_component<XorGate>();
    REQUIRE(xor_gate);

    auto out = circuit->create_component<Connector>("out", 1);
    REQUIRE(out);

    circuit->connect_pins(in_0->pin(0), xor_gate->pin(0));
    circuit->connect_pins(in_1->pin(0), xor_gate->pin(1));
    circuit->connect_pins(xor_gate->pin(2), out->pin(0));

     Value truth_table[][3] = {
        // in_0         in_1         out
        {VALUE_FALSE, VALUE_FALSE, VALUE_FALSE},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE},
        {VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE},

        {VALUE_UNDEFINED, VALUE_FALSE,     VALUE_ERROR},
        {VALUE_UNDEFINED, VALUE_TRUE,      VALUE_ERROR},
        {VALUE_FALSE,     VALUE_UNDEFINED, VALUE_ERROR},
        {VALUE_TRUE,      VALUE_UNDEFINED, VALUE_ERROR}
    };

    sim->init();

    for (auto test : truth_table) {
        in_0->change_data(test[0]);
        in_1->change_data(test[1]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == test[2]);
    }
}

TEST_CASE("XnorGate", "[gate]") {

    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    auto circuit = sim->create_circuit();
    REQUIRE(circuit);

    auto in_0 = circuit->create_component<Constant>(VALUE_FALSE);
    REQUIRE(in_0);
    auto in_1 = circuit->create_component<Constant>(VALUE_TRUE);
    REQUIRE(in_1);

    auto xnor_gate = circuit->create_component<XnorGate>();
    REQUIRE(xnor_gate);

    auto out = circuit->create_component<Connector>("out", 1);
    REQUIRE(out);

    sim->init();
    circuit->connect_pins(xnor_gate->pin(2), out->pin(0));

    SECTION("all inputs are false") {
        circuit->connect_pins(in_0->pin(0), xnor_gate->pin(0));
        circuit->connect_pins(in_0->pin(0), xnor_gate->pin(1));

        sim->run_until_stable(5);
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_TRUE);
    }

    SECTION("all inputs are true") {
        circuit->connect_pins(in_1->pin(0), xnor_gate->pin(0));
        circuit->connect_pins(in_1->pin(0), xnor_gate->pin(1));

        sim->run_until_stable(5);
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_TRUE);
    }

    SECTION("first input is true, second is false") {
        circuit->connect_pins(in_1->pin(0), xnor_gate->pin(0));
        circuit->connect_pins(in_0->pin(0), xnor_gate->pin(1));

        sim->run_until_stable(5);
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_FALSE);
    }

    SECTION("first input is false, second is true") {
        circuit->connect_pins(in_0->pin(0), xnor_gate->pin(0));
        circuit->connect_pins(in_1->pin(0), xnor_gate->pin(1));

        sim->run_until_stable(5);
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_FALSE);
    }
}
