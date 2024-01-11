/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Basic *.obj file exporter

    v - vertex pos
    vt - vertex texture coordinates
    vn - vertex normal vector
    f - face (position index / TexCoords index / Normal vector index)
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

#define LIGID_OBJ_IMPORTER_TRIANGULATE true

// Forward declarations for the utilities
void seperateUnitedVertices(std::vector<std::vector<Vertex>>& unitedVertices, std::vector<std::vector<Vertex>>& meshVertices, std::vector<std::vector<unsigned int>>& meshIndices);
Model createModel(std::vector<std::vector<Vertex>> meshVertices, std::vector<std::vector<unsigned int>> meshIndices, std::vector<std::string> matTitles, std::vector<std::vector<MeshObject>> objectData);
void calculateTangentBitangent(Vertex& v0, Vertex& v1, Vertex& v2);

struct WavefrontObject{
public:
    std::string title;
    std::string material;

    /// @brief Face count * face vertex count * 3(pos, texcoords, normal)
    std::vector<std::vector<glm::vec3>> faces; 

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    void calculateVertices(){
        for (size_t faceI = 0; faceI < faces.size(); faceI++)
        {
            for (size_t vertI = 0; vertI < faces[faceI].size(); vertI++)
            {
                Vertex vertex;
                unsigned int positionI = faces[faceI][vertI].x;
                unsigned int texCoordsI = faces[faceI][vertI].y;
                unsigned int normalVecI = faces[faceI][vertI].z;

                // Positions
                if(positionI < positions.size())
                    vertex.Position = positions[positionI];
                else
                    LGDLOG::start << "ERROR : Wavefront reader : Invalid index value for the vertex position!" << LGDLOG::end;
                
                // TexCoords
                if(positionI < positions.size())
                    vertex.TexCoords = texCoords[texCoordsI];
                else
                    LGDLOG::start << "ERROR : Wavefront reader : Invalid index value for the vertex UV!" << LGDLOG::end;
                
                // Normal vectors
                if(positionI < positions.size())
                    vertex.Normal = normals[normalVecI];
                else
                    LGDLOG::start << "ERROR : Wavefront reader : Invalid index value for the vertex normal vectors!" << LGDLOG::end;

                vertices.push_back(vertex);
                indices.push_back(vertices.size());
            }
            
            // Generate tangents if the face is a triangle 
            if(faces[faceI].size() == 3){
                calculateTangentBitangent(vertices[vertices.size()-3], vertices[vertices.size()-2], vertices[vertices.size()-1]);
            }
        }
    }


    void triangulateFaces(){
        std::vector<std::vector<glm::vec3>> triangulatedFaces; 

        for (size_t faceI = 0; faceI < faces.size(); faceI++){

            /* Triangulation */
            int faceCount = faces[faceI].size() - 2;

            if(faceCount < 0)
                faceCount = 0;

            int vStartI = 0;

            for (size_t fI = 0; fI < faceCount; fI++)
            {
                std::vector<glm::vec3> newFace;

                glm::vec3 vert1 = faces[faceI][vStartI];
                glm::vec3 vert2 = faces[faceI][vStartI + 1 + fI];
                glm::vec3 vert3 = faces[faceI][vStartI + 2 + fI];

                newFace.push_back(vert1);
                newFace.push_back(vert2);
                newFace.push_back(vert3);

                triangulatedFaces.push_back(newFace);
            }
        }
    }

    WavefrontObject(std::string title){
        this->title = title;
    }
};

static std::vector<std::string> parseOBJMeshData(std::vector<WavefrontObject> objects, std::vector<std::vector<Vertex>>& meshVertices, std::vector<std::vector<unsigned int>>& meshIndices, std::vector<std::vector<MeshObject>>& objectData);
static std::vector<WavefrontObject> getDataFromWavefrontFile(std::ifstream& rf);


Model FileHandler::readOBJFile(std::string path){

    std::ifstream rf(path, std::ios::in);
    
    if(!rf){
        LGDLOG::start<< "ERROR : Can't read OBJ file. Can't open : " << path << LGDLOG::end;
    }

    // Unique values retrieved from the wavefront file
    std::vector<WavefrontObject> objects;

    // Retrieve data from the file (exactly whatever written to the wavefront file)
    objects = getDataFromWavefrontFile(rf);

    // Triangulate the faces
    if(LIGID_OBJ_IMPORTER_TRIANGULATE){
        for (size_t i = 0; i < objects.size(); i++)
        {
            objects[i].triangulateFaces();
        }
    }

    // We're done with the obj file from now on
    rf.close();

    std::vector<std::vector<Vertex>> meshVertices;
    std::vector<std::vector<unsigned int>> meshIndices;
    std::vector<std::vector<MeshObject>> objectData;
    std::vector<std::string> matTitles;

    matTitles = parseOBJMeshData(objects, meshVertices, meshIndices, objectData);

    if(meshVertices.size()){
        return createModel(meshVertices, meshIndices, matTitles, objectData);
    }
    else{
        LGDLOG::start<< "WARNING! Reading FBX file : No mesh data received" << LGDLOG::end;
        return Model();
    }

}



static std::vector<WavefrontObject> getDataFromWavefrontFile(
                                                                std::ifstream& rf
                                                            )
{
    std::vector<WavefrontObject> objects;

    std::string line;
    while (std::getline(rf, line))
    {
        // ---- Object data detected
        if(line.substr(0,2) == "o "){

            std::stringstream ss(line); // Create a stringstream object with the input string
            
            std::string fStr;
            ss >> fStr; // Read and discard the first string "o "

            std::string title;
            ss >> title; // Read the object title

            objects.push_back(WavefrontObject(title));
        }

        // ---- Vertex position data detected
        else if(line.substr(0,2) == "v "){

            std::stringstream ss(line); // Create a stringstream object with the input string

            std::string str;
            ss >> str; // Read and discard the first string "v"

            double value1, value2, value3;
            ss >> value1 >> value2 >> value3; // Extract the three numeric values

            if (!ss.fail()) { // Check if the extraction was successful
                if(objects.size())
                    objects[objects.size()-1].positions.push_back(glm::vec3(value1, value2, value3));
                else
                    LGDLOG::start << "WARNING! : Wavefront file reader - tried to write object data without identifying an object!" << LGDLOG::end;
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
                if(objects.size())
                    objects[objects.size()-1].texCoords.push_back(glm::vec2(value1, value2));
                else
                    LGDLOG::start << "WARNING! : Wavefront file reader - tried to write object data without identifying an object!" << LGDLOG::end;
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
                if(objects.size())
                    objects[objects.size()-1].normals.push_back(glm::vec3(value1, value2, value3));
                else
                    LGDLOG::start << "WARNING! : Wavefront file reader - tried to write object data without identifying an object!" << LGDLOG::end;
            } 
            else 
            {
                LGDLOG::start<< "LigidExporter : Failed to extract values from the string. (Vertex Normal data)" << LGDLOG::end;
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
            
            if(objects.size())
                objects[objects.size()-1].faces.push_back(vec3List);
            else
                LGDLOG::start << "WARNING! : Wavefront file reader - tried to write object data without identifying an object!" << LGDLOG::end;
        }

        else if(line.substr(0, 6) == "usemtl"){
            std::stringstream ss(line);

            std::string fStr;
            ss >> fStr; // Read and discard the first string "usemtl"

            std::string matStr;
            ss >> matStr; // Read the material title
            
            if(objects.size())
                objects[objects.size()-1].material = matStr;
            else
                LGDLOG::start << "WARNING! : Wavefront file reader - tried to write object data without identifying an object!" << LGDLOG::end;
        }
    }

    return objects;
}



static std::vector<std::string> parseOBJMeshData(
                                                    std::vector<WavefrontObject> objects,
                                                    std::vector<std::vector<Vertex>>& meshVertices,
                                                    std::vector<std::vector<unsigned int>>& meshIndices,
                                                    std::vector<std::vector<MeshObject>>& objectData
                                                )
{
    for (size_t i = 0; i < objects.size(); i++)
    {
        objects[i].calculateVertices();
    }
    
    std::vector<std::string> matTitles;

    for (size_t objI = 0; objI < objects.size(); objI++)
    {
        bool matTitleMatches = false;
        int index = 0;
        for (size_t matTitleI = 0; matTitleI < matTitles.size(); matTitleI++)
        {
            if(matTitles[matTitleI] == objects[objI].material){
                matTitleMatches = true;
                index = matTitleI;
            }
        }
        
        if(!matTitleMatches){
            matTitles.push_back(objects[objI].material);
            meshVertices.push_back(objects[objI].vertices);
            meshIndices.push_back(objects[objI].indices);
            objectData.push_back({MeshObject(objects[objI].title, glm::ivec2(0, objects[objI].vertices.size()))});
        }
        else{
            int startI = meshVertices[index].size();
            for (size_t i = 0; i < objects[objI].vertices.size(); i++)
            {
                meshIndices[index].push_back(objects[objI].indices[i] + meshVertices[index].size());
            }
            
            for (size_t i = 0; i < objects[objI].vertices.size(); i++)
            {
                meshVertices[index].push_back(objects[objI].vertices[i]);
            }
            objectData[index].push_back(MeshObject(objects[objI].title, glm::ivec2(startI, meshVertices[index].size())));
        }
    }

    return matTitles;
    
} 