// ===================================================================================
//  File:    ISystem.h
//  Project: Adam Engine
//  Purpose: Defines the abstract base class (interface) for all simulation systems.
//           Each "System" encapsulates a specific piece of logic (e.g., metabolism,
//           movement) that operates on the cell data.
// ===================================================================================

#pragma once

// Forward declaration is used here to avoid including the full CellStore header.
// This reduces compile times and prevents potential circular dependency issues.
class CellStore;

/**
 * @class ISystem
 * @brief The interface for any system that processes cell data.
 *
 * Systems are the "verbs" of the engine, applying logic to the "nouns" (cells).
 * The Simulation class will manage a collection of ISystem pointers and call
 * their Update methods each tick.
 */
class ISystem
{
public:
    /**
     * @brief Virtual destructor.
     * Ensures that derived system classes are properly destroyed when deleted
     * through a base class pointer.
     */
    virtual ~ISystem() = default;

    /**
     * @brief The core update function that every system must implement.
     * This method is called once per simulation tick. The system is expected
     * to iterate through the relevant cells in the CellStore and apply its logic.
     * @param cellStore A reference to the central repository of all cell data.
     * @param deltaTime The time in seconds that has passed since the last tick.
     */
    virtual void Update(CellStore& cellStore, float deltaTime) = 0;
};
