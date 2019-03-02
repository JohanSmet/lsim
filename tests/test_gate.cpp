#include "catch.hpp"
#include "circuit.h"
#include "gate.h"

TEST_CASE("Buffer", "[gate]") {

    auto circuit = std::make_unique<Circuit>();
    REQUIRE(circuit);

    auto in = circuit->create_component<Connector>(8);
    REQUIRE(in);

    auto out = circuit->create_component<Connector>(8);
    REQUIRE(out);

    auto buffer = circuit->create_component<Buffer>(8);
    REQUIRE(buffer);

    for (int idx = 0; idx < 8; ++idx) {
        circuit->connect_pins(in->pin(idx), buffer->pin(idx));
        circuit->connect_pins(buffer->pin(idx + 8), out->pin(idx));
    }

    circuit->simulation_init();
    REQUIRE(circuit->read_value(out->pin(0)) == VALUE_UNDEFINED);

    // buffer takes a cycle to change output
    in->change_data(VALUE_TRUE);
    circuit->simulation_tick();
    REQUIRE(circuit->read_value(out->pin(0)) == VALUE_UNDEFINED);
    circuit->simulation_tick();
    REQUIRE(circuit->read_value(out->pin(0)) == VALUE_TRUE);
    REQUIRE(circuit->read_value(out->pin(1)) == VALUE_FALSE);
    REQUIRE(circuit->read_value(out->pin(2)) == VALUE_FALSE);

    in->change_data(VALUE_TRUE << 2 | VALUE_FALSE);
    circuit->simulation_tick();
    REQUIRE(circuit->read_value(out->pin(0)) == VALUE_TRUE);
    REQUIRE(circuit->read_value(out->pin(1)) == VALUE_FALSE);
    REQUIRE(circuit->read_value(out->pin(2)) == VALUE_FALSE);
    circuit->simulation_tick();
    REQUIRE(circuit->read_value(out->pin(0)) == VALUE_FALSE);
    REQUIRE(circuit->read_value(out->pin(1)) == VALUE_FALSE);
    REQUIRE(circuit->read_value(out->pin(2)) == VALUE_TRUE);
}

TEST_CASE("AndGate", "[gate]") {

    auto circuit = std::make_unique<Circuit>();
    REQUIRE(circuit);

    auto in_0 = circuit->create_component<Constant>(1, VALUE_FALSE);
    REQUIRE(in_0);
    auto in_1 = circuit->create_component<Constant>(1, VALUE_TRUE);
    REQUIRE(in_1);

    auto and_gate = circuit->create_component<AndGate>(2);
    REQUIRE(and_gate);

    auto out = circuit->create_component<Connector>(1);
    REQUIRE(out);

    circuit->simulation_init();
    circuit->connect_pins(and_gate->pin(2), out->pin(0));

    SECTION("all inputs are false") {
        circuit->connect_pins(in_0->pin(0), and_gate->pin(0));
        circuit->connect_pins(in_0->pin(0), and_gate->pin(1));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_FALSE);
    }

    SECTION("all inputs are true") {
        circuit->connect_pins(in_1->pin(0), and_gate->pin(0));
        circuit->connect_pins(in_1->pin(0), and_gate->pin(1));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_TRUE);
    }

    SECTION("first input is true, second is false") {
        circuit->connect_pins(in_1->pin(0), and_gate->pin(0));
        circuit->connect_pins(in_0->pin(0), and_gate->pin(1));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_FALSE);
    }

    SECTION("first input is false, second is true") {
        circuit->connect_pins(in_0->pin(0), and_gate->pin(0));
        circuit->connect_pins(in_1->pin(0), and_gate->pin(1));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_FALSE);
    }
}

TEST_CASE("OrGate", "[gate]") {

    auto circuit = std::make_unique<Circuit>();
    REQUIRE(circuit);

    auto in_0 = circuit->create_component<Constant>(1, VALUE_FALSE);
    REQUIRE(in_0);
    auto in_1 = circuit->create_component<Constant>(1, VALUE_TRUE);
    REQUIRE(in_1);

    auto or_gate = circuit->create_component<OrGate>(2);
    REQUIRE(or_gate);

    auto out = circuit->create_component<Connector>(1);
    REQUIRE(out);

    circuit->simulation_init();
    circuit->connect_pins(or_gate->pin(2), out->pin(0));

    SECTION("all inputs are false") {
        circuit->connect_pins(in_0->pin(0), or_gate->pin(0));
        circuit->connect_pins(in_0->pin(0), or_gate->pin(1));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_FALSE);
    }

    SECTION("all inputs are true") {
        circuit->connect_pins(in_1->pin(0), or_gate->pin(0));
        circuit->connect_pins(in_1->pin(0), or_gate->pin(1));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_TRUE);
    }

    SECTION("first input is true, second is false") {
        circuit->connect_pins(in_1->pin(0), or_gate->pin(0));
        circuit->connect_pins(in_0->pin(0), or_gate->pin(1));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_TRUE);
    }

    SECTION("first input is false, second is true") {
        circuit->connect_pins(in_0->pin(0), or_gate->pin(0));
        circuit->connect_pins(in_1->pin(0), or_gate->pin(1));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_TRUE);
    }
}

TEST_CASE("NotGate", "[gate]") {

    auto circuit = std::make_unique<Circuit>();
    REQUIRE(circuit);

    auto in_0 = circuit->create_component<Constant>(1, VALUE_FALSE);
    REQUIRE(in_0);
    auto in_1 = circuit->create_component<Constant>(1, VALUE_TRUE);
    REQUIRE(in_1);

    auto not_gate = circuit->create_component<NotGate>();
    REQUIRE(not_gate);

    auto out = circuit->create_component<Connector>(1);
    REQUIRE(out);

    circuit->simulation_init();
    circuit->connect_pins(not_gate->pin(1), out->pin(0));

    SECTION("input is false") {
        circuit->connect_pins(in_0->pin(0), not_gate->pin(0));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_TRUE);
    }

    SECTION("input is true") {
        circuit->connect_pins(in_1->pin(0), not_gate->pin(0));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_FALSE);
    }
}

TEST_CASE("NandGate", "[gate]") {

    auto circuit = std::make_unique<Circuit>();
    REQUIRE(circuit);

    auto in_0 = circuit->create_component<Constant>(1, VALUE_FALSE);
    REQUIRE(in_0);
    auto in_1 = circuit->create_component<Constant>(1, VALUE_TRUE);
    REQUIRE(in_1);

    auto nand_gate = circuit->create_component<NandGate>(2);
    REQUIRE(nand_gate);

    auto out = circuit->create_component<Connector>(1);
    REQUIRE(out);

    circuit->simulation_init();
    circuit->connect_pins(nand_gate->pin(2), out->pin(0));

    SECTION("all inputs are false") {
        circuit->connect_pins(in_0->pin(0), nand_gate->pin(0));
        circuit->connect_pins(in_0->pin(0), nand_gate->pin(1));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_TRUE);
    }

    SECTION("all inputs are true") {
        circuit->connect_pins(in_1->pin(0), nand_gate->pin(0));
        circuit->connect_pins(in_1->pin(0), nand_gate->pin(1));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_FALSE);
    }

    SECTION("first input is true, second is false") {
        circuit->connect_pins(in_1->pin(0), nand_gate->pin(0));
        circuit->connect_pins(in_0->pin(0), nand_gate->pin(1));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_TRUE);
    }

    SECTION("first input is false, second is true") {
        circuit->connect_pins(in_0->pin(0), nand_gate->pin(0));
        circuit->connect_pins(in_1->pin(0), nand_gate->pin(1));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_TRUE);
    }
}

TEST_CASE("NorGate", "[gate]") {

    auto circuit = std::make_unique<Circuit>();
    REQUIRE(circuit);

    auto in_0 = circuit->create_component<Constant>(1, VALUE_FALSE);
    REQUIRE(in_0);
    auto in_1 = circuit->create_component<Constant>(1, VALUE_TRUE);
    REQUIRE(in_1);

    auto nor_gate = circuit->create_component<NorGate>(2);
    REQUIRE(nor_gate);

    auto out = circuit->create_component<Connector>(1);
    REQUIRE(out);

    circuit->simulation_init();
    circuit->connect_pins(nor_gate->pin(2), out->pin(0));

    SECTION("all inputs are false") {
        circuit->connect_pins(in_0->pin(0), nor_gate->pin(0));
        circuit->connect_pins(in_0->pin(0), nor_gate->pin(1));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_TRUE);
    }

    SECTION("all inputs are true") {
        circuit->connect_pins(in_1->pin(0), nor_gate->pin(0));
        circuit->connect_pins(in_1->pin(0), nor_gate->pin(1));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_FALSE);
    }

    SECTION("first input is true, second is false") {
        circuit->connect_pins(in_1->pin(0), nor_gate->pin(0));
        circuit->connect_pins(in_0->pin(0), nor_gate->pin(1));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_FALSE);
    }

    SECTION("first input is false, second is true") {
        circuit->connect_pins(in_0->pin(0), nor_gate->pin(0));
        circuit->connect_pins(in_1->pin(0), nor_gate->pin(1));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_FALSE);
    }
}

TEST_CASE("XorGate", "[gate]") {

    auto circuit = std::make_unique<Circuit>();
    REQUIRE(circuit);

    auto in_0 = circuit->create_component<Constant>(1, VALUE_FALSE);
    REQUIRE(in_0);
    auto in_1 = circuit->create_component<Constant>(1, VALUE_TRUE);
    REQUIRE(in_1);

    auto xor_gate = circuit->create_component<XorGate>();
    REQUIRE(xor_gate);

    auto out = circuit->create_component<Connector>(1);
    REQUIRE(out);

    circuit->simulation_init();
    circuit->connect_pins(xor_gate->pin(2), out->pin(0));

    SECTION("all inputs are false") {
        circuit->connect_pins(in_0->pin(0), xor_gate->pin(0));
        circuit->connect_pins(in_0->pin(0), xor_gate->pin(1));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_FALSE);
    }

    SECTION("all inputs are true") {
        circuit->connect_pins(in_1->pin(0), xor_gate->pin(0));
        circuit->connect_pins(in_1->pin(0), xor_gate->pin(1));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_FALSE);
    }

    SECTION("first input is true, second is false") {
        circuit->connect_pins(in_1->pin(0), xor_gate->pin(0));
        circuit->connect_pins(in_0->pin(0), xor_gate->pin(1));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_TRUE);
    }

    SECTION("first input is false, second is true") {
        circuit->connect_pins(in_0->pin(0), xor_gate->pin(0));
        circuit->connect_pins(in_1->pin(0), xor_gate->pin(1));

        circuit->simulation_until_pin_change(out->pin(0));
        REQUIRE(circuit->read_value(out->pin(0)) == VALUE_TRUE);
    }
}
