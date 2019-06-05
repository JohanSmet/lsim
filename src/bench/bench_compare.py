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

    if (not lsim.load_user_library("../../examples/compare.lsim")):
        print("Unable to load circuit\n")
        exit(-1)

    circuit_desc = lsim.user_library().circuit_by_name("comp_8bit")
   
    pin_A = []
    pin_B = []
    for i in range(0,8):
        pin_A.append(circuit_desc.port_by_name("A[{}]".format(i)))
        pin_B.append(circuit_desc.port_by_name("B[{}]".format(i)))

    pin_LT = circuit_desc.port_by_name("LT")
    pin_EQ = circuit_desc.port_by_name("EQ")
    pin_GT = circuit_desc.port_by_name("GT")

    circuit = circuit_desc.instantiate(sim)
    sim.init()

    for a in range(0, 2**8):
        circuit.write_byte(pin_A, a)
        for b in range(0, 2**8):
            circuit.write_byte(pin_B, b)
            sim.run_until_stable(5)
            expected_LT = lsimpy.ValueTrue if a < b else lsimpy.ValueFalse
            expected_EQ = lsimpy.ValueTrue if a == b else lsimpy.ValueFalse
            expected_GT = lsimpy.ValueTrue if a > b else lsimpy.ValueFalse
            CHECK(circuit.read_pin(pin_LT), expected_LT, "{} < {}".format(a, b))
            CHECK(circuit.read_pin(pin_EQ), expected_EQ, "{} == {}".format(a, b))
            CHECK(circuit.read_pin(pin_GT), expected_GT, "{} > {}".format(a, b))

    print_stats()


if __name__ == "__main__":
    main()