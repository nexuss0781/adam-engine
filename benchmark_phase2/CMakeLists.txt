# ===================================================================================
#  File:    CMakeLists.txt
#  Project: Adam Engine Benchmark (Phase 2)
#  Purpose: Build script for the Phase 2 benchmark executable, which tests the
#           CellStore's dynamic creation and destruction capabilities.
# ===================================================================================

cmake_minimum_required(VERSION 3.16)
project(AdamBenchmarkPhase2 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the new benchmark executable for Phase 2
add_executable(benchmark_phase2 Main.cpp)

# Link the executable to the source files from the core engine.
# We include the new CellStore.cpp in this build.
target_include_directories(benchmark_phase2 PRIVATE ../)

target_sources(benchmark_phase2 PRIVATE
    ../src/core/ArchetypeLibrary.cpp
    ../src/core/CellBlock.cpp
    ../src/core/CellStore.cpp
)
