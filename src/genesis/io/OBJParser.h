// ===================================================================================
//  File:    OBJParser.h
//  Project: Genesis Engine
//  Purpose: A lightweight, self-contained, header-only OBJ model parser.
//           This parser is framework-free and written in pure C++ to read the
//           Wavefront OBJ format, extracting vertex and face information.
// ===================================================================================

#pragma once

#include "../common/DataTypes.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>

class OBJParser
{
public:
    /**
     * @brief Constructs the parser and loads a model from a given filepath.
     * @param filepath The path to the .obj file.
     */
    explicit OBJParser(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file) {
            throw std::runtime_error("OBJParser: Could not open file " + filepath);
        }

        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string keyword;
            ss >> keyword;

            if (keyword == "v") { // Vertex
                Vec3f vertex;
                ss >> vertex.x >> vertex.y >> vertex.z;
                m_model.vertices.push_back(vertex);
            } else if (keyword == "f") { // Face
                Polygon polygon;
                std::string face_part;
                while (ss >> face_part) {
                    // OBJ format is 1-indexed, so we subtract 1 for our 0-indexed vectors.
                    // We only care about the vertex index, so we ignore texture/normal data (e.g., "v/vt/vn").
                    size_t vertex_index = std::stoul(face_part) - 1;
                    polygon.vertexIndices.push_back(static_cast<uint32>(vertex_index));
                }
                m_model.polygons.push_back(polygon);
            }
            // We ignore other keywords like "vn", "vt", "s", "o", etc. for simplicity.
        }

        if (!m_model.vertices.empty() && !m_model.polygons.empty()) {
            m_valid = true;
        }
    }

    /**
     * @brief Checks if the model was loaded successfully.
     * @return True if the parser found vertex and face data.
     */
    bool IsValid() const { return m_valid; }

    /**
     * @brief Gets the parsed model data.
     * @return A constant reference to the ParsedModel struct.
     */
    const ParsedModel& GetModel() const { return m_model; }

private:
    ParsedModel m_model;
    bool m_valid = false;
};
