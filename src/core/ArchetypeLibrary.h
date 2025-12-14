// ===================================================================================
//  File:    ArchetypeLibrary.h
//  Project: Adam Engine
//  Purpose: Declares the ArchetypeLibrary, a singleton manager for storing and
//           retrieving CellArchetype definitions. This ensures a single,
//           centralized repository for all shared cell data.
// ===================================================================================

#pragma once

#include "CellArchetype.h"
#include <vector>

/**
 * @class ArchetypeLibrary
 * @brief A singleton class that manages all CellArchetype instances.
 *
 * Provides a global access point for registering new cell types and retrieving
 * their read-only data using a simple type ID.
 */
class ArchetypeLibrary
{
public:
    /**
     * @brief Gets the single global instance of the library.
     * @return Reference to the ArchetypeLibrary instance.
     */
    static ArchetypeLibrary& Instance();

    /**
     * @brief Registers a new cell archetype and assigns it a unique ID.
     * @param archetype The CellArchetype data to register.
     * @return The uint8 ID assigned to this archetype. Throws an exception if full.
     */
    uint8 RegisterArchetype(const CellArchetype& archetype);

    /**
     * @brief Retrieves a read-only reference to an archetype by its ID.
     * @param typeID The ID of the archetype to retrieve.
     * @return A constant reference to the CellArchetype. Returns a default
     *         "Unspecialized" archetype if the ID is invalid.
     */
    const CellArchetype& GetArchetype(uint8 typeID) const;

    // --- Delete copy constructor and assignment operator to enforce Singleton pattern ---
    ArchetypeLibrary(const ArchetypeLibrary&) = delete;
    ArchetypeLibrary& operator=(const ArchetypeLibrary&) = delete;

private:
    // --- Private constructor and destructor for Singleton pattern ---
    ArchetypeLibrary();
    ~ArchetypeLibrary() = default;

    std::vector<CellArchetype> m_archetypes;
};
