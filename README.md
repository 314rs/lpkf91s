[![CMake and CTest on ubuntu-latest](https://github.com/314rs/lpkf91s/actions/workflows/cmake-ubuntu-latest.yml/badge.svg)](https://github.com/314rs/lpkf91s/actions/workflows/cmake-ubuntu-latest.yml)
# LPKF91s tooling - gcode to hpgl converter

This project features some helper programs to revive the LPKF91s PCB mill.

The individual components are:
- __gcode2hpgl__: a converter from gcode to hpgl. (cli, gui)
- __hpgl2serial__: send a hpgl file to the PCB mill via a serial port. (cli, gui)
- a manual hpgl sender (gui)

## Download
Download the program for your platform from the releases


## Build yourself
### Dependencies
- CMake
- conan package manager
- WxWidgets
- boost (asio, program_options)

### Build
```bash
conan install conanfile.txt --build=missing
```

```bash
cmake
```
