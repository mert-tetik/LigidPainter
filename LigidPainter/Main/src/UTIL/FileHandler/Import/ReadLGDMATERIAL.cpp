/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Read *.lgdmaterial file

    lgdmaterial file documentation can be found at the : ./LigidPainter/Main/src/UTIL/FileHandler/Notes/LgdMaterial_file_documentation.txt

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
#include "UTIL/Library/Library.hpp"

#include "3D/ThreeD.hpp"

#include "GUI/GUI.hpp"

#define LGDMATERIAL_READBITS(var, type, loc) if(!rf.read(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                LGDLOG::start<< "ERROR : Reading lgdmaterial file. Failed to read at : " << loc << LGDLOG::end;\
                                return false; \
                            }

bool readTextureModifier(std::ifstream& rf, MaterialModifier& modifier, int version, unsigned int textureVersionCode, std::vector<Texture*>* to_generate_txtrs);
bool readSolidModifier(std::ifstream& rf, MaterialModifier& modifier, int version, unsigned int textureVersionCode, std::vector<Texture*>* to_generate_txtrs);
bool readLiquidModifier(std::ifstream& rf, MaterialModifier& modifier, int version, unsigned int textureVersionCode, std::vector<Texture*>* to_generate_txtrs);
bool readMossModifier(std::ifstream& rf, MaterialModifier& modifier, int version, unsigned int textureVersionCode, std::vector<Texture*>* to_generate_txtrs);
bool readRustModifier(std::ifstream& rf, MaterialModifier& modifier, int version, unsigned int textureVersionCode, std::vector<Texture*>* to_generate_txtrs);
bool readSkinModifier(std::ifstream& rf, MaterialModifier& modifier, int version, unsigned int textureVersionCode, std::vector<Texture*>* to_generate_txtrs);
bool readWoodenModifier(std::ifstream& rf, MaterialModifier& modifier, int version, unsigned int textureVersionCode, std::vector<Texture*>* to_generate_txtrs);
bool readAsphaltModifier(std::ifstream& rf, MaterialModifier& modifier, int version, unsigned int textureVersionCode, std::vector<Texture*>* to_generate_txtrs);
bool readDustModifier(std::ifstream& rf, MaterialModifier& modifier, int version, unsigned int textureVersionCode, std::vector<Texture*>* to_generate_txtrs);
bool readMathModifier(std::ifstream& rf, MaterialModifier& modifier, int version, unsigned int textureVersionCode, std::vector<Texture*>* to_generate_txtrs);

bool FileHandler::readMaterialData(std::ifstream& rf, Material& material, std::vector<Texture*>* to_generate_txtrs){
    
    //!HEADER
    
    //Description 
    uint64_t h1; 
    uint64_t h2; 
    uint64_t h3; 
    uint64_t r1 = 0x5A7D0C809A22F3B7; 
    uint64_t r2 = 0xE8F691BE0D45C23A; 
    uint64_t r3 = 0x3FD8A9657B101E8C; 
    LGDMATERIAL_READBITS(h1, uint64_t, "File description header part 1");
    LGDMATERIAL_READBITS(h2, uint64_t, "File description header part 2");
    LGDMATERIAL_READBITS(h3, uint64_t, "File description header part 3");

    if(h1 != r1 || h2 != r2 || h3 != r3){
        LGDLOG::start << "ERROR WHILE READING MATERIAL FILE! Description header is not correct." << LGDLOG::end;
        return false;
    }

    //Version number
    uint32_t versionNumber; 
    LGDMATERIAL_READBITS(versionNumber, uint32_t, "Version number");

    if(versionNumber != 2000 && versionNumber != 2100 && versionNumber != 2200 && versionNumber != 2300){
        LGDLOG::start << "ERROR WHILE READING MATERIAL FILE! This version of the LigidPainter doesn't support this material file's version." << LGDLOG::end;
        return false;
    }

    //ID
    int empty = 0;
    LGDMATERIAL_READBITS(empty, int, "Material ID");

    //!Modifiers

    //Read the material modifier size
    uint64_t materialModifierSize;
    LGDMATERIAL_READBITS(materialModifierSize, uint64_t, "Material modifier size");

    material.materialModifiers.clear();

    for (size_t i = 0; i < materialModifierSize; i++)
    {

        int modifierIndex;
        LGDMATERIAL_READBITS(modifierIndex, int, "Modifier type index");
        
        MaterialModifier modifier;
        if(modifierIndex == 0)
            modifier = MaterialModifier(TEXTURE_MATERIAL_MODIFIER);
        else if(modifierIndex == 1)
            modifier = MaterialModifier(DUST_MATERIAL_MODIFIER);
        else if(modifierIndex == 2)
            modifier = MaterialModifier(ASPHALT_MATERIAL_MODIFIER);
        else if(modifierIndex == 3)
            modifier = MaterialModifier(LIQUID_MATERIAL_MODIFIER);
        else if(modifierIndex == 4)
            modifier = MaterialModifier(MOSS_MATERIAL_MODIFIER);
        else if(modifierIndex == 5)
            modifier = MaterialModifier(RUST_MATERIAL_MODIFIER);
        else if(modifierIndex == 6)
            modifier = MaterialModifier(SKIN_MATERIAL_MODIFIER);
        else if(modifierIndex == 7)
            modifier = MaterialModifier(SOLID_MATERIAL_MODIFIER);
        else if(modifierIndex == 8)
            modifier = MaterialModifier(WOODEN_MATERIAL_MODIFIER);
        else if(modifierIndex == 9)
            modifier = MaterialModifier(MATH_MATERIAL_MODIFIER);
        else{
            LGDLOG::start<< "ERROR : Reading material file : Unknown modifier index." << LGDLOG::end;
            return false;
        }

        unsigned int textureVersionCode = 0;
        
        if(versionNumber >= 2300)
            textureVersionCode = 2;
        else if(versionNumber == 2200)
            textureVersionCode = 1;

        if(!modifier.maskTexture.readTextureData(rf, true, textureVersionCode, to_generate_txtrs == nullptr))
            return false;
        if(to_generate_txtrs != nullptr)
            to_generate_txtrs->push_back(&modifier.maskTexture);

        if(modifier.modifierIndex == TEXTURE_MATERIAL_MODIFIER){
            if(!readTextureModifier(rf, modifier, versionNumber, textureVersionCode, to_generate_txtrs)){
                return false;
            }
        }

        else if (modifier.modifierIndex == SOLID_MATERIAL_MODIFIER){
            if(!readSolidModifier(rf, modifier, versionNumber, textureVersionCode, to_generate_txtrs)){
                return false;
            }
        }

        else if (modifier.modifierIndex == LIQUID_MATERIAL_MODIFIER){
            if(!readLiquidModifier(rf, modifier, versionNumber, textureVersionCode, to_generate_txtrs)){
                return false;
            }
        }

        else if (modifier.modifierIndex == MOSS_MATERIAL_MODIFIER){
            if(!readMossModifier(rf, modifier, versionNumber, textureVersionCode, to_generate_txtrs)){
                return false;
            }
        }

        else if (modifier.modifierIndex == RUST_MATERIAL_MODIFIER){
            if(!readRustModifier(rf, modifier, versionNumber, textureVersionCode, to_generate_txtrs)){
                return false;
            }
        }

        else if (modifier.modifierIndex == SKIN_MATERIAL_MODIFIER){
            if(!readSkinModifier(rf, modifier, versionNumber, textureVersionCode, to_generate_txtrs)){
                return false;
            }
        }

        else if (modifier.modifierIndex == WOODEN_MATERIAL_MODIFIER){
            if(!readWoodenModifier(rf, modifier, versionNumber, textureVersionCode, to_generate_txtrs)){  
                return false;
            }
        }

        else if (modifier.modifierIndex == ASPHALT_MATERIAL_MODIFIER){
            if(!readAsphaltModifier(rf, modifier, versionNumber, textureVersionCode, to_generate_txtrs)){
                return false;
            }
        }

        else if (modifier.modifierIndex == DUST_MATERIAL_MODIFIER){
            if(!readDustModifier(rf, modifier, versionNumber, textureVersionCode, to_generate_txtrs)){
                return false;
            }
        }
        
        else if (modifier.modifierIndex == MATH_MATERIAL_MODIFIER){
            if(!readMathModifier(rf, modifier, versionNumber, textureVersionCode, to_generate_txtrs)){
                return false;
            }
        }


        material.materialModifiers.push_back(modifier);
    }    

    if(versionNumber >= 2200){
        // Shortcuts
        int matShortcutSize;
        LGDMATERIAL_READBITS(matShortcutSize, int, "Material Shortcut - Material Shortcut Size");
        for (size_t i = 0; i < matShortcutSize; i++)
        {
            MaterialShortcut shortcut;
            LGDMATERIAL_READBITS(shortcut.modI, int, "Material Shortcut - modI");
            LGDMATERIAL_READBITS(shortcut.secI, int, "Material Shortcut - secI");
            LGDMATERIAL_READBITS(shortcut.elementI, int, "Material Shortcut - elementI");

            int titleSize;
            LGDMATERIAL_READBITS(titleSize, int, "Material Shortcut - Title Size");
            for (size_t i = 0; i < titleSize; i++)
            {
                char c;
                LGDMATERIAL_READBITS(c, char, "Material Shortcut - Title char");
                shortcut.title.push_back(c);
            }
            
            if(shortcut.secI != -1){
                shortcut.element = &material.materialModifiers[shortcut.modI].sections[shortcut.secI].elements[shortcut.elementI];      
            }
            else{
                shortcut.maskTxtr = &material.materialModifiers[shortcut.modI].maskTexture;      
            }

            material.materialShortcuts.push_back(shortcut);
        }
    }

    return true;
}

bool FileHandler::readLGDMATERIALFile(
                                        std::string path, 
                                        Material& material
                                    )
{
    LGDLOG::start << "Loading Material : " << path  << LGDLOG::end;
    
    if(!std::filesystem::is_regular_file(path)){
        LGDLOG::start << "ERROR : Loading material file : " << path << " is not a regular file!" << LGDLOG::end;
        return false;
    }

    if(path.size()){
        std::ifstream rf(path, std::ios::out | std::ios::binary);
        
        if(!rf) {
            LGDLOG::start<< "ERROR WHILE READING MATERIAL FILE! Cannot open file : " << path << LGDLOG::end;
            return false;
        }
        
        material.title = UTIL::getLastWordBySeparatingWithChar(path, UTIL::folderDistinguisher());
        material.title = UTIL::removeExtension(material.title);

        if(!FileHandler::readMaterialData(rf, material, nullptr)){
            return false;
        }
    }

    return true;
}


bool readTextureModifier(std::ifstream& rf, MaterialModifier& modifier, int version, unsigned int textureVersionCode, std::vector<Texture*>* to_generate_txtrs){
    // ---- Channel Textures ----
    // albedoTexture
    if(!modifier.sections[0].elements[1].button.texture.readTextureData(rf, true, textureVersionCode, to_generate_txtrs == nullptr))
        return false;
    if(to_generate_txtrs != nullptr)
        to_generate_txtrs->push_back(&modifier.sections[0].elements[1].button.texture);
    // roughnessTexture
    if(!modifier.sections[0].elements[2].button.texture.readTextureData(rf, true, textureVersionCode, to_generate_txtrs == nullptr))
        return false;
    if(to_generate_txtrs != nullptr)
        to_generate_txtrs->push_back(&modifier.sections[0].elements[2].button.texture);
    // metallicTexture
    if(!modifier.sections[0].elements[3].button.texture.readTextureData(rf, true, textureVersionCode, to_generate_txtrs == nullptr))
        return false;
    if(to_generate_txtrs != nullptr)
        to_generate_txtrs->push_back(&modifier.sections[0].elements[3].button.texture);
    // normalMapTexture
    if(!modifier.sections[0].elements[4].button.texture.readTextureData(rf, true, textureVersionCode, to_generate_txtrs == nullptr))
        return false;
    if(to_generate_txtrs != nullptr)
        to_generate_txtrs->push_back(&modifier.sections[0].elements[4].button.texture);
    // heightMapTexture
    if(!modifier.sections[0].elements[5].button.texture.readTextureData(rf, true, textureVersionCode, to_generate_txtrs == nullptr))
        return false;
    if(to_generate_txtrs != nullptr)
        to_generate_txtrs->push_back(&modifier.sections[0].elements[5].button.texture);
    // ambientOcclusionTexture
    if(!modifier.sections[0].elements[6].button.texture.readTextureData(rf, true, textureVersionCode, to_generate_txtrs == nullptr))
        return false;
    if(to_generate_txtrs != nullptr)
        to_generate_txtrs->push_back(&modifier.sections[0].elements[6].button.texture);

    // ---- Channel Opacities ----
    // albedoOpacity
    LGDMATERIAL_READBITS(modifier.sections[1].elements[0].rangeBar.value, float, "Albedo Opacity");
    // roughnessOpacity
    LGDMATERIAL_READBITS(modifier.sections[1].elements[1].rangeBar.value, float, "Roughness Opacity");
    // metallicOpacity
    LGDMATERIAL_READBITS(modifier.sections[1].elements[2].rangeBar.value, float, "Metallic Opacity");
    // normalMapOpacity
    LGDMATERIAL_READBITS(modifier.sections[1].elements[3].rangeBar.value, float, "Normal Map Opacity");
    // heightMapOpacity
    LGDMATERIAL_READBITS(modifier.sections[1].elements[4].rangeBar.value, float, "Height Map Opacity");
    // ambientOcclusionOpacity
    LGDMATERIAL_READBITS(modifier.sections[1].elements[5].rangeBar.value, float, "Ambient Occlusion Opacity");

    // ---- Depth Masking & Depth Properties ----
    // depthValue
    LGDMATERIAL_READBITS(modifier.sections[2].elements[0].rangeBar.value, float, "Depth Value");
    if(version >= 2100){
        //blurTheHeightMap
        LGDMATERIAL_READBITS( modifier.sections[2].elements[1].checkBox.clickState1, int, "blurTheHeightMap");
    }
    
    // ---- Filter ----
    // albedoFilter
    modifier.sections[3].elements[0].button.filter.readFilterData(rf, true);
    modifier.sections[3].elements[0].button.texture = modifier.sections[3].elements[0].button.filter.displayingTxtr;
    if(!modifier.sections[3].elements[1].button.texture.readTextureData(rf, true, textureVersionCode, to_generate_txtrs == nullptr))
        return false;
    if(to_generate_txtrs != nullptr)
        to_generate_txtrs->push_back(&modifier.sections[3].elements[1].button.texture);

    return true;
}

bool readSolidModifier(std::ifstream& rf, MaterialModifier& modifier, int version, unsigned int textureVersionCode, std::vector<Texture*>* to_generate_txtrs){
    // ---- Channels ----
    // albedoColor;
    // albedoOpacity
    LGDMATERIAL_READBITS(modifier.sections[0].elements[0].button.color.r, float, "Albedo Color");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[0].button.color.g, float, "Albedo Color");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[0].button.color.b, float, "Albedo Color");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[1].rangeBar.value, float, "Albedo Opacity");
    // roughnessColor;
    // roughnessOpacity
    LGDMATERIAL_READBITS(modifier.sections[0].elements[2].button.color.r, float, "Roughness Color");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[2].button.color.g, float, "Roughness Color");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[2].button.color.b, float, "Roughness Color");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[3].rangeBar.value, float, "Roughness Opacity");
    // metallicColor;
    // metallicOpacity
    LGDMATERIAL_READBITS(modifier.sections[0].elements[4].button.color.r, float, "Metallic Color");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[4].button.color.g, float, "Metallic Color");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[4].button.color.b, float, "Metallic Color");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[5].rangeBar.value, float, "Metallic Opacity");
    // normalMapColor;
    // normalMapOpacity
    LGDMATERIAL_READBITS(modifier.sections[0].elements[6].button.color.r, float, "NormalMap Color");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[6].button.color.g, float, "NormalMap Color");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[6].button.color.b, float, "NormalMap Color");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[7].rangeBar.value, float, "NormalMap Opacity");
    // heightMapColor;
    // heightMapOpacity
    LGDMATERIAL_READBITS(modifier.sections[0].elements[8].button.color.r, float, "HeightMap Color");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[8].button.color.g, float, "HeightMap Color");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[8].button.color.b, float, "HeightMap Color");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[9].rangeBar.value, float, "HeightMap Opacity");
    // ambientOcclusionColor;
    // ambientOcclusionOpacity
    LGDMATERIAL_READBITS(modifier.sections[0].elements[10].button.color.r, float, "AmbientOcclusion Color");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[10].button.color.g, float, "AmbientOcclusion Color");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[10].button.color.b, float, "AmbientOcclusion Color");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[11].rangeBar.value, float, "AmbientOcclusion Opacity");

    // ---- Depth Masking & Depth Properties ----
    // depthValue
    LGDMATERIAL_READBITS(modifier.sections[1].elements[0].rangeBar.value, float, "Depth Value");
    if(version >= 2100){
        //blurTheHeightMap
        LGDMATERIAL_READBITS( modifier.sections[1].elements[1].checkBox.clickState1, int, "blurTheHeightMap");
    }

    // ---- Filter ----
    // albedoFilter
    modifier.sections[2].elements[0].button.filter.readFilterData(rf, true);
    modifier.sections[2].elements[0].button.texture = modifier.sections[2].elements[0].button.filter.displayingTxtr;
    if(!modifier.sections[2].elements[1].button.texture.readTextureData(rf, true, textureVersionCode, to_generate_txtrs == nullptr))
        return false;
    if(to_generate_txtrs != nullptr)
        to_generate_txtrs->push_back(&modifier.sections[2].elements[1].button.texture);

    return true;
}

bool readLiquidModifier(std::ifstream& rf, MaterialModifier& modifier, int version, unsigned int textureVersionCode, std::vector<Texture*>* to_generate_txtrs){
    // ---- Scaling ----
    //scaleVal
    LGDMATERIAL_READBITS( modifier.sections[0].elements[0].rangeBar.value, float, "scaleVal");
    //scaleYAxisVal
    LGDMATERIAL_READBITS( modifier.sections[0].elements[1].rangeBar.value, float, "scaleYAxisVal");

    // ---- Noise ----
    //octaves
    LGDMATERIAL_READBITS( modifier.sections[1].elements[0].rangeBar.value, float, "octaves");
    //fbmXOffset
    LGDMATERIAL_READBITS( modifier.sections[1].elements[1].rangeBar.value, float, "fbmXOffset");
    //fbmYOffset
    LGDMATERIAL_READBITS( modifier.sections[1].elements[2].rangeBar.value, float, "fbmYOffset");
    //fbmXScale
    LGDMATERIAL_READBITS( modifier.sections[1].elements[3].rangeBar.value, float, "fbmXScale");
    //fbmYScale
    LGDMATERIAL_READBITS( modifier.sections[1].elements[4].rangeBar.value, float, "fbmYScale");
    //roughness
    LGDMATERIAL_READBITS( modifier.sections[1].elements[5].rangeBar.value, float, "roughness");
    //frequencyAcceleration
    LGDMATERIAL_READBITS( modifier.sections[1].elements[6].rangeBar.value, float, "frequencyAcceleration");
    //fbmTotal
    LGDMATERIAL_READBITS( modifier.sections[1].elements[7].rangeBar.value, float, "fbmTotal");
    //seed
    LGDMATERIAL_READBITS( modifier.sections[1].elements[8].rangeBar.value, float, "seed");

    // ---- General ----
    //noiseInfluence
    LGDMATERIAL_READBITS( modifier.sections[2].elements[0].rangeBar.value, float, "noiseInfluence");
    //positionInfluence
    LGDMATERIAL_READBITS( modifier.sections[2].elements[1].rangeBar.value, float, "positionInfluence");

    // ---- Colors ----
    //color1
    LGDMATERIAL_READBITS( modifier.sections[3].elements[0].button.color.r, float, "color1");
    LGDMATERIAL_READBITS(modifier.sections[3].elements[0].button.color.g, float, "color1");
    LGDMATERIAL_READBITS(modifier.sections[3].elements[0].button.color.b, float, "color1");
    //color2
    LGDMATERIAL_READBITS( modifier.sections[3].elements[1].button.color.r, float, "color2");
    LGDMATERIAL_READBITS(modifier.sections[3].elements[1].button.color.g, float, "color2");
    LGDMATERIAL_READBITS(modifier.sections[3].elements[1].button.color.b, float, "color2");
    //color3
    LGDMATERIAL_READBITS( modifier.sections[3].elements[2].button.color.r, float, "color3");
    LGDMATERIAL_READBITS(modifier.sections[3].elements[2].button.color.g, float, "color3");
    LGDMATERIAL_READBITS(modifier.sections[3].elements[2].button.color.b, float, "color3");
    //color4
    LGDMATERIAL_READBITS( modifier.sections[3].elements[3].button.color.r, float, "color4");
    LGDMATERIAL_READBITS(modifier.sections[3].elements[3].button.color.g, float, "color4");
    LGDMATERIAL_READBITS(modifier.sections[3].elements[3].button.color.b, float, "color4");

    // ---- Element Properties ----
    //wetness
    LGDMATERIAL_READBITS( modifier.sections[4].elements[0].rangeBar.value, float, "wetness");
    //metallic
    LGDMATERIAL_READBITS( modifier.sections[4].elements[1].rangeBar.value, float, "metallic");
    //height
    LGDMATERIAL_READBITS( modifier.sections[4].elements[2].rangeBar.value, float, "height");
    //ambientOcclusion
    LGDMATERIAL_READBITS( modifier.sections[4].elements[3].rangeBar.value, float, "ambientOcclusion");

    // ---- Channel Opacities ----
    //albedoOpacity
    LGDMATERIAL_READBITS( modifier.sections[5].elements[0].rangeBar.value, float, "Albedo Opacity");
    //roughnessOpacity
    LGDMATERIAL_READBITS( modifier.sections[5].elements[1].rangeBar.value, float, "Roughness Opacity");
    //metallicOpacity
    LGDMATERIAL_READBITS( modifier.sections[5].elements[2].rangeBar.value, float, "Metallic Opacity");
    //normalMapOpacity
    LGDMATERIAL_READBITS( modifier.sections[5].elements[3].rangeBar.value, float, "Normal Map Opacity");
    //heightMapOpacity
    LGDMATERIAL_READBITS( modifier.sections[5].elements[4].rangeBar.value, float, "Height Map Opacity");
    //ambientOcclusionOpacity
    LGDMATERIAL_READBITS( modifier.sections[5].elements[5].rangeBar.value, float, "Ambient Occlusion Opacity");

    // ---- Depth Masking & Depth Properties ----
    //depthValue
    LGDMATERIAL_READBITS( modifier.sections[6].elements[0].rangeBar.value, float, "Depth Value");
    //blurTheHeightMap
    LGDMATERIAL_READBITS( modifier.sections[6].elements[1].checkBox.clickState1, int, "blurTheHeightMap");
    
    // ---- Filter ----
    // albedoFilter
    modifier.sections[7].elements[0].button.filter.readFilterData(rf, true);
    modifier.sections[7].elements[0].button.texture = modifier.sections[7].elements[0].button.filter.displayingTxtr;
    if(!modifier.sections[7].elements[1].button.texture.readTextureData(rf, true, textureVersionCode, to_generate_txtrs == nullptr))
        return false;
    if(to_generate_txtrs != nullptr)
        to_generate_txtrs->push_back(&modifier.sections[7].elements[1].button.texture);

    return true;
}

bool readMossModifier(std::ifstream& rf, MaterialModifier& modifier, int version, unsigned int textureVersionCode, std::vector<Texture*>* to_generate_txtrs){
    // ---- Color ----
    //mossColorBack
    LGDMATERIAL_READBITS( modifier.sections[0].elements[0].button.color.r, float, "mossColorBack");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[0].button.color.g, float, "mossColorBack");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[0].button.color.b, float, "mossColorBack");
    //mossColorFront
    LGDMATERIAL_READBITS( modifier.sections[0].elements[1].button.color.r, float, "mossColorFront");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[1].button.color.g, float, "mossColorFront");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[1].button.color.b, float, "mossColorFront");
    //dirtColor
    LGDMATERIAL_READBITS( modifier.sections[0].elements[2].button.color.r, float, "dirtColor");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[2].button.color.g, float, "dirtColor");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[2].button.color.b, float, "dirtColor");
    
    // ---- Droplets ----
    //dropletsCount
    LGDMATERIAL_READBITS( modifier.sections[1].elements[0].rangeBar.value, float, "dropletsCount");
    //dropletsOpacityJitter
    LGDMATERIAL_READBITS( modifier.sections[1].elements[1].rangeBar.value, float, "dropletsOpacityJitter");
    //dropletsSize
    LGDMATERIAL_READBITS( modifier.sections[1].elements[2].rangeBar.value, float, "dropletsSize");

    // ---- Front Layer ----
    //frontLayerStrength
    LGDMATERIAL_READBITS( modifier.sections[2].elements[0].rangeBar.value, float, "frontLayerStrength");
    //frontLayerScale
    LGDMATERIAL_READBITS( modifier.sections[2].elements[1].rangeBar.value, float, "frontLayerScale");

    // ---- Lighting ----
    //lightStrength
    LGDMATERIAL_READBITS( modifier.sections[3].elements[0].rangeBar.value, float, "lightStrength");
    //darkeningStrength
    LGDMATERIAL_READBITS( modifier.sections[3].elements[1].rangeBar.value, float, "darkeningStrength");

    // ---- Noise ----
    //noiseStrength
    LGDMATERIAL_READBITS( modifier.sections[4].elements[0].rangeBar.value, float, "noiseStrength");

    // ---- Moss Properties ----
    //scale
    LGDMATERIAL_READBITS( modifier.sections[5].elements[0].rangeBar.value, float, "scale");
    
    // ---- Element Properties ----
    //wetness
    LGDMATERIAL_READBITS( modifier.sections[6].elements[0].rangeBar.value, float, "wetness");
    //metallic
    LGDMATERIAL_READBITS( modifier.sections[6].elements[1].rangeBar.value, float, "metallic");
    //height
    LGDMATERIAL_READBITS( modifier.sections[6].elements[2].rangeBar.value, float, "height");

    // ---- Channel Opacities ----
    //albedoOpacity
    LGDMATERIAL_READBITS( modifier.sections[7].elements[0].rangeBar.value, float, "Albedo Opacity");
    //roughnessOpacity
    LGDMATERIAL_READBITS( modifier.sections[7].elements[1].rangeBar.value, float, "Roughness Opacity");
    //metallicOpacity
    LGDMATERIAL_READBITS( modifier.sections[7].elements[2].rangeBar.value, float, "Metallic Opacity");
    //normalMapOpacity
    LGDMATERIAL_READBITS( modifier.sections[7].elements[3].rangeBar.value, float, "Normal Map Opacity");
    //heightMapOpacity
    LGDMATERIAL_READBITS( modifier.sections[7].elements[4].rangeBar.value, float, "Height Map Opacity");
    //ambientOcclusionOpacity
    LGDMATERIAL_READBITS( modifier.sections[7].elements[5].rangeBar.value, float, "Ambient Occlusion Opacity");

    // ---- Depth Masking & Depth Properties ----
    //depthValue
    LGDMATERIAL_READBITS( modifier.sections[8].elements[0].rangeBar.value, float, "Depth Value");
    //blurTheHeightMap
    LGDMATERIAL_READBITS( modifier.sections[8].elements[1].checkBox.clickState1, int, "blurTheHeightMap");
    
    // ---- Filter ----
    // albedoFilter
    modifier.sections[9].elements[0].button.filter.readFilterData(rf, true);
    modifier.sections[9].elements[0].button.texture = modifier.sections[9].elements[0].button.filter.displayingTxtr;
    if(!modifier.sections[9].elements[1].button.texture.readTextureData(rf, true, textureVersionCode, to_generate_txtrs == nullptr))
        return false;
    if(to_generate_txtrs != nullptr)
        to_generate_txtrs->push_back(&modifier.sections[9].elements[1].button.texture);

    return true;
}

bool readRustModifier(std::ifstream& rf, MaterialModifier& modifier, int version, unsigned int textureVersionCode, std::vector<Texture*>* to_generate_txtrs){
    // ---- Colors ----
    //color1
    LGDMATERIAL_READBITS( modifier.sections[0].elements[0].button.color.r, float, "color1");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[0].button.color.g, float, "color1");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[0].button.color.b, float, "color1");
    //color2
    LGDMATERIAL_READBITS( modifier.sections[0].elements[1].button.color.r, float, "color2");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[1].button.color.g, float, "color2");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[1].button.color.b, float, "color2");
    //color3
    LGDMATERIAL_READBITS( modifier.sections[0].elements[2].button.color.r, float, "color3");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[2].button.color.g, float, "color3");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[2].button.color.b, float, "color3");
    //color4
    LGDMATERIAL_READBITS( modifier.sections[0].elements[3].button.color.r, float, "color4");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[3].button.color.g, float, "color4");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[3].button.color.b, float, "color4");

    // ---- Properties ----
    //scale
    LGDMATERIAL_READBITS( modifier.sections[1].elements[0].rangeBar.value, float, "scale");

    // ---- Perlin Properties ----
    //firstOctave
    LGDMATERIAL_READBITS( modifier.sections[2].elements[0].rangeBar.value, float, "firstOctave");
    //octaves
    LGDMATERIAL_READBITS( modifier.sections[2].elements[1].rangeBar.value, float, "octaves");
    //persistence
    LGDMATERIAL_READBITS( modifier.sections[2].elements[2].rangeBar.value, float, "persistence");

    // ---- FBM Properties ----
    //fbmOctaves
    LGDMATERIAL_READBITS( modifier.sections[3].elements[0].rangeBar.value, float, "fbmOctaves");
    //fbmRoughness
    LGDMATERIAL_READBITS( modifier.sections[3].elements[1].rangeBar.value, float, "fbmRoughness");

    // ---- Rust Properties ----
    //rustRadius
    LGDMATERIAL_READBITS( modifier.sections[4].elements[0].rangeBar.value, float, "rustRadius");
    //batteringStrength
    LGDMATERIAL_READBITS( modifier.sections[4].elements[1].rangeBar.value, float, "batteringStrength");

    // ---- Noise Properties ----
    //noiseStrength
    LGDMATERIAL_READBITS( modifier.sections[5].elements[0].rangeBar.value, float, "noiseStrength");

    // ---- Element Properties ----
    //wetness
    LGDMATERIAL_READBITS( modifier.sections[6].elements[0].rangeBar.value, float, "wetness");
    //metallic
    LGDMATERIAL_READBITS( modifier.sections[6].elements[1].rangeBar.value, float, "metallic");
    //height
    LGDMATERIAL_READBITS( modifier.sections[6].elements[2].rangeBar.value, float, "height");

    // ---- Channel Opacities ----
    //albedoOpacity
    LGDMATERIAL_READBITS( modifier.sections[7].elements[0].rangeBar.value, float, "Albedo Opacity");
    //roughnessOpacity
    LGDMATERIAL_READBITS( modifier.sections[7].elements[1].rangeBar.value, float, "Roughness Opacity");
    //metallicOpacity
    LGDMATERIAL_READBITS( modifier.sections[7].elements[2].rangeBar.value, float, "Metallic Opacity");
    //normalMapOpacity
    LGDMATERIAL_READBITS( modifier.sections[7].elements[3].rangeBar.value, float, "Normal Map Opacity");
    //heightMapOpacity
    LGDMATERIAL_READBITS( modifier.sections[7].elements[4].rangeBar.value, float, "Height Map Opacity");
    //ambientOcclusionOpacity
    LGDMATERIAL_READBITS( modifier.sections[7].elements[5].rangeBar.value, float, "Ambient Occlusion Opacity");

    // ---- Depth Masking & Depth Properties ----
    //depthValue
    LGDMATERIAL_READBITS( modifier.sections[8].elements[0].rangeBar.value, float, "Depth Value");
    //blurTheHeightMap
    LGDMATERIAL_READBITS( modifier.sections[8].elements[1].checkBox.clickState1, int, "blurTheHeightMap");
    
    // ---- Filter ----
    // albedoFilter
    modifier.sections[9].elements[0].button.filter.readFilterData(rf, true);
    modifier.sections[9].elements[0].button.texture = modifier.sections[9].elements[0].button.filter.displayingTxtr;
    if(!modifier.sections[9].elements[1].button.texture.readTextureData(rf, true, textureVersionCode, to_generate_txtrs == nullptr))
        return false;
    if(to_generate_txtrs != nullptr)
        to_generate_txtrs->push_back(&modifier.sections[9].elements[1].button.texture);

    return true;
}

bool readSkinModifier(std::ifstream& rf, MaterialModifier& modifier, int version, unsigned int textureVersionCode, std::vector<Texture*>* to_generate_txtrs){
    // ---- Droplets ----
    //dropletsCount
    LGDMATERIAL_READBITS( modifier.sections[0].elements[0].rangeBar.value, float, "dropletsCount");
    //dropletsOpacityJitter
    LGDMATERIAL_READBITS( modifier.sections[0].elements[1].rangeBar.value, float, "dropletsOpacityJitter");
    //dropletsSize
    LGDMATERIAL_READBITS( modifier.sections[0].elements[2].rangeBar.value, float, "dropletsSize");
    
    // ---- Veins ----
    //veinsScale
    LGDMATERIAL_READBITS( modifier.sections[1].elements[0].rangeBar.value, float, "veinsScale");
    //veinsStrength
    LGDMATERIAL_READBITS( modifier.sections[1].elements[1].rangeBar.value, float, "veinsStrength");
    
    // ---- Blushing ----
    //blushingStrength
    LGDMATERIAL_READBITS( modifier.sections[2].elements[0].rangeBar.value, float, "blushingStrength");
    
    // ---- Skin Prints ----
    //skinPrintsScale
    LGDMATERIAL_READBITS( modifier.sections[3].elements[0].rangeBar.value, float, "skinPrintsScale");
    //skinPrintsStrength
    LGDMATERIAL_READBITS( modifier.sections[3].elements[1].rangeBar.value, float, "skinPrintsStrength");
    
    // ---- Noise ----
    //noiseStrength
    LGDMATERIAL_READBITS( modifier.sections[4].elements[0].rangeBar.value, float, "noiseStrength");
    
    // ---- Skin Properties ----
    //skinColor
    LGDMATERIAL_READBITS( modifier.sections[5].elements[0].button.color.r, float, "skinColor");
    LGDMATERIAL_READBITS(modifier.sections[5].elements[0].button.color.g, float, "skinColor");
    LGDMATERIAL_READBITS(modifier.sections[5].elements[0].button.color.b, float, "skinColor");
    //veinColor
    LGDMATERIAL_READBITS( modifier.sections[5].elements[1].button.color.r, float, "veinColor");
    LGDMATERIAL_READBITS(modifier.sections[5].elements[1].button.color.g, float, "veinColor");
    LGDMATERIAL_READBITS(modifier.sections[5].elements[1].button.color.b, float, "veinColor");
    //skinScale
    LGDMATERIAL_READBITS( modifier.sections[5].elements[2].rangeBar.value, float, "skinScale");
    //skinWetness
    LGDMATERIAL_READBITS( modifier.sections[5].elements[3].rangeBar.value, float, "skinWetness");
    //skinMetallic
    LGDMATERIAL_READBITS( modifier.sections[5].elements[4].rangeBar.value, float, "skinMetallic");
    //skinHeight
    LGDMATERIAL_READBITS( modifier.sections[5].elements[5].rangeBar.value, float, "skinHeight");
    //skinAmbientOcclusion
    LGDMATERIAL_READBITS( modifier.sections[5].elements[6].rangeBar.value, float, "skinAmbientOcclusion");
    
    // ---- Channel Opacities ----
    //albedoOpacity
    LGDMATERIAL_READBITS( modifier.sections[6].elements[0].rangeBar.value, float, "Albedo Opacity");
    //roughnessOpacity
    LGDMATERIAL_READBITS( modifier.sections[6].elements[1].rangeBar.value, float, "Roughness Opacity");
    //metallicOpacity
    LGDMATERIAL_READBITS( modifier.sections[6].elements[2].rangeBar.value, float, "Metallic Opacity");
    //normalMapOpacity
    LGDMATERIAL_READBITS( modifier.sections[6].elements[3].rangeBar.value, float, "Normal Map Opacity");
    //heightMapOpacity
    LGDMATERIAL_READBITS( modifier.sections[6].elements[4].rangeBar.value, float, "Height Map Opacity");
    //ambientOcclusionOpacity
    LGDMATERIAL_READBITS( modifier.sections[6].elements[5].rangeBar.value, float, "Ambient Occlusion Opacity");

    // ---- Depth Masking & Depth Properties ----
    //depthValue
    LGDMATERIAL_READBITS( modifier.sections[7].elements[0].rangeBar.value, float, "Depth Value");
    //blurTheHeightMap
    LGDMATERIAL_READBITS( modifier.sections[7].elements[1].checkBox.clickState1, int, "blurTheHeightMap");
    
    // ---- Filter ----
    // albedoFilter
    modifier.sections[8].elements[0].button.filter.readFilterData(rf, true);
    modifier.sections[8].elements[0].button.texture = modifier.sections[8].elements[0].button.filter.displayingTxtr;
    if(!modifier.sections[8].elements[1].button.texture.readTextureData(rf, true, textureVersionCode, to_generate_txtrs == nullptr))
        return false;
    if(to_generate_txtrs != nullptr)
        to_generate_txtrs->push_back(&modifier.sections[8].elements[1].button.texture);

    return true;
}

bool readWoodenModifier(std::ifstream& rf, MaterialModifier& modifier, int version, unsigned int textureVersionCode, std::vector<Texture*>* to_generate_txtrs){
    // ---- Colors ----
    //color1
    LGDMATERIAL_READBITS( modifier.sections[0].elements[0].button.color.r, float, "color1");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[0].button.color.g, float, "color1");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[0].button.color.b, float, "color1");
    //color2
    LGDMATERIAL_READBITS( modifier.sections[0].elements[1].button.color.r, float, "color2");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[1].button.color.g, float, "color2");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[1].button.color.b, float, "color2");
    //color3
    LGDMATERIAL_READBITS( modifier.sections[0].elements[2].button.color.r, float, "color3");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[2].button.color.g, float, "color3");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[2].button.color.b, float, "color3");

    // ---- Properties ----
    //scale
    LGDMATERIAL_READBITS( modifier.sections[1].elements[0].rangeBar.value, float, "scale");
    //noiseOffset
    LGDMATERIAL_READBITS( modifier.sections[1].elements[1].rangeBar.value, float, "noiseOffset");
    //noiseSeed
    LGDMATERIAL_READBITS( modifier.sections[1].elements[2].rangeBar.value, float, "noiseSeed");

    // ---- Perlin Properties ----
    //maxOctaves
    LGDMATERIAL_READBITS( modifier.sections[2].elements[0].rangeBar.value, float, "maxOctaves");
    //persistance
    LGDMATERIAL_READBITS( modifier.sections[2].elements[1].rangeBar.value, float, "persistance");

    // ---- Musgrave Properties ----
    //musgraveLacunarity
    LGDMATERIAL_READBITS( modifier.sections[3].elements[0].rangeBar.value, float, "musgraveLacunarity");
    //musgraveStrength
    LGDMATERIAL_READBITS( modifier.sections[3].elements[1].rangeBar.value, float, "musgraveStrength");
    //musgraveNoise
    LGDMATERIAL_READBITS( modifier.sections[3].elements[2].rangeBar.value, float, "musgraveNoise");

    // ---- Base ----
    //noiseStrength
    LGDMATERIAL_READBITS( modifier.sections[4].elements[0].rangeBar.value, float, "noiseStrength");
    //colorSaturation
    LGDMATERIAL_READBITS( modifier.sections[4].elements[1].rangeBar.value, float, "colorSaturation");

    // ---- FBM Properties ----
    //fbmFrequency
    LGDMATERIAL_READBITS( modifier.sections[5].elements[0].rangeBar.value, float, "fbmFrequency");
    
    // ---- Element Properties ----
    //woodGamma
    LGDMATERIAL_READBITS( modifier.sections[6].elements[0].rangeBar.value, float, "woodGamma");
    //shininess
    LGDMATERIAL_READBITS( modifier.sections[6].elements[1].rangeBar.value, float, "shininess");
    //metallic
    LGDMATERIAL_READBITS( modifier.sections[6].elements[2].rangeBar.value, float, "metallic");
    //height
    LGDMATERIAL_READBITS( modifier.sections[6].elements[3].rangeBar.value, float, "height");

    // ---- Channel Opacities ----
    //albedoOpacity
    LGDMATERIAL_READBITS( modifier.sections[7].elements[0].rangeBar.value, float, "Albedo Opacity");
    //roughnessOpacity
    LGDMATERIAL_READBITS( modifier.sections[7].elements[1].rangeBar.value, float, "Roughness Opacity");
    //metallicOpacity
    LGDMATERIAL_READBITS( modifier.sections[7].elements[2].rangeBar.value, float, "Metallic Opacity");
    //normalMapOpacity
    LGDMATERIAL_READBITS( modifier.sections[7].elements[3].rangeBar.value, float, "Normal Map Opacity");
    //heightMapOpacity
    LGDMATERIAL_READBITS( modifier.sections[7].elements[4].rangeBar.value, float, "Height Map Opacity");
    //ambientOcclusionOpacity
    LGDMATERIAL_READBITS( modifier.sections[7].elements[5].rangeBar.value, float, "Ambient Occlusion Opacity");

    // ---- Depth Masking & Depth Properties ----
    //depthValue
    LGDMATERIAL_READBITS( modifier.sections[8].elements[0].rangeBar.value, float, "Depth Value");
    //blurTheHeightMap
    LGDMATERIAL_READBITS( modifier.sections[8].elements[1].checkBox.clickState1, int, "blurTheHeightMap");
    
    // ---- Filter ----
    // albedoFilter
    modifier.sections[9].elements[0].button.filter.readFilterData(rf, true);
    modifier.sections[9].elements[0].button.texture = modifier.sections[9].elements[0].button.filter.displayingTxtr;
    if(!modifier.sections[9].elements[1].button.texture.readTextureData(rf, true, textureVersionCode, to_generate_txtrs == nullptr))
        return false;
    if(to_generate_txtrs != nullptr)
        to_generate_txtrs->push_back(&modifier.sections[9].elements[1].button.texture);

    return true;
}
bool readAsphaltModifier(std::ifstream& rf, MaterialModifier& modifier, int version, unsigned int textureVersionCode, std::vector<Texture*>* to_generate_txtrs){
    // ---- Colors ----
    //color1
    LGDMATERIAL_READBITS( modifier.sections[0].elements[0].button.color.r, float, "color1");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[0].button.color.g, float, "color1");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[0].button.color.b, float, "color1");
    //color2
    LGDMATERIAL_READBITS( modifier.sections[0].elements[1].button.color.r, float, "color2");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[1].button.color.g, float, "color2");
    LGDMATERIAL_READBITS(modifier.sections[0].elements[1].button.color.b, float, "color2");
    
    // ---- Dirt ----
    //scale
    LGDMATERIAL_READBITS( modifier.sections[1].elements[0].rangeBar.value, float, "scale");
    //strength
    LGDMATERIAL_READBITS( modifier.sections[1].elements[1].rangeBar.value, float, "strength");
    
    // ---- 2nd Color ----
    //noiseScale
    LGDMATERIAL_READBITS( modifier.sections[2].elements[0].rangeBar.value, float, "noiseScale");
    //noiseStrength
    LGDMATERIAL_READBITS( modifier.sections[2].elements[1].rangeBar.value, float, "noiseStrength");
    
    // ---- Noise ----
    //noiseStrength
    LGDMATERIAL_READBITS( modifier.sections[3].elements[0].rangeBar.value, float, "noiseStrength");

    // ---- Element Properties ----
    //wetness
    LGDMATERIAL_READBITS( modifier.sections[4].elements[0].rangeBar.value, float, "wetness");
    //metallic
    LGDMATERIAL_READBITS( modifier.sections[4].elements[1].rangeBar.value, float, "metallic");
    //height
    LGDMATERIAL_READBITS( modifier.sections[4].elements[2].rangeBar.value, float, "height");

    // ---- Perlin Properties ----
    //firstOctave
    LGDMATERIAL_READBITS( modifier.sections[5].elements[0].rangeBar.value, float, "firstOctave");
    //octaves
    LGDMATERIAL_READBITS( modifier.sections[5].elements[1].rangeBar.value, float, "octaves");
    //persistence
    LGDMATERIAL_READBITS( modifier.sections[5].elements[2].rangeBar.value, float, "persistence");

    // ---- Channel Opacities ----
    //albedoOpacity
    LGDMATERIAL_READBITS( modifier.sections[6].elements[0].rangeBar.value, float, "Albedo Opacity");
    //roughnessOpacity
    LGDMATERIAL_READBITS( modifier.sections[6].elements[1].rangeBar.value, float, "Roughness Opacity");
    //metallicOpacity
    LGDMATERIAL_READBITS( modifier.sections[6].elements[2].rangeBar.value, float, "Metallic Opacity");
    //normalMapOpacity
    LGDMATERIAL_READBITS( modifier.sections[6].elements[3].rangeBar.value, float, "Normal Map Opacity");
    //heightMapOpacity
    LGDMATERIAL_READBITS( modifier.sections[6].elements[4].rangeBar.value, float, "Height Map Opacity");
    //ambientOcclusionOpacity
    LGDMATERIAL_READBITS( modifier.sections[6].elements[5].rangeBar.value, float, "Ambient Occlusion Opacity");

    // ---- Depth Masking & Depth Properties ----
    //depthValue
    LGDMATERIAL_READBITS( modifier.sections[7].elements[0].rangeBar.value, float, "Depth Value");
    if(version >= 2100){
        //blurTheHeightMap
        LGDMATERIAL_READBITS( modifier.sections[7].elements[1].checkBox.clickState1, int, "blurTheHeightMap");
    }
    
    // ---- Filter ----
    // albedoFilter
    modifier.sections[8].elements[0].button.filter.readFilterData(rf, true);
    modifier.sections[8].elements[0].button.texture = modifier.sections[8].elements[0].button.filter.displayingTxtr;
    if(!modifier.sections[8].elements[1].button.texture.readTextureData(rf, true, textureVersionCode, to_generate_txtrs == nullptr))
        return false;
    if(to_generate_txtrs != nullptr)
        to_generate_txtrs->push_back(&modifier.sections[8].elements[1].button.texture);

    return true;
}
bool readDustModifier(std::ifstream& rf, MaterialModifier& modifier, int version, unsigned int textureVersionCode, std::vector<Texture*>* to_generate_txtrs){
    // ---- Noise ----
    //size
    LGDMATERIAL_READBITS( modifier.sections[0].elements[0].rangeBar.value, float, "size");
    //offsetIntensity
    LGDMATERIAL_READBITS( modifier.sections[0].elements[1].rangeBar.value, float, "offsetIntensity");
    //rotation
    LGDMATERIAL_READBITS( modifier.sections[0].elements[2].rangeBar.value, float, "rotation");
    //brightness
    LGDMATERIAL_READBITS( modifier.sections[0].elements[3].rangeBar.value, float, "brightness");
    
    // ---- Blur ----
    //intensity
    LGDMATERIAL_READBITS( modifier.sections[1].elements[0].rangeBar.value, float, "intensity");
    //direction
    LGDMATERIAL_READBITS( modifier.sections[1].elements[1].rangeBar.value, float, "direction");
    
    // ---- Scratches ----
    //wavyness
    LGDMATERIAL_READBITS( modifier.sections[2].elements[0].rangeBar.value, float, "wavyness");
    //scale
    LGDMATERIAL_READBITS( modifier.sections[2].elements[1].rangeBar.value, float, "scale");
    //baseFrequency
    LGDMATERIAL_READBITS( modifier.sections[2].elements[2].rangeBar.value, float, "baseFrequency");
    //frequencyStep
    LGDMATERIAL_READBITS( modifier.sections[2].elements[3].rangeBar.value, float, "frequencyStep");

    // ---- Droplets ----
    //count
    LGDMATERIAL_READBITS( modifier.sections[3].elements[0].rangeBar.value, float, "count");
    //opacityJitter
    LGDMATERIAL_READBITS( modifier.sections[3].elements[1].rangeBar.value, float, "opacityJitter");
    //size
    LGDMATERIAL_READBITS( modifier.sections[3].elements[2].rangeBar.value, float, "size");
    
    // ---- Colors ----
    //color1
    LGDMATERIAL_READBITS( modifier.sections[4].elements[0].button.color.r, float, "color1");
    LGDMATERIAL_READBITS(modifier.sections[4].elements[0].button.color.g, float, "color1");
    LGDMATERIAL_READBITS(modifier.sections[4].elements[0].button.color.b, float, "color1");
    //color2
    LGDMATERIAL_READBITS( modifier.sections[4].elements[1].button.color.r, float, "color2");
    LGDMATERIAL_READBITS(modifier.sections[4].elements[1].button.color.g, float, "color2");
    LGDMATERIAL_READBITS(modifier.sections[4].elements[1].button.color.b, float, "color2");
    //color3
    LGDMATERIAL_READBITS( modifier.sections[4].elements[2].button.color.r, float, "color3");
    LGDMATERIAL_READBITS(modifier.sections[4].elements[2].button.color.g, float, "color3");
    LGDMATERIAL_READBITS(modifier.sections[4].elements[2].button.color.b, float, "color3");

    // ---- Element Properties ----
    //wetness
    LGDMATERIAL_READBITS( modifier.sections[5].elements[0].rangeBar.value, float, "wetness");
    //metallic
    LGDMATERIAL_READBITS( modifier.sections[5].elements[1].rangeBar.value, float, "metallic");
    //height
    LGDMATERIAL_READBITS( modifier.sections[5].elements[2].rangeBar.value, float, "height");

    // ---- Channel Opacities ----
    //albedoOpacity
    LGDMATERIAL_READBITS( modifier.sections[6].elements[0].rangeBar.value, float, "Albedo Opacity");
    //roughnessOpacity
    LGDMATERIAL_READBITS( modifier.sections[6].elements[1].rangeBar.value, float, "Roughness Opacity");
    //metallicOpacity
    LGDMATERIAL_READBITS( modifier.sections[6].elements[2].rangeBar.value, float, "Metallic Opacity");
    //normalMapOpacity
    LGDMATERIAL_READBITS( modifier.sections[6].elements[3].rangeBar.value, float, "Normal Map Opacity");
    //heightMapOpacity
    LGDMATERIAL_READBITS( modifier.sections[6].elements[4].rangeBar.value, float, "Height Map Opacity");
    //ambientOcclusionOpacity
    LGDMATERIAL_READBITS( modifier.sections[6].elements[5].rangeBar.value, float, "Ambient Occlusion Opacity");

    // ---- Depth Masking & Depth Properties ----
    //depthValue
    LGDMATERIAL_READBITS( modifier.sections[7].elements[0].rangeBar.value, float, "Depth Value");
    //blurTheHeightMap
    LGDMATERIAL_READBITS( modifier.sections[7].elements[1].checkBox.clickState1, int, "blurTheHeightMap");
    
    // ---- Filter ----
    // albedoFilter
    modifier.sections[8].elements[0].button.filter.readFilterData(rf, true);
    modifier.sections[8].elements[0].button.texture = modifier.sections[8].elements[0].button.filter.displayingTxtr;
    if(!modifier.sections[8].elements[1].button.texture.readTextureData(rf, true, textureVersionCode, to_generate_txtrs == nullptr))
        return false;
    if(to_generate_txtrs != nullptr)
        to_generate_txtrs->push_back(&modifier.sections[8].elements[1].button.texture);

    return true;
}

bool readMathModifier(std::ifstream& rf, MaterialModifier& modifier, int version, unsigned int textureVersionCode, std::vector<Texture*>* to_generate_txtrs){
    // ---- Noise ----
    LGDMATERIAL_READBITS( modifier.sections[0].elements[0].comboBox.selectedIndex, int, "Operation Index");
    LGDMATERIAL_READBITS( modifier.sections[0].elements[3].rangeBar.value, float, "Right side value");
    LGDMATERIAL_READBITS( modifier.sections[0].elements[4].checkBox.clickState1, bool, "Right side use texture");
    if(!modifier.sections[0].elements[5].button.texture.readTextureData(rf, true, textureVersionCode, to_generate_txtrs == nullptr))
        return false;
    if(to_generate_txtrs != nullptr)
        to_generate_txtrs->push_back(&modifier.sections[0].elements[5].button.texture);

    // ---- Channel Opacities ----
    //albedoOpacity
    LGDMATERIAL_READBITS( modifier.sections[1].elements[0].rangeBar.value, float, "Albedo Opacity");
    //roughnessOpacity
    LGDMATERIAL_READBITS( modifier.sections[1].elements[1].rangeBar.value, float, "Roughness Opacity");
    //metallicOpacity
    LGDMATERIAL_READBITS( modifier.sections[1].elements[2].rangeBar.value, float, "Metallic Opacity");
    //normalMapOpacity
    LGDMATERIAL_READBITS( modifier.sections[1].elements[3].rangeBar.value, float, "Normal Map Opacity");
    //heightMapOpacity
    LGDMATERIAL_READBITS( modifier.sections[1].elements[4].rangeBar.value, float, "Height Map Opacity");
    //ambientOcclusionOpacity
    LGDMATERIAL_READBITS( modifier.sections[1].elements[5].rangeBar.value, float, "Ambient Occlusion Opacity");
    
    // ---- Filter ----
    // albedoFilter
    modifier.sections[8].elements[0].button.filter.readFilterData(rf, true);
    modifier.sections[8].elements[0].button.texture = modifier.sections[8].elements[0].button.filter.displayingTxtr;
    if(!modifier.sections[8].elements[1].button.texture.readTextureData(rf, true, textureVersionCode, to_generate_txtrs == nullptr))
        return false;
    if(to_generate_txtrs != nullptr)
        to_generate_txtrs->push_back(&modifier.sections[8].elements[1].button.texture);

    return true;
}