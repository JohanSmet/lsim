#include "catch.hpp"
#include "lsim_context.h"
#include "gate.h"
#include "extra.h"

TEST_CASE("PullResistor", "[extra]") {

    LSimContext lsim_context;
    auto sim = lsim_context.sim();

    auto circuit = lsim_context.user_library()->create_circuit("main");
    REQUIRE(circuit);

    auto in = ConnectorInput(circuit, "in", 1, true);
    REQUIRE(in);

    auto out = ConnectorOutput(circuit, "out", 1);
    REQUIRE(out);

    auto pull = PullResistor(circuit, VALUE_TRUE);
    REQUIRE(pull);

    circuit->connect_pins(in->pin(0), out->pin(0));
    circuit->connect_pins(pull->pin(0), out->pin(0));

    sim->init(circuit);

    Value truth_table[][2] = {
        // in               out
        {VALUE_FALSE,     VALUE_FALSE},
        {VALUE_TRUE,      VALUE_TRUE},
        {VALUE_UNDEFINED, VALUE_TRUE}
    };

    for (const auto &test : truth_table) {
        in->write_pin(0, test[0]);
        sim->run_until_stable(5);
        REQUIRE(out->read_pin(0) == test[1]);
    }
}