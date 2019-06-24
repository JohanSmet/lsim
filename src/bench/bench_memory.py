#!/usr/bin/env python3

import lsimpy
from bench_utils import *

BYTE_UNDEFINED = [lsimpy.ValueUndefined, lsimpy.ValueUndefined, lsimpy.ValueUndefined, lsimpy.ValueUndefined,
                  lsimpy.ValueUndefined, lsimpy.ValueUndefined, lsimpy.ValueUndefined, lsimpy.ValueUndefined]

def test_mem_cell(lsim):
    sim = lsim.sim()

    circuit_desc = lsim.user_library().circuit_by_name('mem_cell')

    pins_D = [circuit_desc.port_by_name("D[0]"),
              circuit_desc.port_by_name("D[1]"),
              circuit_desc.port_by_name("D[2]"),
              circuit_desc.port_by_name("D[3]"),
              circuit_desc.port_by_name("D[4]"),
              circuit_desc.port_by_name("D[5]"),
              circuit_desc.port_by_name("D[6]"),
              circuit_desc.port_by_name("D[7]")]

    circuit = circuit_desc.instantiate(sim)
    sim.init()

    for d in range(0, 2**8):
        circuit.write_byte(pins_D, d)
        circuit.write_port("WE", lsimpy.ValueTrue)
        circuit.write_port("CE", lsimpy.ValueTrue)
        sim.run_until_stable(2)
        circuit.write_port("WE", lsimpy.ValueFalse)
        circuit.write_port("CE", lsimpy.ValueFalse)
        sim.run_until_stable(2)
        circuit.write_pins(pins_D, BYTE_UNDEFINED)
        sim.run_until_stable(2)
        circuit.write_port("OE", lsimpy.ValueTrue)
        circuit.write_port("CE", lsimpy.ValueTrue)
        sim.run_until_stable(2)
        CHECK(circuit.read_byte(pins_D), d, "D{}".format(d))
        circuit.write_port("OE", lsimpy.ValueFalse)
        circuit.write_port("CE", lsimpy.ValueFalse)
        sim.run_until_stable(2)

def test_ram_8byte(lsim):
    sim = lsim.sim()

    circuit_desc = lsim.user_library().circuit_by_name('ram_8byte')

    pins_D = [circuit_desc.port_by_name("D[0]"),
              circuit_desc.port_by_name("D[1]"),
              circuit_desc.port_by_name("D[2]"),
              circuit_desc.port_by_name("D[3]"),
              circuit_desc.port_by_name("D[4]"),
              circuit_desc.port_by_name("D[5]"),
              circuit_desc.port_by_name("D[6]"),
              circuit_desc.port_by_name("D[7]")]

    pins_Addr = [circuit_desc.port_by_name("Addr[0]"),
                 circuit_desc.port_by_name("Addr[1]"),
                 circuit_desc.port_by_name("Addr[2]")]

    circuit = circuit_desc.instantiate(sim)
    sim.init()

    for d in range(0, 2**8):

        for addr in range(0, 2**3):
            circuit.write_pins(pins_Addr, addr)
            circuit.write_pins(pins_D, (d + addr) % 256)
            circuit.write_port("WE", lsimpy.ValueTrue)
            circuit.write_port("CE", lsimpy.ValueTrue)
            sim.run_until_stable(2)
            circuit.write_port("WE", lsimpy.ValueFalse)
            circuit.write_port("CE", lsimpy.ValueFalse)
            sim.run_until_stable(2)
            circuit.write_pins(pins_D, BYTE_UNDEFINED)
            sim.run_until_stable(2)

        for addr in range(0, 2**3):
            circuit.write_pins(pins_Addr, addr)
            circuit.write_port("OE", lsimpy.ValueTrue)
            circuit.write_port("CE", lsimpy.ValueTrue)
            sim.run_until_stable(2)
            expected = (d + addr) % 256
            CHECK(circuit.read_byte(pins_D), expected, "D({}) Addr({})".format(d, addr))
            circuit.write_port("OE", lsimpy.ValueFalse)
            circuit.write_port("CE", lsimpy.ValueFalse)
            sim.run_until_stable(2)

def test_ram_64byte(lsim):
    sim = lsim.sim()

    circuit_desc = lsim.user_library().circuit_by_name('ram_64byte')

    pins_D = [circuit_desc.port_by_name("D[0]"),
              circuit_desc.port_by_name("D[1]"),
              circuit_desc.port_by_name("D[2]"),
              circuit_desc.port_by_name("D[3]"),
              circuit_desc.port_by_name("D[4]"),
              circuit_desc.port_by_name("D[5]"),
              circuit_desc.port_by_name("D[6]"),
              circuit_desc.port_by_name("D[7]")]

    pins_Addr = [circuit_desc.port_by_name("Addr[0]"),
                 circuit_desc.port_by_name("Addr[1]"),
                 circuit_desc.port_by_name("Addr[2]"),
                 circuit_desc.port_by_name("Addr[3]"),
                 circuit_desc.port_by_name("Addr[4]"),
                 circuit_desc.port_by_name("Addr[5]")]

    circuit = circuit_desc.instantiate(sim)
    sim.init()

    for d in range(0, 2**8):

        for addr in range(0, 2**6):
            circuit.write_pins(pins_Addr, addr)
            circuit.write_pins(pins_D, (d + addr) % 256)
            circuit.write_port("WE", lsimpy.ValueTrue)
            circuit.write_port("CE", lsimpy.ValueTrue)
            sim.run_until_stable(2)
            circuit.write_port("WE", lsimpy.ValueFalse)
            circuit.write_port("CE", lsimpy.ValueFalse)
            sim.run_until_stable(2)
            circuit.write_pins(pins_D, BYTE_UNDEFINED)
            sim.run_until_stable(2)

        for addr in range(0, 2**6):
            circuit.write_pins(pins_Addr, addr)
            circuit.write_port("OE", lsimpy.ValueTrue)
            circuit.write_port("CE", lsimpy.ValueTrue)
            sim.run_until_stable(2)
            expected = (d + addr) % 256
            CHECK(circuit.read_byte(pins_D), expected, "D({}) Addr({})".format(d, addr))
            circuit.write_port("OE", lsimpy.ValueFalse)
            circuit.write_port("CE", lsimpy.ValueFalse)
            sim.run_until_stable(2)

def main():
    lsim = lsimpy.LSimContext()
    lsim.add_folder("examples", "../../examples")
   
    if (not lsim.load_user_library("examples/cpu_8bit/lib_memory.lsim")):
        print("Unable to load circuit\n")
        exit(-1)

    test_mem_cell(lsim)
    test_ram_8byte(lsim)
    test_ram_64byte(lsim)

    print_stats()

if __name__ == "__main__":
    main()