// ===================================================================================
//  File:    Main.cpp
//  Project: Adam Engine Benchmark (Phase 2)
//  Purpose: Validates the performance and memory stability of the CellStore
//           lifecycle management system. It subjects the store to a high volume
//           of cell creation and destruction operations over time.
// ===================================================================================

#include "../src/core/CellStore.h"
#include "../src/core/ArchetypeLibrary.h"

#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <numeric>

// --- Benchmark Configuration ---
constexpr size_t INITIAL_CELL_COUNT      = 1'000'000;
constexpr size_t CELLS_TO_RECYCLE_PER_TICK = 10'000;
constexpr int    BENCHMARK_DURATION_SECONDS = 5;

void print_header()
{
    std::cout << "===============================================\n";
    std::cout << "  Adam Engine: Phase 2 Benchmark (Lifecycle) \n";
    std::cout << "===============================================\n";
    std::cout << "Initial Cells: " << INITIAL_CELL_COUNT << "\n";
    std::cout << "Recycle Rate:  " << CELLS_TO_RECYCLE_PER_TICK << " creates/destroys per tick\n";
    std::cout << "Test Duration: " << BENCHMARK_DURATION_SECONDS << " seconds\n\n";
}

int main()
{
    print_header();

    // --- 1. SETUP PHASE ---
    std::cout << "[SETUP] Initializing systems...\n";
    CellStore cellStore(INITIAL_CELL_COUNT);
    auto& library = ArchetypeLibrary::Instance();

    // We only need archetypes for initialization
    CellArchetype neuronArchetype;
    neuronArchetype.name = "Neuron";
    const uint8 neuronTypeID = library.RegisterArchetype(neuronArchetype);

    // --- 2. INITIAL POPULATION ---
    std::cout << "[SETUP] Spawning " << INITIAL_CELL_COUNT << " initial cells...\n";
    std::vector<CellHandle> live_handles;
    live_handles.reserve(INITIAL_CELL_COUNT);

    for (size_t i = 0; i < INITIAL_CELL_COUNT; ++i)
    {
        CellHandle handle = cellStore.CreateCell();
        live_handles.push_back(handle);
        
        // Initialize the new cell
        CellProxy cell = cellStore.GetCell(handle);
        if (cell.IsValid())
        {
            cell.SetTypeID(neuronTypeID);
            cell.SetHealth(100);
        }
    }
    
    std::cout << "Initial population complete. Active cells: " << cellStore.GetActiveCellCount() << "\n\n";
    std::cout << "NOTE: Please observe initial memory usage in your system's Task Manager.\n";
    std::cout << "The memory should remain STABLE during the test.\n";
    std::cout << "Press Enter to begin the benchmark...\n";
    std::cin.get();

    // --- 3. RECYCLING STRESS TEST ---
    std::cout << "[BENCHMARK] Starting " << BENCHMARK_DURATION_SECONDS << " second stress test...\n";
    
    // Setup for random selection
    std::random_device rd;
    std::mt19937 gen(rd());

    auto startTime = std::chrono::high_resolution_clock::now();
    long long totalOperations = 0;
    int ticks = 0;

    while (true)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
        if (elapsedSeconds >= BENCHMARK_DURATION_SECONDS)
        {
            break;
        }

        // --- Destruction Phase ---
        // Destroy a random subset of cells
        std::uniform_int_distribution<size_t> distrib(0, live_handles.size() - 1);
        for (size_t i = 0; i < CELLS_TO_RECYCLE_PER_TICK; ++i)
        {
            size_t randomIndex = distrib(gen);
            CellHandle handleToDestroy = live_handles[randomIndex];
            
            cellStore.DestroyCell(handleToDestroy);

            // Use swap-and-pop to efficiently remove the handle from our tracking vector
            live_handles[randomIndex] = live_handles.back();
            live_handles.pop_back();
        }

        // --- Creation Phase ---
        // Create new cells to replace the destroyed ones
        for (size_t i = 0; i < CELLS_TO_RECYCLE_PER_TICK; ++i)
        {
            CellHandle newHandle = cellStore.CreateCell();
            live_handles.push_back(newHandle);
            // In a real simulation, we'd initialize the new cell here.
        }
        
        // Sanity check
        if (live_handles.size() != INITIAL_CELL_COUNT) {
            std::cerr << "ERROR: Cell count mismatch! Test failed.\n";
            return 1;
        }

        totalOperations += (CELLS_TO_RECYCLE_PER_TICK * 2); // 1 create + 1 destroy
        ticks++;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    // --- 4. REPORTING ---
    std::cout << "\n--- BENCHMARK RESULTS ---\n";
    std::cout << "Test completed in: " << (durationMs / 1000.0) << " seconds over " << ticks << " ticks.\n";
    std::cout << "Total operations (Create/Destroy): " << totalOperations << "\n";
    
    double opsPerSecond = totalOperations / (durationMs / 1000.0);
    std::cout << "Throughput: " << static_cast<long long>(opsPerSecond) << " ops/sec\n";
    
    if (opsPerSecond > 1'000'000 && cellStore.GetActiveCellCount() == INITIAL_CELL_COUNT)
    {
        std::cout << "\n[RESULT] SUCCESS! Memory is stable and throughput is high.\n";
    }
    else
    {
        std::cout << "\n[RESULT] FAILURE! Performance target not met or cell count is incorrect.\n";
    }
    std::cout << "Final active cell count: " << cellStore.GetActiveCellCount() << "\n";
    std::cout << "===============================================\n";

    return 0;
}
