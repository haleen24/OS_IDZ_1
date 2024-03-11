cmake_minimum_required(VERSION 3.22)
project(4b C)

set(CMAKE_C_STANDARD 11)

add_executable(4b main.c ../lib/analysis.c cmake-build-debug/reader.c)
