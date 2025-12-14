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

void MetabolismSystem::Update(CellBlock& block, float deltaTime)
{
    auto& library = ArchetypeLibrary::Instance();

    for (size_t i = 0; i < block.GetCapacity(); ++i)
    {
        if (block.flags[i] & CellBlock::FLAG_ACTIVE)
        {
            // 1. Get Archetype for metabolism rate
            const auto& archetype = library.GetArchetype(block.typeIDs[i]);
            float energyDrain = archetype.metabolismRate * deltaTime;

            // 2. Consume Energy
            float currentEnergy = static_cast<float>(block.energies[i]);
            if (currentEnergy > energyDrain)
            {
                block.energies[i] -= static_cast<uint8_t>(energyDrain);
            }
            else
            {
                block.energies[i] = 0;
            }

            // 3. Apply Starvation Damage
            if (block.energies[i] == 0 && block.healths[i] > 0)
            {
                block.healths[i]--; // Apply 1 damage per tick if starving
            }
        }
    }
}
