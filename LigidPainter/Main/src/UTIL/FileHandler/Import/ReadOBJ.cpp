/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

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

    TODO Process objects

*/

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

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

// Forward declarations for the utilities
void seperateUnitedVertices(std::vector<std::vector<Vertex>>& unitedVertices, std::vector<std::vector<Vertex>>& meshVertices, std::vector<std::vector<unsigned int>>& meshIndices);
Model createModel(std::vector<std::vector<Vertex>> meshVertices, std::vector<std::vector<unsigned int>> meshIndices, std::vector<std::string> matTitles);
void calculateTangentBitangent(Vertex& v0, Vertex& v1, Vertex& v2);

// Forward declarations for the obj file processing functions
static void parseOBJMeshData(std::vector<glm::vec3> uniquePositions,std::vector<glm::vec2> uniqueUVS,std::vector<glm::vec3> uniqueNormals,std::vector<std::string> matTitles,std::vector<std::vector<std::vector<glm::vec3>>> faces,std::vector<std::vector<Vertex>>& meshVertices,std::vector<std::vector<unsigned int>>& meshIndices);
static void getDataFromWavefrontFile( std::ifstream& rf, std::vector<glm::vec3>& uniquePositions, std::vector<glm::vec2>& uniqueUVS, std::vector<glm::vec3>& uniqueNormals, std::vector<std::string>& matTitles, std::vector<std::vector<std::vector<glm::vec3>>>& faces);

#define LIGID_OBJ_IMPORTER_TRIANGULATE true

int __obj_Read_Face_I = 0;

Model FileHandler::readOBJFile(std::string path){

    std::ifstream rf(path, std::ios::in);
    
    if(!rf){
        LGDLOG::start<< "ERROR : Can't read OBJ file. Can't open : " << path << LGDLOG::end;
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

    std::vector<std::vector<Vertex>> meshVertices;
    std::vector<std::vector<unsigned int>> meshIndices;

    parseOBJMeshData(uniquePositions, uniqueUVS, uniqueNormals, matTitles, faces, meshVertices, meshIndices);

    if(meshVertices.size()){
        return createModel(meshVertices, meshIndices, matTitles);
    }
    else{
        LGDLOG::start<< "WARNING! Reading FBX file : No mesh data received" << LGDLOG::end;
        return Model();
    }

}

static void getDataFromWavefrontFile(
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
                LGDLOG::start<< "LigidExporter : Failed to extract values from the string. (Vertex pos data)" << LGDLOG::end;
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
                LGDLOG::start<< "LigidExporter : Failed to extract values from the string. (Vertex UV data)" << LGDLOG::end;
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
                LGDLOG::start<< "LigidExporter : Failed to extract values from the string. (Vertex Normal data)" << LGDLOG::end;
            }
        }

        // ---- Material data detected
        else if(line.substr(0,6) == "usemtl"){

            std::stringstream ss(line); // Create a stringstream object with the input string
            
            std::string fStr;
            ss >> fStr; // Read and discard the first string "usemtl"

            std::string matStr;
            ss >> matStr; // Read the material title

            bool matched = false;
            for (size_t i = 0; i < matTitles.size(); i++)
            {
                if(matTitles[i] == matStr){
                    matched = true;
                    __obj_Read_Face_I = i;
                    break;
                }
            }
            
            if(!matched){
                faces.push_back({});
                __obj_Read_Face_I = faces.size() - 1;
                matTitles.push_back(matStr);
            }
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
            
            if(faces.size() == 0){
                __obj_Read_Face_I = 0;
                faces.push_back({});
            }
            
            if(__obj_Read_Face_I < faces.size())
                faces[__obj_Read_Face_I].push_back(vec3List);
        }
    }
}

static void parseOBJMeshData(
                                std::vector<glm::vec3> uniquePositions,
                                std::vector<glm::vec2> uniqueUVS,
                                std::vector<glm::vec3> uniqueNormals,
                                std::vector<std::string> matTitles,
                                std::vector<std::vector<std::vector<glm::vec3>>> faces,
                                std::vector<std::vector<Vertex>>& meshVertices,
                                std::vector<std::vector<unsigned int>>& meshIndices
                            )
{
    
    //TODO : Unique vertices
    
    std::vector<std::vector<std::vector<glm::vec3>>> triangulatedFaces;

    if(LIGID_OBJ_IMPORTER_TRIANGULATE){
        for (size_t matI = 0; matI < faces.size(); matI++){
            triangulatedFaces.push_back({});

            for (size_t faceI = 0; faceI < faces[matI].size(); faceI++){

                /* Triangulation */
                int faceCount = faces[matI][faceI].size() - 2;

                if(faceCount < 0)
                    faceCount = 0;

                int vStartI = 0;

                for (size_t fI = 0; fI < faceCount; fI++)
                {
                    std::vector<glm::vec3> newFace;

                    glm::vec3 vert1 = faces[matI][faceI][vStartI];
                    glm::vec3 vert2 = faces[matI][faceI][vStartI + 1 + fI];
                    glm::vec3 vert3 = faces[matI][faceI][vStartI + 2 + fI];

                    newFace.push_back(vert1);
                    newFace.push_back(vert2);
                    newFace.push_back(vert3);

                    triangulatedFaces[matI].push_back(newFace);

                }
            }
        }
    }
    else{
        triangulatedFaces = faces;
    }


    for (size_t matI = 0; matI < triangulatedFaces.size(); matI++)
    {
        meshVertices.push_back({});  
        meshIndices.push_back({});  
        
        for (size_t faceI = 0; faceI < triangulatedFaces[matI].size(); faceI++)
        {
            /*Ignore lines and points*/
            if(triangulatedFaces[matI][faceI].size() > 2){

                Vertex vert1;
                vert1.Position = uniquePositions[triangulatedFaces[matI][faceI][0].x - 1];  
                vert1.TexCoords = uniqueUVS[triangulatedFaces[matI][faceI][0].y - 1];  
                vert1.Normal = uniqueNormals[triangulatedFaces[matI][faceI][0].z - 1];
                Vertex vert2;
                vert2.Position = uniquePositions[triangulatedFaces[matI][faceI][1].x - 1];  
                vert2.TexCoords = uniqueUVS[triangulatedFaces[matI][faceI][1].y - 1];  
                vert2.Normal = uniqueNormals[triangulatedFaces[matI][faceI][1].z - 1];
                Vertex vert3;
                vert3.Position = uniquePositions[triangulatedFaces[matI][faceI][2].x - 1];  
                vert3.TexCoords = uniqueUVS[triangulatedFaces[matI][faceI][2].y - 1];  
                vert3.Normal = uniqueNormals[triangulatedFaces[matI][faceI][2].z - 1];

                /* Ignore untriangulated vertices */
                
                /* Tangent calculations */
                calculateTangentBitangent(vert1, vert2, vert3);

                /* Normalize tangent values */
                vert1.Tangent = glm::normalize(vert1.Tangent);
                vert1.Bitangent = glm::normalize(vert1.Bitangent);
                vert2.Tangent = glm::normalize(vert2.Tangent);
                vert2.Bitangent = glm::normalize(vert2.Bitangent);
                vert3.Tangent = glm::normalize(vert3.Tangent);
                vert3.Bitangent = glm::normalize(vert3.Bitangent);

                meshIndices[matI].push_back(meshVertices[matI].size());
                meshVertices[matI].push_back(vert1);  
                meshIndices[matI].push_back(meshVertices[matI].size());
                meshVertices[matI].push_back(vert2);  
                meshIndices[matI].push_back(meshVertices[matI].size());
                meshVertices[matI].push_back(vert3);  
            }
        }
    }
} 