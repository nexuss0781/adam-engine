# AGENT TEST PLAN: Phase 1 Validation

## 1. Objective

This document outlines the procedure for an agent (human tester or CI pipeline) to test and validate the successful implementation of **Phase 1: The Core Memory Engine**.

The primary goal is to verify that the Data-Oriented Design (DoD) architecture meets its stated performance and memory efficiency targets.

## 2. Prerequisites

Before beginning the test, ensure the following tools are installed and available in the system's PATH:

-   A C++17 compliant compiler (e.g., GCC, Clang, MSVC).
-   CMake (version 3.16 or higher).
-   A system monitoring tool to observe RAM usage (e.g., Task Manager on Windows, `htop` on Linux, Activity Monitor on macOS).

## 3. Test Procedure

Follow these steps in the specified order.

### Step 1: Obtain the Source Code

Ensure the complete source code for Phase 1 is present in a directory. The expected structure is:

```
/adam_engine/
├── CMakeLists.txt
├── /src/
│   └── /core/
│       ├── DataTypes.h
│       ├── CellArchetype.h
│       ├── ArchetypeLibrary.h
│       ├── ArchetypeLibrary.cpp
│       ├── CellBlock.h
│       ├── CellBlock.cpp
│       └── CellProxy.h
└── /benchmark/
    ├── CMakeLists.txt
    └── Main.cpp
```

### Step 2: Configure the Build System

Create a build directory and run CMake to generate the build files.

```bash
# Navigate to the root of the project
cd adam_engine

# Create a build directory
mkdir build
cd build

# Run CMake to configure the project
# For Makefiles (Linux/macOS):
cmake ../

# For Visual Studio (Windows):
# cmake -G "Visual Studio 17 2022" ../
```

### Step 3: Compile the Benchmark Executable

Build the project using the generated build files.

```bash
# From within the 'build' directory

# Using Makefiles:
make

# Using CMake's build tool (cross-platform):
# cmake --build . --config Release
```

An executable file named `benchmark` (or `benchmark.exe` on Windows) should now be present in the `/build/benchmark/` directory.

### Step 4: Prepare for Execution

1.  Open your system's memory monitoring tool (Task Manager, `htop`, etc.).
2.  Be prepared to observe the memory consumption of the `benchmark` process when it pauses.

### Step 5: Execute the Benchmark

Run the compiled executable from the `build` directory.

```bash
# From within the 'build' directory
cd benchmark
./benchmark
```

The program will print setup information and then pause with the message:
`NOTE: Please observe memory usage in your system's Task Manager now.`
`Press Enter to begin the performance test...`

At this point, **record the memory usage** of the process. After recording, press `Enter` to continue the test.

### Step 6: Record the Output

The program will complete the performance test and print the final results. Copy the entire output, which will look similar to this:

```
--- BENCHMARK RESULTS ---
Total cells updated: 1000000
Total time taken:    4.123 ms
Target time (60FPS): 16.67 ms

[RESULT] SUCCESS! Iteration time is within the 60 FPS budget.
=========================================
```

## 4. Acceptance Criteria

The test is considered **PASSED** if and only if **ALL** of the following criteria are met:

-   [ ] **Compilation:** The project compiles successfully without any errors.
-   [ ] **Execution:** The benchmark runs to completion without crashing or throwing unhandled exceptions.
-   [ ] **Memory Efficiency:** The recorded memory usage for the `benchmark` process (at the pause) is **less than 64 MB**.
-   [ ] **Performance:** The reported "Total time taken" is **less than 16.67 ms**.

## 5. Failure Conditions

The test is considered **FAILED** if **ANY** of the following occur:

-   The code fails to compile.
-   The program crashes during execution.
-   Memory usage exceeds 64 MB.
-   The reported iteration time is greater than or equal to 16.67 ms.

If the test fails, archive the build logs and test output for review by the development team.
