// speedtest_main.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// Performance analysis test harness 

#include "lsim_context.h"
#include "serialize.h"
#include <cstdio>
#include <cassert>
#include <chrono>

namespace {

const char *TEST_CIRCUIT = "examples/test_led.lsim";
const int CYCLE_COUNT = 5000000;

using namespace std::chrono;
steady_clock::time_point chrono_ref;

inline void chrono_reset() {
    chrono_ref = steady_clock::now();
}

inline double chrono_report() {
    duration<double> span = duration_cast<duration<double>>(steady_clock::now() - chrono_ref);
    return span.count();
}


} // unnamed namespace

int main() {
    lsim::LSimContext lsim_context;

    std::printf("--- setting up LSim\n");
    chrono_reset();
    lsim_context.add_folder("examples", "../examples");
    if (!lsim::deserialize_library(&lsim_context, lsim_context.user_library(), lsim_context.full_file_path(TEST_CIRCUIT).c_str())) {
        std::printf("!!! unable to load circuit (%s)\n", TEST_CIRCUIT);
        return -1;
    }
    std::printf("+++ done (%f seconds)\n", chrono_report());

    std::printf("--- instantiating circuit\n");
    chrono_reset();
    auto circuit_desc = lsim_context.user_library()->circuit_by_name("decimal display");
    auto circuit = circuit_desc->instantiate(lsim_context.sim());
    std::printf("+++ done (%f seconds)\n", chrono_report());

    std::printf("*** initializing simulator\n");
    lsim_context.sim()->init();

    std::printf("+++ running simulation for %d cycles\n", CYCLE_COUNT);
    chrono_reset();
    for (int i = 0; i < CYCLE_COUNT; ++i) {
        lsim_context.sim()->step();
    }
    double duration = chrono_report();
    std::printf("+++ done (%f seconds): %.2f Hz (%.2f kHz)\n", duration, CYCLE_COUNT / duration, CYCLE_COUNT / (duration * 1000));

    return 0;
}
