// ===================================================================================
//  File:    CellArchetype.h
//  Project: Adam Engine
//  Purpose: Defines the structure for a "Cell Archetype". This acts as the
//           Flyweight object, storing all the shared, immutable data for a
//           specific type of cell. We will only create one instance of this
//           for each cell type (e.g., one for "Neuron", one for "Hepatocyte").
// ===================================================================================

#pragma once

#include "DataTypes.h"
#include <string>

/**
 * @struct CellArchetype
 * @brief Contains the intrinsic (shared) properties of a cell type.
 *
 * This data is read-only during the simulation. Each active cell instance in a
 * CellBlock will point to one of these archetypes via its `typeID`.
 */
struct CellArchetype
{
    // --- Identification ---
    std::string name = "Unspecialized";

    // --- Biological Properties ---
    uint8 baseHealth = 100;              // Maximum health this cell type can have.
    float metabolismRate = 1.0f;         // Multiplier for energy consumption per tick.
    uint8 divisionEnergyThreshold = 90;  // Energy level (0-255) needed to trigger mitosis.

    // --- Presentation Properties ---
    // These IDs link to external systems (e.g., a rendering or physics engine).
    uint32 renderMeshID = 0;             // ID of the 3D model to render.
    Vec3   renderColor = {1.0f, 1.0f, 1.0f}; // Default color (R,G,B).
};
