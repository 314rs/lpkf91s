[![CMake and CTest on ubuntu-latest](https://github.com/314rs/cpp_cross_plattform_template_study/actions/workflows/cmake-ubuntu-latest.yml/badge.svg?branch=main)](https://github.com/314rs/cpp_cross_plattform_template_study/actions/workflows/cmake-ubuntu-latest.yml)
# C++ cross-plattform template

with:
- cmake
- conan
- doxygen
- github actions


## Commands

gcode -> hpgl absolute (hpgl relative)

### G00 & G01

#### Parameters

Not all parameters need to be used, but at least one has to be used

```text
Xnnn The position to move to on the X axis
Ynnn The position to move to on the Y axis
Znnn The position to move to on the Z axis
Ennn The amount to extrude between the starting point and ending point
Fnnn The feedrate per minute of the move between the starting point and ending poin (if supplied)
Hnnn (RepRapFirmware) Flag to check if an endstop was hit (H1 to check, H0 to ignore, other Hnnn see note, default is H0)1
Rnnn (RepRapFirmware) Restore point number 4
Snnn Laser cutter/engraver power. In RepRapFirmware, when not in laser mode S in interpreted the same as H.
```

absolute -> `PA (x1,y1{,...xn,yn}){;}` (plot absolute)

relative -> `PR (x1,y1{,...xn,yn}){;}` (plot relative)

-> `PD {;}` (pen down) iff Z position is low _parameter z_

-> `PU {;}` (pen up) iff Z position is high _parameter z_

-> `VS (v{,n}){;}` Velocity select
Defines the track speed in the XY level v=[mm/s] with the tool lowered and allocates this speed to the tool with number n. _parameter F ?!?!?_

### G21

Set units to mm

### G90
set to absolute positioning

### G91 
set to relative positioning

### G92
set position 
> i.e. Give the current position a new name

#### Parameters
This command can be used without any additional parameters.
`Xnnn` new X axis position
`Ynnn` new Y axis position
`Znnn` new Z axis position
`Ennn` new extruder position

### M0
Stop or unconditional stop

#### Parameters
This command can be used without any additional parameters.
`Pnnn` Time to wait, in milliseconds1
`Snnn` Time to wait, in seconds2
#### Example
`M0`


### M3
Spindle On, Clockwise

#### Parameters
```Snnn Spindle RPM```
#### Example
```M3 S4000```


### M5
Spindle off

### M6 
Tool change

### T1 
select tool