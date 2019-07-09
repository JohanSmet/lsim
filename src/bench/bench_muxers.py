#!/usr/bin/env python3

import lsimpy
from bench_utils import *

lsim = lsimpy.LSimContext()

HIGH = lsimpy.ValueTrue
LOW = lsimpy.ValueFalse

def test_mux2to1s():
    truth_table = [
        [{'I0': LOW,  'I1': LOW,  'Sel': LOW,  '/Strobe': LOW},  {'Y': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'Sel': LOW,  '/Strobe': LOW},  {'Y': HIGH}],
        [{'I0': HIGH, 'I1': LOW,  'Sel': HIGH, '/Strobe': LOW},  {'Y': LOW}],
        [{'I0': HIGH, 'I1': HIGH, 'Sel': HIGH, '/Strobe': LOW},  {'Y': HIGH}],
        [{'I0': LOW,  'I1': HIGH, 'Sel': HIGH, '/Strobe': LOW},  {'Y': HIGH}],
        [{'I0': LOW,  'I1': LOW,  'Sel': LOW,  '/Strobe': HIGH}, {'Y': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'Sel': LOW,  '/Strobe': HIGH}, {'Y': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'Sel': HIGH, '/Strobe': HIGH}, {'Y': LOW}],
        [{'I0': HIGH, 'I1': HIGH, 'Sel': HIGH, '/Strobe': HIGH}, {'Y': LOW}],
        [{'I0': LOW,  'I1': HIGH, 'Sel': HIGH, '/Strobe': HIGH}, {'Y': LOW}]
    ]

    run_thruth_table(lsim, "mux2to1s", truth_table)

def test_mux4to1s():
    truth_table = [
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': LOW,  'Sel0': LOW,  'Sel1': LOW,  '/Strobe': LOW},  {'Y': LOW}],

        [{'I0': HIGH, 'I1': LOW,  'I2': LOW,  'I3': LOW,  'Sel0': LOW,  'Sel1': LOW,  '/Strobe': LOW},  {'Y': HIGH}],
        [{'I0': LOW,  'I1': HIGH, 'I2': LOW,  'I3': LOW,  'Sel0': LOW,  'Sel1': LOW,  '/Strobe': LOW},  {'Y': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': HIGH, 'I3': LOW,  'Sel0': LOW,  'Sel1': LOW,  '/Strobe': LOW},  {'Y': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': HIGH, 'Sel0': LOW,  'Sel1': LOW,  '/Strobe': LOW},  {'Y': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'I2': LOW,  'I3': LOW,  'Sel0': HIGH, 'Sel1': LOW,  '/Strobe': LOW},  {'Y': LOW}],
        [{'I0': LOW,  'I1': HIGH, 'I2': LOW,  'I3': LOW,  'Sel0': HIGH, 'Sel1': LOW,  '/Strobe': LOW},  {'Y': HIGH}],
        [{'I0': LOW,  'I1': LOW,  'I2': HIGH, 'I3': LOW,  'Sel0': HIGH, 'Sel1': LOW,  '/Strobe': LOW},  {'Y': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': HIGH, 'Sel0': HIGH, 'Sel1': LOW,  '/Strobe': LOW},  {'Y': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'I2': LOW,  'I3': LOW,  'Sel0': LOW,  'Sel1': HIGH, '/Strobe': LOW},  {'Y': LOW}],
        [{'I0': LOW,  'I1': HIGH, 'I2': LOW,  'I3': LOW,  'Sel0': LOW,  'Sel1': HIGH, '/Strobe': LOW},  {'Y': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': HIGH, 'I3': LOW,  'Sel0': LOW,  'Sel1': HIGH, '/Strobe': LOW},  {'Y': HIGH}],
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': HIGH, 'Sel0': LOW,  'Sel1': HIGH, '/Strobe': LOW},  {'Y': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'I2': LOW,  'I3': LOW,  'Sel0': HIGH, 'Sel1': HIGH, '/Strobe': LOW},  {'Y': LOW}],
        [{'I0': LOW,  'I1': HIGH, 'I2': LOW,  'I3': LOW,  'Sel0': HIGH, 'Sel1': HIGH, '/Strobe': LOW},  {'Y': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': HIGH, 'I3': LOW,  'Sel0': HIGH, 'Sel1': HIGH, '/Strobe': LOW},  {'Y': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': HIGH, 'Sel0': HIGH, 'Sel1': HIGH, '/Strobe': LOW},  {'Y': HIGH}],

        [{'I0': HIGH, 'I1': HIGH, 'I2': HIGH, 'I3': HIGH, 'Sel0': LOW,  'Sel1': LOW,  '/Strobe': LOW},  {'Y': HIGH}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': HIGH, 'I3': HIGH, 'Sel0': LOW,  'Sel1': HIGH, '/Strobe': LOW},  {'Y': HIGH}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': HIGH, 'I3': HIGH, 'Sel0': HIGH, 'Sel1': LOW,  '/Strobe': LOW},  {'Y': HIGH}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': HIGH, 'I3': HIGH, 'Sel0': HIGH, 'Sel1': HIGH, '/Strobe': LOW},  {'Y': HIGH}],

        [{'I0': HIGH, 'I1': LOW,  'I2': LOW,  'I3': LOW,  'Sel0': LOW,  'Sel1': LOW,  '/Strobe': HIGH}, {'Y': LOW}],
        [{'I0': LOW,  'I1': HIGH, 'I2': LOW,  'I3': LOW,  'Sel0': LOW,  'Sel1': LOW,  '/Strobe': HIGH}, {'Y': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': HIGH, 'I3': LOW,  'Sel0': LOW,  'Sel1': LOW,  '/Strobe': HIGH}, {'Y': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': HIGH, 'Sel0': LOW,  'Sel1': LOW,  '/Strobe': HIGH}, {'Y': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'I2': LOW,  'I3': LOW,  'Sel0': HIGH, 'Sel1': LOW,  '/Strobe': HIGH}, {'Y': LOW}],
        [{'I0': LOW,  'I1': HIGH, 'I2': LOW,  'I3': LOW,  'Sel0': HIGH, 'Sel1': LOW,  '/Strobe': HIGH}, {'Y': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': HIGH, 'I3': LOW,  'Sel0': HIGH, 'Sel1': LOW,  '/Strobe': HIGH}, {'Y': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': HIGH, 'Sel0': HIGH, 'Sel1': LOW,  '/Strobe': HIGH}, {'Y': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'I2': LOW,  'I3': LOW,  'Sel0': LOW,  'Sel1': HIGH, '/Strobe': HIGH}, {'Y': LOW}],
        [{'I0': LOW,  'I1': HIGH, 'I2': LOW,  'I3': LOW,  'Sel0': LOW,  'Sel1': HIGH, '/Strobe': HIGH}, {'Y': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': HIGH, 'I3': LOW,  'Sel0': LOW,  'Sel1': HIGH, '/Strobe': HIGH}, {'Y': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': HIGH, 'Sel0': LOW,  'Sel1': HIGH, '/Strobe': HIGH}, {'Y': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'I2': LOW,  'I3': LOW,  'Sel0': HIGH, 'Sel1': HIGH, '/Strobe': HIGH}, {'Y': LOW}],
        [{'I0': LOW,  'I1': HIGH, 'I2': LOW,  'I3': LOW,  'Sel0': HIGH, 'Sel1': HIGH, '/Strobe': HIGH}, {'Y': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': HIGH, 'I3': LOW,  'Sel0': HIGH, 'Sel1': HIGH, '/Strobe': HIGH}, {'Y': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': HIGH, 'Sel0': HIGH, 'Sel1': HIGH, '/Strobe': HIGH}, {'Y': LOW}]
    ]

    run_thruth_table(lsim, "mux4to1s", truth_table)

def test_decode1to2():
    truth_table = [
        [{'I0': LOW,  '/Strobe': LOW},  {'O[0]': HIGH, 'O[1]': LOW}],
        [{'I0': HIGH, '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': HIGH}],
        [{'I0': LOW,  '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW}],
        [{'I0': HIGH, '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW}]
    ]

    run_thruth_table(lsim, "decode1to2", truth_table)

def test_decode2to4():
    truth_table = [
        [{'I[0]': LOW,  'I[1]': LOW,  '/Strobe': LOW},  {'O[0]': HIGH, 'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW}],
        [{'I[0]': HIGH, 'I[1]': LOW,  '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': HIGH, 'O[2]': LOW,  'O[3]': LOW}],
        [{'I[0]': LOW,  'I[1]': HIGH, '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': HIGH, 'O[3]': LOW}],
        [{'I[0]': HIGH, 'I[1]': HIGH, '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': HIGH}],

        [{'I[0]': LOW,  'I[1]': LOW,  '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW}],
        [{'I[0]': HIGH, 'I[1]': LOW,  '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW}],
        [{'I[0]': LOW,  'I[1]': HIGH, '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW}],
        [{'I[0]': HIGH, 'I[1]': HIGH, '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW}]
    ]

    run_thruth_table(lsim, "decode2to4", truth_table)

def test_decode3to8():
    truth_table = [
        [{'I[0]': LOW,  'I[1]': LOW,  'I[2]': LOW,  '/Strobe': LOW},  {'O[0]': HIGH, 'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'I[0]': HIGH, 'I[1]': LOW,  'I[2]': LOW,  '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': HIGH, 'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'I[0]': LOW,  'I[1]': HIGH, 'I[2]': LOW,  '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': HIGH, 'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'I[0]': HIGH, 'I[1]': HIGH, 'I[2]': LOW,  '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': HIGH, 'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'I[0]': LOW,  'I[1]': LOW,  'I[2]': HIGH, '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': HIGH, 'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'I[0]': HIGH, 'I[1]': LOW,  'I[2]': HIGH, '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': HIGH, 'O[6]': LOW,  'O[7]': LOW}],
        [{'I[0]': LOW,  'I[1]': HIGH, 'I[2]': HIGH, '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': HIGH, 'O[7]': LOW}],
        [{'I[0]': HIGH, 'I[1]': HIGH, 'I[2]': HIGH, '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': HIGH}],

        [{'I[0]': LOW,  'I[1]': LOW,  'I[2]': LOW,  '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'I[0]': HIGH, 'I[1]': LOW,  'I[2]': LOW,  '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'I[0]': LOW,  'I[1]': HIGH, 'I[2]': LOW,  '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'I[0]': HIGH, 'I[1]': HIGH, 'I[2]': LOW,  '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'I[0]': LOW,  'I[1]': LOW,  'I[2]': HIGH, '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'I[0]': HIGH, 'I[1]': LOW,  'I[2]': HIGH, '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'I[0]': LOW,  'I[1]': HIGH, 'I[2]': HIGH, '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'I[0]': HIGH, 'I[1]': HIGH, 'I[2]': HIGH, '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}]
    ]

    run_thruth_table(lsim, "decode3to8", truth_table)

def test_decode4to16():
    truth_table = [
        [{'I[0]': LOW,  'I[1]': LOW,  'I[2]': LOW,  'I[3]': LOW,  '/Strobe': LOW},  {'O[0]': HIGH, 'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': HIGH, 'I[1]': LOW,  'I[2]': LOW,  'I[3]': LOW,  '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': HIGH, 'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': LOW,  'I[1]': HIGH, 'I[2]': LOW,  'I[3]': LOW,  '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': HIGH, 'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': HIGH, 'I[1]': HIGH, 'I[2]': LOW,  'I[3]': LOW,  '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': HIGH, 'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': LOW,  'I[1]': LOW,  'I[2]': HIGH, 'I[3]': LOW,  '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': HIGH, 'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': HIGH, 'I[1]': LOW,  'I[2]': HIGH, 'I[3]': LOW,  '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': HIGH, 'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': LOW,  'I[1]': HIGH, 'I[2]': HIGH, 'I[3]': LOW,  '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': HIGH, 'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': HIGH, 'I[1]': HIGH, 'I[2]': HIGH, 'I[3]': LOW,  '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': HIGH, 'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': LOW,  'I[1]': LOW,  'I[2]': LOW,  'I[3]': HIGH, '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': HIGH, 'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': HIGH, 'I[1]': LOW,  'I[2]': LOW,  'I[3]': HIGH, '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': HIGH, 'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': LOW,  'I[1]': HIGH, 'I[2]': LOW,  'I[3]': HIGH, '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': HIGH, 'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': HIGH, 'I[1]': HIGH, 'I[2]': LOW,  'I[3]': HIGH, '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': HIGH, 'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': LOW,  'I[1]': LOW,  'I[2]': HIGH, 'I[3]': HIGH, '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': HIGH, 'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': HIGH, 'I[1]': LOW,  'I[2]': HIGH, 'I[3]': HIGH, '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': HIGH, 'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': LOW,  'I[1]': HIGH, 'I[2]': HIGH, 'I[3]': HIGH, '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': HIGH, 'O[15]': LOW}],
        [{'I[0]': HIGH, 'I[1]': HIGH, 'I[2]': HIGH, 'I[3]': HIGH, '/Strobe': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': HIGH}],

        [{'I[0]': LOW,  'I[1]': LOW,  'I[2]': LOW,  'I[3]': LOW,  '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': HIGH, 'I[1]': LOW,  'I[2]': LOW,  'I[3]': LOW,  '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': LOW,  'I[1]': HIGH, 'I[2]': LOW,  'I[3]': LOW,  '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': HIGH, 'I[1]': HIGH, 'I[2]': LOW,  'I[3]': LOW,  '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': LOW,  'I[1]': LOW,  'I[2]': HIGH, 'I[3]': LOW,  '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': HIGH, 'I[1]': LOW,  'I[2]': HIGH, 'I[3]': LOW,  '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': LOW,  'I[1]': HIGH, 'I[2]': HIGH, 'I[3]': LOW,  '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': HIGH, 'I[1]': HIGH, 'I[2]': HIGH, 'I[3]': LOW,  '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': LOW,  'I[1]': LOW,  'I[2]': LOW,  'I[3]': HIGH, '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': HIGH, 'I[1]': LOW,  'I[2]': LOW,  'I[3]': HIGH, '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': LOW,  'I[1]': HIGH, 'I[2]': LOW,  'I[3]': HIGH, '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': HIGH, 'I[1]': HIGH, 'I[2]': LOW,  'I[3]': HIGH, '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': LOW,  'I[1]': LOW,  'I[2]': HIGH, 'I[3]': HIGH, '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': HIGH, 'I[1]': LOW,  'I[2]': HIGH, 'I[3]': HIGH, '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': LOW,  'I[1]': HIGH, 'I[2]': HIGH, 'I[3]': HIGH, '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}],
        [{'I[0]': HIGH, 'I[1]': HIGH, 'I[2]': HIGH, 'I[3]': HIGH, '/Strobe': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW,  'O[8]': LOW,  'O[9]': LOW,  'O[10]': LOW,  'O[11]': LOW,  'O[12]': LOW,  'O[13]': LOW,  'O[14]': LOW,  'O[15]': LOW}]
    ]

    run_thruth_table(lsim, "decode4to16", truth_table)

def test_decode5to32():
    truth_table = []

    for s in [LOW, HIGH]:
        for v in range(0, 32):
            inputs = {'/Strobe' : s}
            outputs = {}
            for i in range(0, 5):
                inputs.update([('I[{}]'.format(i), HIGH if ((v >> i) & 1) == 1 else LOW)])
            for o in range(0, 32):
                outputs.update([('O[{}]'.format(o), HIGH if v == o and s == LOW else LOW)])

            truth_table.append([inputs, outputs])


    run_thruth_table(lsim, "decode5to32", truth_table)

def test_decode6to64():
    truth_table = []

    for s in [LOW, HIGH]:
        for v in range(0, 64):
            inputs = {'/Strobe' : s}
            outputs = {}
            for i in range(0, 6):
                inputs.update([('I[{}]'.format(i), HIGH if ((v >> i) & 1) == 1 else LOW)])
            for o in range(0, 64):
                outputs.update([('O[{}]'.format(o), HIGH if v == o and s == LOW else LOW)])

            truth_table.append([inputs, outputs])

    run_thruth_table(lsim, "decode6to64", truth_table)

def test_demux1to2():
    truth_table = [
        [{'Sel': LOW,  'I': HIGH}, {'O[0]': HIGH, 'O[1]': LOW}],
        [{'Sel': HIGH, 'I': HIGH}, {'O[0]': LOW,  'O[1]': HIGH}],

        [{'Sel': LOW,  'I': LOW},  {'O[0]': LOW,  'O[1]': LOW}],
        [{'Sel': HIGH, 'I': LOW},  {'O[0]': LOW,  'O[1]': LOW}]
    ]

    run_thruth_table(lsim, "demux1to2", truth_table)

def test_demux1to4():
    truth_table = [
        [{'Sel[0]': LOW,  'Sel[1]': LOW,  'I': HIGH}, {'O[0]': HIGH, 'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW}],
        [{'Sel[0]': HIGH, 'Sel[1]': LOW,  'I': HIGH}, {'O[0]': LOW,  'O[1]': HIGH, 'O[2]': LOW,  'O[3]': LOW}],
        [{'Sel[0]': LOW,  'Sel[1]': HIGH, 'I': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': HIGH, 'O[3]': LOW}],
        [{'Sel[0]': HIGH, 'Sel[1]': HIGH, 'I': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': HIGH}],

        [{'Sel[0]': LOW,  'Sel[1]': LOW,  'I': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW}],
        [{'Sel[0]': HIGH, 'Sel[1]': LOW,  'I': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW}],
        [{'Sel[0]': LOW,  'Sel[1]': HIGH, 'I': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW}],
        [{'Sel[0]': HIGH, 'Sel[1]': HIGH, 'I': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW}]
    ]

    run_thruth_table(lsim, "demux1to4", truth_table)

def test_demux1to8():
    truth_table = [
        [{'Sel[0]': LOW,  'Sel[1]': LOW,  'Sel[2]': LOW,  'I': HIGH}, {'O[0]': HIGH, 'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'Sel[0]': HIGH, 'Sel[1]': LOW,  'Sel[2]': LOW,  'I': HIGH}, {'O[0]': LOW,  'O[1]': HIGH, 'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'Sel[0]': LOW,  'Sel[1]': HIGH, 'Sel[2]': LOW,  'I': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': HIGH, 'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'Sel[0]': HIGH, 'Sel[1]': HIGH, 'Sel[2]': LOW,  'I': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': HIGH, 'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'Sel[0]': LOW,  'Sel[1]': LOW,  'Sel[2]': HIGH, 'I': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': HIGH, 'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'Sel[0]': HIGH, 'Sel[1]': LOW,  'Sel[2]': HIGH, 'I': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': HIGH, 'O[6]': LOW,  'O[7]': LOW}],
        [{'Sel[0]': LOW,  'Sel[1]': HIGH, 'Sel[2]': HIGH, 'I': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': HIGH, 'O[7]': LOW}],
        [{'Sel[0]': HIGH, 'Sel[1]': HIGH, 'Sel[2]': HIGH, 'I': HIGH}, {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': HIGH}],

        [{'Sel[0]': LOW,  'Sel[1]': LOW,  'Sel[2]': LOW,  'I': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'Sel[0]': HIGH, 'Sel[1]': LOW,  'Sel[2]': LOW,  'I': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'Sel[0]': LOW,  'Sel[1]': HIGH, 'Sel[2]': LOW,  'I': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'Sel[0]': HIGH, 'Sel[1]': HIGH, 'Sel[2]': LOW,  'I': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'Sel[0]': LOW,  'Sel[1]': LOW,  'Sel[2]': HIGH, 'I': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'Sel[0]': HIGH, 'Sel[1]': LOW,  'Sel[2]': HIGH, 'I': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'Sel[0]': LOW,  'Sel[1]': HIGH, 'Sel[2]': HIGH, 'I': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}],
        [{'Sel[0]': HIGH, 'Sel[1]': HIGH, 'Sel[2]': HIGH, 'I': LOW},  {'O[0]': LOW,  'O[1]': LOW,  'O[2]': LOW,  'O[3]': LOW,  'O[4]': LOW,  'O[5]': LOW,  'O[6]': LOW,  'O[7]': LOW}]
    ]

    run_thruth_table(lsim, "demux1to8", truth_table)

def test_demux1to16():
    truth_table = []

    for s in [HIGH, LOW]:
        for v in range(0, 16):
            inputs = {'I' : s}
            outputs = {}
            for i in range(0, 4):
                inputs.update([('Sel[{}]'.format(i), HIGH if ((v >> i) & 1) == 1 else LOW)])
            for o in range(0, 16):
                outputs.update([('O[{}]'.format(o), HIGH if v == o and s == HIGH else LOW)])

            truth_table.append([inputs, outputs])

    run_thruth_table(lsim, "demux1to16", truth_table)

def test_demux1to32():
    truth_table = []

    for s in [HIGH, LOW]:
        for v in range(0, 32):
            inputs = {'I' : s}
            outputs = {}
            for i in range(0, 5):
                inputs.update([('Sel[{}]'.format(i), HIGH if ((v >> i) & 1) == 1 else LOW)])
            for o in range(0, 32):
                outputs.update([('O[{}]'.format(o), HIGH if v == o and s == HIGH else LOW)])

            truth_table.append([inputs, outputs])

    run_thruth_table(lsim, "demux1to32", truth_table)

def test_demux1to64():
    truth_table = []

    for s in [HIGH, LOW]:
        for v in range(0, 64):
            inputs = {'I' : s}
            outputs = {}
            for i in range(0, 6):
                inputs.update([('Sel[{}]'.format(i), HIGH if ((v >> i) & 1) == 1 else LOW)])
            for o in range(0, 64):
                outputs.update([('O[{}]'.format(o), HIGH if v == o and s == HIGH else LOW)])

            truth_table.append([inputs, outputs])

    run_thruth_table(lsim, "demux1to64", truth_table)

def test_prio_encode4to2():
    truth_table = [
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': LOW},  {'O0': LOW,  'O1': LOW,  'V': LOW}],

        [{'I0': HIGH, 'I1': LOW,  'I2': LOW,  'I3': LOW},  {'O0': LOW,  'O1': LOW,  'V': HIGH}],
        [{'I0': LOW,  'I1': HIGH, 'I2': LOW,  'I3': LOW},  {'O0': HIGH, 'O1': LOW,  'V': HIGH}],
        [{'I0': LOW,  'I1': LOW,  'I2': HIGH, 'I3': LOW},  {'O0': LOW,  'O1': HIGH, 'V': HIGH}],
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': HIGH}, {'O0': HIGH, 'O1': HIGH, 'V': HIGH}],

        [{'I0': HIGH, 'I1': LOW,  'I2': LOW,  'I3': LOW},  {'O0': LOW,  'O1': LOW,  'V': HIGH}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': LOW,  'I3': LOW},  {'O0': HIGH, 'O1': LOW,  'V': HIGH}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': HIGH, 'I3': LOW},  {'O0': LOW,  'O1': HIGH, 'V': HIGH}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': HIGH, 'I3': HIGH}, {'O0': HIGH, 'O1': HIGH, 'V': HIGH}]
    ]

    run_thruth_table(lsim, "prio_encode4to2", truth_table)

def test_prio_encode8to3():
    truth_table = [
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': LOW,  'I4': LOW,  'I5': LOW,  'I6': LOW,  'I7': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW, 'V': LOW}],

        [{'I0': HIGH, 'I1': LOW,  'I2': LOW,  'I3': LOW,  'I4': LOW,  'I5': LOW,  'I6': LOW,  'I7': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW, 'V': HIGH}],
        [{'I0': LOW,  'I1': HIGH, 'I2': LOW,  'I3': LOW,  'I4': LOW,  'I5': LOW,  'I6': LOW,  'I7': LOW},  {'O0': HIGH, 'O1': LOW,  'O2': LOW, 'V': HIGH}],
        [{'I0': LOW,  'I1': LOW,  'I2': HIGH, 'I3': LOW,  'I4': LOW,  'I5': LOW,  'I6': LOW,  'I7': LOW},  {'O0': LOW,  'O1': HIGH, 'O2': LOW, 'V': HIGH}],
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': HIGH, 'I4': LOW,  'I5': LOW,  'I6': LOW,  'I7': LOW},  {'O0': HIGH, 'O1': HIGH, 'O2': LOW, 'V': HIGH}],
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': LOW,  'I4': HIGH, 'I5': LOW,  'I6': LOW,  'I7': LOW},  {'O0': LOW,  'O1': LOW,  'O2': HIGH,'V': HIGH}],
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': LOW,  'I4': LOW,  'I5': HIGH, 'I6': LOW,  'I7': LOW},  {'O0': HIGH, 'O1': LOW,  'O2': HIGH,'V': HIGH}],
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': LOW,  'I4': LOW,  'I5': LOW,  'I6': HIGH, 'I7': LOW},  {'O0': LOW,  'O1': HIGH, 'O2': HIGH,'V': HIGH}],
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': LOW,  'I4': LOW,  'I5': LOW,  'I6': LOW,  'I7': HIGH}, {'O0': HIGH, 'O1': HIGH, 'O2': HIGH,'V': HIGH}],

        [{'I0': HIGH, 'I1': LOW,  'I2': LOW,  'I3': LOW,  'I4': LOW,  'I5': LOW,  'I6': LOW,  'I7': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW, 'V': HIGH}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': LOW,  'I3': LOW,  'I4': LOW,  'I5': LOW,  'I6': LOW,  'I7': LOW},  {'O0': HIGH, 'O1': LOW,  'O2': LOW, 'V': HIGH}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': HIGH, 'I3': LOW,  'I4': LOW,  'I5': LOW,  'I6': LOW,  'I7': LOW},  {'O0': LOW,  'O1': HIGH, 'O2': LOW, 'V': HIGH}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': HIGH, 'I3': HIGH, 'I4': LOW,  'I5': LOW,  'I6': LOW,  'I7': LOW},  {'O0': HIGH, 'O1': HIGH, 'O2': LOW, 'V': HIGH}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': HIGH, 'I3': HIGH, 'I4': HIGH, 'I5': LOW,  'I6': LOW,  'I7': LOW},  {'O0': LOW,  'O1': LOW,  'O2': HIGH,'V': HIGH}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': HIGH, 'I3': HIGH, 'I4': HIGH, 'I5': HIGH, 'I6': LOW,  'I7': LOW},  {'O0': HIGH, 'O1': LOW,  'O2': HIGH,'V': HIGH}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': HIGH, 'I3': HIGH, 'I4': HIGH, 'I5': HIGH, 'I6': HIGH, 'I7': LOW},  {'O0': LOW,  'O1': HIGH, 'O2': HIGH,'V': HIGH}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': HIGH, 'I3': HIGH, 'I4': HIGH, 'I5': HIGH, 'I6': HIGH, 'I7': HIGH}, {'O0': HIGH, 'O1': HIGH, 'O2': HIGH,'V': HIGH}]
    ]

    run_thruth_table(lsim, "prio_encode8to3", truth_table)

def main():
    if (not lsim.load_user_library("../../examples/cpu_8bit/lib_muxers.lsim")):
        print("Unable to load circuit\n")
        exit(-1)

    test_mux2to1s()
    test_mux4to1s()
    test_decode1to2()
    test_decode2to4()
    test_decode3to8()
    test_decode4to16()
    test_decode5to32()
    test_decode6to64()
    test_demux1to2()
    test_demux1to4()
    test_demux1to8()
    test_demux1to16()
    test_demux1to32()
    test_demux1to64()
    test_prio_encode4to2()
    test_prio_encode8to3()

    print_stats()

if __name__ == "__main__":
    main()