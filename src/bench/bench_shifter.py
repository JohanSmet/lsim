#!/usr/bin/env python3

import lsimpy


def main():
    sim = lsimpy.Simulator()
    
    if (not lsimpy.load_logisim(sim, "../../examples/shifter.circ")):
        print("Unable to load circuit\n")
        exit(-1)

    pin_SHL = sim.connector_by_name("SHL")
    pin_D = sim.connector_by_name("D")
    pin_S = sim.connector_by_name("S")
    pin_O = sim.connector_by_name("O")

    pin_D.change_data(16)
    pin_S.change_data(1)
    pin_SHL.change_data(lsimpy.ValueFalse)
    sim.run_until_stable(5)

if __name__ == "__main__":
    main()