// ===================================================================================
//  File:    DataTypes.h
//  Project: Adam Engine
//  Purpose: Defines fundamental, self-contained data types and aliases used
//           across the entire engine to ensure consistent memory layout and
//           prevent circular dependencies.
// ===================================================================================

#pragma once

#include <cstdint> // Required for fixed-width integer types (e.g., std::uint8_t)

// --- Standard Integer Aliases ---
// Using fixed-width integers is crucial for a data-oriented design. It guarantees
// a predictable memory footprint for our data structures across different
// compilers and machine architectures.

using int8   = std::int8_t;
using uint8  = std::uint8_t;
using int16  = std::int16_t;
using uint16 = std::uint16_t;
using int32  = std::int32_t;
using uint32 = std::uint32_t;
using int64  = std::int64_t;
using uint64 = std::uint64_t;


// --- Core Mathematical Structures ---
// These are simple, Plain Old Data (POD) structs. They contain no logic,
// ensuring they can be tightly packed in memory and easily serialized.

/**
 * @struct Vec3
 * @brief A 3-component vector of floats. Used for positions, scales, colors, etc.
 */
struct Vec3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

/**
 * @struct Quaternion
 * @brief A 4-component vector of floats. Standard representation for 3D rotations
 *        to avoid issues like gimbal lock. (w is the scalar component).
 */
struct Quaternion
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f; // Default to identity rotation
};
