# AGENT TEST PLAN: Genesis Engine - Phase G1 Validation

## 1. Objective

This document outlines the procedure for an agent to test and validate the successful implementation of **Phase G1: The Quantum Canvas & Blueprint Engine**.

The goal is to verify the entire end-to-end pipeline:
1.  Correct parsing of a 3D model file.
2.  Successful instantiation of the `QuantumCanvas`.
3.  Proper functioning of the `BlueprintImprinter` to voxelize the model.
4.  Final validation of the canvas's in-memory state.

## 2. Prerequisites

-   A C++17 compliant compiler (e.g., GCC, Clang, MSVC).
-   CMake (version 3.16 or higher).

## 3. Test Procedure

### Step 1: Verify Source Code Structure

Ensure the project directory includes the new `genesis` directory and the `benchmark_genesis1` directory. The expected structure is:

```
/adam_engine/
├── ... (previous files)
├── /src/
│   └── /genesis/
│       ├── common/
│       │   └── DataTypes.h
│       ├── canvas/
│       │   └── QuantumCanvas.h
│       ├── io/
│       │   ├── PNGParser.h
│       │   └── OBJParser.h
│       └── imprinter/
│           └── BlueprintImprinter.h
└── /benchmark_genesis1/
    ├── CMakeLists.txt
    └── Main.cpp
```

### Step 2: Configure and Compile the Benchmark

Create or clean the build directory and run CMake and the build tool to compile the `benchmark_genesis1` executable.

```bash
# Navigate to the root of the project
cd adam_engine

# Create a fresh build directory
rm -rf build
mkdir build
cd build

# Run CMake to configure the project
cmake ../

# Compile the specific benchmark_genesis1 target
# Using Makefiles:
make benchmark_genesis1

# Using CMake's build tool (cross-platform):
# cmake --build . --target benchmark_genesis1 --config Release
```

An executable file named `benchmark_genesis1` (or `.exe`) should now be present in the `/build/benchmark_genesis1/` directory.

### Step 3: Execute the Benchmark

Navigate to the executable's directory and run it. The test will run automatically and print its results without user interaction.

```bash
# From within the 'build' directory
cd benchmark_genesis1
./benchmark_genesis1
```
The benchmark will first create a `cube.obj` file in this directory and then proceed with the test.

### Step 4: Record the Output

The program will complete the test and print the final results. Copy the entire output from the terminal. A successful run will look like this:

```
====================================================
  Genesis Engine: Phase G1 Benchmark
  (Canvas & Blueprint Imprinter Validation)
====================================================

[SETUP] Creating test model file: cube.obj
[TEST 1] Parsing 3D model...
  - Vertices found: 8
  - Polygons found: 6
[RESULT] SUCCESS! Model parsed correctly.

[TEST 2] Creating QuantumCanvas...
[RESULT] SUCCESS! Canvas initialized.

[TEST 3] Imprinting blueprint onto canvas...
  - Imprinting took XX ms.
[RESULT] SUCCESS! Imprinting process completed.

[TEST 4] Verifying canvas state...
  - Value at center (should be -1.0): -1
  - Value outside (should be 1.0): 1
[RESULT] SUCCESS! Canvas was correctly modified by the imprinter.

====================================================
  Phase G1 has been successfully validated.
====================================================
```

## 4. Acceptance Criteria

The test is considered **PASSED** if and only if **ALL** of the following criteria are met:

-   [ ] **Compilation & Execution:** The project compiles and runs to completion without crashes.
-   [ ] **Parsing:** The benchmark successfully parses the `cube.obj` model.
-   [ ] **Imprinting:** The imprinting process completes without errors.
-   [ ] **Verification:** The final verification test reports **SUCCESS**, confirming that the in-memory state of the `QuantumCanvas` is correct (`-1.0` inside the shape, `1.0` outside).

## 5. Failure Conditions

The test is considered **FAILED** if **ANY** of the following occur:
-   The code fails to compile or crashes.
-   The benchmark reports `FAILURE!` at any stage of the test.

If the test fails, archive the build logs and test output for review.
