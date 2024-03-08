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
    
#define WRITEBITS(var, type, loc) if(!wf.write(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                    LGDLOG::start<< "ERROR : Writing lgdmodel file. Failed to write at : " << loc << LGDLOG::end;\
                                    try{\
                                        if(std::filesystem::exists(path)){\
                                            std::filesystem::remove(path);\
                                        }\
                                    }\
                                    catch (const std::filesystem::filesystem_error& ex) {\
                                        LGDLOG::start << "ERROR : Filesystem : Location ID 54645312 " << ex.what() << LGDLOG::end;\
                                    }\
                                    return false; \
                                }

#define WRITESTR(str)   {size_t strSize = str.size();\
                        WRITEBITS(strSize, size_t, str); \
                        for (size_t charI = 0; charI < strSize; charI++)\
                        {\
                            WRITEBITS(str[charI], char, str);\
                        }}\

bool FileHandler::writeLGDMODELFile(std::string path, Model& model){
    
    if(path == ""){
        path = showFileSystemObjectSelectionDialog(
                                                    "Select a folder to export the model file.", 
                                                    "", 
                                                    {}, 
                                                    false, 
                                                    FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER
                                                );
    }
    
    if(path.size()){

        std::ofstream wf(path + UTIL::folderDistinguisher() + model.title + ".lgdmodel", std::ios::out | std::ios::binary);

        if(!wf) {
            LGDLOG::start<< "ERROR WHILE WRITING BRUSH FILE! Cannot open file : " << path << LGDLOG::end;
            return false;
        }
        
        //!HEADER
        
        //!Description
        uint64_t h1 = 0x8A3F9B0E5CFA2D74; 
        uint64_t h2 = 0xE6754FBD319BAA06; 
        uint64_t h3 = 0x597A5C8296A49ABF; 
        WRITEBITS(h1, uint64_t, "Description 1st");
        WRITEBITS(h2, uint64_t, "Description 2nd");
        WRITEBITS(h3, uint64_t, "Description 3rd");

        //Version number
        uint32_t versionNumber = 2000; 
        WRITEBITS(versionNumber, uint32_t, "Version number");

        // Write meshes
        uint64_t meshSize = model.meshes.size(); 
        WRITEBITS(meshSize, uint64_t, "Mesh size");
        for (size_t meshI = 0; meshI < meshSize; meshI++)
        {
            // Vertices
            uint64_t vertexSize = model.meshes[meshI].vertices.size(); 
            WRITEBITS(vertexSize, uint64_t, "Mesh vertices size");
            for (size_t vertI = 0; vertI < vertexSize; vertI++)
            {
                WRITEBITS(model.meshes[meshI].vertices[vertI].Position.x, float, "Mesh vertex pos x");
                WRITEBITS(model.meshes[meshI].vertices[vertI].Position.y, float, "Mesh vertex pos y");
                WRITEBITS(model.meshes[meshI].vertices[vertI].Position.z, float, "Mesh vertex pos z");
                
                WRITEBITS(model.meshes[meshI].vertices[vertI].TexCoords.x, float, "Mesh vertex TexCoords x");
                WRITEBITS(model.meshes[meshI].vertices[vertI].TexCoords.y, float, "Mesh vertex TexCoords y");
                
                WRITEBITS(model.meshes[meshI].vertices[vertI].Normal.x, float, "Mesh vertex Normal x");
                WRITEBITS(model.meshes[meshI].vertices[vertI].Normal.y, float, "Mesh vertex Normal y");
                WRITEBITS(model.meshes[meshI].vertices[vertI].Normal.z, float, "Mesh vertex Normal z");
                
                WRITEBITS(model.meshes[meshI].vertices[vertI].Tangent.x, float, "Mesh vertex Tangent x");
                WRITEBITS(model.meshes[meshI].vertices[vertI].Tangent.y, float, "Mesh vertex Tangent y");
                WRITEBITS(model.meshes[meshI].vertices[vertI].Tangent.z, float, "Mesh vertex Tangent z");
                
                WRITEBITS(model.meshes[meshI].vertices[vertI].Bitangent.x, float, "Mesh vertex Bitangent x");
                WRITEBITS(model.meshes[meshI].vertices[vertI].Bitangent.y, float, "Mesh vertex Bitangent y");
                WRITEBITS(model.meshes[meshI].vertices[vertI].Bitangent.z, float, "Mesh vertex Bitangent z");
            }
            
            // Indices
            uint64_t indicesSize = model.meshes[meshI].indices.size(); 
            WRITEBITS(indicesSize, uint64_t, "Mesh indices size");
            for (size_t indI = 0; indI < indicesSize; indI++){
                WRITEBITS(model.meshes[meshI].indices[indI], unsigned int, "Mesh indices");
            }
            
            // Objects
            uint64_t objectsSize = model.meshes[meshI].objects.size(); 
            WRITEBITS(objectsSize, uint64_t, "Mesh objects size");
            for (size_t objI = 0; objI < objectsSize; objI++){
                // Object title
                WRITESTR(model.meshes[meshI].objects[objI].title);

                // Object indices (start & end)
                WRITEBITS(model.meshes[meshI].objects[objI].vertIndices.x, int, "Mesh object start index");
                WRITEBITS(model.meshes[meshI].objects[objI].vertIndices.y, int, "Mesh object end index");
            }

            // Material name
            WRITESTR(model.meshes[meshI].materialName);

            // Path to material ID texture
            WRITESTR(model.meshes[meshI].materialIDTxtrPath);
        }
    }

    return true;
}