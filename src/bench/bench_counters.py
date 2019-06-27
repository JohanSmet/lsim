#!/usr/bin/env python3

import lsimpy
from bench_utils import *

def cycle_clock(sim, circuit):
    circuit.write_port("Clk", lsimpy.ValueTrue)
    sim.run_until_stable(2)
    circuit.write_port("Clk", lsimpy.ValueFalse)
    sim.run_until_stable(2)

def test_bin_counter_4b(lsim):
    print("*** running BinCounter 4b")
    sim = lsim.sim()

    circuit_desc = lsim.user_library().circuit_by_name('BinCounter 4b')

    pins_D = [circuit_desc.port_by_name("D[0]"),
              circuit_desc.port_by_name("D[1]"),
              circuit_desc.port_by_name("D[2]"),
              circuit_desc.port_by_name("D[3]")]

    pins_Y = [circuit_desc.port_by_name("Y[0]"),
              circuit_desc.port_by_name("Y[1]"),
              circuit_desc.port_by_name("Y[2]"),
              circuit_desc.port_by_name("Y[3]")]

    circuit = circuit_desc.instantiate(sim)
    sim.init()
    
    circuit.write_nibble(pins_D, 0)
    circuit.write_port("Load", lsimpy.ValueFalse)
    circuit.write_port("Clk",  lsimpy.ValueFalse)
    circuit.write_port("Res",  lsimpy.ValueTrue)
    circuit.write_port("En",   lsimpy.ValueTrue)
    sim.run_until_stable(2)
    circuit.write_port("Res",  lsimpy.ValueFalse)
    sim.run_until_stable(2)
    CHECK(circuit.read_nibble(pins_Y), 0, "reset")

    for i in range(1, 2**4):
        cycle_clock(sim, circuit)
        CHECK(circuit.read_nibble(pins_Y), i, "clock cycle")
        CHECK(circuit.read_port("RCO"), i == (2**4)-1, "")

    circuit.write_nibble(pins_D, 5)
    circuit.write_port("Load", lsimpy.ValueTrue)
    sim.run_until_stable(2)
    circuit.write_port("Load", lsimpy.ValueFalse)
    sim.run_until_stable(2)
    CHECK(circuit.read_nibble(pins_Y), 5, "after load")
    cycle_clock(sim, circuit)
    CHECK(circuit.read_nibble(pins_Y), 6, "increment")

def test_bin_counter_8b(lsim):
    print("*** running BinCounter 8b")
    sim = lsim.sim()

    circuit_desc = lsim.user_library().circuit_by_name('BinCounter 8b')

    pins_D = [circuit_desc.port_by_name("D[0]"),
              circuit_desc.port_by_name("D[1]"),
              circuit_desc.port_by_name("D[2]"),
              circuit_desc.port_by_name("D[3]"),
              circuit_desc.port_by_name("D[4]"),
              circuit_desc.port_by_name("D[5]"),
              circuit_desc.port_by_name("D[6]"),
              circuit_desc.port_by_name("D[7]")]

    pins_Y = [circuit_desc.port_by_name("Y[0]"),
              circuit_desc.port_by_name("Y[1]"),
              circuit_desc.port_by_name("Y[2]"),
              circuit_desc.port_by_name("Y[3]"),
              circuit_desc.port_by_name("Y[4]"),
              circuit_desc.port_by_name("Y[5]"),
              circuit_desc.port_by_name("Y[6]"),
              circuit_desc.port_by_name("Y[7]")]

    circuit = circuit_desc.instantiate(sim)
    sim.init()
    
    circuit.write_byte(pins_D, 0)
    circuit.write_port("Load", lsimpy.ValueFalse)
    circuit.write_port("Clk",  lsimpy.ValueFalse)
    circuit.write_port("Res",  lsimpy.ValueTrue)
    sim.run_until_stable(2)
    circuit.write_port("Res",  lsimpy.ValueFalse)
    sim.run_until_stable(2)
    CHECK(circuit.read_byte(pins_Y), 0, "reset")

    for i in range(1, 2**8):
        cycle_clock(sim, circuit)
        CHECK(circuit.read_byte(pins_Y), i, "clock cycle")
        CHECK(circuit.read_port("RCO"), i == (2**8)-1, "")

    circuit.write_byte(pins_D, 5)
    circuit.write_port("Load", lsimpy.ValueTrue)
    sim.run_until_stable(2)
    circuit.write_port("Load", lsimpy.ValueFalse)
    sim.run_until_stable(2)
    CHECK(circuit.read_byte(pins_Y), 5, "after load")
    cycle_clock(sim, circuit)
    CHECK(circuit.read_byte(pins_Y), 6, "increment")

def main():
    lsim = lsimpy.LSimContext()
    lsim.add_folder("examples", "../../examples")
   
    if (not lsim.load_user_library("examples/cpu_8bit/lib_counter.lsim")):
        print("Unable to load circuit\n")
        exit(-1)

    test_bin_counter_4b(lsim)
    test_bin_counter_8b(lsim)

    print_stats()

if __name__ == "__main__":
    main()