// ===================================================================================
//  File:    CellHandle.h
//  Project: Adam Engine
//  Purpose: Defines the CellHandle, a stable, globally unique identifier for a
//           cell. It uses a generational index to prevent bugs from recycled IDs.
//           This is a header-only Plain Old Data (POD) struct.
// ===================================================================================

#pragma once

#include "DataTypes.h"
#include <limits>

/**
 * @struct CellHandle
 * @brief A stable reference to a cell instance.
 *
 * This handle is safe to store and pass around. Even after a cell is destroyed
 * and its memory slot is recycled for a new cell, this old handle will become
 * invalid and will not mistakenly point to the new cell.
 */
struct CellHandle
{
    // --- Constants ---
    static constexpr uint32 INVALID_ID = std::numeric_limits<uint32>::max();
    static const CellHandle INVALID;

    // --- Members ---
    uint32 id = INVALID_ID;         // The index into the CellStore's master lookup table.
    uint32 generation = INVALID_ID; // The generation count for this ID slot.

    // --- Constructors ---

    /**
     * @brief Default constructor. Creates an invalid handle.
     */
    CellHandle() = default;

    /**
     * @brief Constructs a handle with a specific id and generation.
     */
    CellHandle(uint32 id, uint32 generation)
        : id(id), generation(generation)
    {}

    // --- Operators and Methods ---

    /**
     * @brief Checks if the handle is valid (i.e., not the default invalid handle).
     * @return True if the handle's ID is not the invalid marker.
     */
    bool IsValid() const
    {
        return id != INVALID_ID;
    }

    /**
     * @brief Equality operator for comparing two handles.
     * @return True if both id and generation match.
     */
    bool operator==(const CellHandle& other) const
    {
        return id == other.id && generation == other.generation;
    }

    /**
     * @brief Inequality operator for comparing two handles.
     */
    bool operator!=(const CellHandle& other) const
    {
        return !(*this == other);
    }
};

// Define the static invalid handle instance.
inline const CellHandle CellHandle::INVALID = {};
