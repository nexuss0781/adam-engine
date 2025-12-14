.# ===================================================================================
#  File:    CMakeLists.txt
#  Project: Adam Engine Benchmark (Phase 3)
#  Purpose: Build script for the Phase 3 benchmark executable, which compares
#           the performance of single-threaded vs. multi-threaded simulation.
# ===================================================================================

cmake_minimum_required(VERSION 3.16)
project(AdamBenchmarkPhase3 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the new benchmark executable for Phase 3
add_executable(benchmark_phase3 Main.cpp)

# Link the executable to all necessary source files from the core engine and simulation
target_include_directories(benchmark_phase3 PRIVATE ../)

target_sources(benchmark_phase3 PRIVATE
    # Phase 1 & 2 Core Data Layer
    ../src/core/ArchetypeLibrary.cpp
    ../src/core/CellBlock.cpp
    ../src/core/CellStore.cpp
    
    # Phase 3 Core Parallelism Layer
    ../src/core/JobSystem.cpp
    
    # Phase 3 Simulation Logic Layer
    ../src/simulation/MetabolismSystem.cpp
    ../src/simulation/Simulation.cpp
)

# On POSIX systems (Linux, macOS), we need to link the pthreads library for std::thread
if(UNIX AND NOT APPLE)
    target_link_libraries(benchmark_phase3 PRIVATE pthread)
endif()
