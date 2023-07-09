/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Basic *.obj file exporter

    v - vertex pos
    vt - vertex texture coordinates
    vn - vertex normal vector
    f - face
    usemtl - indicate the material
    # - comment lines

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

const bool _Triangulation = true;

// Utility function forward declerations
std::vector<std::vector<Vertex>> triangulateFaces(const std::vector<Vertex>& faceData);
bool containsVertex(const std::vector<Vertex>& vertexArray, const Vertex& targetVertex);
int getVertexIndex(const std::vector<Vertex>& vertexArray, const Vertex& targetVertex);
void generateTangentBitangent(std::vector<Vertex>& faceData);


Model FileHandler::readOBJFile(std::string path){

    
    std::ifstream rf(path, std::ios::in);
    
    if(!rf){
        std::cout << "ERROR : Can't read OBJ file. Can't open : " << path << std::endl;
    }

    std::vector<glm::vec3> uniquePositions;
    std::vector<glm::vec2> uniqueUVS;
    std::vector<glm::vec3> uniqueNormals;
    std::vector< // Material
                std::vector< // Faces
                            std::vector< // A face
                                        glm::vec3>>> faces; // Indices of a vertex

    // Process uniuqe vertex data
    std::string line;
    while (std::getline(rf, line))
    {
        if(line.substr(0,2) == "v "){

            std::stringstream ss(line); // Create a stringstream object with the input string

            std::string str;
            ss >> str; // Read and discard the first string "v"

            double value1, value2, value3;
            ss >> value1 >> value2 >> value3; // Extract the three numeric values

            if (!ss.fail()) { // Check if the extraction was successful
                uniquePositions.push_back(glm::vec3(value1, value2, value3));
            } 
            else 
            {
                std::cout << "LigidExporter : Failed to extract values from the string. (Vertex pos data)" << std::endl;
            }
        }

        else if(line.substr(0,2) == "vt"){

            std::stringstream ss(line); // Create a stringstream object with the input string

            std::string str;
            ss >> str; // Read and discard the first string "vt"

            double value1, value2;
            ss >> value1 >> value2; // Extract the three numeric values

            if (!ss.fail()) { // Check if the extraction was successful
                uniqueUVS.push_back(glm::vec2(value1, value2));
            } 
            else 
            {
                std::cout << "LigidExporter : Failed to extract values from the string. (Vertex pos data)" << std::endl;
            }
        }
        
        else if(line.substr(0,2) == "vn"){

            std::stringstream ss(line); // Create a stringstream object with the input string

            std::string str;
            ss >> str; // Read and discard the first string "vn"

            double value1, value2, value3;
            ss >> value1 >> value2 >> value3; // Extract the three numeric values

            if (!ss.fail()) { // Check if the extraction was successful
                uniqueNormals.push_back(glm::vec3(value1, value2, value3));
            } 
            else 
            {
                std::cout << "LigidExporter : Failed to extract values from the string. (Vertex pos data)" << std::endl;
            }
        }

        else if(line.substr(0,6) == "usemtl "){
            faces.push_back({});
        }

        else if(line.substr(0,2) == "f "){
            std::stringstream ss(line);

            std::string str;
            ss >> str; // Read and discard the first string "f"

            std::vector<glm::vec3> vec3List;

            while (true) {
                int v, vt, vn;
                char slash;
                
                if (ss >> v >> slash >> vt >> slash >> vn) {
                    // Create a glm::vec3 and add it to the vector
                    glm::vec3 vec(v, vt, vn);
                    vec3List.push_back(vec);
                } 
                
                else {
                    break; // Exit the loop when extraction fails
                }
            }
            if(faces.size() == 0)
                faces.push_back({});

            faces[faces.size()-1].push_back(vec3List);
        }
    }

    // Our job is done with the obj file from now on
    rf.close();

    std::vector<std::vector<Vertex>> unitedVertices;

    for (size_t matI = 0; matI < faces.size(); matI++){
        
        unitedVertices.push_back({});

        for (size_t fI = 0; fI < faces[matI].size(); fI++)
        {
            std::vector<Vertex> _face;

            for (size_t vI = 0; vI < faces[matI][fI].size(); vI++)
            {
                Vertex vert;

                // Decreasing 1 from the indices cause obj file format start's counting indices from 1
                int vPosI = faces[matI][fI][vI].x - 1;
                int vUVI = faces[matI][fI][vI].y - 1;
                int vNI = faces[matI][fI][vI].z - 1;

                vert.Position = uniquePositions[vPosI];
                vert.TexCoords = uniqueUVS[vUVI];
                vert.Normal = uniqueNormals[vNI];

                _face.push_back(vert);
            }

            if(true){ //Triangulation & tangent-bitangent calculations
                std::vector<std::vector<Vertex>> triangulatedFaces = triangulateFaces(_face);

                for (size_t trianFaceI = 0; trianFaceI < triangulatedFaces.size(); trianFaceI++)
                {
                    generateTangentBitangent(triangulatedFaces[trianFaceI]);

                    for (size_t trianVertI = 0; trianVertI < triangulatedFaces[trianFaceI].size(); trianVertI++)
                    {
                        unitedVertices[matI].push_back(triangulatedFaces[trianFaceI][trianVertI]);
                    }
                }
            }
        }
    }

    Model model;

    std::vector<std::vector<Vertex>> meshVertices;
    std::vector<std::vector<unsigned int>> meshIndices;

    
    // Unique vertices
    for (size_t unMatI = 0; unMatI < unitedVertices.size(); unMatI++)
    {
        meshVertices.push_back({});
        for (size_t i = 0; i < unitedVertices[unMatI].size(); i++)
        {
            if(!containsVertex(meshVertices[unMatI], unitedVertices[unMatI][i])){
                meshVertices[unMatI].push_back(unitedVertices[unMatI][i]);
            }   
        }
    }
    
    // Face indices
    for (size_t unMatI = 0; unMatI < unitedVertices.size(); unMatI++)
    {
        meshIndices.push_back({});
        for (size_t i = 0; i < unitedVertices[unMatI].size(); i++)
        {
            meshIndices[unMatI].push_back(getVertexIndex(meshVertices[unMatI], unitedVertices[unMatI][i]));
        }
    }

    for (int i = 0; i < meshVertices.size(); i++)
    {
        model.meshes.push_back(Mesh(meshVertices[i], meshIndices[i], (std::string)"DefMat_", i));
    }

    model.newModelAdded = true;

    return model;

}

/// @brief Triangulates the given face data
/// @return Triangulated faces 
std::vector<std::vector<Vertex>> triangulateFaces(const std::vector<Vertex>& faceData) {
    std::vector<std::vector<Vertex>> triangulatedData;

    std::vector<Vertex> currentFace;

    // Triangulate each face
    for (const auto& vertex : faceData) {
        currentFace.push_back(vertex);

        if (currentFace.size() >= 3) {
            triangulatedData.push_back(currentFace);

            if (currentFace.size() > 3) {
                std::vector<Vertex> newFace;
                newFace.push_back(currentFace[0]);
                newFace.push_back(currentFace[currentFace.size() - 2]);
                newFace.push_back(currentFace[currentFace.size() - 1]);
                triangulatedData.push_back(newFace);
            }
        }
    }

    return triangulatedData;
}

bool containsVertex(const std::vector<Vertex>& vertexArray, const Vertex& targetVertex) {
    for (const auto& vertex : vertexArray) {
        if (vertex.Position == targetVertex.Position &&
            vertex.TexCoords == targetVertex.TexCoords &&
            vertex.Normal == targetVertex.Normal) {
            return true; // Vertex found in the array
        }
    }
    return false; // Vertex not found in the array
}

int getVertexIndex(const std::vector<Vertex>& vertexArray, const Vertex& targetVertex) {
    int i = 0;
    for (const auto& vertex : vertexArray) {
        if (vertex.Position == targetVertex.Position &&
            vertex.TexCoords == targetVertex.TexCoords &&
            vertex.Normal == targetVertex.Normal) {
            return i; // Vertex found in the array
        }
        i++;
    }
    return i; // Vertex not found in the array
}

void calculateTangentBitangent(Vertex& v0, Vertex& v1, Vertex& v2) {
    // Calculate position differences
    glm::vec3 deltaPos1 = v1.Position - v0.Position;
    glm::vec3 deltaPos2 = v2.Position - v0.Position;

    // Calculate texture coordinate differences
    glm::vec2 deltaUV1 = v1.TexCoords - v0.TexCoords;
    glm::vec2 deltaUV2 = v2.TexCoords - v0.TexCoords;

    // Calculate the reciprocal of the determinant
    float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

    // Calculate tangent and bitangent
    glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
    glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

    // Accumulate tangent and bitangent vectors
    v0.Tangent += tangent;
    v1.Tangent += tangent;
    v2.Tangent += tangent;

    v0.Bitangent += bitangent;
    v1.Bitangent += bitangent;
    v2.Bitangent += bitangent;
}

void generateTangentBitangent(std::vector<Vertex>& faceData) {
    // Initialize tangent and bitangent vectors to zero
    for (auto& vertex : faceData) {
        vertex.Tangent = glm::vec3(0.0f);
        vertex.Bitangent = glm::vec3(0.0f);
    }

    // Calculate tangent and bitangent for each triangle in the face
    for (size_t i = 0; i < faceData.size(); i += 3) {
        Vertex& v0 = faceData[i];
        Vertex& v1 = faceData[i + 1];
        Vertex& v2 = faceData[i + 2];

        calculateTangentBitangent(v0, v1, v2);
    }

    // Normalize tangent and bitangent vectors
    for (auto& vertex : faceData) {
        vertex.Tangent = glm::normalize(vertex.Tangent);
        vertex.Bitangent = glm::normalize(vertex.Bitangent);
    }
}