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
#include "Toolkit/Layers/Layers.hpp"
    
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

#define WRITE_VEC3(vec, loc)    WRITEBITS(vec.x, float, loc);\
                                WRITEBITS(vec.y, float, loc);\
                                WRITEBITS(vec.z, float, loc);\


#define WRITESTR(str)   {size_t strSize = str.size();\
                        WRITEBITS(strSize, size_t, str); \
                        for (size_t charI = 0; charI < strSize; charI++)\
                        {\
                            WRITEBITS(str[charI], char, str);\
                        }}\

#define FNC(function){ \
    if(!function)\
        return false;\
}\

static bool write_channel_alpha(std::ofstream& wf, std::string path, ChannelAlpha channelAlpha){
    WRITEBITS(channelAlpha.alphaValue, float, "Channel alpha - Alpha value");
    if(!channelAlpha.alphaMap.writeTextureData(wf))
        return false;

    int resolution = channelAlpha.alphaMapProceduralTxtr.getResolution().x;
    WRITEBITS(resolution, int, "Channel alpha - Resolution value");
    
    return true;
}   

bool FileHandler::writeLGDMODELFile(std::string path, Model model){
    
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

        UTIL::correctFolderDistinguishers(path);

        // Create the file path if path leads to a directory
        if(std::filesystem::is_directory(path))    
            path += UTIL::folderDistinguisher() + model.title + ".lgdmodel";

        std::ofstream wf(path, std::ios::out | std::ios::binary);

        if(!wf) {
            LGDLOG::start<< "ERROR WHILE WRITING MODEL FILE! Cannot open file : " << path << LGDLOG::end;
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

            // Layers
            uint64_t layersSize = model.meshes[meshI].layerScene.layers.size(); 
            WRITEBITS(layersSize, uint64_t, "Mesh layers size");
            for (size_t layerI = 0; layerI < layersSize; layerI++)
            {
                Layer*& layer = model.meshes[meshI].layerScene.layers[layerI]; 
                
                // Type of the layer
                WRITESTR(layer->layerType); 

                // Opacity / alpha values
                FNC(write_channel_alpha(wf, path, layer->alpha.general_Alpha))
                FNC(write_channel_alpha(wf, path, layer->alpha.albedo_Alpha))
                FNC(write_channel_alpha(wf, path, layer->alpha.roughness_Alpha))
                FNC(write_channel_alpha(wf, path, layer->alpha.metallic_Alpha))
                FNC(write_channel_alpha(wf, path, layer->alpha.normalMap_Alpha))
                FNC(write_channel_alpha(wf, path, layer->alpha.heightMap_Alpha))
                FNC(write_channel_alpha(wf, path, layer->alpha.ambientOcclusion_Alpha))
            
                // Is layer hidden
                WRITEBITS(layer->hiden, bool, "Layer - hidden");
                
                // Title of the layer
                WRITESTR(layer->title); 

                // Type specific data
                if(layer->layerType == "texture"){
                    MaterialChannels* channels;
                    layer->get_type_specific_variable(nullptr, nullptr, nullptr, nullptr, nullptr, &channels, nullptr);
                    
                    channels->albedo.writeTextureData(wf);
                    channels->roughness.writeTextureData(wf);
                    channels->metallic.writeTextureData(wf);
                    channels->normalMap.writeTextureData(wf);
                    channels->heightMap.writeTextureData(wf);
                    channels->ambientOcclusion.writeTextureData(wf);
                }
                else if(layer->layerType == "painting"){
                    FNC(layer->result.albedo.writePixelData(wf))
                    FNC(layer->result.roughness.writePixelData(wf))
                    FNC(layer->result.metallic.writePixelData(wf))
                    FNC(layer->result.normalMap.writePixelData(wf))
                    FNC(layer->result.heightMap.writePixelData(wf))
                    FNC(layer->result.ambientOcclusion.writePixelData(wf))
                }
                else if(layer->layerType == "material"){
                    Material* material;
                    layer->get_type_specific_variable(&material, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

                    FileHandler::writeMaterialData(wf, *material);
                }
                else if(layer->layerType == "vector"){
                    VectorScene* vectorScene;
                    Brush* vector_stroke_brush;
                    CheckComboList* color_checkComboList; 
                    CheckComboList* mirror_checkComboList;

                    layer->get_type_specific_variable(nullptr, &vectorScene, &vector_stroke_brush, &color_checkComboList, &mirror_checkComboList, nullptr, nullptr);

                    // Color Settings 
                    WRITEBITS(color_checkComboList->panel.sections[0].elements[1].checkBox.clickState1, bool, "Enable Albedo Channel");
                    WRITE_VEC3(color_checkComboList->panel.sections[0].elements[2].painterColorSelection.clr1_Btn.color, "Selected color 1");
                    WRITE_VEC3(color_checkComboList->panel.sections[0].elements[2].painterColorSelection.clr2_Btn.color, "Selected color 2");
                    WRITE_VEC3(color_checkComboList->panel.sections[0].elements[2].painterColorSelection.clr3_Btn.color, "Selected color 3");
                    WRITEBITS(color_checkComboList->panel.sections[0].elements[2].painterColorSelection.selectedI, int, "Selected color index");
                    WRITEBITS(color_checkComboList->panel.sections[0].elements[3].checkBox.clickState1, bool, "Enable Roughness Channel");
                    WRITEBITS(color_checkComboList->panel.sections[0].elements[4].rangeBar.value, float, "Roughness Channel Value");
                    WRITEBITS(color_checkComboList->panel.sections[0].elements[5].checkBox.clickState1, bool, "Enable Metallic Channel");
                    WRITEBITS(color_checkComboList->panel.sections[0].elements[6].rangeBar.value, float, "Metallic Channel Value");
                    WRITEBITS(color_checkComboList->panel.sections[0].elements[7].checkBox.clickState1, bool, "Enable NormalMap Channel");
                    WRITEBITS(color_checkComboList->panel.sections[0].elements[8].rangeBar.value, float, "NormalMap Channel Value");
                    WRITEBITS(color_checkComboList->panel.sections[0].elements[9].checkBox.clickState1, bool, "Enable HeightMap Channel");
                    WRITEBITS(color_checkComboList->panel.sections[0].elements[10].rangeBar.value, float, "HeightMap Channel Value");
                    WRITEBITS(color_checkComboList->panel.sections[0].elements[11].checkBox.clickState1, bool, "Enable AO Channel");
                    WRITEBITS(color_checkComboList->panel.sections[0].elements[12].rangeBar.value, float, "AO Channel Value");
                    WRITEBITS(color_checkComboList->panel.sections[0].elements[13].checkBox.clickState1, bool, "Use custom material");
                    FileHandler::writeMaterialData(wf, color_checkComboList->panel.sections[0].elements[14].button.material);
                    
                    // Mirror settings
                    WRITEBITS(mirror_checkComboList->panel.sections[0].elements[0].checkBox.clickState1, bool, "X axis");
                    WRITEBITS(mirror_checkComboList->panel.sections[0].elements[1].rangeBar.value, float, "X axis mirror offset");
                    WRITEBITS(mirror_checkComboList->panel.sections[0].elements[2].checkBox.clickState1, bool, "Y axis");
                    WRITEBITS(mirror_checkComboList->panel.sections[0].elements[3].rangeBar.value, float, "Y axis mirror offset");
                    WRITEBITS(mirror_checkComboList->panel.sections[0].elements[4].checkBox.clickState1, bool, "Z axis");
                    WRITEBITS(mirror_checkComboList->panel.sections[0].elements[5].rangeBar.value, float, "Z axis mirror offset");

                    FileHandler::writeBrushData(wf, *vector_stroke_brush, path);
                    WRITEBITS(vector_stroke_brush->properties.radius, float, "vector_stroke_brush->properties.radius");
                    WRITEBITS(vector_stroke_brush->properties.opacity, float, "vector_stroke_brush->properties.opacity");

                    vectorScene->write_data(wf);
                } 
            }

            int prim_count = model.meshes[meshI].face_selection_data.selectedPrimitiveIDs.size();
            WRITEBITS(prim_count, int, "prim count");
            for (size_t primI = 0; primI < prim_count; primI++)
            {
                WRITEBITS(model.meshes[meshI].face_selection_data.selectedPrimitiveIDs[primI], byte, "prim array");
            }
            
        }
    }

    return true;
}