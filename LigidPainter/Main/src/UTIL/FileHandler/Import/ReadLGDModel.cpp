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

#define READ_VEC3(vec, loc)    READBITS(vec.x, float, loc);\
                                READBITS(vec.y, float, loc);\
                                READBITS(vec.z, float, loc);\

#define READSTR(str){size_t strSize;\
                    READBITS(strSize, size_t, str); \
                    for (size_t charI = 0; charI < strSize; charI++)\
                    {\
                        char c;\
                        READBITS(c, char, str);\
                        str.push_back(c);\
                    }}\


#define FNC(function){ \
    if(!function)\
        return false;\
}\

static bool read_channel_alpha(std::ifstream& rf, std::string path, ChannelAlpha& channelAlpha, Mesh& mesh){
    
    READBITS(channelAlpha.alphaValue, float, "Channel alpha - Alpha value");
    
    if(!channelAlpha.alphaMap.readTextureData(rf, true, 2, false))
        return false;
    
    int resolution;
    READBITS(resolution, int, "Channel alpha - Resolution value");
    
    channelAlpha.alphaMapProceduralTxtr.update((char*)nullptr, resolution, resolution);
    channelAlpha.alphaMap.generateProceduralTexture(mesh, channelAlpha.alphaMapProceduralTxtr, resolution);

    return true;
}   

bool FileHandler::readLGDMODELFile(std::string path, Model& model){
    
    UTIL::correctFolderDistinguishers(path);

    if(path.size()){
        std::ifstream rf(path, std::ios::in | std::ios::binary);

        if(!rf) {
            LGDLOG::start<< "ERROR WHILE READING MODEL FILE! Cannot open file : " << path << LGDLOG::end;
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

            // Layers
            uint64_t layersSize; 
            READBITS(layersSize, uint64_t, "Mesh layers size");
            for (size_t layerI = 0; layerI < layersSize; layerI++)
            {
                Layer* layer; 
                
                // Type of the layer
                std::string layerType;
                READSTR(layerType); 

                if(layerType == "texture"){
                    layer = new TextureLayer(1024);
                }
                else if(layerType == "painting"){
                    layer = new PaintingLayer(1024);
                }
                else if(layerType == "material"){
                    layer = new MaterialLayer(1024);
                }
                else if(layerType == "vector"){
                    layer = new VectorLayer(1024);
                }                 

                // Opacity / alpha values
                FNC(read_channel_alpha(rf, path, layer->alpha.general_Alpha, resMesh))
                FNC(read_channel_alpha(rf, path, layer->alpha.albedo_Alpha, resMesh))
                FNC(read_channel_alpha(rf, path, layer->alpha.roughness_Alpha, resMesh))
                FNC(read_channel_alpha(rf, path, layer->alpha.metallic_Alpha, resMesh))
                FNC(read_channel_alpha(rf, path, layer->alpha.normalMap_Alpha, resMesh))
                FNC(read_channel_alpha(rf, path, layer->alpha.heightMap_Alpha, resMesh))
                FNC(read_channel_alpha(rf, path, layer->alpha.ambientOcclusion_Alpha, resMesh))
            
                // Is layer hidden
                READBITS(layer->hiden, bool, "Layer - hidden");
                
                // Title of the layer
                READSTR(layer->title); 

                // Type specific data
                if(layer->layerType == "texture"){
                    MaterialChannels* channels;
                    layer->get_type_specific_variable(nullptr, nullptr, nullptr, nullptr, nullptr, &channels, nullptr);
                    
                    channels->albedo.readTextureData(rf, true, 2, false);
                    channels->roughness.readTextureData(rf, true, 2, false);
                    channels->metallic.readTextureData(rf, true, 2, false);
                    channels->normalMap.readTextureData(rf, true, 2, false);
                    channels->heightMap.readTextureData(rf, true, 2, false);
                    channels->ambientOcclusion.readTextureData(rf, true, 2, false);
                }
                else if(layer->layerType == "painting"){
                    FNC(layer->result.albedo.readPixelData(rf))
                    FNC(layer->result.roughness.readPixelData(rf))
                    FNC(layer->result.metallic.readPixelData(rf))
                    FNC(layer->result.normalMap.readPixelData(rf))
                    FNC(layer->result.heightMap.readPixelData(rf))
                    FNC(layer->result.ambientOcclusion.readPixelData(rf))
                }
                else if(layer->layerType == "material"){
                    Material* material;
                    layer->get_type_specific_variable(&material, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
                    
                    FileHandler::readMaterialData(rf, *material, nullptr);
                    material->updateMaterialDisplayingTexture(256, true, Camera(), 0, false, getMaterialDisplayerModel()->meshes[0], getMaterialDisplayerModel()->meshes[0].material_channels);

                }
                else if(layer->layerType == "vector"){
                    VectorScene* vectorScene;
                    Brush* vector_stroke_brush;
                    CheckComboList* color_checkComboList; 
                    CheckComboList* mirror_checkComboList;

                    layer->get_type_specific_variable(nullptr, &vectorScene, &vector_stroke_brush, &color_checkComboList, &mirror_checkComboList, nullptr, nullptr);
                    
                    // Color Settings 
                    READBITS(color_checkComboList->panel.sections[0].elements[1].checkBox.clickState1, bool, "Enable Albedo Channel");
                    READ_VEC3(color_checkComboList->panel.sections[0].elements[2].painterColorSelection.clr1_Btn.color, "Selected color 1");
                    READ_VEC3(color_checkComboList->panel.sections[0].elements[2].painterColorSelection.clr2_Btn.color, "Selected color 2");
                    READ_VEC3(color_checkComboList->panel.sections[0].elements[2].painterColorSelection.clr3_Btn.color, "Selected color 3");
                    READBITS(color_checkComboList->panel.sections[0].elements[3].checkBox.clickState1, bool, "Enable Roughness Channel");
                    READBITS(color_checkComboList->panel.sections[0].elements[4].rangeBar.value, float, "Roughness Channel Value");
                    READBITS(color_checkComboList->panel.sections[0].elements[5].checkBox.clickState1, bool, "Enable Metallic Channel");
                    READBITS(color_checkComboList->panel.sections[0].elements[6].rangeBar.value, float, "Metallic Channel Value");
                    READBITS(color_checkComboList->panel.sections[0].elements[7].checkBox.clickState1, bool, "Enable NormalMap Channel");
                    READBITS(color_checkComboList->panel.sections[0].elements[8].rangeBar.value, float, "NormalMap Channel Value");
                    READBITS(color_checkComboList->panel.sections[0].elements[9].checkBox.clickState1, bool, "Enable HeightMap Channel");
                    READBITS(color_checkComboList->panel.sections[0].elements[10].rangeBar.value, float, "HeightMap Channel Value");
                    READBITS(color_checkComboList->panel.sections[0].elements[11].checkBox.clickState1, bool, "Enable AO Channel");
                    READBITS(color_checkComboList->panel.sections[0].elements[12].rangeBar.value, float, "AO Channel Value");
                    READBITS(color_checkComboList->panel.sections[0].elements[13].checkBox.clickState1, bool, "Use custom material");
                    FileHandler::readMaterialData(rf, color_checkComboList->panel.sections[0].elements[14].button.material, {});
                    
                    // Mirror settings
                    READBITS(mirror_checkComboList->panel.sections[0].elements[0].checkBox.clickState1, bool, "X axis");
                    READBITS(mirror_checkComboList->panel.sections[0].elements[1].rangeBar.value, float, "X axis mirror offset");
                    READBITS(mirror_checkComboList->panel.sections[0].elements[2].checkBox.clickState1, bool, "Y axis");
                    READBITS(mirror_checkComboList->panel.sections[0].elements[3].rangeBar.value, float, "Y axis mirror offset");
                    READBITS(mirror_checkComboList->panel.sections[0].elements[4].checkBox.clickState1, bool, "Z axis");
                    READBITS(mirror_checkComboList->panel.sections[0].elements[5].rangeBar.value, float, "Z axis mirror offset");

                    FileHandler::readBrushData(rf, *vector_stroke_brush);

                    vectorScene->read_data(rf);
                } 

                resMesh.layerScene.layers.push_back(layer);
            }

            int prim_count;
            READBITS(prim_count, int, "prim count");
            std::vector<byte> selected_prims;
            for (size_t primI = 0; primI < prim_count; primI++)
            {
                byte data; 
                READBITS(data, byte, "prim array");
                selected_prims.push_back(data);
            }

            if(resMesh.face_selection_data.selectedPrimitiveIDs.size() == selected_prims.size()){
                resMesh.face_selection_data.selectedPrimitiveIDs = selected_prims;
            }

            model.meshes.push_back(resMesh);
        }
    }

    model.newModelAdded = true;
    model.generateDisplayingTexture();

    model.resize_meshes();

    return true;
}