# Building LSim

## Requirements

- a C++14 compatible compiler
- CMake (version 3.13 or newer)
- (optional) Emscripten to build the browser front-end
- SDL2 development libraries have to be available on your system

## Building the native executables

LSim uses the CMake build system, all you need is the traditional CMake workflow. On Linux it would be something like this:

```bash
git clone --recursive https://github.com/JohanSmet/lsim.git
cd lsim
cmake -B _build
cmake --build _build
```

Specify -DPYTHON_BINDINGS=ON with the cmake command to build the Python bindings.

## Building the wasm executables

Requires a working Emscripten installation.

```bash
git clone --recursive https://github.com/JohanSmet/lsim.git
cd lsim
emcmake cmake -B _build_wasm
cmake --build _build_wasm
```

This builds the WebAssembly and glue-files. Copy lsim.* to a location that is accessible from your webserver.

## Continuous Integration

LSim uses GitHub workflows to automatically run and test a build for each supported workflow.
The [configuration file](../.github/workflows/cmake.yml) is also a good reference for, tested, build instructions.
