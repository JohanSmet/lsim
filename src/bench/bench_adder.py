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

    if (not lsim.load_user_library("../../examples/adder.lsim")):
        print("Unable to load circuit\n")
        exit(-1)

    circuit_desc = lsim.user_library().circuit_by_name("adder_4bit")

    pin_Ci = circuit_desc.port_by_name("Ci")
    pin_A = [ circuit_desc.port_by_name("A0"),
              circuit_desc.port_by_name("A1"),
              circuit_desc.port_by_name("A2"),
              circuit_desc.port_by_name("A3")]
    pin_B = [ circuit_desc.port_by_name("B0"),
              circuit_desc.port_by_name("B1"),
              circuit_desc.port_by_name("B2"),
              circuit_desc.port_by_name("B3")]
    pin_Y = [ circuit_desc.port_by_name("Y0"),
              circuit_desc.port_by_name("Y1"),
              circuit_desc.port_by_name("Y2"),
              circuit_desc.port_by_name("Y3")]
    pin_Co = circuit_desc.port_by_name("Co")

    circuit = circuit_desc.instantiate(sim)
    sim.init()

    for ci in [0, 1]:
        circuit.write_pin(pin_Ci, lsimpy.ValueTrue if ci else lsimpy.ValueFalse)

        for a in range(0, 2**4):
            circuit.write_nibble(pin_A, a)

            for b in range(0, 2**4):
                circuit.write_nibble(pin_B, b)
                sim.run_until_stable(5)

                expected_y = a + b + ci
                expected_co = 0
                if expected_y >= 2**4:
                    expected_co = 1
                    expected_y = expected_y - 2**4

                CHECK(circuit.read_pin(pin_Co), expected_co, "{} + {} + {}".format(ci, a, b))
                CHECK(circuit.read_nibble(pin_Y), expected_y, "{} + {} + {}".format(ci, a, b))
   
    print_stats()

if __name__ == "__main__":
    main()