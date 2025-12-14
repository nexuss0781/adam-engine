// ===================================================================================
//  File:    Main.cpp
//  Project: Genesis Engine Benchmark (Phase G1)
//  Purpose: Validates the entire Phase G1 pipeline: parsing a 3D model,
//           creating a QuantumCanvas, and imprinting the model onto it as a
//           volumetric representation.
// ===================================================================================

#include "genesis/io/OBJParser.h"
#include "genesis/canvas/QuantumCanvas.h"
#include "genesis/imprinter/BlueprintImprinter.h"

#include <iostream>
#include <fstream>
#include <chrono>

void print_header()
{
    std::cout << "====================================================\n";
    std::cout << "  Genesis Engine: Phase G1 Benchmark               \n";
    std::cout << "  (Canvas & Blueprint Imprinter Validation)        \n";
    std::cout << "====================================================\n\n";
}

// Helper function to create a dummy OBJ file for the test
void create_test_model(const std::string& filepath)
{
    std::ofstream file(filepath);
    if (!file) return;

    file << "# Simple Cube for Genesis Engine Benchmark\n";
    file << "v -1.0 -1.0 -1.0\n";
    file << "v -1.0 -1.0  1.0\n";
    file << "v -1.0  1.0 -1.0\n";
    file << "v -1.0  1.0  1.0\n";
    file << "v  1.0 -1.0 -1.0\n";
    file << "v  1.0 -1.0  1.0\n";
    file << "v  1.0  1.0 -1.0\n";
    file << "v  1.0  1.0  1.0\n";
    file << "f 1 3 4 2\n";
    file << "f 5 6 8 7\n";
    file << "f 1 5 7 3\n";
    file << "f 2 4 8 6\n";
    file << "f 3 7 8 4\n";
    file << "f 1 2 6 5\n";
}

int main()
{
    print_header();
    const std::string model_path = "cube.obj";

    // --- 1. SETUP: Create the test model file ---
    std::cout << "[SETUP] Creating test model file: " << model_path << "\n";
    create_test_model(model_path);

    // --- 2. PARSING TEST ---
    std::cout << "[TEST 1] Parsing 3D model...\n";
    OBJParser parser(model_path);
    if (!parser.IsValid()) {
        std::cerr << "[RESULT] FAILURE! Could not parse the OBJ model.\n";
        return 1;
    }
    const ParsedModel& model = parser.GetModel();
    std::cout << "  - Vertices found: " << model.vertices.size() << "\n";
    std::cout << "  - Polygons found: " << model.polygons.size() << "\n";
    std::cout << "[RESULT] SUCCESS! Model parsed correctly.\n\n";

    // --- 3. CANVAS CREATION TEST ---
    std::cout << "[TEST 2] Creating QuantumCanvas...\n";
    // The canvas will store a float (for the signed distance field) and be 3D.
    // Default value is 1.0f (far outside).
    Canvas3D canvas({0.0, 0.0, 0.0}, 4.0, 8, 1.0f);
    std::cout << "[RESULT] SUCCESS! Canvas initialized.\n\n";

    // --- 4. IMPRINTING TEST ---
    std::cout << "[TEST 3] Imprinting blueprint onto canvas...\n";
    BlueprintImprinter imprinter;
    // With an odd resolution, a voxel will be perfectly centered at (0,0,0)
    int resolution = 33;

    auto startTime = std::chrono::high_resolution_clock::now();
    imprinter.ImprintModel(canvas, model, resolution);
    auto endTime = std::chrono::high_resolution_clock::now();
    auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    std::cout << "  - Imprinting took " << durationMs << " ms.\n";
    std::cout << "[RESULT] SUCCESS! Imprinting process completed.\n\n";

    // --- 5. VERIFICATION TEST ---
    std::cout << "[TEST 4] Verifying canvas state...\n";
    // Check a point that should be deep inside the cube
    Vec3d inside_point = {0.0, 0.0, 0.0};
    float value_inside = canvas.GetValueAtPoint(inside_point);

    // Check a point that should be far outside the cube
    Vec3d outside_point = {1.5, 1.5, 1.5};
    float value_outside = canvas.GetValueAtPoint(outside_point);

    std::cout << "  - Value at center (should be -1.0): " << value_inside << "\n";
    std::cout << "  - Value outside (should be 1.0): " << value_outside << "\n";

    if (value_inside == -1.0f && value_outside == 1.0f) {
        std::cout << "[RESULT] SUCCESS! Canvas was correctly modified by the imprinter.\n\n";
    } else {
        std::cerr << "[RESULT] FAILURE! Canvas state is incorrect after imprinting.\n";
        return 1;
    }
    
    std::cout << "====================================================\n";
    std::cout << "  Phase G1 has been successfully validated.        \n";
    std::cout << "====================================================\n";

    return 0;
}
