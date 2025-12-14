// ===================================================================================
//  File:    Main.cpp
//  Project: Adam Engine Benchmark
//  Purpose: Validates the performance and memory efficiency of the Phase 1
//           core engine components. It spawns a large number of cells and
//           measures the time required to iterate and update them.
// ===================================================================================

#include "../src/core/ArchetypeLibrary.h"
#include "../src/core/CellBlock.h"
#include "../src/core/CellProxy.h"

#include <iostream>
#include <vector>
#include <chrono>
#include <memory> // For std::unique_ptr

// --- Benchmark Configuration ---
constexpr size_t TOTAL_CELLS_TO_SPAWN = 1'000'000;
constexpr size_t CELLS_PER_BLOCK      = 65'536; // 2^16, a common block size
constexpr float  TARGET_FRAME_TIME_MS = 16.67f;   // Target for 60 FPS

void print_header()
{
    std::cout << "=========================================\n";
    std::cout << "  Adam Engine: Phase 1 Benchmark         \n";
    std::cout << "=========================================\n";
    std::cout << "Target Cells: " << TOTAL_CELLS_TO_SPAWN << "\n";
    std::cout << "Block Capacity: " << CELLS_PER_BLOCK << "\n\n";
}

int main()
{
    print_header();

    // --- 1. SETUP PHASE ---
    std::cout << "[SETUP] Initializing Archetype Library...\n";
    auto& library = ArchetypeLibrary::Instance();

    CellArchetype skinArchetype;
    skinArchetype.name = "Skin Cell";
    skinArchetype.baseHealth = 80;
    skinArchetype.renderColor = {0.8f, 0.6f, 0.5f};
    const uint8 skinTypeID = library.RegisterArchetype(skinArchetype);

    CellArchetype muscleArchetype;
    muscleArchetype.name = "Muscle Cell";
    muscleArchetype.baseHealth = 150;
    muscleArchetype.metabolismRate = 2.5f;
    muscleArchetype.renderColor = {0.9f, 0.2f, 0.2f};
    const uint8 muscleTypeID = library.RegisterArchetype(muscleArchetype);
    
    std::cout << "Registered '" << library.GetArchetype(skinTypeID).name << "' with ID " << (int)skinTypeID << "\n";
    std::cout << "Registered '" << library.GetArchetype(muscleTypeID).name << "' with ID " << (int)muscleTypeID << "\n\n";

    // --- 2. SPAWNING PHASE ---
    std::cout << "[SPAWN] Spawning " << TOTAL_CELLS_TO_SPAWN << " cells...\n";
    std::vector<std::unique_ptr<CellBlock>> blocks;
    blocks.emplace_back(std::make_unique<CellBlock>(CELLS_PER_BLOCK));

    for (size_t i = 0; i < TOTAL_CELLS_TO_SPAWN; ++i)
    {
        CellBlock* currentBlock = blocks.back().get();
        if (currentBlock->GetActiveCount() >= currentBlock->GetCapacity())
        {
            blocks.emplace_back(std::make_unique<CellBlock>(CELLS_PER_BLOCK));
            currentBlock = blocks.back().get();
        }
        
        size_t index = currentBlock->ActivateCell();
        CellProxy cell(currentBlock, index);

        // Initialize the new cell with some data
        cell.SetTypeID((i % 2 == 0) ? skinTypeID : muscleTypeID);
        cell.SetHealth(cell.GetBaseHealth());
        cell.SetEnergy(100 + (i % 155));
        cell.SetPosition({ (float)(i % 100), (float)((i/100)%100), (float)(i/10000) });
    }
    std::cout << "Spawn complete. Total blocks created: " << blocks.size() << "\n\n";
    std::cout << "NOTE: Please observe memory usage in your system's Task Manager now.\n";
    std::cout << "Press Enter to begin the performance test...\n";
    std::cin.get();

    // --- 3. ITERATION BENCHMARK ---
    std::cout << "[BENCHMARK] Running single-tick simulation update...\n";
    
    auto startTime = std::chrono::high_resolution_clock::now();

    // This is the core simulation loop. Note the direct array access for max performance.
    for (const auto& blockPtr : blocks)
    {
        CellBlock* block = blockPtr.get();
        const size_t activeCount = block->GetActiveCount();
        for (size_t i = 0; i < activeCount; ++i)
        {
            // Simulate some work: move the cell slightly and consume energy
            block->positions[i].x += 0.01f;
            if (block->energies[i] > 0)
            {
                block->energies[i]--;
            }
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    float durationMs = duration.count() / 1000.0f;

    // --- 4. REPORTING ---
    std::cout << "\n--- BENCHMARK RESULTS ---\n";
    std::cout << "Total cells updated: " << TOTAL_CELLS_TO_SPAWN << "\n";
    std::cout << "Total time taken:    " << durationMs << " ms\n";
    std::cout << "Target time (60FPS): " << TARGET_FRAME_TIME_MS << " ms\n";

    if (durationMs < TARGET_FRAME_TIME_MS)
    {
        std::cout << "\n[RESULT] SUCCESS! Iteration time is within the 60 FPS budget.\n";
    }
    else
    {
        std::cout << "\n[RESULT] FAILURE! Iteration time exceeded the 60 FPS budget.\n";
    }
    std::cout << "=========================================\n";

    return 0;
}
