// ===================================================================================
//  File:    QuantumCanvas.h
//  Project: Genesis Engine
//  Purpose: A memory-efficient, pointerless, N-dimensional sparse octree.
//           This is the foundational spatial data structure that represents the
//           "canvas" upon which matter will be constructed.
// ===================================================================================

#pragma once

#include "../common/DataTypes.h"
#include <vector>
#include <array>
#include <cmath>
#include <numeric>

/**
 * @class QuantumCanvas
 * @brief A sparse, pointerless, N-dimensional octree (hyperoctree).
 * @tparam T The type of data to store in the leaf nodes.
 * @tparam Dimensions The number of spatial dimensions (e.g., 3 for an octree).
 */
template <typename T, size_t Dimensions>
class QuantumCanvas
{
public:
    static constexpr size_t   ChildCount = 1 << Dimensions; // 2^Dimensions
    static constexpr uint32_t INVALID_NODE = std::numeric_limits<uint32_t>::max();

private:
    struct Node
    {
        bool isLeaf = true;
        union {
            std::array<uint32_t, ChildCount> childNodeIndices;
            T data;
        };

        // Constructor for a leaf node
        Node(const T& val) : isLeaf(true), data(val) {}

        // Constructor for a branch node
        Node() : isLeaf(false) {
            childNodeIndices.fill(INVALID_NODE);
        }
    };

public:
    /**
     * @brief Constructs a QuantumCanvas.
     * @param center The center of the entire volume.
     * @param size The total size (width, height, depth) of the volume.
     * @param maxDepth The maximum number of subdivisions.
     * @param defaultValue The initial value for the entire space.
     */
    QuantumCanvas(const Vec3d& center, double size, uint32_t maxDepth, T defaultValue)
        : m_maxDepth(maxDepth)
    {
        m_rootBounds.min = { center.x - size / 2.0, center.y - size / 2.0, center.z - size / 2.0 };
        m_rootBounds.max = { center.x + size / 2.0, center.y + size / 2.0, center.z + size / 2.0 };
        
        // Create the root node, which is initially a single leaf representing the whole space
        m_nodes.emplace_back(defaultValue);
    }

    /**
     * @brief Inserts or updates a data point at a specific coordinate.
     * @param point The coordinate for the data.
     * @param data The data to insert.
     */
    void Insert(const Vec3d& point, const T& data)
    {
        InsertRecursive(0, m_rootBounds, point, data, 0);
    }

    /**
     * @brief Retrieves the data value at a specific coordinate.
     * @param point The coordinate to query.
     * @return The data at that point.
     */
    T GetValueAtPoint(const Vec3d& point) const
    {
        return GetValueAtPointRecursive(0, m_rootBounds, point);
    }

private:
    void InsertRecursive(uint32_t nodeIndex, const BoundingBox<double>& nodeBounds, const Vec3d& point, const T& data, uint32_t currentDepth)
    {
        // Do not store a reference to the node across potential vector reallocations.
        // Access m_nodes[nodeIndex] directly.

        if (m_nodes[nodeIndex].isLeaf)
        {
            // If we've reached max depth, or if the new data is the same as the old,
            // we can't or don't need to subdivide further.
            if (currentDepth >= m_maxDepth || m_nodes[nodeIndex].data == data)
            {
                m_nodes[nodeIndex].data = data; // Overwrite if different
                return;
            }

            // --- Subdivide this leaf node into a branch ---
            T oldData = m_nodes[nodeIndex].data;
            m_nodes[nodeIndex].isLeaf = false;

            // Manually construct the child array in the union's memory space.
            new (&m_nodes[nodeIndex].childNodeIndices) std::array<uint32_t, ChildCount>();

            for (size_t i = 0; i < ChildCount; ++i)
            {
                uint32_t childIndex = static_cast<uint32_t>(m_nodes.size());
                // CRITICAL: emplace_back can invalidate references to vector elements.
                m_nodes.emplace_back(oldData);
                // CRITICAL: Must use the index to access the node, as its address may have changed.
                m_nodes[nodeIndex].childNodeIndices[i] = childIndex;
            }
        }
        
        // --- Recurse into the correct child ---
        Vec3d center = {
            (nodeBounds.min.x + nodeBounds.max.x) / 2.0,
            (nodeBounds.min.y + nodeBounds.max.y) / 2.0,
            (nodeBounds.min.z + nodeBounds.max.z) / 2.0
        };

        size_t childIdx = 0;
        BoundingBox<double> childBounds;

        // This logic assumes 3D for simplicity of bounds calculation
        childBounds.min.x = (point.x < center.x) ? nodeBounds.min.x : center.x;
        childBounds.max.x = (point.x < center.x) ? center.x : nodeBounds.max.x;
        childIdx |= (point.x < center.x) ? 0 : 1;

        childBounds.min.y = (point.y < center.y) ? nodeBounds.min.y : center.y;
        childBounds.max.y = (point.y < center.y) ? center.y : nodeBounds.max.y;
        childIdx |= (point.y < center.y) ? 0 : 2;

        childBounds.min.z = (point.z < center.z) ? nodeBounds.min.z : center.z;
        childBounds.max.z = (point.z < center.z) ? center.z : nodeBounds.max.z;
        childIdx |= (point.z < center.z) ? 0 : 4;
        
        // Read the child index *after* potential reallocations.
        uint32_t childNodeIdx = m_nodes[nodeIndex].childNodeIndices[childIdx];
        InsertRecursive(childNodeIdx, childBounds, point, data, currentDepth + 1);
    }
    
    T GetValueAtPointRecursive(uint32_t nodeIndex, const BoundingBox<double>& nodeBounds, const Vec3d& point) const
    {
        const Node& node = m_nodes[nodeIndex];
        if (node.isLeaf) {
            return node.data;
        }

        Vec3d center = {
            (nodeBounds.min.x + nodeBounds.max.x) / 2.0,
            (nodeBounds.min.y + nodeBounds.max.y) / 2.0,
            (nodeBounds.min.z + nodeBounds.max.z) / 2.0
        };

        size_t childIdx = 0;
        BoundingBox<double> childBounds;

        childBounds.min.x = (point.x < center.x) ? nodeBounds.min.x : center.x;
        childBounds.max.x = (point.x < center.x) ? center.x : nodeBounds.max.x;
        childIdx |= (point.x < center.x) ? 0 : 1;

        childBounds.min.y = (point.y < center.y) ? nodeBounds.min.y : center.y;
        childBounds.max.y = (point.y < center.y) ? center.y : nodeBounds.max.y;
        childIdx |= (point.y < center.y) ? 0 : 2;

        childBounds.min.z = (point.z < center.z) ? nodeBounds.min.z : center.z;
        childBounds.max.z = (point.z < center.z) ? center.z : nodeBounds.max.z;
        childIdx |= (point.z < center.z) ? 0 : 4;
        
        return GetValueAtPointRecursive(node.childNodeIndices[childIdx], childBounds, point);
    }

    std::vector<Node> m_nodes;
    BoundingBox<double> m_rootBounds;
    uint32_t m_maxDepth;
};

// --- Convenience Type Alias for our 3D use case ---
// We will be storing a signed distance value (float) in our 3D canvas.
using Canvas3D = QuantumCanvas<float, 3>;

