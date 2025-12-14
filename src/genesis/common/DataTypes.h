// ===================================================================================
//  File:    DataTypes.h
//  Project: Genesis Engine
//  Purpose: Defines fundamental, self-contained data types and aliases for the
//           Genesis Engine. This ensures consistent memory layout and prevents
//           circular dependencies across all Genesis modules.
// ===================================================================================

#pragma once

#include <cstdint> // Required for fixed-width integer types
#include <vector>  // For basic data containers

// --- Standard Integer Aliases ---
// Guarantees predictable memory footprints for our data structures.

using int8   = std::int8_t;
using uint8  = std::uint8_t;
using int16  = std::int16_t;
using uint16 = std::uint16_t;
using int32  = std::int32_t;
using uint32 = std::uint32_t;
using int64  = std::int64_t;
using uint64 = std::uint64_t;


// --- Core Mathematical & Geometric Structures ---
// Plain Old Data (POD) structs for maximum efficiency and easy serialization.

/**
 * @struct Vec3
 * @brief A 3-component vector, templated for various numeric types (float, double, int).
 */
template <typename T>
struct Vec3
{
    T x{}, y{}, z{};
};

using Vec3f = Vec3<float>;
using Vec3i = Vec3<int32_t>;
using Vec3d = Vec3<double>;

/**
 * @struct BoundingBox
 * @brief An Axis-Aligned Bounding Box (AABB) defined by two points.
 */
template <typename T>
struct BoundingBox
{
    Vec3<T> min;
    Vec3<T> max;
};

using BoundingBoxf = BoundingBox<float>;

/**
 * @struct Polygon
 * @brief Represents a single face of a 3D model, defined by indices to a vertex list.
 */
struct Polygon
{
    std::vector<uint32> vertexIndices;
};

/**
 * @struct ParsedModel
 * @brief A simple, intermediate representation of a 3D model, parsed from a file.
 */
struct ParsedModel
{
    std::vector<Vec3f> vertices;
    std::vector<Polygon> polygons;
};
