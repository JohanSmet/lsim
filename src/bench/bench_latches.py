#!/usr/bin/env python3

import lsimpy
from bench_utils import *

lsim = lsimpy.LSimContext()

HIGH = lsimpy.ValueTrue
LOW = lsimpy.ValueFalse

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

    print_stats()

if __name__ == "__main__":
    main()