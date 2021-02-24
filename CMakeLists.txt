cmake_minimum_required(VERSION 3.17)
project(megadrive C)

set(CMAKE_C_STANDARD 99)

add_executable(megadrive main.c m68k/cpu.c m68k/cpu.h utils/utils.c utils/utils.h memory/memory.c memory/memory.h)