// ===================================================================================
//  File:    MetabolismSystem.h
//  Project: Adam Engine
//  Purpose: Declares the MetabolismSystem, a concrete implementation of ISystem.
//           This system is responsible for handling cell energy consumption and
//           the consequences of starvation.
// ===================================================================================

#pragma once

#include "ISystem.h"

class MetabolismSystem : public ISystem
{
public:
    /**
     * @brief Default constructor.
     */
    MetabolismSystem() = default;

    /**
     * @brief Virtual destructor.
     */
    virtual ~MetabolismSystem() = default;

    /**
     * @brief Updates all active cells, applying metabolism logic.
     * @details Iterates through all cells, consumes energy based on their
     *          archetype's metabolism rate, and applies health damage if
     *          a cell runs out of energy.
     * @param cellStore A reference to the central repository of all cell data.
     * @param deltaTime The time in seconds that has passed since the last tick.
     */
    void Update(CellStore& cellStore, float deltaTime) override;
};
