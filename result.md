# Change Report: Genesis Engine G1 Benchmark Fix

## Introduction

The primary goal of this task was to resolve a failing validation test in the `benchmark_genesis1` suite. The benchmark was designed to test the `QuantumCanvas` and `BlueprintImprinter` components of the Genesis Engine. The investigation revealed two distinct root causes: a critical memory corruption bug in the `QuantumCanvas` data structure and a subtle logical flaw in the benchmark's test setup. This report details the problems and their solutions.

---

### 1. Critical Bug Fix: Memory Corruption in `QuantumCanvas`

The most significant issue was a memory corruption bug that caused unpredictable segmentation faults. The bug was located in the `InsertRecursive` function of the `QuantumCanvas` class.

**Problem:**

The function stored a reference (`Node& node`) to an element within the `m_nodes` `std::vector`. Later in the function, it would call `m_nodes.emplace_back()` to add new child nodes. If the vector's capacity was exceeded, `emplace_back` would trigger a reallocation, moving the vector's entire memory block to a new location. This would invalidate all existing pointers and references to its elements, turning `node` into a **dangling reference**. Subsequent writes to `node` would corrupt memory, leading to a crash.

**Pseudocode (Before):**

```cpp
function InsertRecursive(nodeIndex, ...):
  // 1. A reference is taken to an element in the vector.
  Node& node = m_nodes[nodeIndex];

  if (node.isLeaf):
    // ...
    for i from 0 to ChildCount:
      // 2. This call might reallocate the entire 'm_nodes' vector,
      //    invalidating the 'node' reference.
      m_nodes.emplace_back(oldData);

      // 3. DANGEROUS: Writing to 'node' here could be writing to deallocated memory.
      node.childNodeIndices[i] = new_child_index;
    // ...
```

**Solution:**

The fix was to avoid holding a reference across any operation that could cause a vector reallocation. Instead of storing a reference, the node is now accessed directly by its index (`m_nodes[nodeIndex]`) whenever it is needed. This ensures that even if the vector reallocates, the code always accesses the correct, current memory location.

**Pseudocode (After):**

```cpp
function InsertRecursive(nodeIndex, ...):
  // Do NOT store a long-lived reference.

  // 1. Access by index to check the leaf status.
  if (m_nodes[nodeIndex].isLeaf):
    // ...
    for i from 0 to ChildCount:
      // 2. This call can still reallocate the vector.
      m_nodes.emplace_back(oldData);

      // 3. SAFE: Access the node again by index. This will point to the
      //    correct memory location, whether a reallocation happened or not.
      m_nodes[nodeIndex].childNodeIndices[i] = new_child_index;
    // ...
```

---

### 2. Logical Fix: Voxel Grid Alignment in Benchmark

After fixing the memory corruption, the benchmark ran to completion but still failed its final validation check. The logical issue was in the test setup within `benchmark_genesis1/Main.cpp`.

**Problem:**

The test verifies that the value at the exact center of the canvas `(0,0,0)` is correctly set to `-1.0` (inside). However, the `BlueprintImprinter` was configured with a voxel grid resolution of `32x32x32`. A grid with an even number of divisions does not have a voxel perfectly centered at the origin. Instead, the origin falls on the boundary plane between multiple voxels, so the center point was never being marked as "inside".

**Conceptual Illustration (1D):**

A grid with `resolution = 4` has no central cell:
```
Grid: | Voxel 1 | Voxel 2 | Voxel 3 | Voxel 4 |
Center Point:           ^ (falls on the boundary)
```

A grid with `resolution = 3` has a clear central cell:
```
Grid: | Voxel 1 | Voxel 2 | Voxel 3 |
Center Point:               ^ (falls inside Voxel 2)
```

**Solution:**

The resolution was changed from `32` to `33`. By using an odd number, we ensure that a single voxel is perfectly centered at the origin, aligning the voxelization logic with the benchmark's verification point.

---

### 3. Code Restoration and Housekeeping

- **Restored `BlueprintImprinter.h`**: Previous attempts to fix the benchmark involved simplifying the geometric tests. These workarounds were reverted, and the original, more robust plane-based intersection algorithm was restored. With the root causes fixed, this algorithm now functions correctly.
- **Updated `.gitignore`**: The benchmark generates a `cube.obj` file at runtime. This file was added to `.gitignore` to prevent generated artifacts from being tracked in the repository.

---

## Conclusion

By addressing both a critical memory safety issue and a subtle logical flaw in the test's design, the `benchmark_genesis1` is now reliable. The fixes ensure that the Genesis Engine's core components are validated correctly, and the underlying `QuantumCanvas` data structure is more robust and safe.