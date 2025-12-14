// ===================================================================================
//  File:    CellStore.cpp
//  Project: Adam Engine
//  Purpose: Implements the CellStore, managing cell lifecycle and recycling.
//           (Refactored for Phase 2 to use the new CellBlock free-list model).
// ===================================================================================

#include "CellStore.h"
#include <stdexcept>

// A reasonable default size for new CellBlocks.
constexpr size_t DEFAULT_BLOCK_CAPACITY = 16384; // 2^14

CellStore::CellStore(size_t initialCapacity)
{
    if (initialCapacity > 0)
    {
        m_lookupTable.reserve(initialCapacity);
        const size_t numBlocks = (initialCapacity + DEFAULT_BLOCK_CAPACITY - 1) / DEFAULT_BLOCK_CAPACITY;
        m_blocks.reserve(numBlocks);
        m_blocksWithFreeSlots.reserve(numBlocks);
        for (size_t i = 0; i < numBlocks; ++i)
        {
            m_blocks.emplace_back(std::make_unique<CellBlock>(DEFAULT_BLOCK_CAPACITY));
            m_blocksWithFreeSlots.push_back(static_cast<uint32>(i));
        }
    }
}

CellHandle CellStore::CreateCell()
{
    uint32 handleID;

    // Step 1: Get a handle ID. Recycle one from the free list or create a new one.
    if (!m_freeHandleIDs.empty())
    {
        handleID = m_freeHandleIDs.back();
        m_freeHandleIDs.pop_back();
    }
    else
    {
        handleID = static_cast<uint32>(m_lookupTable.size());
        m_lookupTable.emplace_back();
    }

    // Step 2: Find and acquire a free memory slot in a CellBlock.
    CellLocation location;
    size_t slotIndex = CellBlock::INVALID_INDEX;

    // If no existing block has space, create a new one.
    if (m_blocksWithFreeSlots.empty())
    {
        AddNewBlock();
    }

    // Get a block that has free slots and acquire a slot from it.
    uint32 blockIndex = m_blocksWithFreeSlots.back();
    slotIndex = m_blocks[blockIndex]->AcquireSlot();
    if (slotIndex == CellBlock::INVALID_INDEX)
    {
        // This should never happen with the new logic.
        throw std::runtime_error("Failed to acquire slot from a block that should have free slots.");
    }

    location.blockIndex = blockIndex;
    location.indexInBlock = static_cast<uint32>(slotIndex);

    // If the block is now full, remove it from the list of blocks with free slots.
    if (m_blocks[blockIndex]->GetActiveCount() == m_blocks[blockIndex]->GetCapacity())
    {
        m_blocksWithFreeSlots.pop_back();
    }

    // Step 3: Update the lookup table with the new cell's location.
    m_lookupTable[handleID].location = location;
    // The generation is already correct from the last time this handleID was used.
    
    // Step 4: Return the new, valid handle.
    return CellHandle(handleID, m_lookupTable[handleID].generation);
}

void CellStore::DestroyCell(CellHandle handle)
{
    if (!IsHandleValid(handle))
    {
        return; // Handle is already invalid or stale, do nothing.
    }

    // Step 1: Get the location from the lookup table.
    const auto& entry = m_lookupTable[handle.id];
    const auto& location = entry.location;
    CellBlock* block = m_blocks[location.blockIndex].get();

    // Check if the block was full before we release the slot.
    const bool wasFull = (block->GetActiveCount() == block->GetCapacity());

    // Step 2: Release the slot in the appropriate CellBlock. This is now O(1).
    block->ReleaseSlot(location.indexInBlock);

    // If the block was full, it now has a free slot, so add it back to the list.
    if (wasFull)
    {
        m_blocksWithFreeSlots.push_back(location.blockIndex);
    }

    // Step 3: Invalidate the handle by incrementing the generation.
    // This is the key to preventing the ABA problem.
    m_lookupTable[handle.id].generation++;

    // Step 4: Add the handle ID back to the free list for recycling.
    m_freeHandleIDs.push_back(handle.id);
}

CellProxy CellStore::GetCell(CellHandle handle)
{
    if (!IsHandleValid(handle))
    {
        return CellProxy(nullptr, 0); // Return an invalid proxy.
    }

    const auto& entry = m_lookupTable[handle.id];
    return CellProxy(m_blocks[entry.location.blockIndex].get(), entry.location.indexInBlock);
}

bool CellStore::IsHandleValid(CellHandle handle) const
{
    return handle.IsValid() &&
           handle.id < m_lookupTable.size() &&
           m_lookupTable[handle.id].generation == handle.generation;
}

size_t CellStore::GetActiveCellCount() const
{
    size_t total = 0;
    for(const auto& block : m_blocks)
    {
        total += block->GetActiveCount();
    }
    return total;
}

void CellStore::AddNewBlock()
{
    uint32 newBlockIndex = static_cast<uint32>(m_blocks.size());
    m_blocks.emplace_back(std::make_unique<CellBlock>(DEFAULT_BLOCK_CAPACITY));
    m_blocksWithFreeSlots.push_back(newBlockIndex);
}
