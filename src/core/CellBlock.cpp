// ===================================================================================
//  File:    CellBlock.cpp
//  Project: Adam Engine
//  Purpose: Implements the CellBlock class, handling memory allocation and
//           management for the SoA data structure.
// ===================================================================================

#include "CellBlock.h"
#include <new>      // For std::align_val_t
#include <cstring>  // For std::memset
#include <utility>  // For std::swap

// Memory alignment for SIMD operations. 64 bytes is a common cache line size.
constexpr size_t MEMORY_ALIGNMENT = 64;

CellBlock::CellBlock(size_t capacity)
    : m_capacity(capacity), m_activeCount(0)
{
    // C++17 aligned allocation. This is crucial for performance.
    flags     = new (std::align_val_t(MEMORY_ALIGNMENT)) uint8[capacity];
    typeIDs   = new (std::align_val_t(MEMORY_ALIGNMENT)) uint8[capacity];
    positions = new (std::align_val_t(MEMORY_ALIGNMENT)) Vec3[capacity];
    rotations = new (std::align_val_t(MEMORY_ALIGNMENT)) Quaternion[capacity];
    healths   = new (std::align_val_t(MEMORY_ALIGNMENT)) uint8[capacity];
    energies  = new (std::align_val_t(MEMORY_ALIGNMENT)) uint8[capacity];

    // Initialize all flags to inactive (0).
    std::memset(flags, 0, sizeof(uint8) * capacity);
}

CellBlock::~CellBlock()
{
    // C++17 aligned deallocation must be used to match allocation.
    operator delete[](flags,     std::align_val_t(MEMORY_ALIGNMENT));
    operator delete[](typeIDs,   std::align_val_t(MEMORY_ALIGNMENT));
    operator delete[](positions, std::align_val_t(MEMORY_ALIGNMENT));
    operator delete[](rotations, std::align_val_t(MEMORY_ALIGNMENT));
    operator delete[](healths,   std::align_val_t(MEMORY_ALIGNMENT));
    operator delete[](energies,  std::align_val_t(MEMORY_ALIGNMENT));
}

size_t CellBlock::ActivateCell()
{
    if (m_activeCount >= m_capacity)
    {
        return INVALID_INDEX; // The block is full
    }

    // Use the next available slot.
    size_t newIndex = m_activeCount;
    flags[newIndex] = FLAG_ACTIVE;

    // The active count now points to the next free slot.
    m_activeCount++;

    return newIndex;
}

void CellBlock::DeactivateCell(size_t index)
{
    // Ensure the index is valid and within the active range.
    if (index >= m_activeCount)
    {
        return;
    }

    // "Swap and Pop": To keep the active block contiguous and avoid gaps,
    // we move the data from the *last active element* into the slot
    // we are deactivating. Then, we just shrink the active count.
    size_t lastIndex = m_activeCount - 1;

    if (index != lastIndex)
    {
        // Swap all data components
        flags[index]     = flags[lastIndex];
        typeIDs[index]   = typeIDs[lastIndex];
        positions[index] = positions[lastIndex];
        rotations[index] = rotations[lastIndex];
        healths[index]   = healths[lastIndex];
        energies[index]  = energies[lastIndex];
    }
    
    // Decrease the active count, effectively "popping" the last element.
    m_activeCount--;
    flags[m_activeCount] = 0; // Mark the now-inactive slot as such.
}
