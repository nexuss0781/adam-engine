// ===================================================================================
//  File:    CellBlock.h
//  Project: Adam Engine
//  Purpose: Declares the CellBlock class, the core memory container for cell
//           instances. It utilizes the Structure of Arrays (SoA) pattern for
//           maximum cache efficiency during iteration.
// ===================================================================================

#pragma once

#include "DataTypes.h"
#include <cstddef> // For size_t
#include <limits>  // For std::numeric_limits

/**
 * @class CellBlock
 * @brief Manages a fixed-size block of memory for cell components using SoA.
 *
 * This class allocates parallel arrays for each cell attribute. By exposing the
 * raw data pointers, it allows external systems to perform highly efficient,
 * cache-friendly operations across large sets of cells.
 */
class CellBlock
{
public:
    // --- Public Constants ---
    static constexpr size_t INVALID_INDEX = std::numeric_limits<size_t>::max();
    static constexpr uint8 FLAG_ACTIVE = 1 << 0; // 00000001

    // --- Public Data Arrays (The SoA) ---
    // Exposing these directly is a key part of Data-Oriented Design.
    uint8*      flags;      // Bitmask: IsActive, IsDirty, etc.
    uint8*      typeIDs;    // ID pointing to an Archetype in the ArchetypeLibrary.
    Vec3*       positions;  // 3D positions of the cells.
    Quaternion* rotations;  // 3D rotations of the cells.
    uint8*      healths;    // Current health (e.g., 0-255).
    uint8*      energies;   // Current energy (e.g., 0-255).

    /**
     * @brief Constructs a CellBlock and allocates memory for a set capacity.
     * @param capacity The maximum number of cells this block can hold.
     */
    explicit CellBlock(size_t capacity);

    /**
     * @brief Destructor that deallocates all associated memory.
     */
    ~CellBlock();

    /**
     * @brief Activates the next available cell slot and returns its index.
     * @return The index of the newly activated cell, or INVALID_INDEX if full.
     */
    size_t ActivateCell();

    /**
     * @brief Deactivates a cell at a given index.
     * @details Uses the efficient "swap and pop" method to keep active cells contiguous.
     * @param index The index of the cell to deactivate.
     */
    void DeactivateCell(size_t index);

    /**
     * @brief Gets the number of currently active cells in the block.
     * @return The number of active cells.
     */
    size_t GetActiveCount() const { return m_activeCount; }

    /**
     * @brief Gets the maximum number of cells this block can hold.
     * @return The total capacity.
     */
    size_t GetCapacity() const { return m_capacity; }

    // --- Disable copying and moving to prevent memory management issues ---
    CellBlock(const CellBlock&) = delete;
    CellBlock& operator=(const CellBlock&) = delete;
    CellBlock(CellBlock&&) = delete;
    CellBlock& operator=(CellBlock&&) = delete;

private:
    size_t m_capacity;
    size_t m_activeCount;
};
