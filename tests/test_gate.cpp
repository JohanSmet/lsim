#include "catch.hpp"
#include "lsim_context.h"
#include "circuit_instance.h"

using namespace lsim;

TEST_CASE("Buffer", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit_desc = lsim_context.create_user_circuit("main");
    REQUIRE(circuit_desc);

    auto in = circuit_desc->add_connector_in("in", 4);
    REQUIRE(in);

    auto out = circuit_desc->add_connector_out("out", 4);
    REQUIRE(out);

    auto buffer = circuit_desc->add_buffer(4);
    REQUIRE(buffer);

    for (int idx = 0; idx < 4; ++idx) {
        circuit_desc->connect(in->output_pin_id(idx), buffer->input_pin_id(idx));
        circuit_desc->connect(buffer->output_pin_id(idx), out->input_pin_id(idx));
    }

    auto circuit = circuit_desc->instantiate(sim);
    REQUIRE(circuit);

    sim->init();
    REQUIRE(circuit->read_pin(out->pin_id(0)) == VALUE_UNDEFINED);

    // buffer takes a cycle to change output
    circuit->write_output_pins(in->id(), {VALUE_TRUE, VALUE_FALSE, VALUE_FALSE, VALUE_UNDEFINED});
    REQUIRE(circuit->read_pin(out->pin_id(0)) == VALUE_UNDEFINED);
    sim->step();
    REQUIRE(circuit->read_pin(out->pin_id(0)) == VALUE_TRUE);
    REQUIRE(circuit->read_pin(out->pin_id(1)) == VALUE_FALSE);
    REQUIRE(circuit->read_pin(out->pin_id(2)) == VALUE_FALSE);
    REQUIRE(circuit->read_pin(out->pin_id(3)) == VALUE_UNDEFINED);

    circuit->write_output_pins(in->id(), {VALUE_FALSE, VALUE_FALSE, VALUE_TRUE, VALUE_ERROR});
    sim->step();
    REQUIRE(circuit->read_pin(out->pin_id(0)) == VALUE_FALSE);
    REQUIRE(circuit->read_pin(out->pin_id(1)) == VALUE_FALSE);
    REQUIRE(circuit->read_pin(out->pin_id(2)) == VALUE_TRUE);
    REQUIRE(circuit->read_pin(out->pin_id(3)) == VALUE_ERROR);
}

TEST_CASE("TriStateBuffer", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit_desc = lsim_context.create_user_circuit("main");
    REQUIRE(circuit_desc);

    auto in = circuit_desc->add_connector_in("in", 2);
    REQUIRE(in);

    auto out = circuit_desc->add_connector_out("out", 2);
    REQUIRE(out);

    auto en = circuit_desc->add_connector_in("en", 1);
    REQUIRE(en);

    auto buffer = circuit_desc->add_tristate_buffer(2);
    REQUIRE(buffer);

    circuit_desc->connect(en->pin_id(0), buffer->control_pin_id(0));
    for (int idx = 0; idx < 2; ++idx) {
        circuit_desc->connect(in->output_pin_id(idx), buffer->input_pin_id(idx));
        circuit_desc->connect(buffer->output_pin_id(idx), out->input_pin_id(idx));
    }

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

    auto circuit = circuit_desc->instantiate(sim);
    REQUIRE(circuit);

    sim->init();

    for (const auto &test : truth_table) {
        circuit->write_pin(in->pin_id(0), test[0]);
        circuit->write_pin(in->pin_id(1), test[1]);
        circuit->write_pin(en->pin_id(0), test[2]);
        sim->run_until_stable(5);
        REQUIRE(circuit->read_pin(out->pin_id(0)) == test[3]);
        REQUIRE(circuit->read_pin(out->pin_id(1)) == test[4]);
    }
}

TEST_CASE("Multiple TriState Buffers", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit_desc = lsim_context.create_user_circuit("main");
    REQUIRE(circuit_desc);

    auto pin_A = circuit_desc->add_connector_in("A", 1);
    auto pin_B = circuit_desc->add_connector_in("B", 1);
    auto pin_Sel = circuit_desc->add_connector_in("Sel", 1);
    auto pin_O = circuit_desc->add_connector_out("O", 1);

    auto buf_0 = circuit_desc->add_tristate_buffer(1);
    auto buf_1 = circuit_desc->add_tristate_buffer(1);
    auto not_gate = circuit_desc->add_not_gate();
    auto buf_2 = circuit_desc->add_buffer(1);

    circuit_desc->connect(pin_A->output_pin_id(0), buf_0->input_pin_id(0));
    circuit_desc->connect(pin_B->output_pin_id(0), buf_1->input_pin_id(0));
    circuit_desc->connect(pin_Sel->output_pin_id(0), not_gate->input_pin_id(0));
    circuit_desc->connect(not_gate->output_pin_id(0), buf_0->control_pin_id(0));
    circuit_desc->connect(pin_Sel->output_pin_id(0), buf_2->input_pin_id(0));
    circuit_desc->connect(buf_2->output_pin_id(0), buf_1->control_pin_id(0));
    circuit_desc->connect(buf_0->output_pin_id(0), pin_O->input_pin_id(0));
    circuit_desc->connect(buf_1->output_pin_id(0), pin_O->input_pin_id(0));

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

    auto circuit = circuit_desc->instantiate(sim);
    REQUIRE(circuit);

    sim->init();

    for (const auto &test : truth_table) {
        circuit->write_pin(pin_A->pin_id(0), test[0]);
        circuit->write_pin(pin_B->pin_id(0), test[1]);
        circuit->write_pin(pin_Sel->pin_id(0), test[2]);
        sim->run_until_stable(5);
        REQUIRE(circuit->read_pin(pin_O->pin_id(0)) == test[3]);
    }
}

TEST_CASE("AndGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit_desc = lsim_context.create_user_circuit("main");
    REQUIRE(circuit_desc);

    auto in_0 = circuit_desc->add_connector_in("i0", 1);
    REQUIRE(in_0);
    auto in_1 = circuit_desc->add_connector_in("i1", 1);
    REQUIRE(in_1);

    auto and_gate = circuit_desc->add_and_gate(2);
    REQUIRE(and_gate);

    auto out = circuit_desc->add_connector_out("out", 1);
    REQUIRE(out);

    circuit_desc->connect(and_gate->pin_id(0), in_0->pin_id(0));
    circuit_desc->connect(and_gate->pin_id(1), in_1->pin_id(0));
    circuit_desc->connect(and_gate->pin_id(2), out->pin_id(0));

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

    auto circuit = circuit_desc->instantiate(sim);
    REQUIRE(circuit);

    sim->init();

    for (const auto &test : truth_table) {
        circuit->write_pin(in_0->pin_id(0), test[0]);
        circuit->write_pin(in_1->pin_id(0), test[1]);
        sim->run_until_stable(5);
        REQUIRE(circuit->read_pin(out->pin_id(0)) == test[2]);
    } 
}

TEST_CASE("OrGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit_desc = lsim_context.create_user_circuit("main");
    REQUIRE(circuit_desc);

    auto in_0 = circuit_desc->add_connector_in("i0", 1);
    REQUIRE(in_0);
    auto in_1 = circuit_desc->add_connector_in("i1", 1);
    REQUIRE(in_1);

    auto or_gate = circuit_desc->add_or_gate(2);
    REQUIRE(or_gate);

    auto out = circuit_desc->add_connector_out("out", 1);
    REQUIRE(out);

    circuit_desc->connect(or_gate->pin_id(0), in_0->pin_id(0));
    circuit_desc->connect(or_gate->pin_id(1), in_1->pin_id(0));
    circuit_desc->connect(or_gate->pin_id(2), out->pin_id(0));

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

    auto circuit = circuit_desc->instantiate(sim);
    REQUIRE(circuit);

    sim->init();

    for (auto test : truth_table) {
        circuit->write_pin(in_0->pin_id(0), test[0]);
        circuit->write_pin(in_1->pin_id(0), test[1]);
        sim->run_until_stable(5);
        REQUIRE(circuit->read_pin(out->pin_id(0)) == test[2]);
    }
}

TEST_CASE("NotGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit_desc = lsim_context.create_user_circuit("main");
    REQUIRE(circuit_desc);

    auto in = circuit_desc->add_connector_in("in", 1);
    REQUIRE(in);

    auto not_gate = circuit_desc->add_not_gate();
    REQUIRE(not_gate);

    auto out = circuit_desc->add_connector_out("out", 1);
    REQUIRE(out);

    circuit_desc->connect(not_gate->pin_id(0), in->pin_id(0));
    circuit_desc->connect(not_gate->pin_id(1), out->pin_id(0));

    Value truth_table[][2] = {
        // in               out
        {VALUE_FALSE,     VALUE_TRUE},
        {VALUE_TRUE,      VALUE_FALSE},
        {VALUE_UNDEFINED, VALUE_ERROR}
    };

    auto circuit = circuit_desc->instantiate(sim);
    REQUIRE(circuit);

    sim->init();

    for (auto test : truth_table) {
        circuit->write_pin(in->pin_id(0), test[0]);
        sim->run_until_stable(5);
        REQUIRE(circuit->read_pin(out->pin_id(0)) == test[1]);
    }
}

TEST_CASE("NandGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit_desc = lsim_context.create_user_circuit("main");
    REQUIRE(circuit_desc);

    auto in_0 = circuit_desc->add_connector_in("i0", 1);
    REQUIRE(in_0);
    auto in_1 = circuit_desc->add_connector_in("i1", 1);
    REQUIRE(in_1);

    auto nand_gate = circuit_desc->add_nand_gate(2);
    REQUIRE(nand_gate);

    auto out = circuit_desc->add_connector_out("out", 1);
    REQUIRE(out);

    circuit_desc->connect(in_0->pin_id(0), nand_gate->pin_id(0));
    circuit_desc->connect(in_1->pin_id(0), nand_gate->pin_id(1));
    circuit_desc->connect(nand_gate->pin_id(2), out->pin_id(0));

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

    auto circuit = circuit_desc->instantiate(sim);
    REQUIRE(circuit);

    sim->init();

    for (auto test : truth_table) {
        circuit->write_pin(in_0->pin_id(0), test[0]);
        circuit->write_pin(in_1->pin_id(0), test[1]);
        sim->run_until_stable(5);
        REQUIRE(circuit->read_pin(out->pin_id(0)) == test[2]);
    }
}

TEST_CASE("NorGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit_desc = lsim_context.create_user_circuit("main");
    REQUIRE(circuit_desc);

    auto in_0 = circuit_desc->add_connector_in("i0", 1);
    REQUIRE(in_0);
    auto in_1 = circuit_desc->add_connector_in("i1", 1);
    REQUIRE(in_1);

    auto nor_gate = circuit_desc->add_nor_gate(2);
    REQUIRE(nor_gate);

    auto out = circuit_desc->add_connector_out("out", 1);
    REQUIRE(out);

    circuit_desc->connect(in_0->pin_id(0), nor_gate->pin_id(0));
    circuit_desc->connect(in_1->pin_id(0), nor_gate->pin_id(1));
    circuit_desc->connect(nor_gate->pin_id(2), out->pin_id(0));

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

    auto circuit = circuit_desc->instantiate(sim);
    REQUIRE(circuit);

    sim->init();

    for (auto test : truth_table) {
        circuit->write_pin(in_0->pin_id(0), test[0]);
        circuit->write_pin(in_1->pin_id(0), test[1]);
        sim->run_until_stable(5);
        REQUIRE(circuit->read_pin(out->pin_id(0)) == test[2]);
    }
}

TEST_CASE("XorGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit_desc = lsim_context.create_user_circuit("main");
    REQUIRE(circuit_desc);

    auto in_0 = circuit_desc->add_connector_in("i0", 1);
    REQUIRE(in_0);
    auto in_1 = circuit_desc->add_connector_in("i1", 1);
    REQUIRE(in_1);

    auto xor_gate = circuit_desc->add_xor_gate();
    REQUIRE(xor_gate);

    auto out = circuit_desc->add_connector_out("out", 1);
    REQUIRE(out);

    circuit_desc->connect(in_0->pin_id(0), xor_gate->pin_id(0));
    circuit_desc->connect(in_1->pin_id(0), xor_gate->pin_id(1));
    circuit_desc->connect(xor_gate->pin_id(2), out->pin_id(0));

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

    auto circuit = circuit_desc->instantiate(sim);
    REQUIRE(circuit);

    sim->init();

    for (auto test : truth_table) {
        circuit->write_pin(in_0->pin_id(0), test[0]);
        circuit->write_pin(in_1->pin_id(0), test[1]);
        sim->run_until_stable(5);
        REQUIRE(circuit->read_pin(out->pin_id(0)) == test[2]);
    }
}

TEST_CASE("XnorGate", "[gate]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit_desc = lsim_context.create_user_circuit("main");
    REQUIRE(circuit_desc);

    auto in_0 = circuit_desc->add_connector_in("i0", 1);
    REQUIRE(in_0);
    auto in_1 = circuit_desc->add_connector_in("i1", 1);
    REQUIRE(in_1);

    auto xnor_gate = circuit_desc->add_xnor_gate();
    REQUIRE(xnor_gate);

    auto out = circuit_desc->add_connector_out("out", 1);
    REQUIRE(out);

    circuit_desc->connect(in_0->pin_id(0), xnor_gate->pin_id(0));
    circuit_desc->connect(in_1->pin_id(0), xnor_gate->pin_id(1));
    circuit_desc->connect(xnor_gate->pin_id(2), out->pin_id(0));

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

    auto circuit = circuit_desc->instantiate(sim);
    REQUIRE(circuit);

    sim->init();

    for (auto test : truth_table) {
        circuit->write_pin(in_0->pin_id(0), test[0]);
        circuit->write_pin(in_1->pin_id(0), test[1]);
        sim->run_until_stable(5);
        REQUIRE(circuit->read_pin(out->pin_id(0)) == test[2]);
    }
}