# LPKF91s tooling - gcode to hpgl converter

[![CMake and CTest on ubuntu-latest](https://github.com/314rs/lpkf91s/actions/workflows/cmake-ubuntu-latest.yml/badge.svg)](https://github.com/314rs/lpkf91s/actions/workflows/cmake-ubuntu-latest.yml)

This project features some helper programs to revive the LPKF91s PCB mill.

The individual components are:

- __gcode2hpgl__: a converter from gcode to hpgl. (cli, gui)
- __hpgl2serial__: send a hpgl file to the PCB mill via a serial port. (cli, gui)
- a manual hpgl sender (gui)

## Download

Download the program for your platform from the releases.

## Build yourself

### Dependencies

- CMake
- Conan
  - boost (asio, program_options)
  - gtest
- wxWidgets

### Build

```sh
conan install conanfile.txt --build=missing
cmake -DCMAKE_TOOLCHAIN_FILE=./build/generators/conan_toolchain.cmake -S. -B./build -G "YourGeneratorHere"
```

### Testing

```sh
cd build
ctest
```

## Wiki

For further information check out the [wiki](https://github.com/314rs/lpkf91s/wiki).

## License

This software is open source and released under the MIT License.
