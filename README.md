# LSim
LSim (or Logicial SIMulator) is a tool to simulate digital logic circuits. It's a hobby/toy project I started to help me learn about digital logic design.

Please note: if you're not into custom-built experimental logic simulators this might not be the place for you. For a motivation (or more accurately: a justification) why I build LSim please read this [document](docs/motivation.md).

![lsim_small](docs/lsim_small.png)

## Features

- simulation logic circuits at the gate level
- a somewhat usable GUI to build and visualize digital circuits
- construct basic circuits a reuse them as buildings blocks for more complex circuits
- Python-bindings to enable easy exhaustive simulation of your circuit
- the Python-bindings also allow procedural generation of circuits (e.g. roms)
- can simulate basic digital circuits (e.g. flipflops and counters) and more complex circuits. As an example, LSim includes a simplistic 8-bit computer that can do some basic operations (e.g. compute the Fibonacci sequence).
- Runs on multiple platforms: Linux, Windows and using WebAssembly in most modern browsers. MacOS X should work in theory, needs somebody with a Mac to verify.

## Structure

The core of the project is a library called `lsim` . The library contains the logic simulator and the corresponding logic gates and supporting components. The library separates construction and modification of the digital circuit from the simulation of said circuit in a two-phased approach. 

In the first phase, a model of the circuit is constructed using basic components, nested circuits, and wires. The second phase instantiates a flattened, immutable representation of said model that is easier to simulate.

The library also contains the necessary support to store and load circuit models and preliminary Python bindings that allows easy scripting of circuit tests and procedural generation of large, repetitive circuits.

The GUI application, `lsim_gui`, wraps the library and provides an user interface to build circuits and visualize the circuit being simulated. 

## Todo

- While usable the GUI still needs a lot of TLC to be enjoyable to use. Up until now, the GUI has not been the focus of attention itself just a necessary evil to be able to build circuits. As such a user needs to be familiar, and willing to be put up, with its many quirks and shortcomings.
- Improve the simulation engine:
  - Every basic component now always takes exactly 1 simulation step to produce its result. To provide a more realistic simulation gate processing should be different durations, random between the lower and upper bounds as documented in the datasheets of common gate implementations.
  - The simulation engine spends a lot of time process each fixed simulation step even if it should be able to tell nothings going to change in the next batch of cycles (barring user input). A more event-driven approach might allow us to bring the simulation cycle length down to the micro-, or even the nanosecond in real-time.
- Improve the Python bindings: full coverage + add parameter definitions
- A logic analyzer view or export of data for [GTKWave](http://gtkwave.sourceforge.net/) or [sigrok](https://sigrok.org/) would be a great help in debugging more complex circuits.

## License

This project is licensed under the 3-clause BSD License. See [LICENSE](LICENSE) for the full text.

## **Acknowledgments**

- The GUI is built with the awesome [Dear ImGui](https://github.com/ocornut/imgui). Building the cross-platform UI would not have been as much fun without it.
- I trust upon [pugixml](https://pugixml.org/) to save me from the horrors of parsing XML files.
- [Catch2](https://github.com/catchorg/Catch2) is the unit-test framework that, hopefully, saves me from making the same mistake twice. 
- [pybind11](https://github.com/pybind/pybind11) enabled me to add Python bindings without to much hassle.
- cute_files (part of [cute_headers](https://github.com/RandyGaul/cute_headers)) allows zipping around the filesystem on all supported platforms.
- Logisim (http://www.cburch.com/logisim/) for being part of the inspiration to start this project.
