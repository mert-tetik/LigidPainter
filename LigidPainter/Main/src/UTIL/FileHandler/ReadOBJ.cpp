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
void generateTangentBitangent(std::vector<Vertex>& faceData);
void getDataFromWavefrontFile( std::ifstream& rf, std::vector<glm::vec3>& uniquePositions, std::vector<glm::vec2>& uniqueUVS, std::vector<glm::vec3>& uniqueNormals, std::vector<std::string>& matTitles, std::vector<std::vector<std::vector<glm::vec3>>>& faces);
std::vector<std::vector<Vertex>> getUnitedVerticesData(std::vector<glm::vec3>& uniquePositions, std::vector<glm::vec2>& uniqueUVS, std::vector<glm::vec3>& uniqueNormals, std::vector<std::vector<std::vector<glm::vec3>>>& faces);
void seperateUnitedVertices(std::vector<std::vector<Vertex>>& unitedVertices, std::vector<std::vector<Vertex>>& meshVertices, std::vector<std::vector<unsigned int>>& meshIndices);
Model createModel(std::vector<std::vector<Vertex>> meshVertices, std::vector<std::vector<unsigned int>> meshIndices, std::vector<std::string> matTitles);

Model FileHandler::readOBJFile(std::string path){

    return Model();
    
    std::ifstream rf(path, std::ios::in);
    
    if(!rf){
        std::cout << "ERROR : Can't read OBJ file. Can't open : " << path << std::endl;
    }

    // Unique values retrieved from the wavefront file
    std::vector<glm::vec3> uniquePositions;
    std::vector<glm::vec2> uniqueUVS;
    std::vector<glm::vec3> uniqueNormals;
    std::vector<std::string> matTitles;
    std::vector< // Material
                std::vector< // Faces
                            std::vector< // A face
                                        glm::vec3>>> faces; // Indices of a vertex


    // Retrieve data from the file (exactly whatever written to the wavefront file)
    getDataFromWavefrontFile(rf, uniquePositions, uniqueUVS, uniqueNormals, matTitles, faces);

    // We're done with the obj file from now on
    rf.close();

    // Create a whole vertex array seperated into materials by interpreting the faces array.
    std::vector<std::vector<Vertex>> unitedVertices = getUnitedVerticesData(uniquePositions, uniqueUVS, uniqueNormals, faces);


    std::vector<std::vector<Vertex>> meshVertices;
    std::vector<std::vector<unsigned int>> meshIndices;
    seperateUnitedVertices(unitedVertices, meshVertices, meshIndices);

    Model model = createModel(meshVertices, meshIndices, matTitles);

    return model;

}

void getDataFromWavefrontFile(
                                std::ifstream& rf,
                                std::vector<glm::vec3>& uniquePositions,
                                std::vector<glm::vec2>& uniqueUVS,
                                std::vector<glm::vec3>& uniqueNormals,
                                std::vector<std::string>& matTitles,
                                std::vector<std::vector<std::vector<glm::vec3>>>& faces
                            ) // Indices of a vertex)
{
    std::string line;
    while (std::getline(rf, line))
    {
        // ---- Vertex position data detected
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
        
        // ---- Vertex texture coordinate data detected
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
                std::cout << "LigidExporter : Failed to extract values from the string. (Vertex UV data)" << std::endl;
            }
        }
        
        // ---- Vertex normal vector data detected
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
                std::cout << "LigidExporter : Failed to extract values from the string. (Vertex Normal data)" << std::endl;
            }
        }

        // ---- Material data detected
        else if(line.substr(0,6) == "usemtl "){
            faces.push_back({});

            std::stringstream ss(line); // Create a stringstream object with the input string
            
            std::string fStr;
            ss >> fStr; // Read and discard the first string "usemtl"

            std::string matStr;
            ss >> matStr; // Read the material title

            matTitles.push_back(matStr);
        }

        // ---- Face index data detected
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
}