cmake_minimum_required(VERSION 3.29)
project(oronyx LANGUAGES CXX)

option(BUILD_TESTS "Build tests" ON)
option(BUILD_BENCHMARKS "Build benchmarks" OFF)

cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0091 NEW)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(compiler)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif ()