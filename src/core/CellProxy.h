// ===================================================================================
//  File:    CellProxy.h
//  Project: Adam Engine
//  Purpose: A lightweight proxy/cursor for interacting with a single cell's
//           data within a CellBlock. This provides a convenient API while
//           maintaining the performance of the underlying SoA structure.
//           This is a header-only class to encourage compiler inlining.
// ===================================================================================

#pragma once

#include "CellBlock.h"
#include "ArchetypeLibrary.h"

class CellProxy
{
public:
    /**
     * @brief Constructs a CellProxy.
     * @param block Pointer to the CellBlock containing the cell's data.
     * @param index The index of the cell within the block.
     */
    CellProxy(CellBlock* block, size_t index)
        : m_block(block), m_index(index)
    {}

    // --- Core Getters ---
    size_t GetIndex() const { return m_index; }
    bool IsValid() const { return m_block != nullptr && m_index < m_block->GetActiveCount(); }

    // --- Instance Data Accessors (Extrinsic Properties) ---
    // These read/write directly to the CellBlock's arrays.

    uint8 GetTypeID() const { return m_block->typeIDs[m_index]; }
    void SetTypeID(uint8 typeID) { m_block->typeIDs[m_index] = typeID; }

    const Vec3& GetPosition() const { return m_block->positions[m_index]; }
    void SetPosition(const Vec3& position) { m_block->positions[m_index] = position; }

    const Quaternion& GetRotation() const { return m_block->rotations[m_index]; }
    void SetRotation(const Quaternion& rotation) { m_block->rotations[m_index] = rotation; }

    uint8 GetHealth() const { return m_block->healths[m_index]; }
    void SetHealth(uint8 health) { m_block->healths[m_index] = health; }

    uint8 GetEnergy() const { return m_block->energies[m_index]; }
    void SetEnergy(uint8 energy) { m_block->energies[m_index] = energy; }

    // --- Archetype Data Accessors (Intrinsic Properties) ---
    // These are read-only and retrieve data from the ArchetypeLibrary.

    const std::string& GetName() const
    {
        return ArchetypeLibrary::Instance().GetArchetype(GetTypeID()).name;
    }

    uint8 GetBaseHealth() const
    {
        return ArchetypeLibrary::Instance().GetArchetype(GetTypeID()).baseHealth;
    }

    float GetMetabolismRate() const
    {
        return ArchetypeLibrary::Instance().GetArchetype(GetTypeID()).metabolismRate;
    }

    // --- Lifecycle Methods ---

    /**
     * @brief Deactivates the cell represented by this proxy.
     * @warning After calling this, the proxy becomes INVALID. The data at its
     *          index will have been replaced by the last active cell in the block
     *          (due to the swap-and-pop algorithm). Do not use this proxy object
     *          after calling Deactivate().
     */
    void Deactivate()
    {
        if (IsValid())
        {
            m_block->DeactivateCell(m_index);
            // This proxy is now invalid.
        }
    }

private:
    CellBlock* m_block;
    size_t     m_index;
};
