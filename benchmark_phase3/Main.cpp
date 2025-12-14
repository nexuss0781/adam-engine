// ===================================================================================
//  File:    Main.cpp
//  Project: Adam Engine Benchmark (Phase 3)
//  Purpose: Validates the performance of the parallel simulation engine. It
//           directly compares a single-threaded update loop against the
//           multi-threaded JobSystem to measure the speedup factor.
// ===================================================================================

#include "../src/core/JobSystem.h"
#include "../src/core/CellStore.h"
#include "../src/simulation/Simulation.h"
#include "../src/simulation/MetabolismSystem.h" // Our existing system

#include <iostream>
#include <chrono>
#include <numeric>

// --- Benchmark Configuration ---
constexpr size_t CELL_COUNT = 1'000'000;
constexpr float  DELTA_TIME = 0.016f; // A single frame at ~60 FPS

// ===================================================================================
// A simple, new system for this benchmark to increase the workload.
// It will be defined here for simplicity.
// ===================================================================================
class MovementSystem : public ISystem
{
public:
    void Update(CellBlock& block, float deltaTime) override
    {
        // This is the logic that will be run in parallel on a single block.
        for (size_t i = 0; i < block.GetCapacity(); ++i)
        {
            if (block.flags[i] & CellBlock::FLAG_ACTIVE)
            {
                block.positions[i].x += 0.01f * deltaTime;
                block.positions[i].y -= 0.005f * deltaTime;
            }
        }
    }
};
// ===================================================================================


void print_header()
{
    std::cout << "====================================================\n";
    std::cout << "  Adam Engine: Phase 3 Benchmark (Parallel Speedup) \n";
    std::cout << "====================================================\n";
    std::cout << "Total Cells: " << CELL_COUNT << "\n\n";
}

int main()
{
    print_header();

    // --- 1. SETUP PHASE ---
    std::cout << "[SETUP] Initializing core systems...\n";
    JobSystem jobSystem;
    CellStore cellStore(CELL_COUNT);
    Simulation simulation(cellStore, jobSystem);
    
    std::cout << "[SETUP] Using " << jobSystem.GetThreadCount() << " worker threads.\n";

    // Register the systems that will participate in the simulation.
    simulation.RegisterSystem(std::make_unique<MetabolismSystem>());
    simulation.RegisterSystem(std::make_unique<MovementSystem>());
    std::cout << "[SETUP] Registered 2 simulation systems.\n";

    // Populate the world with cells.
    for (size_t i = 0; i < CELL_COUNT; ++i)
    {
        cellStore.CreateCell();
    }
    std::cout << "[SETUP] Spawned " << cellStore.GetActiveCellCount() << " cells.\n\n";

    // --- 2. SINGLE-THREADED BENCHMARK ---
    std::cout << "[BENCHMARK] Running SINGLE-THREADED simulation tick...\n";
    auto st_startTime = std::chrono::high_resolution_clock::now();
    
    // Manually replicate the Simulation::Tick logic on the main thread.
    MetabolismSystem st_metabolism;
    MovementSystem st_movement;
    for (const auto& blockPtr : cellStore.GetBlocks())
    {
        if (blockPtr->GetActiveCount() > 0)
        {
            st_metabolism.Update(*blockPtr, DELTA_TIME);
            st_movement.Update(*blockPtr, DELTA_TIME);
        }
    }

    auto st_endTime = std::chrono::high_resolution_clock::now();
    auto st_duration = std::chrono::duration_cast<std::chrono::microseconds>(st_endTime - st_startTime);
    double st_durationMs = st_duration.count() / 1000.0;
    std::cout << "Single-threaded time: " << st_durationMs << " ms\n\n";


    // --- 3. MULTI-THREADED BENCHMARK ---
    std::cout << "[BENCHMARK] Running MULTI-THREADED simulation tick...\n";
    auto mt_startTime = std::chrono::high_resolution_clock::now();

    // This single call does the same work, but in parallel.
    simulation.Tick(DELTA_TIME);

    auto mt_endTime = std::chrono::high_resolution_clock::now();
    auto mt_duration = std::chrono::duration_cast<std::chrono::microseconds>(mt_endTime - mt_startTime);
    double mt_durationMs = mt_duration.count() / 1000.0;
    std::cout << "Multi-threaded time:  " << mt_durationMs << " ms\n\n";


    // --- 4. REPORTING ---
    std::cout << "--- BENCHMARK RESULTS ---\n";
    double speedup = st_durationMs / mt_durationMs;
    std::cout << "Speedup Factor: " << speedup << "x\n";

    // A good speedup is typically >50% of the number of worker threads.
    if (speedup > (jobSystem.GetThreadCount() * 0.5))
    {
        std::cout << "\n[RESULT] SUCCESS! Significant performance gain from parallelization.\n";
    }
    else if (speedup > 1.2)
    {
        std::cout << "\n[RESULT] OK. Modest performance gain achieved.\n";
    }
    else
    {
        std::cout << "\n[RESULT] FAILURE! Parallelization overhead outweighs the benefits.\n";
    }
    std::cout << "====================================================\n";

    return 0;
}
