# ===================================================================================
#  File:    CMakeLists.txt
#  Project: Genesis Engine Benchmark (Phase G1)
#  Purpose: Build script for the G1 benchmark executable. This test validates the
#           entire pipeline: model parsing, canvas creation, and imprinting.
# ===================================================================================

cmake_minimum_required(VERSION 3.16)
project(GenesisBenchmarkG1 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the benchmark executable
add_executable(benchmark_genesis1 Main.cpp)

# Since all Genesis modules in Phase G1 are header-only, we only need to
# provide the include path. No other source files need to be compiled.
target_include_directories(benchmark_genesis1 PRIVATE ../src)

# On POSIX systems (Linux, macOS), link the pthreads library if you plan to
# use std::thread in the benchmark's Main.cpp, although it's not needed for G1.
if(UNIX AND NOT APPLE)
    target_link_libraries(benchmark_genesis1 PRIVATE pthread)
endif()

