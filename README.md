# LPKF91s tooling - gcode to hpgl converter

<!-- markdownlint-disable MD033 -->
<img src="./media/lpkf91s_controller_logo.svg" alt="logo" align="right" align="left" width="100" height="100" />

[![build](https://github.com/314rs/lpkf91s/actions/workflows/build.yml/badge.svg)](https://github.com/314rs/lpkf91s/actions/workflows/build.yml)
[![test](https://github.com/314rs/lpkf91s/actions/workflows/test.yml/badge.svg)](https://github.com/314rs/lpkf91s/actions/workflows/test.yml)
[![CodeQL](https://github.com/314rs/lpkf91s/actions/workflows/codeql.yml/badge.svg)](https://github.com/314rs/lpkf91s/actions/workflows/codeql.yml)
[![Doxygen](https://github.com/314rs/lpkf91s/actions/workflows/doxygen.yml/badge.svg)](https://github.com/314rs/lpkf91s/actions/workflows/doxygen.yml)
![GitHub License](https://img.shields.io/github/license/314rs/lpkf91s)

This project features some helper programs to revive the LPKF91s PCB mill.

The individual components are:

- __gcode2hpgl__: a converter from gcode to hpgl. (cli, gui)
- __hpgl2serial__: send a hpgl file to the PCB mill via a serial port. (cli, gui)
- a manual hpgl sender (gui)

## Download

Download the program for your platform from the latest [build action](https://github.com/314rs/lpkf91s/actions/workflows/cmake.yml).

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
cmake --preset conan-default
cmake -DCMAKE_TOOLCHAIN_FILE=./build/generators/conan_toolchain.cmake -S. -B./build -G "YourGeneratorHere"
```

### Testing

```sh
cd build
ctest
```

## Doxygen

Doxygen documentation can be found [here](https://314rs.github.io/lpkf91s/).

Or build it yourself:

```sh
git clone https://github.com/jothepro/doxygen-awesome-css.git docs/doxygen-awesome-css
echo "PROJECT_NUMBER = $(git describe --tags --always --dirty=-dev)" > docs/PROJECT_NUMBER # important, always do this before running doxygen
doxygen
```

## Wiki

For further information check out the [wiki](https://github.com/314rs/lpkf91s/wiki).

## License

This software is open source and released under the [MIT License](LICENSE.txt).
