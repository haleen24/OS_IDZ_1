cmake_minimum_required(VERSION 3.22)
project(lib C)

set(CMAKE_C_STANDARD 11)

add_executable(lib
        analysis.c
        extension.c)
