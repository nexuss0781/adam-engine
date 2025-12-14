# **Nexuss Biomimetic Engine: Technical Documentation**

## **Table of Contents**
1.  [Introduction](#1-introduction)
    *   [Design Philosophy: Data-Oriented Design](#design-philosophy-data-oriented-design)
2.  [Core Architectural Concepts](#2-core-architectural-concepts)
    *   [The ECS Pattern: Entity, Component, System](#the-ecs-pattern-entity-component-system)
    *   [Structure of Arrays (SoA) vs. Array of Structs (AoS)](#structure-of-arrays-soa-vs-array-of-structs-aos)
    *   [The Flyweight Pattern: Archetypes](#the-flyweight-pattern-archetypes)
    *   [The Generational Handle: Stable Entity References](#the-generational-handle-stable-entity-references)
3.  [Module Deep Dive & API Reference](#3-module-deep-dive--api-reference)
    *   [The Data Core Layer](#the-data-core-layer)
    *   [The Lifecycle Management Layer](#the-lifecycle-management-layer)
    *   [The Parallel Execution Layer](#the-parallel-execution-layer)
4.  [Developer How-To Guides](#4-developer-how-to-guides)
    *   [Guide 1: Defining a New Cell Type](#guide-1-defining-a-new-cell-type)
    *   [Guide 2: Spawning, Accessing, and Destroying Cells](#guide-2-spawning-accessing-and-destroying-cells)
    *   [Guide 3: Creating a New Biological System](#guide-3-creating-a-new-biological-system)
    *   [Guide 4: Setting Up a Main Simulation Loop](#guide-4-setting-up-a-main-simulation-loop)
5.  [Performance Best Practices](#5-performance-best-practices)
6.  [Glossary of Terms](#6-glossary-of-terms)

---

## **1. Introduction**

The Nexuss Biomimetic Engine is a C++17 high-performance simulation framework designed for massive-scale, dynamic systems. It is built from the ground up to overcome the performance and memory limitations inherent in traditional Object-Oriented (OOP) architectures.

### **Design Philosophy: Data-Oriented Design**

The central philosophy of Nexuss is **Data-Oriented Design (DoD)**. Instead of focusing on objects and their behaviors, we focus on the data itself and how it is transformed. This leads to an architecture that is exceptionally friendly to modern CPU caches, resulting in orders-of-magnitude performance gains.

| Traditional OOP | Nexuss Engine (DoD) |
| :--- | :--- |
| `Cell myCell = new Cell();` | `CellHandle myHandle = cellStore.CreateCell();` |
| Data and logic are tightly coupled in objects. | Data is decoupled from logic. |
| Memory is scattered, leading to cache misses. | Memory is contiguous, leading to cache hits. |
| Scales poorly with entity count. | Scales almost linearly with entity count. |

## **2. Core Architectural Concepts**

### **The ECS Pattern: Entity, Component, System**

Nexuss is a pure implementation of the Entity-Component-System (ECS) architectural pattern.

*   **Entity:** An **Entity** is a unique identifier. In Nexuss, this is the `CellHandle`. It is not a container for data; it is simply a key.
*   **Component:** **Components** are the raw data. In Nexuss, these are the attributes stored in the `CellBlock`'s parallel arrays (`positions`, `healths`, etc.).
*   **System:** A **System** is the logic. In Nexuss, this is any class that inherits from `ISystem` (e.g., `MetabolismSystem`). Systems iterate over Components of Entities that match their criteria.

### **Structure of Arrays (SoA) vs. Array of Structs (AoS)**

This is the most critical performance concept in the engine.

**AoS (Bad for Performance):**
```cpp
// Data is interleaved and not cache-friendly for single-attribute processing.
struct Cell { Vec3 position; uint8 health; };
Cell cells[1000];
```

**SoA (The Nexuss Way - Excellent for Performance):**
```cpp
// Data is contiguous. When a system processes positions, the CPU loads
// only other positions into its cache lines.
Vec3 positions[1000];
uint8 healths[1000];
```
The `CellBlock` class implements the SoA pattern.

### **The Flyweight Pattern: Archetypes**

To avoid storing redundant, static data for every cell, we use the Flyweight pattern.

*   `CellArchetype`: A struct holding the shared "DNA" of a cell type (e.g., base health, name, metabolism rate).
*   `ArchetypeLibrary`: A singleton that stores one instance of each `CellArchetype`.
*   The `typeID` component in a `CellBlock` is a simple `uint8` that acts as an index into the `ArchetypeLibrary`.

### **The Generational Handle: Stable Entity References**

A raw index into an array is unsafe. If an entity is destroyed and its index is reused, an old reference will point to the new, incorrect entity. `CellHandle` solves this:

*   `id`: The index into the `CellStore`'s master lookup table.
*   `generation`: A counter for that `id` slot.
*   When a cell is destroyed, `generation` is incremented. An old handle `(id: 10, gen: 1)` will no longer match the new state `(id: 10, gen: 2)`, safely invalidating it.

## **3. Module Deep Dive & API Reference**

### **The Data Core Layer**
*Namespace: `core` | Files: `CellBlock.h`, `CellArchetype.h`, `ArchetypeLibrary.h`*

*   **`CellBlock`**:
    *   **Purpose:** The raw SoA memory container.
    *   **API:** `AcquireSlot()`, `ReleaseSlot(index)`, `GetActiveCount()`.
    *   **Usage:** Developers rarely interact with `CellBlock` directly. It is managed entirely by the `CellStore`.
*   **`ArchetypeLibrary`**:
    *   **Purpose:** Singleton registry for cell DNA.
    *   **API:** `ArchetypeLibrary::Instance()`, `RegisterArchetype(archetype)`, `GetArchetype(typeID)`.

### **The Lifecycle Management Layer**
*Namespace: `core` | Files: `CellStore.h`, `CellHandle.h`, `CellProxy.h`*

*   **`CellStore`**:
    *   **Purpose:** The primary public interface for all cell lifecycle operations.
    *   **Key API:**
        *   `CellHandle CreateCell()`: Creates a new cell, returns a stable handle.
        *   `void DestroyCell(CellHandle)`: Destroys a cell and recycles its memory.
        *   `CellProxy GetCell(CellHandle)`: Returns a temporary proxy object to safely access a cell's data.
        *   `bool IsHandleValid(CellHandle)`: Checks if a handle points to a live cell.
        *   `const auto& GetBlocks() const`: Provides read-only access to the `CellBlock` list for the `Simulation` engine.
*   **`CellProxy`**:
    *   **Purpose:** A lightweight, temporary "view" into a cell's data.
    *   **API:** Provides getters and setters like `GetPosition()`, `SetHealth()`, etc. These methods compile down to direct array access, offering convenience with zero performance overhead.

### **The Parallel Execution Layer**
*Namespace: `core`, `simulation` | Files: `JobSystem.h`, `ISystem.h`, `Simulation.h`*

*   **`JobSystem`**:
    *   **Purpose:** A general-purpose, high-performance thread pool.
    *   **Key API:**
        *   `void Dispatch(Job)`: Submits a task (a `std::function<void()>`) to the queue.
        *   `void Wait()`: Blocks the calling thread until all dispatched jobs are complete.
*   **`ISystem`**:
    *   **Purpose:** The abstract interface for all game logic.
    *   **Key API:**
        *   `virtual void Update(CellBlock& block, float deltaTime) = 0;`: The function a developer implements to define a system's behavior on a single chunk of data.
*   **`Simulation`**:
    *   **Purpose:** The main orchestrator.
    *   **Key API:**
        *   `void RegisterSystem(std::unique_ptr<ISystem>)`: Adds a logic module to the simulation.
        *   `void Tick(float deltaTime)`: Executes one full frame of the simulation in parallel.

## **4. Developer How-To Guides**

### **Guide 1: Defining a New Cell Type**
```cpp
// 1. Get the library instance
auto& library = ArchetypeLibrary::Instance();

// 2. Define the archetype's properties
CellArchetype liverCell;
liverCell.name = "Hepatocyte";
liverCell.baseHealth = 120;
liverCell.metabolismRate = 1.5f;

// 3. Register it and store the ID
const uint8 LIVER_CELL_ID = library.RegisterArchetype(liverCell);
```

### **Guide 2: Spawning, Accessing, and Destroying Cells**
```cpp
// Assumes a CellStore instance named 'store'
CellStore store;

// --- Spawning ---
CellHandle myHandle = store.CreateCell();

// --- Accessing and Modifying ---
CellProxy myCell = store.GetCell(myHandle);
if (myCell.IsValid()) {
    myCell.SetTypeID(LIVER_CELL_ID);
    myCell.SetPosition({10.0f, 5.0f, 2.0f});
    myCell.SetHealth(myCell.GetBaseHealth());
}

// --- Destroying ---
store.DestroyCell(myHandle);

// --- Validation ---
// This will now return false
bool isStillValid = store.IsHandleValid(myHandle);
```

### **Guide 3: Creating a New Biological System**
```cpp
// ToxinProcessingSystem.h
#include "simulation/ISystem.h"
class ToxinProcessingSystem : public ISystem {
public:
    void Update(CellBlock& block, float deltaTime) override;
};

// ToxinProcessingSystem.cpp
#include "ToxinProcessingSystem.h"
#include "core/CellBlock.h"
#include "core/ArchetypeLibrary.h" // Assume LIVER_CELL_ID is globally available

void ToxinProcessingSystem::Update(CellBlock& block, float deltaTime) {
    for (size_t i = 0; i < block.GetCapacity(); ++i) {
        // IMPORTANT: Only process active cells!
        if ((block.flags[i] & CellBlock::FLAG_ACTIVE) && (block.typeIDs[i] == LIVER_CELL_ID)) {
            // Your logic here: e.g., consume energy to reduce a hypothetical toxin level.
            if (block.energies[i] > 1) {
                block.energies[i]--;
            }
        }
    }
}
```

### **Guide 4: Setting Up a Main Simulation Loop**
```cpp
#include "core/JobSystem.h"
#include "core/CellStore.h"
#include "simulation/Simulation.h"
#include "ToxinProcessingSystem.h" // Your new system

int main() {
    JobSystem jobSystem;
    CellStore cellStore(1'000'000); // Pre-allocate for 1 million cells
    Simulation simulation(cellStore, jobSystem);

    // Register all logic systems
    simulation.RegisterSystem(std::make_unique<ToxinProcessingSystem>());
    // ... register other systems ...

    // Main Loop
    bool isRunning = true;
    while(isRunning) {
        float deltaTime = 0.016f; // Calculate real delta time
        simulation.Tick(deltaTime);
        // ... handle input, rendering, etc. ...
    }

    return 0;
}
```

## **5. Performance Best Practices**

1.  **Iterate Contiguously:** When writing a system, always iterate through a `CellBlock`'s indices from `0` to `capacity`. This is how you leverage the CPU cache. Do **not** jump between random cells in different blocks.
2.  **Keep Systems Focused:** A system should do one thing well. A system that processes both movement and metabolism will have poor cache performance, as it will be accessing both `position` and `energy` arrays, polluting the cache.
3.  **Minimize Data Dependencies:** The `JobSystem` shines when jobs are independent. Avoid creating systems where the output of one cell's calculation in a frame is required as input for another cell in the same frame.
4.  **Trust the `CellStore`:** Do not attempt to manage memory yourself. The `CellStore` is highly optimized for its purpose. Always use it to create and destroy cells.

## **6. Glossary of Terms**
*   **DoD (Data-Oriented Design):** An architectural approach that prioritizes the layout of data in memory to maximize CPU performance.
*   **ECS (Entity-Component-System):** A specific DoD pattern that decouples identities (Entities) from data (Components) and logic (Systems).
*   **SoA (Structure of Arrays):** The memory layout pattern where each attribute of a collection is stored in its own contiguous array.
*   **Flyweight:** A design pattern used to minimize memory usage by sharing as much data as possible with other similar objects.
*   **Generational Index:** A strategy for creating stable entity references that are safe from reuse errors.
*   **Job:** A small unit of work (typically a lambda function) that can be executed by a thread pool.
