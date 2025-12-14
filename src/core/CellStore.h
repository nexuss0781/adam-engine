// ===================================================================================
//  File:    CellStore.h
//  Project: Adam Engine
//  Purpose: Declares the CellStore, the central manager for all cell instances.
//           It handles the entire lifecycle: creation, destruction, and stable
//           access via CellHandles.
// ===================================================================================

#pragma once

#include "CellHandle.h"
#include "CellProxy.h"
#include "CellBlock.h"
#include <vector>
#include <memory>

class CellStore
{
public:
    explicit CellStore(size_t initialCapacity = 0);
    ~CellStore() = default;

    CellHandle CreateCell();
    void DestroyCell(CellHandle handle);
    CellProxy GetCell(CellHandle handle);
    bool IsHandleValid(CellHandle handle) const;
    size_t GetActiveCellCount() const;

    // --- NEW METHOD FOR PHASE 3 ---
    /**
     * @brief Provides read-only access to the underlying CellBlocks.
     * @details This is used by the Simulation engine to iterate over blocks and
     *          dispatch parallel jobs. It is const to prevent external systems
     *          from modifying the block list itself.
     * @return A constant reference to the vector of CellBlock unique_ptrs.
     */
    const std::vector<std::unique_ptr<CellBlock>>& GetBlocks() const { return m_blocks; }
    // ----------------------------

    // --- Disable copying and moving ---
    CellStore(const CellStore&) = delete;
    CellStore& operator=(const CellStore&) = delete;

private:
    // --- Private Data Structures ---
    struct CellLocation
    {
        uint32 blockIndex;
        uint32 indexInBlock;
    };

    struct LookupEntry
    {
        CellLocation location;
        uint32 generation = 1;
    };
    
    // --- Private Methods ---
    void AddNewBlock();

    // --- Member Variables ---
    std::vector<std::unique_ptr<CellBlock>> m_blocks;
    std::vector<LookupEntry> m_lookupTable;
    std::vector<uint32> m_freeHandleIDs;
};
