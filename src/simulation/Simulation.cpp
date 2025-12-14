// ===================================================================================
//  File:    Simulation.cpp
//  Project: Adam Engine
//  Purpose: Implements the Simulation class logic.
// ===================================================================================

#include "Simulation.h"
#include "ISystem.h"
#include "../core/CellStore.h"
#include "../core/JobSystem.h"

Simulation::Simulation(CellStore& cellStore, JobSystem& jobSystem)
    : m_cellStore(cellStore), m_jobSystem(jobSystem)
{}

void Simulation::RegisterSystem(std::unique_ptr<ISystem> system)
{
    m_systems.push_back(std::move(system));
}

void Simulation::Tick(float deltaTime)
{
    // --- DISPATCH PHASE ---
    // Iterate through each system. For each system, create a job for each CellBlock.
    for (const auto& system : m_systems)
    {
        for (const auto& blockPtr : m_cellStore.GetBlocks())
        {
            CellBlock* block = blockPtr.get();
            if (block->GetActiveCount() > 0)
            {
                // Create a job (lambda function) that captures the necessary context.
                Job job = [system = system.get(), block, deltaTime]() {
                    // This is where the actual work is done by a worker thread.
                    // The system's logic is applied to a single block of data.
                    system->Update(*block, deltaTime);
                };

                // Dispatch the job to the thread pool.
                m_jobSystem.Dispatch(std::move(job));
            }
        }
    }

    // --- SYNCHRONIZATION PHASE ---
    // The main thread waits here until ALL dispatched jobs for this tick are complete.
    m_jobSystem.Wait();
}
