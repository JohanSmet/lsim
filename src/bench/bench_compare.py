#!/usr/bin/env python3

import lsimpy
from bench_utils import *

def main():
    lsim = lsimpy.LSimContext()
    lsim.add_folder("examples", "../../examples")
    sim = lsim.sim()

    if (not lsim.load_user_library("examples/compare.lsim")):
        print("Unable to load circuit\n")
        exit(-1)

    circuit_desc = lsim.user_library().circuit_by_name("comp_8bit")
   
    pin_A = [circuit_desc.port_by_name(f"A[{i:}]") for i in range(0,8)]
    pin_B = [circuit_desc.port_by_name(f"B[{i:}]") for i in range(0,8)]
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
