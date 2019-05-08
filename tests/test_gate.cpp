#include "catch.hpp"
#include "lsim_context.h"
#include "gate.h"

TEST_CASE("Buffer", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("main");
    REQUIRE(circuit);

    auto in = ConnectorInput(circuit, "in", 4);
    REQUIRE(in);

    auto out = ConnectorOutput(circuit, "out", 4);
    REQUIRE(out);

    auto buffer = Buffer(circuit, 4);
    REQUIRE(buffer);

    for (int idx = 0; idx < 4; ++idx) {
        circuit->connect_pins(in->output_pin(idx), buffer->input_pin(idx));
        circuit->connect_pins(buffer->output_pin(idx), out->input_pin(idx));
    }

    sim->init(circuit);
    REQUIRE(circuit->read_value(out->pin(0)) == VALUE_UNDEFINED);

    // buffer takes a cycle to change output
    in->write_output_pins({VALUE_TRUE, VALUE_FALSE, VALUE_FALSE, VALUE_UNDEFINED});
    sim->step();
    REQUIRE(circuit->read_value(out->pin(0)) == VALUE_UNDEFINED);
    sim->step();
    REQUIRE(circuit->read_value(out->pin(0)) == VALUE_TRUE);
    REQUIRE(circuit->read_value(out->pin(1)) == VALUE_FALSE);
    REQUIRE(circuit->read_value(out->pin(2)) == VALUE_FALSE);
    REQUIRE(circuit->read_value(out->pin(3)) == VALUE_UNDEFINED);

    in->write_output_pins({VALUE_FALSE, VALUE_FALSE, VALUE_TRUE, VALUE_ERROR});
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

    auto in = ConnectorInput(circuit, "in", 2);
    REQUIRE(in);

    auto out = ConnectorOutput(circuit, "out", 2);
    REQUIRE(out);

    auto en = ConnectorInput(circuit, "en", 1);
    REQUIRE(en);

    auto buffer = TriStateBuffer(circuit, 2);
    REQUIRE(buffer);

    circuit->connect_pins(en->pin(0), buffer->control_pin(0));
    for (int idx = 0; idx < 2; ++idx) {
        circuit->connect_pins(in->output_pin(idx), buffer->input_pin(idx));
        circuit->connect_pins(buffer->output_pin(idx), out->input_pin(idx));
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
        in->write_pin(0, truth_table[test_idx][0]);
        in->write_pin(1, truth_table[test_idx][1]);
        en->write_pin(0, truth_table[test_idx][2]);
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

    auto pin_A = ConnectorInput(circuit, "A", 1);
    auto pin_B = ConnectorInput(circuit, "B", 1);
    auto pin_Sel = ConnectorInput(circuit, "Sel", 1);
    auto pin_O = ConnectorOutput(circuit, "O", 1);

    auto buf_0 = TriStateBuffer(circuit, 1);
    auto buf_1 = TriStateBuffer(circuit, 1);
    auto not_gate = NotGate(circuit);
    auto buf_2 = Buffer(circuit, 1);

    sim->connect_pins(pin_A->output_pin(0), buf_0->input_pin(0));
    sim->connect_pins(pin_B->output_pin(0), buf_1->input_pin(0));
    sim->connect_pins(pin_Sel->output_pin(0), not_gate->input_pin(0));
    sim->connect_pins(not_gate->output_pin(0), buf_0->control_pin(0));
    sim->connect_pins(pin_Sel->output_pin(0), buf_2->input_pin(0));
    sim->connect_pins(buf_2->output_pin(0), buf_1->control_pin(0));
    sim->connect_pins(buf_0->output_pin(0), pin_O->input_pin(0));
    sim->connect_pins(buf_1->output_pin(0), pin_O->input_pin(0));
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
        pin_A->write_pin(0, test[0]);
        pin_B->write_pin(0, test[1]);
        pin_Sel->write_pin(0, test[2]);
        sim->run_until_stable(5);
        REQUIRE(pin_O->read_pin(0) == test[3]);
    }
}

TEST_CASE("AndGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("main");
    REQUIRE(circuit);

    auto in_0 = ConnectorInput(circuit, "i0", 1);
    REQUIRE(in_0);
    auto in_1 = ConnectorInput(circuit, "i1", 1);
    REQUIRE(in_1);

    auto and_gate = AndGate(circuit, 2);
    REQUIRE(and_gate);

    auto out = ConnectorOutput(circuit, "out", 1);
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
        in_0->write_pin(0, truth_table[test_idx][0]);
        in_1->write_pin(0, truth_table[test_idx][1]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == truth_table[test_idx][2]);
    } 
}

TEST_CASE("OrGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("main");
    REQUIRE(circuit);

    auto in_0 = ConnectorInput(circuit, "i0", 1);
    REQUIRE(in_0);
    auto in_1 = ConnectorInput(circuit, "i1", 1);
    REQUIRE(in_1);

    auto or_gate = OrGate(circuit, 2);
    REQUIRE(or_gate);

    auto out = ConnectorOutput(circuit, "out", 1);
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

    sim->init(circuit);

    for (auto test : truth_table) {
        in_0->write_pin(0, test[0]);
        in_1->write_pin(0, test[1]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == test[2]);
    }
}

TEST_CASE("NotGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("main");
    REQUIRE(circuit);

    auto in = ConnectorInput(circuit, "in", 1);
    REQUIRE(in);

    auto not_gate = NotGate(circuit);
    REQUIRE(not_gate);

    auto out = ConnectorOutput(circuit, "out", 1);
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
        in->write_pin(0, test[0]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == test[1]);
    }
}

TEST_CASE("NandGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("main");
    REQUIRE(circuit);

    auto in_0 = ConnectorInput(circuit, "i0", 1);
    REQUIRE(in_0);
    auto in_1 = ConnectorInput(circuit, "i1", 1);
    REQUIRE(in_1);

    auto nand_gate = NandGate(circuit, 2);
    REQUIRE(nand_gate);

    auto out = ConnectorOutput(circuit, "out", 1);
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
        in_0->write_pin(0, test[0]);
        in_1->write_pin(0, test[1]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == test[2]);
    }
}

TEST_CASE("NorGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("main");
    REQUIRE(circuit);

    auto in_0 = ConnectorInput(circuit, "i0", 1);
    REQUIRE(in_0);
    auto in_1 = ConnectorInput(circuit, "i1", 1);
    REQUIRE(in_1);

    auto nor_gate = NorGate(circuit, 2);
    REQUIRE(nor_gate);

    auto out = ConnectorOutput(circuit, "out", 1);
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
        in_0->write_pin(0, test[0]);
        in_1->write_pin(0, test[1]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == test[2]);
    }
}

TEST_CASE("XorGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("main");
    REQUIRE(circuit);

    auto in_0 = ConnectorInput(circuit, "i0", 1);
    REQUIRE(in_0);
    auto in_1 = ConnectorInput(circuit, "i1", 1);
    REQUIRE(in_1);

    auto xor_gate = XorGate(circuit);
    REQUIRE(xor_gate);

    auto out = ConnectorOutput(circuit, "out", 1);
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
        in_0->write_pin(0, test[0]);
        in_1->write_pin(0, test[1]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == test[2]);
    }
}

TEST_CASE("XnorGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("main");
    REQUIRE(circuit);

    auto in_0 = ConnectorInput(circuit, "i0", 1);
    REQUIRE(in_0);
    auto in_1 = ConnectorInput(circuit, "i1", 1);
    REQUIRE(in_1);

    auto xnor_gate = XnorGate(circuit);
    REQUIRE(xnor_gate);

    auto out = ConnectorOutput(circuit, "out", 1);
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
        in_0->write_pin(0, test[0]);
        in_1->write_pin(0, test[1]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == test[2]);
    }
}