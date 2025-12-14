// ===================================================================================
//  File:    MetabolismSystem.cpp
//  Project: Adam Engine
//  Purpose: Implements the logic for the MetabolismSystem.
// ===================================================================================

#include "MetabolismSystem.h"
#include "../core/CellStore.h"
#include "../core/ArchetypeLibrary.h"
#include <iostream> // For temporary debug output

// NOTE: To implement this, we must assume that CellStore exposes its CellBlocks
// for efficient iteration. The next step in the plan will be to add this accessor.
// For now, we will add a temporary placeholder method to CellStore for this to compile.

void MetabolismSystem::Update(CellStore& cellStore, float deltaTime)
{
    auto& library = ArchetypeLibrary::Instance();

    // This is a temporary, inefficient way to iterate until CellStore is updated.
    // In a real scenario, we would get direct access to the CellBlocks.
    // Let's simulate this by assuming a hypothetical way to iterate all handles.
    // The proper implementation will directly iterate blocks.

    // A more realistic (but still temporary) approach for the single-threaded baseline:
    // We'd need to add a method like `ForEachCell` to the CellStore.
    // For now, let's just log a message to show the system is being called.
    // The REAL implementation will be in the Simulation class which will iterate blocks.

    // The logic below is what we *will* run inside the parallel job for each block.
    // For now, it remains conceptual until the Simulation runner is built.

    /*
    Conceptual logic for a single CellBlock:

    for (const auto& block : cellStore.GetBlocks()) // Assuming GetBlocks() exists
    {
        for (size_t i = 0; i < block->m_highWaterMark; ++i)
        {
            if (block->flags[i] & CellBlock::FLAG_ACTIVE)
            {
                // 1. Get Archetype for metabolism rate
                const auto& archetype = library.GetArchetype(block->typeIDs[i]);
                float energyDrain = archetype.metabolismRate * deltaTime;

                // 2. Consume Energy
                float currentEnergy = static_cast<float>(block->energies[i]);
                if (currentEnergy > energyDrain)
                {
                    block->energies[i] -= static_cast<uint8>(energyDrain);
                }
                else
                {
                    block->energies[i] = 0;
                }

                // 3. Apply Starvation Damage
                if (block->energies[i] == 0 && block->healths[i] > 0)
                {
                    block->healths[i]--; // Apply 1 damage per tick if starving
                }
            }
        }
    }
    */
    
    // For this specific file commit, we will just indicate the system ran.
    // The true iteration logic will be added in the Simulation class.
    // std::cout << "MetabolismSystem::Update called for " << cellStore.GetActiveCellCount() << " cells.\n";
}
