// ===================================================================================
//  File:    CellBlock.cpp
//  Project: Adam Engine
//  Purpose: Implements the CellBlock class, handling memory allocation and
//           management for the SoA data structure using a free list for recycling.
// ===================================================================================

#include "CellBlock.h"
#include <new>
#include <cstring>

constexpr size_t MEMORY_ALIGNMENT = 64;

CellBlock::CellBlock(size_t capacity)
    : m_capacity(capacity), 
      m_activeCount(0),
      m_highWaterMark(0)
{
    // Reserve space in the free list to avoid reallocations during intense recycling.
    m_freeIndices.reserve(capacity / 4); 

    flags     = new (std::align_val_t(MEMORY_ALIGNMENT)) uint8[capacity];
    typeIDs   = new (std::align_val_t(MEMORY_ALIGNMENT)) uint8[capacity];
    positions = new (std::align_val_t(MEMORY_ALIGNMENT)) Vec3[capacity];
    rotations = new (std::align_val_t(MEMORY_ALIGNMENT)) Quaternion[capacity];
    healths   = new (std::align_val_t(MEMORY_ALIGNMENT)) uint8[capacity];
    energies  = new (std::align_val_t(MEMORY_ALIGNMENT)) uint8[capacity];

    std::memset(flags, 0, sizeof(uint8) * capacity);
}

CellBlock::~CellBlock()
{
    operator delete[](flags,     std::align_val_t(MEMORY_ALIGNMENT));
    operator delete[](typeIDs,   std::align_val_t(MEMORY_ALIGNMENT));
    operator delete[](positions, std::align_val_t(MEMORY_ALIGNMENT));
    operator delete[](rotations, std::align_val_t(MEMORY_ALIGNMENT));
    operator delete[](healths,   std::align_val_t(MEMORY_ALIGNMENT));
    operator delete[](energies,  std::align_val_t(MEMORY_ALIGNMENT));
}

size_t CellBlock::AcquireSlot()
{
    size_t index;

    // First, try to recycle an index from the free list.
    if (!m_freeIndices.empty())
    {
        index = m_freeIndices.back();
        m_freeIndices.pop_back();
    }
    // If no recycled indices are available, use the next contiguous slot.
    else if (m_highWaterMark < m_capacity)
    {
        index = m_highWaterMark;
        m_highWaterMark++;
    }
    // If we can't do either, the block is full.
    else
    {
        return INVALID_INDEX;
    }

    flags[index] = FLAG_ACTIVE;
    m_activeCount++;
    return index;
}

void CellBlock::ReleaseSlot(size_t index)
{
    // Check for valid index and that the slot is not already inactive.
    if (index >= m_highWaterMark || !(flags[index] & FLAG_ACTIVE))
    {
        return; // Releasing an invalid or already-released slot.
    }

    // Mark as inactive.
    flags[index] = 0;
    
    // Add the index to the free list for future recycling.
    m_freeIndices.push_back(index);

    m_activeCount--;
}
