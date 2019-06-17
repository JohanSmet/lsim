#!/usr/bin/env python3

import lsimpy
from bench_utils import *

def main():
    lsim = lsimpy.LSimContext()
    lsim.add_folder("examples", "../../examples")
    sim = lsim.sim()
   
    if (not lsim.load_user_library("examples/shifter.lsim")):
        print("Unable to load circuit\n")
        exit(-1)

    circuit_desc = lsim.user_library().circuit_by_name('barrel_8bit')

    pin_SHL = circuit_desc.port_by_name("Shl")
    pin_A = [circuit_desc.port_by_name("A[0]"),
             circuit_desc.port_by_name("A[1]"),
             circuit_desc.port_by_name("A[2]"),
             circuit_desc.port_by_name("A[3]"),
             circuit_desc.port_by_name("A[4]"),
             circuit_desc.port_by_name("A[5]"),
             circuit_desc.port_by_name("A[6]"),
             circuit_desc.port_by_name("A[7]")]
    pin_S = [circuit_desc.port_by_name("S[0]"),
             circuit_desc.port_by_name("S[1]"),
             circuit_desc.port_by_name("S[2]"),
             circuit_desc.port_by_name("S[3]")]
    pin_Y = [circuit_desc.port_by_name("Y[0]"),
             circuit_desc.port_by_name("Y[1]"),
             circuit_desc.port_by_name("Y[2]"),
             circuit_desc.port_by_name("Y[3]"),
             circuit_desc.port_by_name("Y[4]"),
             circuit_desc.port_by_name("Y[5]"),
             circuit_desc.port_by_name("Y[6]"),
             circuit_desc.port_by_name("Y[7]")]

    circuit = circuit_desc.instantiate(sim)
    sim.init()

    for shl in range(0, 2):
        circuit.write_pin(pin_SHL, lsimpy.ValueTrue if shl else lsimpy.ValueFalse)
        for a in range (0, 2**8):
            circuit.write_byte(pin_A, a)
            for s in range (0, 2**3):
                circuit.write_nibble(pin_S, s)
                sim.run_until_stable(5)

                result = circuit.read_byte(pin_Y)
                expected = (a << s if shl else a >> s) & (2**8-1)
                CHECK(result, expected, "SHL({}), A({}), S({})".format(shl,a,s))

    print_stats()

if __name__ == "__main__":
    main()
