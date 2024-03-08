/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
    
#define READBITS(var, type, loc) if(!rf.read(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                    LGDLOG::start<< "ERROR : Reading lgdmodel file. Failed to read at : " << loc << LGDLOG::end;\
                                    return false; \
                                }

#define READSTR(str){size_t strSize;\
                    READBITS(strSize, size_t, str); \
                    for (size_t charI = 0; charI < strSize; charI++)\
                    {\
                        char c;\
                        READBITS(c, char, str);\
                        str.push_back(c);\
                    }}\


bool FileHandler::readLGDMODELFile(std::string path, Model& model){
    
    if(path.size()){

        std::ifstream rf(path, std::ios::in | std::ios::binary);

        if(!rf) {
            LGDLOG::start<< "ERROR WHILE WRITING BRUSH FILE! Cannot open file : " << path << LGDLOG::end;
            return false;
        }
        
        //!HEADER
        
        //!Description
        uint64_t h1 = 0x8A3F9B0E5CFA2D74; 
        uint64_t h2 = 0xE6754FBD319BAA06; 
        uint64_t h3 = 0x597A5C8296A49ABF; 
        READBITS(h1, uint64_t, "Description 1st");
        READBITS(h2, uint64_t, "Description 2nd");
        READBITS(h3, uint64_t, "Description 3rd");

        //Version number
        uint32_t versionNumber = 2000; 
        READBITS(versionNumber, uint32_t, "Version number");

        // Title of the model
        model.title = UTIL::removeExtension(UTIL::getLastWordBySeparatingWithChar(path, UTIL::folderDistinguisher()));

        // Path to the model
        model.path = path;

        // Write meshes
        uint64_t meshSize = model.meshes.size(); 
        READBITS(meshSize, uint64_t, "Mesh size");
        for (size_t meshI = 0; meshI < meshSize; meshI++)
        {
            Mesh mesh;

            // Vertices
            uint64_t vertexSize; 
            READBITS(vertexSize, uint64_t, "Mesh vertices size");
            for (size_t vertI = 0; vertI < vertexSize; vertI++)
            {
                Vertex vertex;
                READBITS(vertex.Position.x, float, "Mesh vertex pos x");
                READBITS(vertex.Position.y, float, "Mesh vertex pos y");
                READBITS(vertex.Position.z, float, "Mesh vertex pos z");
                
                READBITS(vertex.TexCoords.x, float, "Mesh vertex TexCoords x");
                READBITS(vertex.TexCoords.y, float, "Mesh vertex TexCoords y");
                
                READBITS(vertex.Normal.x, float, "Mesh vertex Normal x");
                READBITS(vertex.Normal.y, float, "Mesh vertex Normal y");
                READBITS(vertex.Normal.z, float, "Mesh vertex Normal z");
                
                READBITS(vertex.Tangent.x, float, "Mesh vertex Tangent x");
                READBITS(vertex.Tangent.y, float, "Mesh vertex Tangent y");
                READBITS(vertex.Tangent.z, float, "Mesh vertex Tangent z");
                
                READBITS(vertex.Bitangent.x, float, "Mesh vertex Bitangent x");
                READBITS(vertex.Bitangent.y, float, "Mesh vertex Bitangent y");
                READBITS(vertex.Bitangent.z, float, "Mesh vertex Bitangent z");
            
                mesh.vertices.push_back(vertex);
            }
            
            // Indices
            uint64_t indicesSize; 
            READBITS(indicesSize, uint64_t, "Mesh indices size");
            for (size_t indI = 0; indI < indicesSize; indI++){
                unsigned int index;
                READBITS(index, unsigned int, "Mesh indices");
                mesh.indices.push_back(index);
            }
            
            // Objects
            uint64_t objectsSize; 
            READBITS(objectsSize, uint64_t, "Mesh objects size");
            for (size_t objI = 0; objI < objectsSize; objI++){
                MeshObject object;
                
                // Object title
                READSTR(object.title);

                // Object indices (start & end)
                READBITS(object.vertIndices.x, int, "Mesh object start index");
                READBITS(object.vertIndices.y, int, "Mesh object end index");
            
                mesh.objects.push_back(object);
            }

            // Material name
            READSTR(mesh.materialName);

            // Path to material ID texture
            READSTR(mesh.materialIDTxtrPath);
            mesh.materialIDTxtr.load(mesh.materialIDTxtrPath.c_str());
            mesh.materialIDColors = mesh.materialIDTxtr.getMaterialIDPalette();
        
            Mesh resMesh = Mesh(mesh.vertices, mesh.indices, mesh.materialName, true);
            resMesh.materialIDTxtr = mesh.materialIDTxtr;
            resMesh.materialIDColors = mesh.materialIDColors;
            resMesh.materialIDTxtrPath = mesh.materialIDTxtrPath;
            resMesh.objects = mesh.objects;
            model.meshes.push_back(resMesh);
        }
    }

    model.newModelAdded = true;
    model.generateDisplayingTexture();

    return true;
}