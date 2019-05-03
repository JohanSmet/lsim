#!/usr/bin/env python3

import lsimpy

count_check = 0
count_failure = 0

def CHECK(was, expected, op_str):
    global count_check, count_failure
    count_check = count_check + 1
    if was != expected:
        count_failure = count_failure + 1
        print("FAILURE: {} = {} (expected {})".format(op_str, was, expected))

def print_stats():
    global count_check, count_failure
    print("======================================================")
    if count_failure == 0:
        print("All tests passed ({} checks executed)".format(count_check))
    else:
        print("{} out of {} checks failed!".format(count_failure, count_check))

def main():
    lsim = lsimpy.LSimContext()
    sim = lsim.sim()
   
    if (not lsimpy.load_logisim(lsim, "../../examples/compare.circ")):
        print("Unable to load circuit\n")
        exit(-1)

    pin_A = sim.connector_by_name("A")
    pin_B = sim.connector_by_name("B")
    pin_LT = sim.connector_by_name("LT")
    pin_EQ = sim.connector_by_name("EQ")
    pin_GT = sim.connector_by_name("GT")

    for a in range(0, 2**8):
        pin_A.change_data(a)
        for b in range(0, 2**8):
            pin_B.change_data(b)
            sim.run_until_stable(5)
            expected_LT = lsimpy.ValueTrue if a < b else lsimpy.ValueFalse
            expected_EQ = lsimpy.ValueTrue if a == b else lsimpy.ValueFalse
            expected_GT = lsimpy.ValueTrue if a > b else lsimpy.ValueFalse
            CHECK(pin_LT.read_pin(0), expected_LT, "{} < {}".format(a, b))
            CHECK(pin_EQ.read_pin(0), expected_EQ, "{} == {}".format(a, b))
            CHECK(pin_GT.read_pin(0), expected_GT, "{} > {}".format(a, b))


if __name__ == "__main__":
    main()