cmake_minimum_required(VERSION 3.10.0)
project(atmega33 VERSION 0.1.0 LANGUAGES C)

add_executable(atmega33
    header.h
    memory.h memory.c
    cpu.h cpu.c
    instgen.h
    opcode.h
    assembler.h assembler.c
    tui.h tui.c
    main.h main.c)

