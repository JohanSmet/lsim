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
   
    if (not lsimpy.load_logisim(lsim, "../../examples/shifter.circ")):
        print("Unable to load circuit\n")
        exit(-1)

    pin_SHL = sim.connector_by_name("SHL")
    pin_D = sim.connector_by_name("D")
    pin_S = sim.connector_by_name("S")
    pin_O = sim.connector_by_name("O")
    pin_CE = sim.connector_by_name("CE")
    pin_OE = sim.connector_by_name("OE")

    pin_OE.change_data(lsimpy.ValueTrue)

    for shl in range(0, 2):
        pin_SHL.change_data(shl)
        for d in range (0, 2**4):
            pin_D.change_data(d)
            for s in range (0, 2**3):
                pin_S.change_data(s)
                sim.run_until_stable(5)
                pin_CE.change_data(lsimpy.ValueTrue)
                sim.run_until_stable(5)
                pin_CE.change_data(lsimpy.ValueFalse)

                result = sim.read_nibble(pin_O.pins())
                expected = (d << s if shl else d >> s) & (2**4-1)
                CHECK(result, expected, "SHL({}), D({}), S({})".format(shl,d,s))

    print_stats()

if __name__ == "__main__":
    main()
