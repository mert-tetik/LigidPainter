/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    IMPLEMENTED FROM THE BLENDER'S FBX FILE FORMAT DOCUMENTATION

    https://code.blender.org/2013/08/fbx-binary-file-format-specification/#binary-file-structure

*/

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"

// Forward declarations for the export utilities
void seperateUnitedVertices(std::vector<std::vector<Vertex>>& unitedVertices, std::vector<std::vector<Vertex>>& meshVertices, std::vector<std::vector<unsigned int>>& meshIndices);
Model createModel(std::vector<std::vector<Vertex>> meshVertices, std::vector<std::vector<unsigned int>> meshIndices, std::vector<std::string> matTitles);

struct Property {
    char typeCode;
    std::vector<char> data;
};

struct Node {
    std::string nodeType;
    std::vector<Property> properties;
    std::vector<Node> nestedNodes;
};

void ReadProperties(std::ifstream& file, std::vector<Property>& properties) {
    uint32_t numProperties;
    file.read(reinterpret_cast<char*>(&numProperties), sizeof(uint32_t));

    for (uint32_t i = 0; i < numProperties; ++i) {
        Property prop;

        // Read type code
        file.read(&prop.typeCode, sizeof(char));

        // Read property data
        uint32_t propLen;
        file.read(reinterpret_cast<char*>(&propLen), sizeof(uint32_t));
        prop.data.resize(propLen);
        file.read(prop.data.data(), propLen);

        properties.push_back(prop);
    }
}

void ReadNestedNodes(std::ifstream& file, std::vector<Node>& nestedNodes) {
    Node nestedNode;

    while (true) {
        // Read nested node name length
        uint8_t nameLen;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(uint8_t));

        if (nameLen == 0) {
            break;  // End of nested nodes
        }

        // Read nested node name
        nestedNode.nodeType.resize(nameLen);
        file.read(nestedNode.nodeType.data(), nameLen);

        // Read nested node properties
        ReadProperties(file, nestedNode.properties);

        // Recursively read nested nodes
        ReadNestedNodes(file, nestedNode.nestedNodes);

        nestedNodes.push_back(nestedNode);
    }
}

void ProcessNodeHierarchy( std::vector<Node>& nodes, std::vector<std::vector<Vertex>>& materialVertices, int depth = 0) {
    std::vector<Vertex> currentMaterialVertices;

    for ( auto& node : nodes) {
        if (node.nodeType == "Material") {
            // Start a new material, store the previous material's vertices
            if (!currentMaterialVertices.empty()) {
                materialVertices.push_back(currentMaterialVertices);
                currentMaterialVertices.clear();
            }
        }
        else if (node.nodeType == "Vertex") {
            Vertex vertex;

            // Process vertex properties
            for (auto& prop : node.properties) {
                if (prop.typeCode == 'F' && prop.data.size() == sizeof(float) * 3) {
                    float* floatData = reinterpret_cast<float*>(prop.data.data());
                    vertex.Position = glm::vec3(floatData[0], floatData[1], floatData[2]);
                }
                else if (prop.typeCode == 'D' && prop.data.size() == sizeof(double) * 3) {
                    double* doubleData = reinterpret_cast<double*>(prop.data.data());
                    vertex.Position = glm::vec3(static_cast<float>(doubleData[0]), static_cast<float>(doubleData[1]), static_cast<float>(doubleData[2]));
                }
                else if (prop.typeCode == 'F' && prop.data.size() == sizeof(float) * 2) {
                    float* floatData = reinterpret_cast<float*>(prop.data.data());
                    vertex.TexCoords = glm::vec2(floatData[0], floatData[1]);
                }
                else if (prop.typeCode == 'D' && prop.data.size() == sizeof(double) * 2) {
                    double* doubleData = reinterpret_cast<double*>(prop.data.data());
                    vertex.TexCoords = glm::vec2(static_cast<float>(doubleData[0]), static_cast<float>(doubleData[1]));
                }
                else if (prop.typeCode == 'F' && prop.data.size() == sizeof(float) * 3) {
                    float* floatData = reinterpret_cast<float*>(prop.data.data());
                    vertex.Normal = glm::vec3(floatData[0], floatData[1], floatData[2]);
                }
                else if (prop.typeCode == 'D' && prop.data.size() == sizeof(double) * 3) {
                    double* doubleData = reinterpret_cast<double*>(prop.data.data());
                    vertex.Normal = glm::vec3(static_cast<float>(doubleData[0]), static_cast<float>(doubleData[1]), static_cast<float>(doubleData[2]));
                }
                // Add additional conditions to handle other property types or fields in the Vertex structure
            }

            // Add the filled vertex to the current material's vertices
            currentMaterialVertices.push_back(vertex);
        }

        // Recursively process nested nodes
        ProcessNodeHierarchy(node.nestedNodes, materialVertices, depth + 1);
    }

    // Store the last material's vertices
    if (!currentMaterialVertices.empty()) {
        materialVertices.push_back(currentMaterialVertices);
    }
}

Model readFBX(std::string path) {
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "ERROR : Can't read FBX file. Can't open : " << path << std::endl;
        return;
    }

    // Read header
    char header[27];
    file.read(header, sizeof(header));

    // Read top-level object record
    Node topLevelObject;
    ReadNestedNodes(file, topLevelObject.nestedNodes);

    // Process the FBX data
    std::vector<std::vector<Vertex>> unitedVertices;
    ProcessNodeHierarchy(topLevelObject.nestedNodes, unitedVertices);

    std::vector<std::vector<Vertex>> meshVertices;
    std::vector<std::vector<unsigned int>> meshIndices;

    seperateUnitedVertices(unitedVertices, meshVertices, meshIndices);

    // Close the file
    file.close();

    return createModel(meshVertices, meshIndices, {});
}