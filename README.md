# STM32F446RE Bare-Metal Template

Target:
- STM32F446RE
- NUCLEO-F446RE
- Cortex-M4F

Toolchain:
- arm-none-eabi-gcc
- CMake
- Ninja
- VS Code
- ST-LINK

Build:

    cmake --preset Debug
    cmake --build --preset Debug

Debug:

    Press F5 in VS Code.

Main device header:

    #include "stm32f446xx.h"