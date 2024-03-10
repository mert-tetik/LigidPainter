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


#define FNC(function){ \
    if(!function)\
        return false;\
}\

static bool read_channel_alpha(std::ifstream& rf, std::string path, ChannelAlpha& channelAlpha, Mesh& mesh){
    
    READBITS(channelAlpha.alphaValue, float, "Channel alpha - Alpha value");
    
    if(!channelAlpha.alphaMap.readTextureData(rf, true, 2))
        return false;
    
    int resolution;
    READBITS(resolution, int, "Channel alpha - Resolution value");
    
    channelAlpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);
    channelAlpha.alphaMap.generateProceduralTexture(mesh, channelAlpha.alphaMapProceduralTxtr, resolution);

    return true;
}   

static bool read_texture_pixels(std::ifstream& rf, std::string path, Texture& texture){

    int x_res; 
    READBITS(x_res, int, "Texture resolution data - X");
    int y_res; 
    READBITS(y_res, int, "Texture resolution data - Y");

    char* pxs = new char[x_res * y_res * 4];
    for (size_t i = 0; i < x_res * y_res * 4; i++)
    {
        READBITS(pxs[i], char, "Texture pixel data");
    }

    texture = Texture(pxs, x_res, y_res);
}

bool FileHandler::readLGDMODELFile(std::string path, Model& model){
    
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
                    layer->get_type_specific_variable(nullptr, nullptr, channels);
                    
                    channels->albedo.readTextureData(rf, true, 2);
                    channels->roughness.readTextureData(rf, true, 2);
                    channels->metallic.readTextureData(rf, true, 2);
                    channels->normalMap.readTextureData(rf, true, 2);
                    channels->heightMap.readTextureData(rf, true, 2);
                    channels->ambientOcclusion.readTextureData(rf, true, 2);
                }
                else if(layer->layerType == "painting"){
                    FNC(read_texture_pixels(rf, path, layer->result.albedo))
                    FNC(read_texture_pixels(rf, path, layer->result.roughness))
                    FNC(read_texture_pixels(rf, path, layer->result.metallic))
                    FNC(read_texture_pixels(rf, path, layer->result.normalMap))
                    FNC(read_texture_pixels(rf, path, layer->result.heightMap))
                    FNC(read_texture_pixels(rf, path, layer->result.ambientOcclusion))
                }
                else if(layer->layerType == "material"){
                    Material* material;
                    layer->get_type_specific_variable(material, nullptr, nullptr);
                    
                    FileHandler::readMaterialData(rf, *material);

                }
                else if(layer->layerType == "vector"){
                    std::vector<VectorStroke3D>* strokes;
                    layer->get_type_specific_variable(nullptr, strokes, nullptr);

                    uint64_t strokesSize; 
                    READBITS(strokesSize, uint64_t, "Mesh strokes size");        
                    for (size_t strokeI = 0; strokeI < strokesSize; strokeI++)
                    {
                        VectorStroke3D stroke;
                        READBITS(stroke.startPoint.pos.x, float, "Vector stroke start position - X");
                        READBITS(stroke.startPoint.pos.y, float, "Vector stroke start position - Y");
                        READBITS(stroke.startPoint.pos.z, float, "Vector stroke start position - Z");
                        
                        READBITS(stroke.startPoint.normal.x, float, "Vector stroke start normal - X");
                        READBITS(stroke.startPoint.normal.y, float, "Vector stroke start normal - Y");
                        READBITS(stroke.startPoint.normal.z, float, "Vector stroke start normal - Z");
                        
                        READBITS(stroke.endPoint.pos.x, float, "Vector stroke end position - X");
                        READBITS(stroke.endPoint.pos.y, float, "Vector stroke end position - Y");
                        READBITS(stroke.endPoint.pos.z, float, "Vector stroke end position - Z");
                        
                        READBITS(stroke.endPoint.normal.x, float, "Vector stroke end normal - X");
                        READBITS(stroke.endPoint.normal.y, float, "Vector stroke end normal - Y");
                        READBITS(stroke.endPoint.normal.z, float, "Vector stroke end normal - Z");
                    
                        strokes->push_back(stroke);
                    }
                } 

                resMesh.layerScene.layers.push_back(layer);
            }

            model.meshes.push_back(resMesh);
        }
    }

    model.newModelAdded = true;
    model.generateDisplayingTexture();

    return true;
}