# BNO055 Unity Orientation Controller

Source code for controlling Unity objects through BNO055 sensor using a RP2040 MCU.

## Description

This project can be conceptualized into 3 main parts

1. Embedded driver for BNO055 to query orientation data
2. Socket-Based Middle Layer that funnels data from sensor to unity
3. Unity scripts that handle the incoming data and translates that to appropriate movements in the engine

## Directories ##

- `src/`  
    - `unity/`  
        - `BNO055.cs`             - Socket server to grab sensor data
        - `ConnectToBNO055.cs`    - Script can be applied to object to move it according to sensor values
    - `bno055.c`                  - Embedded Code for RP2040 to grab data from BNO055
    - `driver.c`                  - Middle translation layer that acts as socket client to send teensy data to Unity

## Getting Started

### Dependencies

* Raspberry Pi C/C++ SDK

### Executing program

The embedded code has to be built using the CMakeLists.txt and the u2f file placed on the RP2040.
The unity scripts need to be placed on the appropriate objects in Unity Engine with scene running.
Finally, the server intermediate code needs to be built (simple gcc command) and run.

## Authors

[@Conner Sommerfield](https://github.com/Repo-Factory/) Github
