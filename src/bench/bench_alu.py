#!/usr/bin/env python3

import lsimpy
from bench_utils import *

pins_A = []
pins_B = []
pins_Y = []
pins_Instr = []
pin_CE = 0
pin_OE = 0

instr_ADD = 0b0000
instr_SUB = 0b0001
instr_SHR = 0b0010
instr_SHL = 0b0011
instr_AND = 0b0100
instr_OR  = 0b0110
instr_XOR = 0b1000

def check_addition(circuit, sim):
    print("*** running addition")

    circuit.write_pin(pin_CE, lsimpy.ValueFalse)
    circuit.write_pin(pin_OE, lsimpy.ValueFalse)
    sim.run_until_stable(2)
    circuit.write_nibble(pins_Instr, instr_ADD)
    sim.run_until_stable(2)

    for a in range(0, 2**8):
        circuit.write_byte(pins_A, a)

        for b in range(0, 2**8):
            circuit.write_byte(pins_B, b)
            sim.run_until_stable(2)
            circuit.write_pin(pin_CE, lsimpy.ValueTrue)
            circuit.write_pin(pin_OE, lsimpy.ValueTrue)
            sim.run_until_stable(2)

            expected_y = a + b
            expected_co = 0
            if expected_y >= 2**8:
                expected_co = 1
                expected_y = expected_y - 2**8

            CHECK(circuit.read_port("CF"), expected_co, "{} + {}".format(a, b))
            CHECK(circuit.read_port("ZF"), expected_y == 0, "{} + {}".format(a, b))
            CHECK(circuit.read_byte(pins_Y), expected_y, "{} + {}".format(a, b))

            circuit.write_pin(pin_CE, lsimpy.ValueFalse)
            circuit.write_pin(pin_OE, lsimpy.ValueFalse)
            sim.run_until_stable(2)

def check_subtraction(circuit, sim):
    print("*** running subtraction")

    circuit.write_pin(pin_CE, lsimpy.ValueFalse)
    circuit.write_pin(pin_OE, lsimpy.ValueFalse)
    sim.run_until_stable(2)
    circuit.write_nibble(pins_Instr, instr_SUB)
    sim.run_until_stable(2)

    for a in range(0, 2**8):
        circuit.write_byte(pins_A, a)

        for b in range(0, 2**8):
            circuit.write_byte(pins_B, b)
            sim.run_until_stable(2)
            circuit.write_pin(pin_CE, lsimpy.ValueTrue)
            circuit.write_pin(pin_OE, lsimpy.ValueTrue)
            sim.run_until_stable(2)

            expected_y = a - b
            expected_co = 0
            if expected_y < 0:
                expected_co = 1
                expected_y = expected_y + 2**8

            CHECK(circuit.read_port("CF"), expected_co, "{} - {}".format(a, b))
            CHECK(circuit.read_port("ZF"), expected_y == 0, "{} - {}".format(a, b))
            CHECK(circuit.read_byte(pins_Y), expected_y, "{} - {}".format(a, b))

            circuit.write_pin(pin_CE, lsimpy.ValueFalse)
            circuit.write_pin(pin_OE, lsimpy.ValueFalse)
            sim.run_until_stable(2)

def check_shift_right(circuit, sim):
    print("*** running shift right")

    circuit.write_pin(pin_CE, lsimpy.ValueFalse)
    circuit.write_pin(pin_OE, lsimpy.ValueFalse)
    sim.run_until_stable(2)
    circuit.write_nibble(pins_Instr, instr_SHR)
    sim.run_until_stable(2)

    for a in range (0, 2**8):
        circuit.write_byte(pins_A, a)
        for b in range (0, 2**4):
            circuit.write_byte(pins_B, b)
            sim.run_until_stable(2)
            circuit.write_pin(pin_CE, lsimpy.ValueTrue)
            circuit.write_pin(pin_OE, lsimpy.ValueTrue)
            sim.run_until_stable(2)

            result = circuit.read_byte(pins_Y)
            expected = (a >> b) & (2**8-1)
            CHECK(result, expected, "A({}), B({})".format(a,b))

            circuit.write_pin(pin_CE, lsimpy.ValueFalse)
            circuit.write_pin(pin_OE, lsimpy.ValueFalse)
            sim.run_until_stable(2)

def check_shift_left(circuit, sim):
    print("*** running shift left")

    circuit.write_pin(pin_CE, lsimpy.ValueFalse)
    circuit.write_pin(pin_OE, lsimpy.ValueFalse)
    sim.run_until_stable(2)
    circuit.write_nibble(pins_Instr, instr_SHL)
    sim.run_until_stable(2)

    for a in range (0, 2**8):
        circuit.write_byte(pins_A, a)
        for b in range (0, 2**4):
            circuit.write_byte(pins_B, b)
            sim.run_until_stable(2)
            circuit.write_pin(pin_CE, lsimpy.ValueTrue)
            circuit.write_pin(pin_OE, lsimpy.ValueTrue)
            sim.run_until_stable(2)

            result = circuit.read_byte(pins_Y)
            expected = (a << b) & (2**8-1)
            CHECK(result, expected, "A({}), B({})".format(a,b))

            circuit.write_pin(pin_CE, lsimpy.ValueFalse)
            circuit.write_pin(pin_OE, lsimpy.ValueFalse)
            sim.run_until_stable(2)

def check_and(circuit, sim):
    print("*** running and")

    circuit.write_pin(pin_CE, lsimpy.ValueFalse)
    circuit.write_pin(pin_OE, lsimpy.ValueFalse)
    sim.run_until_stable(2)
    circuit.write_nibble(pins_Instr, instr_AND)
    sim.run_until_stable(2)

    for a in range(0, 2**8):
        circuit.write_byte(pins_A, a)

        for b in range(0, 2**8):
            circuit.write_byte(pins_B, b)
            sim.run_until_stable(2)
            circuit.write_pin(pin_CE, lsimpy.ValueTrue)
            circuit.write_pin(pin_OE, lsimpy.ValueTrue)
            sim.run_until_stable(2)

            expected_y = a & b
            CHECK(circuit.read_byte(pins_Y), expected_y, "{} + {}".format(a, b))

            circuit.write_pin(pin_CE, lsimpy.ValueFalse)
            circuit.write_pin(pin_OE, lsimpy.ValueFalse)
            sim.run_until_stable(2)

def check_or(circuit, sim):
    print("*** running or")

    circuit.write_pin(pin_CE, lsimpy.ValueFalse)
    circuit.write_pin(pin_OE, lsimpy.ValueFalse)
    sim.run_until_stable(2)
    circuit.write_nibble(pins_Instr, instr_OR)
    sim.run_until_stable(2)

    for a in range(0, 2**8):
        circuit.write_byte(pins_A, a)

        for b in range(0, 2**8):
            circuit.write_byte(pins_B, b)
            sim.run_until_stable(2)
            circuit.write_pin(pin_CE, lsimpy.ValueTrue)
            circuit.write_pin(pin_OE, lsimpy.ValueTrue)
            sim.run_until_stable(2)

            expected_y = a | b
            CHECK(circuit.read_byte(pins_Y), expected_y, "{} + {}".format(a, b))

            circuit.write_pin(pin_CE, lsimpy.ValueFalse)
            circuit.write_pin(pin_OE, lsimpy.ValueFalse)
            sim.run_until_stable(2)

def check_xor(circuit, sim):
    print("*** running xor")

    circuit.write_pin(pin_CE, lsimpy.ValueFalse)
    circuit.write_pin(pin_OE, lsimpy.ValueFalse)
    sim.run_until_stable(2)
    circuit.write_nibble(pins_Instr, instr_XOR)
    sim.run_until_stable(2)

    for a in range(0, 2**8):
        circuit.write_byte(pins_A, a)

        for b in range(0, 2**8):
            circuit.write_byte(pins_B, b)
            sim.run_until_stable(2)
            circuit.write_pin(pin_CE, lsimpy.ValueTrue)
            circuit.write_pin(pin_OE, lsimpy.ValueTrue)
            sim.run_until_stable(2)

            expected_y = a ^ b
            CHECK(circuit.read_byte(pins_Y), expected_y, "{} + {}".format(a, b))

            circuit.write_pin(pin_CE, lsimpy.ValueFalse)
            circuit.write_pin(pin_OE, lsimpy.ValueFalse)
            sim.run_until_stable(2)

def main():
    global pins_A, pins_B, pins_Y, pins_Instr, pin_CE, pin_OE

    lsim = lsimpy.LSimContext()
    lsim.add_folder("examples", "../../examples")
    sim = lsim.sim()

    if (not lsim.load_user_library("examples/cpu_8bit/alu.lsim")):
        print("Unable to load circuit\n")
        exit(-1)

    circuit_desc = lsim.user_library().circuit_by_name("alu")

    pins_A = [circuit_desc.port_by_name(f"A[{i:}]") for i in range(0,8)]
    pins_B = [circuit_desc.port_by_name(f"B[{i:}]") for i in range(0,8)]
    pins_Y = [circuit_desc.port_by_name(f"Y[{i:}]") for i in range(0,8)]
    pins_Instr = [circuit_desc.port_by_name(f"Instr[{i:}]") for i in range (0,4)]
    pin_CE = circuit_desc.port_by_name("CE")
    pin_OE = circuit_desc.port_by_name("OE")

    circuit = circuit_desc.instantiate(sim)
    sim.init()

    circuit.write_port("CE", lsimpy.ValueFalse)
    circuit.write_port("OE", lsimpy.ValueFalse)
    circuit.write_nibble(pins_Instr, 0)
    circuit.write_byte(pins_A, 0)
    circuit.write_byte(pins_B, 0)

    check_addition(circuit, sim)
    check_subtraction(circuit, sim)
    check_shift_right(circuit, sim)
    check_shift_left(circuit, sim)
    check_and(circuit, sim)
    check_or(circuit, sim)
    check_xor(circuit, sim)

    print_stats()

if __name__ == "__main__":
    main()