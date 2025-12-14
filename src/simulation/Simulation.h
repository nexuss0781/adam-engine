.// ===================================================================================
//  File:    Simulation.h
//  Project: Adam Engine
//  Purpose: Declares the Simulation class, the main orchestrator for the engine.
//           It manages all systems and uses the JobSystem to execute them in
//           parallel across the cell data.
// ===================================================================================

#pragma once

#include <vector>
#include <memory>

// Forward declarations to keep header clean
class ISystem;
class CellStore;
class JobSystem;

class Simulation
{
public:
    /**
     * @brief Constructs the Simulation orchestrator.
     * @param cellStore A reference to the central data store.
     * @param jobSystem A reference to the thread pool for parallel execution.
     */
    Simulation(CellStore& cellStore, JobSystem& jobSystem);
    ~Simulation() = default;

    /**
     * @brief Registers a new system to be run every tick.
     * @param system A unique_ptr to the system instance.
     */
    void RegisterSystem(std::unique_ptr<ISystem> system);

    /**
     * @brief Executes a single tick of the simulation.
     * @details This is the main update loop. It iterates through all registered
     *          systems and all CellBlocks, dispatching the work as jobs to the
     *          JobSystem, and then waits for completion.
     * @param deltaTime The time in seconds since the last tick.
     */
    void Tick(float deltaTime);

    // --- Disable copying and moving ---
    Simulation(const Simulation&) = delete;
    Simulation& operator=(const Simulation&) = delete;

private:
    CellStore& m_cellStore;
    JobSystem& m_jobSystem;
    std::vector<std::unique_ptr<ISystem>> m_systems;
};
