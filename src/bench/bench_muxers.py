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
        [{'I0': LOW,  '/Strobe': LOW},  {'O0': HIGH, 'O1': LOW}],
        [{'I0': HIGH, '/Strobe': LOW},  {'O0': LOW,  'O1': HIGH}],
        [{'I0': LOW,  '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW}],
        [{'I0': HIGH, '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW}]
    ]

    run_thruth_table(lsim, "decode1to2", truth_table)

def test_decode2to4():
    truth_table = [
        [{'I0': LOW,  'I1': LOW,  '/Strobe': LOW},  {'O0': HIGH, 'O1': LOW,  'O2': LOW,  'O3': LOW}],
        [{'I0': HIGH, 'I1': LOW,  '/Strobe': LOW},  {'O0': LOW,  'O1': HIGH, 'O2': LOW,  'O3': LOW}],
        [{'I0': LOW,  'I1': HIGH, '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': HIGH, 'O3': LOW}],
        [{'I0': HIGH, 'I1': HIGH, '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': HIGH}],

        [{'I0': LOW,  'I1': LOW,  '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW}],
        [{'I0': HIGH, 'I1': LOW,  '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW}],
        [{'I0': LOW,  'I1': HIGH, '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW}],
        [{'I0': HIGH, 'I1': HIGH, '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW}]
    ]

    run_thruth_table(lsim, "decode2to4", truth_table)

def test_decode3to8():
    truth_table = [
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  '/Strobe': LOW},  {'O0': HIGH, 'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'I2': LOW,  '/Strobe': LOW},  {'O0': LOW,  'O1': HIGH, 'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'I0': LOW,  'I1': HIGH, 'I2': LOW,  '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': HIGH, 'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': LOW,  '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': HIGH, 'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': HIGH, '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': HIGH, 'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'I2': HIGH, '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': HIGH, 'O6': LOW,  'O7': LOW}],
        [{'I0': LOW,  'I1': HIGH, 'I2': HIGH, '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': HIGH, 'O7': LOW}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': HIGH, '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': HIGH}],

        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'I2': LOW,  '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'I0': LOW,  'I1': HIGH, 'I2': LOW,  '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': LOW,  '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': HIGH, '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'I2': HIGH, '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'I0': LOW,  'I1': HIGH, 'I2': HIGH, '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': HIGH, '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}]
    ]

    run_thruth_table(lsim, "decode3to8", truth_table)

def test_decode4to16():
    truth_table = [
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': LOW,  '/Strobe': LOW},  {'O0': HIGH, 'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'I2': LOW,  'I3': LOW,  '/Strobe': LOW},  {'O0': LOW,  'O1': HIGH, 'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': LOW,  'I1': HIGH, 'I2': LOW,  'I3': LOW,  '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': HIGH, 'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': LOW,  'I3': LOW,  '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': HIGH, 'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': HIGH, 'I3': LOW,  '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': HIGH, 'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'I2': HIGH, 'I3': LOW,  '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': HIGH, 'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': LOW,  'I1': HIGH, 'I2': HIGH, 'I3': LOW,  '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': HIGH, 'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': HIGH, 'I3': LOW,  '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': HIGH, 'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': HIGH, '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': HIGH, 'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'I2': LOW,  'I3': HIGH, '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': HIGH, 'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': LOW,  'I1': HIGH, 'I2': LOW,  'I3': HIGH, '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': HIGH, 'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': LOW,  'I3': HIGH, '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': HIGH, 'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': HIGH, 'I3': HIGH, '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': HIGH, 'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'I2': HIGH, 'I3': HIGH, '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': HIGH, 'O14': LOW,  'O15': LOW}],
        [{'I0': LOW,  'I1': HIGH, 'I2': HIGH, 'I3': HIGH, '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': HIGH, 'O15': LOW}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': HIGH, 'I3': HIGH, '/Strobe': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': HIGH}],

        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': LOW,  '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'I2': LOW,  'I3': LOW,  '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': LOW,  'I1': HIGH, 'I2': LOW,  'I3': LOW,  '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': LOW,  'I3': LOW,  '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': HIGH, 'I3': LOW,  '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'I2': HIGH, 'I3': LOW,  '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': LOW,  'I1': HIGH, 'I2': HIGH, 'I3': LOW,  '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': HIGH, 'I3': LOW,  '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': LOW,  'I3': HIGH, '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'I2': LOW,  'I3': HIGH, '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': LOW,  'I1': HIGH, 'I2': LOW,  'I3': HIGH, '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': LOW,  'I3': HIGH, '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': LOW,  'I1': LOW,  'I2': HIGH, 'I3': HIGH, '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': HIGH, 'I1': LOW,  'I2': HIGH, 'I3': HIGH, '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': LOW,  'I1': HIGH, 'I2': HIGH, 'I3': HIGH, '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}],
        [{'I0': HIGH, 'I1': HIGH, 'I2': HIGH, 'I3': HIGH, '/Strobe': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW,  'O8': LOW,  'O9': LOW,  'O10': LOW,  'O11': LOW,  'O12': LOW,  'O13': LOW,  'O14': LOW,  'O15': LOW}]
    ]

    run_thruth_table(lsim, "decode4to16", truth_table)

def test_demux1to2():
    truth_table = [
        [{'Sel0': LOW,  'I': HIGH}, {'O0': HIGH, 'O1': LOW}],
        [{'Sel0': HIGH, 'I': HIGH}, {'O0': LOW,  'O1': HIGH}],

        [{'Sel0': LOW,  'I': LOW},  {'O0': LOW,  'O1': LOW}],
        [{'Sel0': HIGH, 'I': LOW},  {'O0': LOW,  'O1': LOW}]
    ]

    run_thruth_table(lsim, "demux1to2", truth_table)

def test_demux1to4():
    truth_table = [
        [{'Sel0': LOW,  'Sel1': LOW,  'I': HIGH}, {'O0': HIGH, 'O1': LOW,  'O2': LOW,  'O3': LOW}],
        [{'Sel0': HIGH, 'Sel1': LOW,  'I': HIGH}, {'O0': LOW,  'O1': HIGH, 'O2': LOW,  'O3': LOW}],
        [{'Sel0': LOW,  'Sel1': HIGH, 'I': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': HIGH, 'O3': LOW}],
        [{'Sel0': HIGH, 'Sel1': HIGH, 'I': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': HIGH}],

        [{'Sel0': LOW,  'Sel1': LOW,  'I': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW}],
        [{'Sel0': HIGH, 'Sel1': LOW,  'I': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW}],
        [{'Sel0': LOW,  'Sel1': HIGH, 'I': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW}],
        [{'Sel0': HIGH, 'Sel1': HIGH, 'I': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW}]
    ]

    run_thruth_table(lsim, "demux1to4", truth_table)

def test_demux1to8():
    truth_table = [
        [{'Sel0': LOW,  'Sel1': LOW,  'Sel2': LOW,  'I': HIGH}, {'O0': HIGH, 'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'Sel0': HIGH, 'Sel1': LOW,  'Sel2': LOW,  'I': HIGH}, {'O0': LOW,  'O1': HIGH, 'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'Sel0': LOW,  'Sel1': HIGH, 'Sel2': LOW,  'I': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': HIGH, 'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'Sel0': HIGH, 'Sel1': HIGH, 'Sel2': LOW,  'I': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': HIGH, 'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'Sel0': LOW,  'Sel1': LOW,  'Sel2': HIGH, 'I': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': HIGH, 'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'Sel0': HIGH, 'Sel1': LOW,  'Sel2': HIGH, 'I': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': HIGH, 'O6': LOW,  'O7': LOW}],
        [{'Sel0': LOW,  'Sel1': HIGH, 'Sel2': HIGH, 'I': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': HIGH, 'O7': LOW}],
        [{'Sel0': HIGH, 'Sel1': HIGH, 'Sel2': HIGH, 'I': HIGH}, {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': HIGH}],

        [{'Sel0': LOW,  'Sel1': LOW,  'Sel2': LOW,  'I': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'Sel0': HIGH, 'Sel1': LOW,  'Sel2': LOW,  'I': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'Sel0': LOW,  'Sel1': HIGH, 'Sel2': LOW,  'I': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'Sel0': HIGH, 'Sel1': HIGH, 'Sel2': LOW,  'I': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'Sel0': LOW,  'Sel1': LOW,  'Sel2': HIGH, 'I': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'Sel0': HIGH, 'Sel1': LOW,  'Sel2': HIGH, 'I': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'Sel0': LOW,  'Sel1': HIGH, 'Sel2': HIGH, 'I': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}],
        [{'Sel0': HIGH, 'Sel1': HIGH, 'Sel2': HIGH, 'I': LOW},  {'O0': LOW,  'O1': LOW,  'O2': LOW,  'O3': LOW,  'O4': LOW,  'O5': LOW,  'O6': LOW,  'O7': LOW}]
    ]

    run_thruth_table(lsim, "demux1to8", truth_table)

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
    test_demux1to2()
    test_demux1to4()
    test_demux1to8()
    test_prio_encode4to2()
    test_prio_encode8to3()

    print_stats()

if __name__ == "__main__":
    main()