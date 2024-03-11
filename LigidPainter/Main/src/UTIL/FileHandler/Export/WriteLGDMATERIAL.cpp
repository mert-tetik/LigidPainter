/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Write *.lgdmaterial file

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

#include "3D/ThreeD.hpp"
#include "GUI/GUI.hpp"

#define LGDMATERIAL_WRITEBITS(var, type, loc) if(!wf.write(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                    LGDLOG::start<< "ERROR : Writing lgdmaterial file. Failed to write at : " << loc << LGDLOG::end;\
                                    return false; \
                                }


bool FileHandler::writeMaterialData(std::ofstream& wf, Material& material){
    //!HEADER
    
    //Description 
    uint64_t h1 = 0x5A7D0C809A22F3B7; 
    uint64_t h2 = 0xE8F691BE0D45C23A; 
    uint64_t h3 = 0x3FD8A9657B101E8C; 
    LGDMATERIAL_WRITEBITS(h1, uint64_t, "Description 1st");
    LGDMATERIAL_WRITEBITS(h2, uint64_t, "Description 2nd");
    LGDMATERIAL_WRITEBITS(h3, uint64_t, "Description 3rd");
    
    //Version number
    uint32_t versionNumber = 2300; 
    LGDMATERIAL_WRITEBITS(versionNumber, uint32_t, "Version number");
    
    //ID
    LGDMATERIAL_WRITEBITS(material.uniqueID, int, "Material ID");

    //!Modifiers
    uint64_t materialModifierSize = material.materialModifiers.size();
    LGDMATERIAL_WRITEBITS(materialModifierSize, uint64_t, "Modifier size");

    for (size_t i = 0; i < materialModifierSize; i++)
    {
        // Modifier index
        LGDMATERIAL_WRITEBITS(material.materialModifiers[i].modifierIndex, int, "Modifier index");
        
        // Write mask texture data
        if(!material.materialModifiers[i].maskTexture.writeTextureData(wf))
            return false;


        // Writing the texture modifier
        if(material.materialModifiers[i].modifierIndex == TEXTURE_MATERIAL_MODIFIER){
            
            // ---- Channel Textures ----
            Texture albedoTexture = material.materialModifiers[i].sections[0].elements[1].button.texture;
            albedoTexture.writeTextureData(wf);
            Texture roughnessTexture = material.materialModifiers[i].sections[0].elements[2].button.texture;
            roughnessTexture.writeTextureData(wf);
            Texture metallicTexture = material.materialModifiers[i].sections[0].elements[3].button.texture;
            metallicTexture.writeTextureData(wf);
            Texture normalMapTexture = material.materialModifiers[i].sections[0].elements[4].button.texture;
            normalMapTexture.writeTextureData(wf);
            Texture heightMapTexture = material.materialModifiers[i].sections[0].elements[5].button.texture;
            heightMapTexture.writeTextureData(wf);
            Texture ambientOcclusionTexture = material.materialModifiers[i].sections[0].elements[6].button.texture;
            ambientOcclusionTexture.writeTextureData(wf);
        
            // ---- Channel Opacities ----
            float albedoOpacity = material.materialModifiers[i].sections[1].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(albedoOpacity, float, "Albedo Opacity");
            float roughnessOpacity = material.materialModifiers[i].sections[1].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(roughnessOpacity, float, "Roughness Opacity");
            float metallicOpacity = material.materialModifiers[i].sections[1].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(metallicOpacity, float, "Metallic Opacity");
            float normalMapOpacity = material.materialModifiers[i].sections[1].elements[3].rangeBar.value;
            LGDMATERIAL_WRITEBITS(normalMapOpacity, float, "Normal Map Opacity");
            float heightMapOpacity = material.materialModifiers[i].sections[1].elements[4].rangeBar.value;
            LGDMATERIAL_WRITEBITS(heightMapOpacity, float, "Height Map Opacity");
            float ambientOcclusionOpacity = material.materialModifiers[i].sections[1].elements[5].rangeBar.value;
            LGDMATERIAL_WRITEBITS(ambientOcclusionOpacity, float, "Ambient Occlusion Opacity");

            // ---- Depth Masking & Depth Properties ----
            float depthValue = material.materialModifiers[i].sections[2].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(depthValue, float, "Depth Value");
            int blurTheHeightMap = material.materialModifiers[i].sections[2].elements[1].checkBox.clickState1;
            LGDMATERIAL_WRITEBITS(blurTheHeightMap, int, "blurTheHeightMap");
            
            // ---- Filter ----
            Filter albedoFilter = material.materialModifiers[i].sections[3].elements[0].button.filter;
            albedoFilter.writeFilterData(wf);
            Texture albedoFilterMask = material.materialModifiers[i].sections[3].elements[1].button.texture;
            albedoFilterMask.writeTextureData(wf);
        }

        else if (material.materialModifiers[i].modifierIndex == SOLID_MATERIAL_MODIFIER){
            
            // ---- Channels ----
            glm::vec4 albedoColor = material.materialModifiers[i].sections[0].elements[0].button.color;
            float albedoOpacity = material.materialModifiers[i].sections[0].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(albedoColor.r, float, "Albedo Color");
            LGDMATERIAL_WRITEBITS(albedoColor.g, float, "Albedo Color");
            LGDMATERIAL_WRITEBITS(albedoColor.b, float, "Albedo Color");
            LGDMATERIAL_WRITEBITS(albedoOpacity, float, "Albedo Opacity");
            glm::vec4 roughnessColor = material.materialModifiers[i].sections[0].elements[2].button.color;
            float roughnessOpacity = material.materialModifiers[i].sections[0].elements[3].rangeBar.value;
            LGDMATERIAL_WRITEBITS(roughnessColor.r, float, "Roughness Color");
            LGDMATERIAL_WRITEBITS(roughnessColor.g, float, "Roughness Color");
            LGDMATERIAL_WRITEBITS(roughnessColor.b, float, "Roughness Color");
            LGDMATERIAL_WRITEBITS(roughnessOpacity, float, "Roughness Opacity");
            glm::vec4 metallicColor = material.materialModifiers[i].sections[0].elements[4].button.color;
            float metallicOpacity = material.materialModifiers[i].sections[0].elements[5].rangeBar.value;
            LGDMATERIAL_WRITEBITS(metallicColor.r, float, "Metallic Color");
            LGDMATERIAL_WRITEBITS(metallicColor.g, float, "Metallic Color");
            LGDMATERIAL_WRITEBITS(metallicColor.b, float, "Metallic Color");
            LGDMATERIAL_WRITEBITS(metallicOpacity, float, "Metallic Opacity");
            glm::vec4 normalMapColor = material.materialModifiers[i].sections[0].elements[6].button.color;
            float normalMapOpacity = material.materialModifiers[i].sections[0].elements[7].rangeBar.value;
            LGDMATERIAL_WRITEBITS(normalMapColor.r, float, "NormalMap Color");
            LGDMATERIAL_WRITEBITS(normalMapColor.g, float, "NormalMap Color");
            LGDMATERIAL_WRITEBITS(normalMapColor.b, float, "NormalMap Color");
            LGDMATERIAL_WRITEBITS(normalMapOpacity, float, "NormalMap Opacity");
            glm::vec4 heightMapColor = material.materialModifiers[i].sections[0].elements[8].button.color;
            float heightMapOpacity = material.materialModifiers[i].sections[0].elements[9].rangeBar.value;
            LGDMATERIAL_WRITEBITS(heightMapColor.r, float, "HeightMap Color");
            LGDMATERIAL_WRITEBITS(heightMapColor.g, float, "HeightMap Color");
            LGDMATERIAL_WRITEBITS(heightMapColor.b, float, "HeightMap Color");
            LGDMATERIAL_WRITEBITS(heightMapOpacity, float, "HeightMap Opacity");
            glm::vec4 ambientOcclusionColor = material.materialModifiers[i].sections[0].elements[10].button.color;
            float ambientOcclusionOpacity = material.materialModifiers[i].sections[0].elements[11].rangeBar.value;
            LGDMATERIAL_WRITEBITS(ambientOcclusionColor.r, float, "AmbientOcclusion Color");
            LGDMATERIAL_WRITEBITS(ambientOcclusionColor.g, float, "AmbientOcclusion Color");
            LGDMATERIAL_WRITEBITS(ambientOcclusionColor.b, float, "AmbientOcclusion Color");
            LGDMATERIAL_WRITEBITS(ambientOcclusionOpacity, float, "AmbientOcclusion Opacity");

            // ---- Depth Masking & Depth Properties ----
            float depthValue = material.materialModifiers[i].sections[1].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(depthValue, float, "Depth Value");
            int blurTheHeightMap = material.materialModifiers[i].sections[1].elements[1].checkBox.clickState1;
            LGDMATERIAL_WRITEBITS(blurTheHeightMap, int, "blurTheHeightMap");
            
            // ---- Filter ----
            Filter albedoFilter = material.materialModifiers[i].sections[2].elements[0].button.filter;
            albedoFilter.writeFilterData(wf);
            Texture albedoFilterMask = material.materialModifiers[i].sections[2].elements[1].button.texture;
            albedoFilterMask.writeTextureData(wf);
        }

        else if (material.materialModifiers[i].modifierIndex == LIQUID_MATERIAL_MODIFIER){
            
            // ---- Scaling ----
            float scaleVal = material.materialModifiers[i].sections[0].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(scaleVal, float, "scaleVal");
            float scaleYAxisVal = material.materialModifiers[i].sections[0].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(scaleYAxisVal, float, "scaleYAxisVal");

            // ---- Noise ----
            float octaves = material.materialModifiers[i].sections[1].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(octaves, float, "octaves");
            float fbmXOffset = material.materialModifiers[i].sections[1].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(fbmXOffset, float, "fbmXOffset");
            float fbmYOffset = material.materialModifiers[i].sections[1].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(fbmYOffset, float, "fbmYOffset");
            float fbmXScale = material.materialModifiers[i].sections[1].elements[3].rangeBar.value;
            LGDMATERIAL_WRITEBITS(fbmXScale, float, "fbmXScale");
            float fbmYScale = material.materialModifiers[i].sections[1].elements[4].rangeBar.value;
            LGDMATERIAL_WRITEBITS(fbmYScale, float, "fbmYScale");
            float roughness = material.materialModifiers[i].sections[1].elements[5].rangeBar.value;
            LGDMATERIAL_WRITEBITS(roughness, float, "roughness");
            float frequencyAcceleration = material.materialModifiers[i].sections[1].elements[6].rangeBar.value;
            LGDMATERIAL_WRITEBITS(frequencyAcceleration, float, "frequencyAcceleration");
            float fbmTotal = material.materialModifiers[i].sections[1].elements[7].rangeBar.value;
            LGDMATERIAL_WRITEBITS(fbmTotal, float, "fbmTotal");
            float seed = material.materialModifiers[i].sections[1].elements[8].rangeBar.value;              
            LGDMATERIAL_WRITEBITS(seed, float, "seed");

            // ---- General ----
            float noiseInfluence = material.materialModifiers[i].sections[2].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(noiseInfluence, float, "noiseInfluence");
            float positionInfluence = material.materialModifiers[i].sections[2].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(positionInfluence, float, "positionInfluence");

            // ---- Colors ----
            glm::vec4 color1 = material.materialModifiers[i].sections[3].elements[0].button.color;
            LGDMATERIAL_WRITEBITS(color1.r, float, "color1");
            LGDMATERIAL_WRITEBITS(color1.g, float, "color1");
            LGDMATERIAL_WRITEBITS(color1.b, float, "color1");
            glm::vec4 color2 = material.materialModifiers[i].sections[3].elements[1].button.color;
            LGDMATERIAL_WRITEBITS(color2.r, float, "color2");
            LGDMATERIAL_WRITEBITS(color2.g, float, "color2");
            LGDMATERIAL_WRITEBITS(color2.b, float, "color2");
            glm::vec4 color3 = material.materialModifiers[i].sections[3].elements[2].button.color;
            LGDMATERIAL_WRITEBITS(color3.r, float, "color3");
            LGDMATERIAL_WRITEBITS(color3.g, float, "color3");
            LGDMATERIAL_WRITEBITS(color3.b, float, "color3");
            glm::vec4 color4 = material.materialModifiers[i].sections[3].elements[3].button.color;
            LGDMATERIAL_WRITEBITS(color4.r, float, "color4");
            LGDMATERIAL_WRITEBITS(color4.g, float, "color4");
            LGDMATERIAL_WRITEBITS(color4.b, float, "color4");

            // ---- Element Properties ----
            float wetness = material.materialModifiers[i].sections[4].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(wetness, float, "wetness");
            float metallic = material.materialModifiers[i].sections[4].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(metallic, float, "metallic");
            float height = material.materialModifiers[i].sections[4].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(height, float, "height");
            float ambientOcclusion = material.materialModifiers[i].sections[4].elements[3].rangeBar.value;
            LGDMATERIAL_WRITEBITS(ambientOcclusion, float, "ambientOcclusion");

            // ---- Channel Opacities ----
            float albedoOpacity = material.materialModifiers[i].sections[5].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(albedoOpacity, float, "Albedo Opacity");
            float roughnessOpacity = material.materialModifiers[i].sections[5].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(roughnessOpacity, float, "Roughness Opacity");
            float metallicOpacity = material.materialModifiers[i].sections[5].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(metallicOpacity, float, "Metallic Opacity");
            float normalMapOpacity = material.materialModifiers[i].sections[5].elements[3].rangeBar.value;
            LGDMATERIAL_WRITEBITS(normalMapOpacity, float, "Normal Map Opacity");
            float heightMapOpacity = material.materialModifiers[i].sections[5].elements[4].rangeBar.value;
            LGDMATERIAL_WRITEBITS(heightMapOpacity, float, "Height Map Opacity");
            float ambientOcclusionOpacity = material.materialModifiers[i].sections[5].elements[5].rangeBar.value;
            LGDMATERIAL_WRITEBITS(ambientOcclusionOpacity, float, "Ambient Occlusion Opacity");

            // ---- Depth Masking & Depth Properties ----
            float depthValue = material.materialModifiers[i].sections[6].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(depthValue, float, "Depth Value");
            int blurTheHeightMap = material.materialModifiers[i].sections[6].elements[1].checkBox.clickState1;
            LGDMATERIAL_WRITEBITS(blurTheHeightMap, int, "blurTheHeightMap");
            
            // ---- Filter ----
            Filter albedoFilter = material.materialModifiers[i].sections[7].elements[0].button.filter;
            albedoFilter.writeFilterData(wf);
            Texture albedoFilterMask = material.materialModifiers[i].sections[7].elements[1].button.texture;
            albedoFilterMask.writeTextureData(wf);
        }

        else if (material.materialModifiers[i].modifierIndex == MOSS_MATERIAL_MODIFIER){
            
            // ---- Color ----
            glm::vec4 mossColorBack = material.materialModifiers[i].sections[0].elements[0].button.color;
            LGDMATERIAL_WRITEBITS(mossColorBack.r, float, "mossColorBack");
            LGDMATERIAL_WRITEBITS(mossColorBack.g, float, "mossColorBack");
            LGDMATERIAL_WRITEBITS(mossColorBack.b, float, "mossColorBack");
            glm::vec4 mossColorFront = material.materialModifiers[i].sections[0].elements[1].button.color;
            LGDMATERIAL_WRITEBITS(mossColorFront.r, float, "mossColorFront");
            LGDMATERIAL_WRITEBITS(mossColorFront.g, float, "mossColorFront");
            LGDMATERIAL_WRITEBITS(mossColorFront.b, float, "mossColorFront");
            glm::vec4 dirtColor = material.materialModifiers[i].sections[0].elements[2].button.color;
            LGDMATERIAL_WRITEBITS(dirtColor.r, float, "dirtColor");
            LGDMATERIAL_WRITEBITS(dirtColor.g, float, "dirtColor");
            LGDMATERIAL_WRITEBITS(dirtColor.b, float, "dirtColor");
            
            // ---- Droplets ----
            float dropletsCount = material.materialModifiers[i].sections[1].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(dropletsCount, float, "dropletsCount");
            float dropletsOpacityJitter = material.materialModifiers[i].sections[1].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(dropletsOpacityJitter, float, "dropletsOpacityJitter");
            float dropletsSize = material.materialModifiers[i].sections[1].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(dropletsSize, float, "dropletsSize");

            // ---- Front Layer ----
            float frontLayerStrength = material.materialModifiers[i].sections[2].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(frontLayerStrength, float, "frontLayerStrength");
            float frontLayerScale = material.materialModifiers[i].sections[2].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(frontLayerScale, float, "frontLayerScale");

            // ---- Lighting ----
            float lightStrength = material.materialModifiers[i].sections[3].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(lightStrength, float, "lightStrength");
            float darkeningStrength = material.materialModifiers[i].sections[3].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(darkeningStrength, float, "darkeningStrength");

            // ---- Noise ----
            float noiseStrength = material.materialModifiers[i].sections[4].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(noiseStrength, float, "noiseStrength");

            // ---- Moss Properties ----
            float scale = material.materialModifiers[i].sections[5].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(scale, float, "scale");
            
            // ---- Element Properties ----
            float wetness = material.materialModifiers[i].sections[6].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(wetness, float, "wetness");
            float metallic = material.materialModifiers[i].sections[6].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(metallic, float, "metallic");
            float height = material.materialModifiers[i].sections[6].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(height, float, "height");

            // ---- Channel Opacities ----
            float albedoOpacity = material.materialModifiers[i].sections[7].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(albedoOpacity, float, "Albedo Opacity");
            float roughnessOpacity = material.materialModifiers[i].sections[7].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(roughnessOpacity, float, "Roughness Opacity");
            float metallicOpacity = material.materialModifiers[i].sections[7].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(metallicOpacity, float, "Metallic Opacity");
            float normalMapOpacity = material.materialModifiers[i].sections[7].elements[3].rangeBar.value;
            LGDMATERIAL_WRITEBITS(normalMapOpacity, float, "Normal Map Opacity");
            float heightMapOpacity = material.materialModifiers[i].sections[7].elements[4].rangeBar.value;
            LGDMATERIAL_WRITEBITS(heightMapOpacity, float, "Height Map Opacity");
            float ambientOcclusionOpacity = material.materialModifiers[i].sections[7].elements[5].rangeBar.value;
            LGDMATERIAL_WRITEBITS(ambientOcclusionOpacity, float, "Ambient Occlusion Opacity");

            // ---- Depth Masking & Depth Properties ----
            float depthValue = material.materialModifiers[i].sections[8].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(depthValue, float, "Depth Value");
            int blurTheHeightMap = material.materialModifiers[i].sections[8].elements[1].checkBox.clickState1;
            LGDMATERIAL_WRITEBITS(blurTheHeightMap, int, "blurTheHeightMap");
            
            // ---- Filter ----
            Filter albedoFilter = material.materialModifiers[i].sections[9].elements[0].button.filter;
            albedoFilter.writeFilterData(wf);
            Texture albedoFilterMask = material.materialModifiers[i].sections[9].elements[1].button.texture;
            albedoFilterMask.writeTextureData(wf);
        }

        else if (material.materialModifiers[i].modifierIndex == RUST_MATERIAL_MODIFIER){
            
            // ---- Colors ----
            glm::vec4 color1 = material.materialModifiers[i].sections[0].elements[0].button.color;
            LGDMATERIAL_WRITEBITS(color1.r, float, "color1");
            LGDMATERIAL_WRITEBITS(color1.g, float, "color1");
            LGDMATERIAL_WRITEBITS(color1.b, float, "color1");
            glm::vec4 color2 = material.materialModifiers[i].sections[0].elements[1].button.color;
            LGDMATERIAL_WRITEBITS(color2.r, float, "color2");
            LGDMATERIAL_WRITEBITS(color2.g, float, "color2");
            LGDMATERIAL_WRITEBITS(color2.b, float, "color2");
            glm::vec4 color3 = material.materialModifiers[i].sections[0].elements[2].button.color;
            LGDMATERIAL_WRITEBITS(color3.r, float, "color3");
            LGDMATERIAL_WRITEBITS(color3.g, float, "color3");
            LGDMATERIAL_WRITEBITS(color3.b, float, "color3");
            glm::vec4 color4 = material.materialModifiers[i].sections[0].elements[2].button.color;
            LGDMATERIAL_WRITEBITS(color4.r, float, "color4");
            LGDMATERIAL_WRITEBITS(color4.g, float, "color4");
            LGDMATERIAL_WRITEBITS(color4.b, float, "color4");

            // ---- Properties ----
            float scale = material.materialModifiers[i].sections[1].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(scale, float, "scale");

            // ---- Perlin Properties ----
            float firstOctave = material.materialModifiers[i].sections[2].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(firstOctave, float, "firstOctave");
            float octaves = material.materialModifiers[i].sections[2].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(octaves, float, "octaves");
            float persistence = material.materialModifiers[i].sections[2].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(persistence, float, "persistence");

            // ---- FBM Properties ----
            float fbmOctaves = material.materialModifiers[i].sections[3].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(fbmOctaves, float, "fbmOctaves");
            float fbmRoughness = material.materialModifiers[i].sections[3].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(fbmRoughness, float, "fbmRoughness");

            // ---- Rust Properties ----
            float rustRadius = material.materialModifiers[i].sections[4].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(rustRadius, float, "rustRadius");
            float batteringStrength = material.materialModifiers[i].sections[4].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(batteringStrength, float, "batteringStrength");

            // ---- Noise Properties ----
            float noiseStrength = material.materialModifiers[i].sections[5].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(noiseStrength, float, "noiseStrength");

            // ---- Element Properties ----
            float wetness = material.materialModifiers[i].sections[6].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(wetness, float, "wetness");
            float metallic = material.materialModifiers[i].sections[6].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(metallic, float, "metallic");
            float height = material.materialModifiers[i].sections[6].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(height, float, "height");

            // ---- Channel Opacities ----
            float albedoOpacity = material.materialModifiers[i].sections[7].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(albedoOpacity, float, "Albedo Opacity");
            float roughnessOpacity = material.materialModifiers[i].sections[7].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(roughnessOpacity, float, "Roughness Opacity");
            float metallicOpacity = material.materialModifiers[i].sections[7].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(metallicOpacity, float, "Metallic Opacity");
            float normalMapOpacity = material.materialModifiers[i].sections[7].elements[3].rangeBar.value;
            LGDMATERIAL_WRITEBITS(normalMapOpacity, float, "Normal Map Opacity");
            float heightMapOpacity = material.materialModifiers[i].sections[7].elements[4].rangeBar.value;
            LGDMATERIAL_WRITEBITS(heightMapOpacity, float, "Height Map Opacity");
            float ambientOcclusionOpacity = material.materialModifiers[i].sections[7].elements[5].rangeBar.value;
            LGDMATERIAL_WRITEBITS(ambientOcclusionOpacity, float, "Ambient Occlusion Opacity");

            // ---- Depth Masking & Depth Properties ----
            float depthValue = material.materialModifiers[i].sections[8].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(depthValue, float, "Depth Value");
            int blurTheHeightMap = material.materialModifiers[i].sections[8].elements[1].checkBox.clickState1;
            LGDMATERIAL_WRITEBITS(blurTheHeightMap, int, "blurTheHeightMap");
            
            // ---- Filter ----
            Filter albedoFilter = material.materialModifiers[i].sections[9].elements[0].button.filter;
            albedoFilter.writeFilterData(wf);
            Texture albedoFilterMask = material.materialModifiers[i].sections[9].elements[1].button.texture;
            albedoFilterMask.writeTextureData(wf);
        }

        else if (material.materialModifiers[i].modifierIndex == SKIN_MATERIAL_MODIFIER){
            
            // ---- Droplets ----
            float dropletsCount = material.materialModifiers[i].sections[0].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(dropletsCount, float, "dropletsCount");
            float dropletsOpacityJitter = material.materialModifiers[i].sections[0].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(dropletsOpacityJitter, float, "dropletsOpacityJitter");
            float dropletsSize = material.materialModifiers[i].sections[0].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(dropletsSize, float, "dropletsSize");
            
            // ---- Veins ----
            float veinsScale = material.materialModifiers[i].sections[1].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(veinsScale, float, "veinsScale");
            float veinsStrength = material.materialModifiers[i].sections[1].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(veinsStrength, float, "veinsStrength");
            
            // ---- Blushing ----
            float blushingStrength = material.materialModifiers[i].sections[2].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(blushingStrength, float, "blushingStrength");
            
            // ---- Skin Prints ----
            float skinPrintsScale = material.materialModifiers[i].sections[3].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(skinPrintsScale, float, "skinPrintsScale");
            float skinPrintsStrength = material.materialModifiers[i].sections[3].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(skinPrintsStrength, float, "skinPrintsStrength");
            
            // ---- Noise ----
            float noiseStrength = material.materialModifiers[i].sections[4].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(noiseStrength, float, "noiseStrength");
            
            // ---- Skin Properties ----
            glm::vec4 skinColor = material.materialModifiers[i].sections[5].elements[0].button.color;
            LGDMATERIAL_WRITEBITS(skinColor.r, float, "skinColor");
            LGDMATERIAL_WRITEBITS(skinColor.g, float, "skinColor");
            LGDMATERIAL_WRITEBITS(skinColor.b, float, "skinColor");
            glm::vec4 veinColor = material.materialModifiers[i].sections[5].elements[1].button.color;
            LGDMATERIAL_WRITEBITS(veinColor.r, float, "veinColor");
            LGDMATERIAL_WRITEBITS(veinColor.g, float, "veinColor");
            LGDMATERIAL_WRITEBITS(veinColor.b, float, "veinColor");
            float skinScale = material.materialModifiers[i].sections[5].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(skinScale, float, "skinScale");
            float skinWetness = material.materialModifiers[i].sections[5].elements[3].rangeBar.value;
            LGDMATERIAL_WRITEBITS(skinWetness, float, "skinWetness");
            float skinMetallic = material.materialModifiers[i].sections[5].elements[4].rangeBar.value;
            LGDMATERIAL_WRITEBITS(skinMetallic, float, "skinMetallic");
            float skinHeight = material.materialModifiers[i].sections[5].elements[5].rangeBar.value;
            LGDMATERIAL_WRITEBITS(skinHeight, float, "skinHeight");
            float skinAmbientOcclusion = material.materialModifiers[i].sections[5].elements[6].rangeBar.value;
            LGDMATERIAL_WRITEBITS(skinAmbientOcclusion, float, "skinAmbientOcclusion");
            
            // ---- Channel Opacities ----
            float albedoOpacity = material.materialModifiers[i].sections[6].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(albedoOpacity, float, "Albedo Opacity");
            float roughnessOpacity = material.materialModifiers[i].sections[6].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(roughnessOpacity, float, "Roughness Opacity");
            float metallicOpacity = material.materialModifiers[i].sections[6].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(metallicOpacity, float, "Metallic Opacity");
            float normalMapOpacity = material.materialModifiers[i].sections[6].elements[3].rangeBar.value;
            LGDMATERIAL_WRITEBITS(normalMapOpacity, float, "Normal Map Opacity");
            float heightMapOpacity = material.materialModifiers[i].sections[6].elements[4].rangeBar.value;
            LGDMATERIAL_WRITEBITS(heightMapOpacity, float, "Height Map Opacity");
            float ambientOcclusionOpacity = material.materialModifiers[i].sections[6].elements[5].rangeBar.value;
            LGDMATERIAL_WRITEBITS(ambientOcclusionOpacity, float, "Ambient Occlusion Opacity");

            // ---- Depth Masking & Depth Properties ----
            float depthValue = material.materialModifiers[i].sections[7].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(depthValue, float, "Depth Value");
            int blurTheHeightMap = material.materialModifiers[i].sections[7].elements[1].checkBox.clickState1;
            LGDMATERIAL_WRITEBITS(blurTheHeightMap, int, "blurTheHeightMap");
            
            // ---- Filter ----
            Filter albedoFilter = material.materialModifiers[i].sections[8].elements[0].button.filter;
            albedoFilter.writeFilterData(wf);
            Texture albedoFilterMask = material.materialModifiers[i].sections[8].elements[1].button.texture;
            albedoFilterMask.writeTextureData(wf);
        }

        else if (material.materialModifiers[i].modifierIndex == WOODEN_MATERIAL_MODIFIER){
            
            // ---- Colors ----
            glm::vec4 color1 = material.materialModifiers[i].sections[0].elements[0].button.color;
            LGDMATERIAL_WRITEBITS(color1.r, float, "color1");
            LGDMATERIAL_WRITEBITS(color1.g, float, "color1");
            LGDMATERIAL_WRITEBITS(color1.b, float, "color1");
            glm::vec4 color2 = material.materialModifiers[i].sections[0].elements[1].button.color;
            LGDMATERIAL_WRITEBITS(color2.r, float, "color2");
            LGDMATERIAL_WRITEBITS(color2.g, float, "color2");
            LGDMATERIAL_WRITEBITS(color2.b, float, "color2");
            glm::vec4 color3 = material.materialModifiers[i].sections[0].elements[2].button.color;
            LGDMATERIAL_WRITEBITS(color3.r, float, "color3");
            LGDMATERIAL_WRITEBITS(color3.g, float, "color3");
            LGDMATERIAL_WRITEBITS(color3.b, float, "color3");

            // ---- Properties ----
            float scale = material.materialModifiers[i].sections[1].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(scale, float, "scale");
            float noiseOffset = material.materialModifiers[i].sections[1].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(noiseOffset, float, "noiseOffset");
            float noiseSeed = material.materialModifiers[i].sections[1].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(noiseSeed, float, "noiseSeed");

            // ---- Perlin Properties ----
            float maxOctaves = material.materialModifiers[i].sections[2].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(maxOctaves, float, "maxOctaves");
            float persistance = material.materialModifiers[i].sections[2].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(persistance, float, "persistance");

            // ---- Musgrave Properties ----
            float musgraveLacunarity = material.materialModifiers[i].sections[3].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(musgraveLacunarity, float, "musgraveLacunarity");
            float musgraveStrength = material.materialModifiers[i].sections[3].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(musgraveStrength, float, "musgraveStrength");
            float musgraveNoise = material.materialModifiers[i].sections[3].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(musgraveNoise, float, "musgraveNoise");

            // ---- Base ----
            float noiseStrength = material.materialModifiers[i].sections[4].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(noiseStrength, float, "noiseStrength");
            float colorSaturation = material.materialModifiers[i].sections[4].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(colorSaturation, float, "colorSaturation");

            // ---- FBM Properties ----
            float fbmFrequency = material.materialModifiers[i].sections[5].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(fbmFrequency, float, "fbmFrequency");
            
            // ---- Element Properties ----
            float woodGamma = material.materialModifiers[i].sections[6].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(woodGamma, float, "woodGamma");
            float shininess = material.materialModifiers[i].sections[6].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(shininess, float, "shininess");
            float metallic = material.materialModifiers[i].sections[6].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(metallic, float, "metallic");
            float height = material.materialModifiers[i].sections[6].elements[3].rangeBar.value;
            LGDMATERIAL_WRITEBITS(height, float, "height");

            // ---- Channel Opacities ----
            float albedoOpacity = material.materialModifiers[i].sections[7].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(albedoOpacity, float, "Albedo Opacity");
            float roughnessOpacity = material.materialModifiers[i].sections[7].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(roughnessOpacity, float, "Roughness Opacity");
            float metallicOpacity = material.materialModifiers[i].sections[7].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(metallicOpacity, float, "Metallic Opacity");
            float normalMapOpacity = material.materialModifiers[i].sections[7].elements[3].rangeBar.value;
            LGDMATERIAL_WRITEBITS(normalMapOpacity, float, "Normal Map Opacity");
            float heightMapOpacity = material.materialModifiers[i].sections[7].elements[4].rangeBar.value;
            LGDMATERIAL_WRITEBITS(heightMapOpacity, float, "Height Map Opacity");
            float ambientOcclusionOpacity = material.materialModifiers[i].sections[7].elements[5].rangeBar.value;
            LGDMATERIAL_WRITEBITS(ambientOcclusionOpacity, float, "Ambient Occlusion Opacity");

            // ---- Depth Masking & Depth Properties ----
            float depthValue = material.materialModifiers[i].sections[8].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(depthValue, float, "Depth Value");
            int blurTheHeightMap = material.materialModifiers[i].sections[8].elements[1].checkBox.clickState1;
            LGDMATERIAL_WRITEBITS(blurTheHeightMap, int, "blurTheHeightMap");
            
            // ---- Filter ----
            Filter albedoFilter = material.materialModifiers[i].sections[9].elements[0].button.filter;
            albedoFilter.writeFilterData(wf);
            Texture albedoFilterMask = material.materialModifiers[i].sections[9].elements[1].button.texture;
            albedoFilterMask.writeTextureData(wf);
        }

        else if (material.materialModifiers[i].modifierIndex == ASPHALT_MATERIAL_MODIFIER){

            // ---- Colors ----
            glm::vec4 color1 = material.materialModifiers[i].sections[0].elements[0].button.color;
            LGDMATERIAL_WRITEBITS(color1.r, float, "color1");
            LGDMATERIAL_WRITEBITS(color1.g, float, "color1");
            LGDMATERIAL_WRITEBITS(color1.b, float, "color1");
            glm::vec4 color2 = material.materialModifiers[i].sections[0].elements[1].button.color;
            LGDMATERIAL_WRITEBITS(color2.r, float, "color2");
            LGDMATERIAL_WRITEBITS(color2.g, float, "color2");
            LGDMATERIAL_WRITEBITS(color2.b, float, "color2");
            
            // ---- Dirt ----
            float scale = material.materialModifiers[i].sections[1].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(scale, float, "scale");
            float strength = material.materialModifiers[i].sections[1].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(strength, float, "strength");
            
            // ---- 2nd Color ----
            float noiseScale = material.materialModifiers[i].sections[2].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(noiseScale, float, "noiseScale");
            float sndClrnoiseStrength = material.materialModifiers[i].sections[2].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(sndClrnoiseStrength, float, "sndClrnoiseStrength");
            
            // ---- Noise ----
            float noiseStrength = material.materialModifiers[i].sections[3].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(noiseStrength, float, "noiseStrength");

            // ---- Element Properties ----
            float wetness = material.materialModifiers[i].sections[4].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(wetness, float, "wetness");
            float metallic = material.materialModifiers[i].sections[4].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(metallic, float, "metallic");
            float height = material.materialModifiers[i].sections[4].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(height, float, "height");

            // ---- Perlin Properties ----
            float firstOctave = material.materialModifiers[i].sections[5].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(firstOctave, float, "firstOctave");
            float octaves = material.materialModifiers[i].sections[5].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(octaves, float, "octaves");
            float persistence = material.materialModifiers[i].sections[5].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(persistence, float, "persistence");

            // ---- Channel Opacities ----
            float albedoOpacity = material.materialModifiers[i].sections[6].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(albedoOpacity, float, "Albedo Opacity");
            float roughnessOpacity = material.materialModifiers[i].sections[6].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(roughnessOpacity, float, "Roughness Opacity");
            float metallicOpacity = material.materialModifiers[i].sections[6].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(metallicOpacity, float, "Metallic Opacity");
            float normalMapOpacity = material.materialModifiers[i].sections[6].elements[3].rangeBar.value;
            LGDMATERIAL_WRITEBITS(normalMapOpacity, float, "Normal Map Opacity");
            float heightMapOpacity = material.materialModifiers[i].sections[6].elements[4].rangeBar.value;
            LGDMATERIAL_WRITEBITS(heightMapOpacity, float, "Height Map Opacity");
            float ambientOcclusionOpacity = material.materialModifiers[i].sections[6].elements[5].rangeBar.value;
            LGDMATERIAL_WRITEBITS(ambientOcclusionOpacity, float, "Ambient Occlusion Opacity");

            // ---- Depth Masking & Depth Properties ----
            float depthValue = material.materialModifiers[i].sections[7].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(depthValue, float, "Depth Value");
            int blurTheHeightMap = material.materialModifiers[i].sections[7].elements[1].checkBox.clickState1;
            LGDMATERIAL_WRITEBITS(blurTheHeightMap, int, "blurTheHeightMap");
            
            // ---- Filter ----
            Filter albedoFilter = material.materialModifiers[i].sections[8].elements[0].button.filter;
            albedoFilter.writeFilterData(wf);
            Texture albedoFilterMask = material.materialModifiers[i].sections[8].elements[1].button.texture;
            albedoFilterMask.writeTextureData(wf);
        
        }

        else if (material.materialModifiers[i].modifierIndex == DUST_MATERIAL_MODIFIER){
            
            // ---- Noise ----
            float size = material.materialModifiers[i].sections[0].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(size, float, "size");
            float offsetIntensity = material.materialModifiers[i].sections[0].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(offsetIntensity, float, "offsetIntensity");
            float rotation = material.materialModifiers[i].sections[0].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(rotation, float, "rotation");
            float brightness = material.materialModifiers[i].sections[0].elements[3].rangeBar.value;
            LGDMATERIAL_WRITEBITS(brightness, float, "brightness");
            
            // ---- Blur ----
            float intensity = material.materialModifiers[i].sections[1].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(intensity, float, "intensity");
            float direction = material.materialModifiers[i].sections[1].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(direction, float, "direction");
            
            // ---- Scratches ----
            float wavyness = material.materialModifiers[i].sections[2].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(wavyness, float, "wavyness");
            float scale = material.materialModifiers[i].sections[2].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(scale, float, "scale");
            float baseFrequency = material.materialModifiers[i].sections[2].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(baseFrequency, float, "baseFrequency");
            float frequencyStep = material.materialModifiers[i].sections[2].elements[3].rangeBar.value;
            LGDMATERIAL_WRITEBITS(frequencyStep, float, "frequencyStep");

            // ---- Droplets ----
            float count = material.materialModifiers[i].sections[3].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(count, float, "count");
            float opacityJitter = material.materialModifiers[i].sections[3].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(opacityJitter, float, "opacityJitter");
            float dropletsSize = material.materialModifiers[i].sections[3].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(dropletsSize, float, "dropletsSize");
            
            // ---- Colors ----
            glm::vec4 color1 = material.materialModifiers[i].sections[4].elements[0].button.color;
            LGDMATERIAL_WRITEBITS(color1.r, float, "color1");
            LGDMATERIAL_WRITEBITS(color1.g, float, "color1");
            LGDMATERIAL_WRITEBITS(color1.b, float, "color1");
            glm::vec4 color2 = material.materialModifiers[i].sections[4].elements[1].button.color;
            LGDMATERIAL_WRITEBITS(color2.r, float, "color2");
            LGDMATERIAL_WRITEBITS(color2.g, float, "color2");
            LGDMATERIAL_WRITEBITS(color2.b, float, "color2");
            glm::vec4 color3 = material.materialModifiers[i].sections[4].elements[2].button.color;
            LGDMATERIAL_WRITEBITS(color3.r, float, "color3");
            LGDMATERIAL_WRITEBITS(color3.g, float, "color3");
            LGDMATERIAL_WRITEBITS(color3.b, float, "color3");

            // ---- Element Properties ----
            float wetness = material.materialModifiers[i].sections[5].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(wetness, float, "wetness");
            float metallic = material.materialModifiers[i].sections[5].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(metallic, float, "metallic");
            float height = material.materialModifiers[i].sections[5].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(height, float, "height");

            // ---- Channel Opacities ----
            float albedoOpacity = material.materialModifiers[i].sections[6].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(albedoOpacity, float, "Albedo Opacity");
            float roughnessOpacity = material.materialModifiers[i].sections[6].elements[1].rangeBar.value;
            LGDMATERIAL_WRITEBITS(roughnessOpacity, float, "Roughness Opacity");
            float metallicOpacity = material.materialModifiers[i].sections[6].elements[2].rangeBar.value;
            LGDMATERIAL_WRITEBITS(metallicOpacity, float, "Metallic Opacity");
            float normalMapOpacity = material.materialModifiers[i].sections[6].elements[3].rangeBar.value;
            LGDMATERIAL_WRITEBITS(normalMapOpacity, float, "Normal Map Opacity");
            float heightMapOpacity = material.materialModifiers[i].sections[6].elements[4].rangeBar.value;
            LGDMATERIAL_WRITEBITS(heightMapOpacity, float, "Height Map Opacity");
            float ambientOcclusionOpacity = material.materialModifiers[i].sections[6].elements[5].rangeBar.value;
            LGDMATERIAL_WRITEBITS(ambientOcclusionOpacity, float, "Ambient Occlusion Opacity");

            // ---- Depth Masking & Depth Properties ----
            float depthValue = material.materialModifiers[i].sections[7].elements[0].rangeBar.value;
            LGDMATERIAL_WRITEBITS(depthValue, float, "Depth Value");
            int blurTheHeightMap = material.materialModifiers[i].sections[7].elements[1].checkBox.clickState1;
            LGDMATERIAL_WRITEBITS(blurTheHeightMap, int, "blurTheHeightMap");
            
            // ---- Filter ----
            Filter albedoFilter = material.materialModifiers[i].sections[8].elements[0].button.filter;
            albedoFilter.writeFilterData(wf);
            Texture albedoFilterMask = material.materialModifiers[i].sections[8].elements[1].button.texture;
            albedoFilterMask.writeTextureData(wf);
        }
    }        


    // Shortcuts
    int matShortcutSize = material.materialShortcuts.size();
    LGDMATERIAL_WRITEBITS(matShortcutSize, int, "Material Shortcut - Material Shortcut Size");
    for (size_t i = 0; i < matShortcutSize; i++)
    {
        LGDMATERIAL_WRITEBITS(material.materialShortcuts[i].modI, int, "Material Shortcut - modI");
        LGDMATERIAL_WRITEBITS(material.materialShortcuts[i].secI, int, "Material Shortcut - secI");
        LGDMATERIAL_WRITEBITS(material.materialShortcuts[i].elementI, int, "Material Shortcut - elementI");

        int titleSize = material.materialShortcuts[i].title.size();
        LGDMATERIAL_WRITEBITS(titleSize, int, "Material Shortcut - Title Size");
        for (size_t ii = 0; ii < titleSize; ii++)
        {
            LGDMATERIAL_WRITEBITS(material.materialShortcuts[i].title[ii], char, "Material Shortcut - Title char");
        }
    }
    
    return true;
}

bool FileHandler::writeLGDMATERIALFile(
                                        std::string path, 
                                        Material material)
                                    {
    if(path == ""){
        path = showFileSystemObjectSelectionDialog(
                                                    "Select a folder to export the material file", 
                                                    "", 
                                                    {}, 
                                                    false, 
                                                    FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER
                                                );
                                                
    }
    
    if(path.size()){
        // Create the file path if path leads to a directory
        if(std::filesystem::is_directory(path))    
            path += UTIL::folderDistinguisher() + material.title + ".lgdmaterial";
    
        std::ofstream wf(path, std::ios::out | std::ios::binary);

        if(!wf) {
            LGDLOG::start<< "ERROR WHILE WRITING MATERIAL FILE! Cannot open file : " << path << LGDLOG::end;
            return false;
        }
        
        if(!FileHandler::writeMaterialData(wf, material))
            return false;
    }

    return true;
}