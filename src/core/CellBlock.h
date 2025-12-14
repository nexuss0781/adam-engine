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
#include <vector>  // For the free list

class CellBlock
{
public:
    // --- Public Constants ---
    static constexpr size_t INVALID_INDEX = std::numeric_limits<size_t>::max();
    static constexpr uint8 FLAG_ACTIVE = 1 << 0; // 00000001

    // --- Public Data Arrays (The SoA) ---
    uint8*      flags;
    uint8*      typeIDs;
    Vec3*       positions;
    Quaternion* rotations;
    uint8*      healths;
    uint8*      energies;

    explicit CellBlock(size_t capacity);
    ~CellBlock();

    /**
     * @brief Acquires a free slot in the block for a new cell.
     * @details Pulls a recycled index from the internal free list or uses the
     *          next available contiguous slot. Marks the slot as active.
     * @return The index of the acquired slot, or INVALID_INDEX if the block is full.
     */
    size_t AcquireSlot();

    /**
     * @brief Releases a slot, marking it as inactive and available for recycling.
     * @param index The index of the slot to release.
     */
    void ReleaseSlot(size_t index);

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

    // --- Disable copying and moving ---
    CellBlock(const CellBlock&) = delete;
    CellBlock& operator=(const CellBlock&) = delete;
    CellBlock(CellBlock&&) = delete;
    CellBlock& operator=(CellBlock&&) = delete;

private:
    size_t m_capacity;
    size_t m_activeCount;
    size_t m_highWaterMark; // The highest index ever used + 1.
    std::vector<size_t> m_freeIndices; // A stack of recycled indices.
};
