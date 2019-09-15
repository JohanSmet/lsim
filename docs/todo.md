# LSim: Todo

## Simulation Engine

- [ ] Each basic component always takes exactly 1 simulation step to procedure its output. To provide a more realistic simulation a little entropy should be introduced here. For example, the datasheet of a 74LS00 chip (quad 2-input nand gate) specifies it typically takes 9ns to toggle from LOW to HIGH but it could take up to 15ns worst case.
- [ ] The simulation engine spends a lot of time process each fixed simulation step even if it should be able to tell nothings going to change in the next batch of cycles (barring user input). A more event-driven approach might allow us to bring the simulation cycle length down to the micro-, or even the nanosecond in real-time.
- [ ] The simulation engine is single-threaded. Extending it to use multiple threads is non-trivial and probably unnecessary for the typical circuit being simulated. But for larger circuits, e.g. with large ROMs or RAM subcircuits, it might prove to be helpful.

## User interface

Although it's usable the GUI still needs a lot of TLC to be enjoyable to use. Up until now, the GUI has not been the focus of attention itself just a necessary evil to be able to build circuits. As such a user needs to be familiar, and willing to be put up, with its many quirks and shortcomings.

- [ ] Allow wires to be selected and moved along with regular components
- [ ] Auto-route wires when moving components
- [ ] Specify the speed of in Hertz; not in steps per display frame as it is now
- [ ] Streamline saving/loading of circuit libraries
- [ ] Indication that circuit was changed but not saved yet (+ popup when closing LSim)

## Python bindings

- [ ] Cleanup and complete exposed Python API
- [ ] Add parameter information to all exposed functions
- [ ] Figure out how to package/install the bindings so they can be used with explicitly setting the PYTHONPATH environment variable

## WebUI (LSim wasm)

- [ ] Add persistent storage option; or at least warn user that saving isn't persistent
- [ ] Import/export circuits

## New features

- [ ] A logic analyzer view or export of data for [GTKWave](http://gtkwave.sourceforge.net/) or [sigrok](https://sigrok.org/) would be a great help in debugging more complex circuits.

