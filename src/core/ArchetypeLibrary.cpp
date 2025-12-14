// ===================================================================================
//  File:    ArchetypeLibrary.cpp
//  Project: Adam Engine
//  Purpose: Implements the ArchetypeLibrary singleton.
// ===================================================================================

#include "ArchetypeLibrary.h"
#include <stdexcept> // For std::runtime_error
#include <limits>    // For std::numeric_limits

// --- Singleton Instance ---

ArchetypeLibrary& ArchetypeLibrary::Instance()
{
    // C++11 guarantees that this is initialized only once, in a thread-safe way.
    static ArchetypeLibrary instance;
    return instance;
}


// --- Constructor ---

ArchetypeLibrary::ArchetypeLibrary()
{
    // Reserve a reasonable amount of space to prevent frequent reallocations.
    m_archetypes.reserve(32);

    // IMPORTANT: Register a default "Unspecialized" archetype at ID 0.
    // This provides a safe fallback for invalid IDs and a default state for new cells.
    CellArchetype unspecialized; // Uses default values from the struct definition
    RegisterArchetype(unspecialized);
}


// --- Public Methods ---

uint8 ArchetypeLibrary::RegisterArchetype(const CellArchetype& archetype)
{
    // Check if we have exceeded the maximum number of archetypes for a uint8 ID.
    if (m_archetypes.size() >= std::numeric_limits<uint8>::max())
    {
        throw std::runtime_error("ArchetypeLibrary is full! Cannot register more cell types.");
    }

    m_archetypes.push_back(archetype);

    // The new ID is the index of the element we just added.
    // We cast to uint8 as we've already checked the size limit.
    return static_cast<uint8>(m_archetypes.size() - 1);
}

const CellArchetype& ArchetypeLibrary::GetArchetype(uint8 typeID) const
{
    // Bounds check: If the requested ID is invalid, return the default
    // "Unspecialized" archetype at index 0 for safety.
    if (typeID >= m_archetypes.size())
    {
        // Log an error or warning here in a real application
        return m_archetypes[0];
    }

    return m_archetypes[typeID];
}
