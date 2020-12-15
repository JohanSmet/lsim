# Using the LSim Python bindings

LSim provides rudimentary Python 3 bindings to allow you to write test scripts for a circuit or even generate repetitive circuits programmatically.

1. Build LSim with Python bindings enabled (`cmake -DPYTHON_BINDINGS=ON`)
2. Make the resulting module available to Python. You could install it to your system but I just add the build directory to the PYTHONPATH environment variable
3. Profit.

## Testing a circuit
Stepping through a simple test script is probably the easiest way to explain the procedure. There are several test-bench scripts in the `src/bench` directory. We'll look at the test script for an 8-bit comparator circuit.

First some standard python header stuff. Import the LSim bindings module `lsimpy` and some utility functions from `bench_utils.py`provided in the test-bench directory.

```python
#!/usr/bin/env python3

import lsimpy
from bench_utils import *
```

Now we start the main function by creating a new LSim context. Add a folder so LSim knows where to find any referenced circuit libraries.

```python
def main():
    lsim = lsimpy.LSimContext()
    lsim.add_folder("examples", "../../examples")
    sim = lsim.sim()
```

Next we load the circuit library we want to test:

```python
    if (not lsim.load_user_library("examples/compare.lsim")):
        print("Unable to load circuit\n")
        exit(-1)
```

Get the description of the specific circuit we want to test and retrieve pointers to the pins we'll need to write to and read from later on.

```python
    circuit_desc = lsim.user_library().circuit_by_name("comp_8bit")
   
    pin_A = [circuit_desc.port_by_name(f"A[{i:}]") for i in range(0,8)]
    pin_B = [circuit_desc.port_by_name(f"B[{i:}]") for i in range(0,8)]
    pin_LT = circuit_desc.port_by_name("LT")
    pin_EQ = circuit_desc.port_by_name("EQ")
    pin_GT = circuit_desc.port_by_name("GT")
```
Instantiate the circuit in the simulator and start the simulation:

```python
    circuit = circuit_desc.instantiate(sim)
    sim.init()
```

Next we loop through all possible values for the two 8-bit input pins and check if the output pins have to values we expect them to have. `CHECK()` is a function from `bench_utils.py` which reports an error but does not stop the execution of the script.

```python
    for a in range(0, 2**8):
        circuit.write_byte(pin_A, a)
        for b in range(0, 2**8):
            circuit.write_byte(pin_B, b)
            sim.run_until_stable(5)
            expected_LT = lsimpy.ValueTrue if a < b else lsimpy.ValueFalse
            expected_EQ = lsimpy.ValueTrue if a == b else lsimpy.ValueFalse
            expected_GT = lsimpy.ValueTrue if a > b else lsimpy.ValueFalse
            CHECK(circuit.read_pin(pin_LT), expected_LT, "{} < {}".format(a, b))
            CHECK(circuit.read_pin(pin_EQ), expected_EQ, "{} == {}".format(a, b))
            CHECK(circuit.read_pin(pin_GT), expected_GT, "{} > {}".format(a, b))
```

Finally we print some statistics and end the Python script by calling the main function.

```python
    print_stats()

if __name__ == "__main__":
    main()
```

## Creating a circuit

For an example of creating circuits see `src/tools/rom_builder.py`. This scripts takes a binary files and creates a ROM-circuit that can be used in other circuits. 
