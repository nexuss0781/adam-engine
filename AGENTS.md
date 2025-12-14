# AGENT TEST PLAN: Phase 2 Validation

## 1. Objective

This document outlines the procedure for an agent to test and validate the successful implementation of **Phase 2: The Lifecycle & Recycling System**.

The primary goal is to verify that the `CellStore` management system can handle high-volume, continuous creation and destruction of cells while meeting three critical criteria:
1.  **Memory Stability:** No memory leaks occur during the recycling process.
2.  **Performance:** The system can sustain a high throughput of create/destroy operations.
3.  **Correctness:** The `CellHandle` and lookup systems remain consistent and do not lead to errors.

## 2. Prerequisites

-   A C++17 compliant compiler (e.g., GCC, Clang, MSVC).
-   CMake (version 3.16 or higher).
-   A system monitoring tool to observe RAM usage (e.g., Task Manager, `htop`).

## 3. Test Procedure

### Step 1: Verify Source Code Structure

Ensure the project directory includes the new and modified files for Phase 2. The expected structure now includes:

```
/adam_engine/
├── ... (previous files)
├── /src/
│   └── /core/
│       ├── CellHandle.h         # NEW
│       ├── CellStore.h          # NEW
│       ├── CellStore.cpp        # NEW
│       └── ... (modified CellBlock files)
└── /benchmark_phase2/           # NEW DIRECTORY
    ├── CMakeLists.txt
    └── Main.cpp
```

### Step 2: Configure and Compile the Benchmark

Create or clean the build directory and run CMake and the build tool to compile the new `benchmark_phase2` executable.

```bash
# Navigate to the root of the project
cd adam_engine

# Create a fresh build directory
rm -rf build
mkdir build
cd build

# Run CMake to configure the project
cmake ../

# Compile the specific benchmark_phase2 target
# Using Makefiles:
make benchmark_phase2

# Using CMake's build tool (cross-platform):
# cmake --build . --target benchmark_phase2 --config Release
```

An executable file named `benchmark_phase2` (or `.exe`) should now be present in the `/build/benchmark_phase2/` directory.

### Step 3: Execute and Monitor the Benchmark

1.  Open your system's memory monitoring tool and have it ready to observe the `benchmark_phase2` process.
2.  Navigate to the executable's directory and run it.

    ```bash
    # From within the 'build' directory
    cd benchmark_phase2
    ./benchmark_phase2
    ```
3.  The program will spawn the initial cell population and pause. At the prompt `Press Enter to begin the benchmark...`, **record the initial memory usage**.
4.  Press `Enter`. The 5-second stress test will begin.
5.  **CRITICAL:** During these 5 seconds, **watch the memory usage graph/value**. It should fluctuate slightly but must not show a continuous, upward trend.
6.  The test will complete and print the final results.

### Step 4: Record the Output

Copy the entire output from the terminal. It will look similar to this:

```
--- BENCHMARK RESULTS ---
Test completed in: 5.001 seconds over 250 ticks.
Total operations (Create/Destroy): 5000000
Throughput: 999800 ops/sec

[RESULT] SUCCESS! Memory is stable and throughput is high.
Final active cell count: 1000000
===============================================
```

## 4. Acceptance Criteria

The test is considered **PASSED** if and only if **ALL** of the following criteria are met:

-   [ ] **Compilation & Execution:** The project compiles and runs to completion without crashes.
-   [ ] **Memory Stability:** The observed memory usage during the 5-second test **remains stable**. There is no significant, sustained increase from the initial recorded value.
-   [ ] **Correctness:** The "Final active cell count" printed at the end is **exactly equal** to the `INITIAL_CELL_COUNT` (1,000,000).
-   [ ] **Performance:** The reported "Throughput" is greater than **1,000,000 ops/sec**.

## 5. Failure Conditions

The test is considered **FAILED** if **ANY** of the following occur:

-   The code fails to compile or crashes.
-   Memory usage grows steadily during the test, indicating a memory leak.
-   The final cell count is not 1,000,000.
-   Throughput is below 1,000,000 ops/sec.

If the test fails, archive the build logs and test output for review.
