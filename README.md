# ATMega33 - Custom Implementation of ATMega32 emulator

## About this Project

This repository contains a custom, educational emulator implementing the core of an AVR microcontroller compatible with the ATmega32 instruction set and architecture. The emulator focuses on modeling CPU internals and a small subset of the instruction set so you can step through and reason about program execution.

This project is primarily intended for learning, testing, and experimentation rather than production-grade cycle-accurate simulation.

## What is ATmega32?

The ATmega32 is a member of Microchip/Atmel’s AVR 8-bit microcontroller family. It is commonly used in embedded systems and educational projects. The microcontroller features:

- 8-bit RISC CPU core
- 32 x 8-bit general-purpose registers (R0–R31)
- Program memory (Flash), data memory (SRAM), and EEPROM spaces
- A 16-bit Program Counter (PC) and 16-bit Stack Pointer (SP)
- A Status Register (SREG) that tracks ALU flags such as Carry (C), Zero (Z), Negative (N), Overflow (V), Sign (S), Half-carry (H), T and Global Interrupt Flag (I)
- Hardware peripherals (timers, I/O ports, ADC, etc.) — not fully modeled in this emulator

## High-level Architecture Modeled

The emulator models the essential CPU components required to execute AVR firmware:

- Register file: R0…R31
- Program memory: Flash image used to fetch instructions
- Data memory: SRAM and IO space for loads/stores
- Program counter (PC) and stack pointer (SP)
- Status register (SREG) and a minimal set of flags relevant to supported arithmetic instructions
- Instruction decode & execute loop with support for a subset of opcodes

## Implemented Instructions

This project currently implements the following instructions. Each entry includes a brief semantic description and a small example.

- ldi (Load Immediate)
    - Syntax: ldi Rd, K
    - Semantics: Load 8-bit constant K into register Rd. (LDI encodes only for Rd = R16..R31.)
    - Example: ldi r20, 0x10 ; r20 ← 0x10

- add (Add without carry)
    - Syntax: add Rd, Rr
    - Semantics: Rd ← Rd + Rr; updates SREG flags (C, Z, N, V, S, H) accordingly.
    - Example: add r1, r2 ; r1 ← r1 + r2

- lds (Load Direct from Data Space)
    - Syntax: lds Rd, k
    - Semantics: Load 8-bit value at data memory address k into register Rd.
    - Example: lds r0, 0x0100 ; r0 ← data[0x0100]

- sts (Store Direct to Data Space)
    - Syntax: sts k, Rr
    - Semantics: Store 8-bit value from register Rr into data memory address k.
    - Example: sts 0x0100, r0 ; data[0x0100] ← r0

- jmp (Jump)
    - Syntax: jmp k
    - Semantics: PC ← k. Some AVR variants use long jumps occupying more bytes.
    - Example: jmp 0x0040 ; PC ← 0x0040

Note: The emulator implements the basic binary/bit semantics of these instructions (register behavior and arithmetic/logic results). The SREG flags are updated where relevant (e.g., after add).

## Examples

Simple assembly snippet to test load/store/arith/jump:

- Example 1 (LDI + ADD):
    - ldi r16, 0x05
    - ldi r17, 0x03
    - add r16, r17 ; r16 == 0x08

- Example 2 (LDS + STS + JMP):
    - lds r0, 0x0100   ; read an SRAM byte into r0
    - ldi r1, 0x01
    - add r0, r1
    - sts 0x0100, r0   ; write updated value back to SRAM
    - jmp 0x0006       ; jump to some label

## Build and Usage

### Build & run

Prerequisites: CMake (3.15+ recommended) and a C/C++ compiler (GCC/Clang/MSVC). Example build commands:

Unix/macOS (out-of-source build):
```
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug -- -j8
```

Alternative (Ninja generator):
```
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . -j8
```

Windows (Visual Studio generator):
```
cmake -S . -B build -A x64 -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug --target all
```

Run the emulator:
- From inside the build directory:
    ```
    ./atmega33
    ```
    (Windows: build\Debug\atmega33.exe or build\atmega33.exe depending on generator/config.)
- From the repository root (if the program expects relative paths):
    ```
    ./build/atmega33
    ```

If you see an error like “can't find file”, it usually means the emulator expects test files relative to the current working directory. Either run the binary from the build directory, pass an explicit path to the input file (if supported), or run the one in the build folder from the repository root as shown above.

## Contribution

Please fork the repository, create a topic branch for your change, and open a pull request with a short description and tests demonstrating the new behavior.

## Where to Learn More

- Microchip/Atmel ATmega32 datasheet — comprehensive device specs and peripherals
- AVR Instruction Set Manual — detailed instruction descriptions, formats, and opcode encodings
- AVR libc and avr-gcc documentation for toolchain and assembly usage
- AVR Freaks (community) and official Microchip documentation for examples and application notes
- Open-source AVR emulators and educational projects (search for “AVR emulator” or “avr simulator”) for reference implementations

If you want links to specific documentation files (datasheets or manuals), let me know and I’ll add URLs based on the latest resources.