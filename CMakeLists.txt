cmake_minimum_required(VERSION 3.10.0)
project(atmega33 VERSION 0.1.0 LANGUAGES C)

add_executable(atmega33
    header.h
    memory.h memory.c
    main.c)

