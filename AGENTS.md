# AGENT TEST PLAN: Phase 3 Validation

## 1. Objective

This document outlines the procedure for an agent to test and validate the successful implementation of **Phase 3: The Simulation Engine & Job System**.

The primary goal is to verify that the multi-threaded simulation architecture provides a significant, measurable performance speedup compared to an equivalent single-threaded execution of the same logic.

## 2. Prerequisites

-   A C++17 compliant compiler (e.g., GCC, Clang, MSVC).
-   CMake (version 3.16 or higher).
-   A multi-core CPU (at least 2 cores, 4+ recommended to see significant gains).

## 3. Test Procedure

### Step 1: Verify Source Code Structure

Ensure the project directory includes the new files and directories for Phase 3. The expected structure now includes:

```
/adam_engine/
├── ... (previous files)
├── /src/
│   ├── /core/
│   │   ├── JobSystem.h
│   │   └── JobSystem.cpp
│   └── /simulation/
│       ├── ISystem.h
│       ├── MetabolismSystem.h
│       ├── MetabolismSystem.cpp
│       ├── Simulation.h
│       └── Simulation.cpp
└── /benchmark_phase3/
    ├── CMakeLists.txt
    └── Main.cpp
```

### Step 2: Configure and Compile the Benchmark

Create or clean the build directory and run CMake and the build tool to compile the `benchmark_phase3` executable.

```bash
# Navigate to the root of the project
cd adam_engine

# Create a fresh build directory
rm -rf build
mkdir build
cd build

# Run CMake to configure the project
cmake ../

# Compile the specific benchmark_phase3 target
# Using Makefiles:
make benchmark_phase3

# Using CMake's build tool (cross-platform):
# cmake --build . --target benchmark_phase3 --config Release
```

An executable file named `benchmark_phase3` (or `.exe`) should now be present in the `/build/benchmark_phase3/` directory.

### Step 3: Execute the Benchmark

Navigate to the executable's directory and run it. The test will run automatically and print its results without user interaction.

```bash
# From within the 'build' directory
cd benchmark_phase3
./benchmark_phase3
```

### Step 4: Record the Output

The program will complete the performance test and print the final results. Copy the entire output, which will look similar to this:

```
--- BENCHMARK RESULTS ---
Speedup Factor: 3.85x

[RESULT] SUCCESS! Significant performance gain from parallelization.
====================================================
```

## 4. Acceptance Criteria

The test is considered **PASSED** if and only if **ALL** of the following criteria are met:

-   [ ] **Compilation & Execution:** The project compiles and runs to completion without crashes.
-   [ ] **Performance Gain:** The reported "Speedup Factor" is **greater than 1.5x**. This demonstrates a clear and meaningful advantage from the multi-threaded architecture.
-   [ ] **Benchmark Verdict:** The final `[RESULT]` message is either `SUCCESS` or `OK`.

## 5. Failure Conditions

The test is considered **FAILED** if **ANY** of the following occur:

-   The code fails to compile or crashes.
-   The reported "Speedup Factor" is less than or equal to 1.5x, indicating that the parallelization overhead is too high or the implementation is not effective.
-   The benchmark reports `[RESULT] FAILURE!`.

If the test fails, archive the build logs and test output for review by the development team.
