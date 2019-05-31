#include "catch.hpp"
#include "lsim_context.h"
#include "circuit_instance.h"

using namespace lsim;

TEST_CASE("PullResistor", "[extra]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit_desc = lsim_context.user_library()->create_circuit("main");
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