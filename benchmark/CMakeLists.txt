# CMakeLists.txt for the benchmark executable

cmake_minimum_required(VERSION 3.16)
project(AdamBenchmark CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the benchmark executable
add_executable(benchmark Main.cpp)

# Link the executable to the source files from the core engine
# We are not building a separate library for simplicity here, just including the .cpp files
target_include_directories(benchmark PRIVATE ../)

target_sources(benchmark PRIVATE
    ../src/core/ArchetypeLibrary.cpp
    ../src/core/CellBlock.cpp
)
