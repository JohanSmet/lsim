#include "catch.hpp"
#include "algebra.h"

using namespace lsim;
using namespace Catch::literals;

TEST_CASE ("Basic transform operations", "[algebra]") {

    auto transform_is_identity = [](const Transform &tf) {
        REQUIRE(tf.el(0,0) == 1.0_a);
        REQUIRE(tf.el(0,1) == 0.0_a);
        REQUIRE(tf.el(1,0) == 0.0_a);
        REQUIRE(tf.el(1,1) == 1.0_a);
        REQUIRE(tf.el(2,0) == 0.0_a);
        REQUIRE(tf.el(2,1) == 0.0_a);
    };

    // default constructor
    Transform trans_1;
    transform_is_identity(trans_1);

    // translate
    trans_1.translate({5.0f, -7.0f});
    REQUIRE(trans_1.el(0,0) == 1.0_a);
    REQUIRE(trans_1.el(0,1) == 0.0_a);
    REQUIRE(trans_1.el(1,0) == 0.0_a);
    REQUIRE(trans_1.el(1,1) == 1.0_a);
    REQUIRE(trans_1.el(2,0) == 5.0_a);
    REQUIRE(trans_1.el(2,1) == -7.0_a);

    // copy constructor
    Transform trans_2(trans_1);
    REQUIRE(trans_2.el(0,0) == 1.0_a);
    REQUIRE(trans_2.el(0,1) == 0.0_a);
    REQUIRE(trans_2.el(1,0) == 0.0_a);
    REQUIRE(trans_2.el(1,1) == 1.0_a);
    REQUIRE(trans_2.el(2,0) == 5.0_a);
    REQUIRE(trans_2.el(2,1) == -7.0_a);

    // reset
    trans_1.reset();
    transform_is_identity(trans_1);
}