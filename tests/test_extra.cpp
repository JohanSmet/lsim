#include "catch.hpp"
#include "simulator.h"
#include "circuit.h"
#include "gate.h"
#include "extra.h"

TEST_CASE("PullResistor", "[extra]") {

    auto sim = std::make_unique<Simulator>();
    REQUIRE (sim);

    auto circuit = sim->create_circuit("main");
    REQUIRE(circuit);

    auto in = circuit->create_component<Connector>("in", 1, Connector::INPUT);
    REQUIRE(in);
    in->set_tristate(true);

    auto out = circuit->create_component<Connector>("out", 1, Connector::OUTPUT);
    REQUIRE(out);

    auto pull = circuit->create_component<PullResistor>(VALUE_TRUE);
    REQUIRE(pull);

    circuit->connect_pins(in->pin(0), out->pin(0));
    circuit->connect_pins(pull->pin(0), out->pin(0));

    sim->init();

    Value truth_table[][2] = {
        // in               out
        {VALUE_FALSE,     VALUE_FALSE},
        {VALUE_TRUE,      VALUE_TRUE},
        {VALUE_UNDEFINED, VALUE_TRUE}
    };

    for (const auto &test : truth_table) {
        in->change_data(test[0]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == test[1]);
    }
}