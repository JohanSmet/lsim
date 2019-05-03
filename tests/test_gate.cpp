#include "catch.hpp"
#include "lsim_context.h"
#include "gate.h"

TEST_CASE("Buffer", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("main");
    REQUIRE(circuit);

    auto in = circuit->create_component<Connector>("in", 4, Connector::INPUT);
    REQUIRE(in);

    auto out = circuit->create_component<Connector>("out", 4, Connector::OUTPUT);
    REQUIRE(out);

    auto buffer = circuit->create_component<Buffer>(4);
    REQUIRE(buffer);

    for (int idx = 0; idx < 4; ++idx) {
        circuit->connect_pins(in->pin(idx), buffer->pin(idx));
        circuit->connect_pins(buffer->pin(idx + 4), out->pin(idx));
    }

    sim->init(circuit);
    REQUIRE(circuit->read_value(out->pin(0)) == VALUE_UNDEFINED);

    // buffer takes a cycle to change output
    in->change_data({VALUE_TRUE, VALUE_FALSE, VALUE_FALSE, VALUE_UNDEFINED});
    sim->step();
    REQUIRE(circuit->read_value(out->pin(0)) == VALUE_UNDEFINED);
    sim->step();
    REQUIRE(circuit->read_value(out->pin(0)) == VALUE_TRUE);
    REQUIRE(circuit->read_value(out->pin(1)) == VALUE_FALSE);
    REQUIRE(circuit->read_value(out->pin(2)) == VALUE_FALSE);
    REQUIRE(circuit->read_value(out->pin(3)) == VALUE_UNDEFINED);

    in->change_data({VALUE_FALSE, VALUE_FALSE, VALUE_TRUE, VALUE_ERROR});
    sim->step();
    REQUIRE(circuit->read_value(out->pin(0)) == VALUE_TRUE);
    REQUIRE(circuit->read_value(out->pin(1)) == VALUE_FALSE);
    REQUIRE(circuit->read_value(out->pin(2)) == VALUE_FALSE);
    REQUIRE(circuit->read_value(out->pin(3)) == VALUE_UNDEFINED);
    sim->step();
    REQUIRE(circuit->read_value(out->pin(0)) == VALUE_FALSE);
    REQUIRE(circuit->read_value(out->pin(1)) == VALUE_FALSE);
    REQUIRE(circuit->read_value(out->pin(2)) == VALUE_TRUE);
    REQUIRE(circuit->read_value(out->pin(3)) == VALUE_ERROR);
}

TEST_CASE("TriStateBuffer", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("main");
    REQUIRE(circuit);

    auto in = circuit->create_component<Connector>("in", 2, Connector::INPUT);
    REQUIRE(in);

    auto out = circuit->create_component<Connector>("out", 2, Connector::OUTPUT);
    REQUIRE(out);

    auto en = circuit->create_component<Connector>("en", 1, Connector::INPUT);
    REQUIRE(en);

    auto buffer = circuit->create_component<TriStateBuffer>(2);
    REQUIRE(buffer);

    circuit->connect_pins(en->pin(0), buffer->enable_pin());
    for (int idx = 0; idx < 2; ++idx) {
        circuit->connect_pins(in->pin(idx), buffer->pin(idx));
        circuit->connect_pins(buffer->pin(idx + 3), out->pin(idx));
    }

    sim->init(circuit);

    Value truth_table[][5] = {
        // in[0]        in[1]         en          out[0]           out[1]
        {VALUE_FALSE, VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE,     VALUE_FALSE},
        {VALUE_FALSE, VALUE_FALSE, VALUE_FALSE, VALUE_UNDEFINED, VALUE_UNDEFINED},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE,     VALUE_TRUE},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE, VALUE_UNDEFINED, VALUE_UNDEFINED},

        {VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE,      VALUE_FALSE},
        {VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE, VALUE_UNDEFINED, VALUE_UNDEFINED},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE,      VALUE_TRUE},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE, VALUE_UNDEFINED, VALUE_UNDEFINED},

        {VALUE_ERROR, VALUE_UNDEFINED, VALUE_TRUE,      VALUE_ERROR,     VALUE_UNDEFINED},
        {VALUE_ERROR, VALUE_UNDEFINED, VALUE_FALSE,     VALUE_UNDEFINED, VALUE_UNDEFINED},
        {VALUE_ERROR, VALUE_UNDEFINED, VALUE_UNDEFINED, VALUE_UNDEFINED, VALUE_UNDEFINED}
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

TEST_CASE("Multiple TriState Buffers", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("main");
    REQUIRE(circuit);

    auto pin_A = circuit->create_component<Connector>("A", 1, Connector::INPUT);
    auto pin_B = circuit->create_component<Connector>("B", 1, Connector::INPUT);
    auto pin_Sel = circuit->create_component<Connector>("Sel", 1, Connector::INPUT);
    auto pin_O = circuit->create_component<Connector>("O", 1, Connector::OUTPUT);

    auto buf_0 = circuit->create_component<TriStateBuffer>(1);
    auto buf_1 = circuit->create_component<TriStateBuffer>(1);
    auto not_gate = circuit->create_component<NotGate>();
    auto buf_2 = circuit->create_component<Buffer>(1);

    sim->connect_pins(pin_A->pin(0), buf_0->pin(0));
    sim->connect_pins(pin_B->pin(0), buf_1->pin(0));
    sim->connect_pins(pin_Sel->pin(0), not_gate->pin(0));
    sim->connect_pins(not_gate->pin(1), buf_0->pin(1));
    sim->connect_pins(pin_Sel->pin(0), buf_2->pin(0));
    sim->connect_pins(buf_2->pin(1), buf_1->pin(1));
    sim->connect_pins(buf_0->pin(2), pin_O->pin(0));
    sim->connect_pins(buf_1->pin(2), pin_O->pin(0));
    sim->init(circuit);

    Value truth_table[][4] = {
        // A            B            Sel          O
        {VALUE_FALSE, VALUE_FALSE, VALUE_FALSE, VALUE_FALSE},
        {VALUE_FALSE, VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE},
        {VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE, VALUE_TRUE},
        {VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_FALSE, VALUE_TRUE},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE}
    };

    for (const auto &test : truth_table) {
        pin_A->change_data(test[0]);
        pin_B->change_data(test[1]);
        pin_Sel->change_data(test[2]);
        sim->run_until_stable(5);
        REQUIRE(pin_O->read_pin(0) == test[3]);
    }
}

TEST_CASE("AndGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("main");
    REQUIRE(circuit);

    auto in_0 = circuit->create_component<Connector>("i0", 1, Connector::INPUT);
    REQUIRE(in_0);
    auto in_1 = circuit->create_component<Connector>("i1", 1, Connector::INPUT);
    REQUIRE(in_1);

    auto and_gate = circuit->create_component<AndGate>(2);
    REQUIRE(and_gate);

    auto out = circuit->create_component<Connector>("out", 1, Connector::OUTPUT);
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

    sim->init(circuit);

    for (auto test_idx = 0u; test_idx < num_tests; ++test_idx) {
        in_0->change_data(truth_table[test_idx][0]);
        in_1->change_data(truth_table[test_idx][1]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == truth_table[test_idx][2]);
    }
}

TEST_CASE("OrGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("main");
    REQUIRE(circuit);

    auto in_0 = circuit->create_component<Connector>("i0", 1, Connector::INPUT);
    REQUIRE(in_0);
    auto in_1 = circuit->create_component<Connector>("i1", 1, Connector::INPUT);
    REQUIRE(in_1);

    auto or_gate = circuit->create_component<OrGate>(2);
    REQUIRE(or_gate);

    auto out = circuit->create_component<Connector>("out", 1, Connector::OUTPUT);
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

    sim->init(circuit);

    for (auto test_idx = 0u; test_idx < num_tests; ++test_idx) {
        in_0->change_data(truth_table[test_idx][0]);
        in_1->change_data(truth_table[test_idx][1]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == truth_table[test_idx][2]);
    }
}

TEST_CASE("NotGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("main");
    REQUIRE(circuit);

    auto in = circuit->create_component<Connector>("in", 1, Connector::INPUT);
    REQUIRE(in);

    auto not_gate = circuit->create_component<NotGate>();
    REQUIRE(not_gate);

    auto out = circuit->create_component<Connector>("out", 1, Connector::OUTPUT);
    REQUIRE(out);

    circuit->connect_pins(not_gate->pin(0), in->pin(0));
    circuit->connect_pins(not_gate->pin(1), out->pin(0));

    Value truth_table[][2] = {
        // in               out
        {VALUE_FALSE,     VALUE_TRUE},
        {VALUE_TRUE,      VALUE_FALSE},
        {VALUE_UNDEFINED, VALUE_ERROR}
    };

    sim->init(circuit);

    for (auto test : truth_table) {
        in->change_data(test[0]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == test[1]);
    }
}

TEST_CASE("NandGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("main");
    REQUIRE(circuit);

    auto in_0 = circuit->create_component<Connector>("in_0", 1, Connector::INPUT);
    REQUIRE(in_0);
    auto in_1 = circuit->create_component<Connector>("in_1", 1, Connector::INPUT);
    REQUIRE(in_1);

    auto nand_gate = circuit->create_component<NandGate>(2);
    REQUIRE(nand_gate);

    auto out = circuit->create_component<Connector>("out", 1, Connector::OUTPUT);
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

    sim->init(circuit);

    for (auto test : truth_table) {
        in_0->change_data(test[0]);
        in_1->change_data(test[1]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == test[2]);
    }
}

TEST_CASE("NorGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("main");
    REQUIRE(circuit);

    auto in_0 = circuit->create_component<Connector>("in_0", 1, Connector::INPUT);
    REQUIRE(in_0);
    auto in_1 = circuit->create_component<Connector>("in_1", 1, Connector::INPUT);
    REQUIRE(in_1);

    auto nor_gate = circuit->create_component<NorGate>(2);
    REQUIRE(nor_gate);

    auto out = circuit->create_component<Connector>("out", 1, Connector::OUTPUT);
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

    sim->init(circuit);

    for (auto test : truth_table) {
        in_0->change_data(test[0]);
        in_1->change_data(test[1]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == test[2]);
    }
}

TEST_CASE("XorGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("main");
    REQUIRE(circuit);

    auto in_0 = circuit->create_component<Connector>("in_0", 1, Connector::INPUT);
    REQUIRE(in_0);
    auto in_1 = circuit->create_component<Connector>("in_1", 1, Connector::INPUT);
    REQUIRE(in_1);

    auto xor_gate = circuit->create_component<XorGate>();
    REQUIRE(xor_gate);

    auto out = circuit->create_component<Connector>("out", 1, Connector::OUTPUT);
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

    sim->init(circuit);

    for (auto test : truth_table) {
        in_0->change_data(test[0]);
        in_1->change_data(test[1]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == test[2]);
    }
}

TEST_CASE("XnorGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("main");
    REQUIRE(circuit);

    auto in_0 = circuit->create_component<Connector>("in_0", 1, Connector::INPUT);
    REQUIRE(in_0);
    auto in_1 = circuit->create_component<Connector>("in_1", 1, Connector::INPUT);
    REQUIRE(in_1);

    auto xnor_gate = circuit->create_component<XnorGate>();
    REQUIRE(xnor_gate);

    auto out = circuit->create_component<Connector>("out", 1, Connector::OUTPUT);
    REQUIRE(out);

    circuit->connect_pins(in_0->pin(0), xnor_gate->pin(0));
    circuit->connect_pins(in_1->pin(0), xnor_gate->pin(1));
    circuit->connect_pins(xnor_gate->pin(2), out->pin(0));

    Value truth_table[][3] = {
        // in_0         in_1         out
        {VALUE_FALSE, VALUE_FALSE, VALUE_TRUE},
        {VALUE_FALSE, VALUE_TRUE,  VALUE_FALSE},
        {VALUE_TRUE,  VALUE_FALSE, VALUE_FALSE},
        {VALUE_TRUE,  VALUE_TRUE,  VALUE_TRUE},

        {VALUE_UNDEFINED, VALUE_FALSE,     VALUE_ERROR},
        {VALUE_UNDEFINED, VALUE_TRUE,      VALUE_ERROR},
        {VALUE_FALSE,     VALUE_UNDEFINED, VALUE_ERROR},
        {VALUE_TRUE,      VALUE_UNDEFINED, VALUE_ERROR}
    };

    sim->init(circuit);

    for (auto test : truth_table) {
        in_0->change_data(test[0]);
        in_1->change_data(test[1]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == test[2]);
    }
}
