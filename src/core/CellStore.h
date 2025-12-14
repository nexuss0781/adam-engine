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
#include "CellBlock.h" // Forward declaration would be enough if not for unique_ptr
#include <vector>
#include <memory>

class CellStore
{
public:
    /**
     * @brief Constructs a CellStore, optionally with a pre-allocated capacity.
     * @param initialCapacity The total number of cells to reserve memory for initially.
     */
    explicit CellStore(size_t initialCapacity = 0);
    ~CellStore() = default;

    /**
     * @brief Creates a new cell and returns a stable handle to it.
     * @return A valid CellHandle for the newly created cell.
     */
    CellHandle CreateCell();

    /**
     * @brief Destroys a cell, recycling its memory and invalidating its handle.
     * @param handle The handle of the cell to destroy. If the handle is invalid,
     *        this function has no effect.
     */
    void DestroyCell(CellHandle handle);

    /**
     * @brief Retrieves a proxy object to interact with a cell's data.
     * @param handle The handle of the cell to access.
     * @return A CellProxy for the cell. If the handle is invalid, the returned
     *         proxy will also be invalid (proxy.IsValid() will be false).
     */
    CellProxy GetCell(CellHandle handle);

    /**
     * @brief Checks if a handle currently points to a live cell.
     * @param handle The handle to check.
     * @return True if the handle is valid and its generation matches.
     */
    bool IsHandleValid(CellHandle handle) const;

    /**
     * @brief Gets the total number of active cells currently managed by the store.
     * @return The number of active cells.
     */
    size_t GetActiveCellCount() const;

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
        uint32 generation = 1; // Start generation at 1
    };
    
    // --- Private Methods ---
    
    /**
     * @brief Adds a new, empty CellBlock to the store when existing ones are full.
     */
    void AddNewBlock();

    // --- Member Variables ---

    std::vector<std::unique_ptr<CellBlock>> m_blocks;
    std::vector<LookupEntry> m_lookupTable;
    std::vector<uint32> m_freeHandleIDs; // A list of recycled IDs in the lookup table.
};
