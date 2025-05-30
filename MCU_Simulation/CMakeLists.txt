cmake_minimum_required(VERSION 3.24)
project(MCU_Simulation C)

set(CMAKE_C_STANDARD 11)

add_executable(MCU_Simulation main.c
        sha256.c)
