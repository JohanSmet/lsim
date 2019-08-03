#!/usr/bin/env python3

import lsimpy
from bench_utils import *

lsim = lsimpy.LSimContext()

HIGH = lsimpy.ValueTrue
LOW = lsimpy.ValueFalse
NONE = lsimpy.ValueUndefined

def test_sr_latch():
    truth_table = [
        [{'R': HIGH, 'S': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'R': LOW,  'S': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'R': LOW,  'S': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'R': LOW,  'S': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'R': HIGH, 'S': HIGH}, {'Q': LOW,  '/Q': LOW}]
    ]

    run_thruth_table(lsim, "S-R Latch", truth_table)

def test_gated_sr_latch():
    truth_table = [
        [{'En': HIGH, 'R': HIGH, 'S': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'En': LOW,  'R': HIGH, 'S': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'En': LOW,  'R': LOW,  'S': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'En': LOW,  'R': LOW,  'S': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'En': HIGH, 'R': LOW,  'S': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'En': LOW,  'R': LOW,  'S': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'En': HIGH, 'R': LOW,  'S': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'En': HIGH, 'R': HIGH, 'S': LOW},  {'Q': LOW,  '/Q': HIGH}]
    ]

    run_thruth_table(lsim, "Gated S-R Latch", truth_table)

def test_d_latch():
    truth_table = [
        [{'En': HIGH, 'D': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'En': LOW,  'D': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'En': LOW,  'D': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'En': LOW,  'D': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'En': HIGH, 'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'En': LOW,  'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'En': HIGH, 'D': LOW},  {'Q': LOW,  '/Q': HIGH}]
    ]

    run_thruth_table(lsim, "D Latch", truth_table)

def test_async_sr_latch():
    truth_table = [
        [{'En': LOW,  'Res': HIGH, 'Pre': LOW,  'R': HIGH, 'S': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'En': LOW,  'Res': LOW,  'Pre': HIGH, 'R': HIGH, 'S': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'En': LOW,  'Res': HIGH, 'Pre': LOW,  'R': HIGH, 'S': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'En': LOW,  'Res': LOW,  'Pre': LOW,  'R': HIGH, 'S': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'En': LOW,  'Res': LOW,  'Pre': LOW,  'R': LOW,  'S': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'En': LOW,  'Res': LOW,  'Pre': LOW,  'R': LOW,  'S': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'En': HIGH, 'Res': LOW,  'Pre': LOW,  'R': LOW,  'S': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'En': LOW,  'Res': LOW,  'Pre': LOW,  'R': LOW,  'S': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'En': HIGH, 'Res': LOW,  'Pre': LOW,  'R': LOW,  'S': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'En': HIGH, 'Res': LOW,  'Pre': LOW,  'R': HIGH, 'S': LOW},  {'Q': LOW,  '/Q': HIGH}]
    ]

    run_thruth_table(lsim, "Async S-R Latch", truth_table)

def test_sr_flipflop():
    truth_table = [
        [{'Clk': LOW,  'R': HIGH, 'S': LOW},  {}],
        [{'Clk': HIGH, 'R': HIGH, 'S': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'R': LOW,  'S': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'R': LOW,  'S': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'R': LOW,  'S': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'R': HIGH, 'S': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'R': LOW,  'S': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'R': LOW,  'S': HIGH}, {'Q': HIGH, '/Q': LOW}]
    ]

    run_thruth_table(lsim, "S-R FlipFlop", truth_table)

def test_d_flipflop():
    truth_table = [
        [{'Clk': LOW,  'D': LOW},  {}],
        [{'Clk': LOW,  'D': LOW},  {}],
        [{'Clk': LOW,  'D': HIGH}, {}],
        [{'Clk': HIGH, 'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'D': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'D': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'D': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'D': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'D': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'D': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'D': LOW},  {'Q': HIGH, '/Q': LOW}]
    ]

    run_thruth_table(lsim, "D FlipFlop", truth_table)

def test_jk_flipflop():
    truth_table = [
        [{'Clk': LOW,  'J': LOW,  'K': HIGH}, {}],
        [{'Clk': HIGH, 'J': LOW,  'K': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'J': HIGH, 'K': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'J': HIGH, 'K': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'J': HIGH, 'K': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'J': HIGH, 'K': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'J': LOW,  'K': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'J': LOW,  'K': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'J': HIGH, 'K': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'J': HIGH, 'K': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'J': HIGH, 'K': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'J': HIGH, 'K': HIGH}, {'Q': HIGH, '/Q': LOW}]
    ]

    run_thruth_table(lsim, "J-K FlipFlop", truth_table)

def test_async_sr_flipflop():
    truth_table = [
        [{'Clk': LOW,  'Res': HIGH, 'Pre': LOW,  'R': HIGH, 'S': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': HIGH, 'R': HIGH, 'S': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'Res': HIGH, 'Pre': LOW,  'R': HIGH, 'S': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'R': HIGH, 'S': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'R': LOW,  'S': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'R': LOW,  'S': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'R': LOW,  'S': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'R': HIGH, 'S': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'R': LOW,  'S': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'R': LOW,  'S': HIGH}, {'Q': HIGH, '/Q': LOW}]
    ]

    run_thruth_table(lsim, "Async S-R FlipFlop", truth_table)

def test_async_d_flipflop():
    truth_table = [
        [{'Clk': LOW,  'Res': HIGH, 'Pre': LOW,  'D': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': HIGH, 'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'Res': HIGH, 'Pre': LOW,  'D': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'D': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': LOW},  {'Q': HIGH, '/Q': LOW}]
    ]

    run_thruth_table(lsim, "Async D FlipFlop", truth_table)

def test_async_jk_flipflop():
    truth_table = [
        [{'Clk': LOW,  'Res': HIGH, 'Pre': LOW,  'J': LOW,  'K': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': HIGH, 'J': LOW,  'K': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'Res': HIGH, 'Pre': LOW,  'J': LOW,  'K': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'J': LOW,  'K': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'J': HIGH, 'K': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'J': HIGH, 'K': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'J': HIGH, 'K': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'J': HIGH, 'K': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'J': LOW,  'K': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'J': LOW,  'K': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'J': HIGH, 'K': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'J': HIGH, 'K': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'J': HIGH, 'K': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'J': HIGH, 'K': HIGH}, {'Q': HIGH, '/Q': LOW}]
    ]

    run_thruth_table(lsim, "Async J-K FlipFlop", truth_table)

def test_masterslave_d_flipflop():
    truth_table = [
        [{'Clk': LOW,  'Res': HIGH, 'Pre': LOW,  'D': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': HIGH, 'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'Res': HIGH, 'Pre': LOW,  'D': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'D': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'D': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'D': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': LOW},  {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': LOW,  '/Q': HIGH}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'D': LOW},  {'Q': HIGH, '/Q': LOW}],
        [{'Clk': LOW,  'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': HIGH}, {'Q': HIGH, '/Q': LOW}],
        [{'Clk': HIGH, 'Res': LOW,  'Pre': LOW,  'D': LOW},  {'Q': HIGH, '/Q': LOW}]
    ]

    run_thruth_table(lsim, "MasterSlave D FlipFlop", truth_table)

def test_async_d_flipflop_8bit():
    truth_table = [
        [{'Clk': LOW, 'Res': LOW,  'Pre': HIGH, 'D[0]': LOW, 'D[1]': LOW, 'D[2]': LOW, 'D[3]': LOW, 'D[4]': LOW, 'D[5]': LOW, 'D[6]': LOW, 'D[7]': LOW }, { 'Q[0]': HIGH, 'Q[1]': HIGH, 'Q[2]': HIGH, 'Q[3]': HIGH, 'Q[4]': HIGH, 'Q[5]': HIGH, 'Q[6]': HIGH, 'Q[7]': HIGH}],
        [{'Clk': LOW, 'Res': HIGH, 'Pre': LOW,  'D[0]': LOW, 'D[1]': LOW, 'D[2]': LOW, 'D[3]': LOW, 'D[4]': LOW, 'D[5]': LOW, 'D[6]': LOW, 'D[7]': LOW }, { 'Q[0]': LOW,  'Q[1]': LOW,  'Q[2]': LOW,  'Q[3]': LOW,  'Q[4]': LOW,  'Q[5]': LOW,  'Q[6]': LOW,  'Q[7]': LOW}]
    ]

    for v in range(256):
        inputs = {f"D[{i:}]" : HIGH if ((v >> i) & 1) == 1 else LOW for i in range(8)}
        inputs.update([('Clk', HIGH), ('Res', LOW), ('Pre', LOW)])

        outputs = {f"Q[{i:}]" : HIGH if ((v >> i) & 1) == 1 else LOW for i in range(8)}
        truth_table.append([inputs, outputs])

        inputs = {f"D[{i:}]" : HIGH if ((v >> i) & 1) == 1 else LOW for i in range(8)}
        inputs.update([('Clk', LOW), ('Res', LOW), ('Pre', LOW)])
        truth_table.append([inputs, outputs])

    run_thruth_table(lsim, "Async D FlipFlop 8b", truth_table)

def test_async_d_flipflop_8bit_tristate():
    truth_table = [
        [{'Clk': LOW, 'Res': LOW,  'Pre': HIGH, 'OE': HIGH, 'D[0]': LOW, 'D[1]': LOW, 'D[2]': LOW, 'D[3]': LOW, 'D[4]': LOW, 'D[5]': LOW, 'D[6]': LOW, 'D[7]': LOW }, { 'Q[0]': HIGH, 'Q[1]': HIGH, 'Q[2]': HIGH, 'Q[3]': HIGH, 'Q[4]': HIGH, 'Q[5]': HIGH, 'Q[6]': HIGH, 'Q[7]': HIGH}],
        [{'Clk': LOW, 'Res': LOW,  'Pre': HIGH, 'OE': LOW,  'D[0]': LOW, 'D[1]': LOW, 'D[2]': LOW, 'D[3]': LOW, 'D[4]': LOW, 'D[5]': LOW, 'D[6]': LOW, 'D[7]': LOW }, { 'Q[0]': NONE, 'Q[1]': NONE, 'Q[2]': NONE, 'Q[3]': NONE, 'Q[4]': NONE, 'Q[5]': NONE, 'Q[6]': NONE, 'Q[7]': NONE}],
        [{'Clk': LOW, 'Res': HIGH, 'Pre': LOW,  'OE': HIGH, 'D[0]': LOW, 'D[1]': LOW, 'D[2]': LOW, 'D[3]': LOW, 'D[4]': LOW, 'D[5]': LOW, 'D[6]': LOW, 'D[7]': LOW }, { 'Q[0]': LOW,  'Q[1]': LOW,  'Q[2]': LOW,  'Q[3]': LOW,  'Q[4]': LOW,  'Q[5]': LOW,  'Q[6]': LOW,  'Q[7]': LOW}],
        [{'Clk': LOW, 'Res': HIGH, 'Pre': LOW,  'OE': LOW,  'D[0]': LOW, 'D[1]': LOW, 'D[2]': LOW, 'D[3]': LOW, 'D[4]': LOW, 'D[5]': LOW, 'D[6]': LOW, 'D[7]': LOW }, { 'Q[0]': NONE, 'Q[1]': NONE, 'Q[2]': NONE, 'Q[3]': NONE, 'Q[4]': NONE, 'Q[5]': NONE, 'Q[6]': NONE, 'Q[7]': NONE}]
    ]

    outputs_none = {f"Q[{i:}]" : NONE for i in range(8)}

    for v in range(256):
        inputs = {f"D[{i:}]" : HIGH if ((v >> i) & 1) == 1 else LOW for i in range(8)}
        inputs.update([('Clk', HIGH), ('Res', LOW), ('Pre', LOW), ('OE', LOW)])
        truth_table.append([inputs, outputs_none])

        inputs = {f"D[{i:}]" : HIGH if ((v >> i) & 1) == 1 else LOW for i in range(8)}
        inputs.update([('Clk', LOW), ('Res', LOW), ('Pre', LOW), ('OE', LOW)])
        truth_table.append([inputs, outputs_none])

        inputs = {f"D[{i:}]" : HIGH if ((v >> i) & 1) == 1 else LOW for i in range(8)}
        inputs.update([('Clk', LOW), ('Res', LOW), ('Pre', LOW), ('OE', HIGH)])
        outputs = {f"Q[{i:}]" : HIGH if ((v >> i) & 1) == 1 else LOW for i in range(8)}
        truth_table.append([inputs, outputs])

    run_thruth_table(lsim, "Async D FlipFlop 8bt", truth_table)

def main():
    if (not lsim.load_user_library("../../examples/cpu_8bit/lib_latches.lsim")):
        print("Unable to load circuit\n")
        exit(-1)

    test_sr_latch()
    test_gated_sr_latch()
    test_d_latch()
    test_async_sr_latch()
    test_sr_flipflop()
    test_d_flipflop()
    test_jk_flipflop()
    test_async_sr_flipflop()
    test_async_d_flipflop()
    test_async_jk_flipflop()
    test_masterslave_d_flipflop()
    test_async_d_flipflop_8bit()
    test_async_d_flipflop_8bit_tristate()

    print_stats()

if __name__ == "__main__":
    main()