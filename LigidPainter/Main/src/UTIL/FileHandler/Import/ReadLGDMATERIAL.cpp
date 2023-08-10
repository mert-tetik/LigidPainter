/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

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
#include "3D/ThreeD.hpp"
#include "GUI/GUI.hpp"

#define LGDMATERIAL_READBITS(var, type, loc) if(!rf.read(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                std::cout << "ERROR : Reading lgdmaterial file. Failed to read at : " << loc << std::endl;\
                                return false; \
                            }


bool FileHandler::readLGDMATERIALFile(
                                        std::string path, 
                                        Material& material, 
                                        ColorPalette colorPalette,
                                        
                                        AppTextures appTextures, 
                                        AppMaterialModifiers appMaterialModifiers,
                                        const std::vector<Material> materials
                                    )
{
    if(path.size()){
        std::ifstream rf(path, std::ios::out | std::ios::binary);
        
        material.title = UTIL::getLastWordBySeparatingWithChar(path, UTIL::folderDistinguisher());
        material.title = UTIL::removeExtension(material.title);
        
        if(!rf) {
            std::cout << "ERROR WHILE READING MATERIAL FILE! Cannot open file : " << path << std::endl;
            return false;
        }
        
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
            std::cout << "ERROR WHILE READING MATERIAL FILE! Description header is not correct." << std::endl;
            return false;
        }

        //Version number
        uint32_t versionNumber = 0x000007D0; //2000 
        LGDMATERIAL_READBITS(versionNumber, uint32_t, "Version number");

        if(versionNumber != 0x000007D0){
            std::cout << "WARNING! : Material version number was : " << versionNumber << ". Results might be unexpected." << std::endl; 
        }

        //ID
        LGDMATERIAL_READBITS(material.uniqueID, int, "Material ID");

        //!Modifiers

        //Read the material modifier size
        uint64_t materialModifierSize;
        LGDMATERIAL_READBITS(materialModifierSize, uint64_t, "Material modifier size");

        material.materialModifiers.clear();

        for (size_t i = 0; i < materialModifierSize; i++)
        {

            int modifierIndex = NULL;
            LGDMATERIAL_READBITS(modifierIndex, int, "Modifier type index");
            
            MaterialModifier modifier;
            if(modifierIndex == 0)
                modifier = appMaterialModifiers.textureModifier;
            else if(modifierIndex == 1)
                modifier = appMaterialModifiers.dustModifier;
            else if(modifierIndex == 2)
                modifier = appMaterialModifiers.asphaltModifier;
            else if(modifierIndex == 3)
                modifier = appMaterialModifiers.fabricModifier;
            else if(modifierIndex == 4)
                modifier = appMaterialModifiers.mossModifier;
            else if(modifierIndex == 5)
                modifier = appMaterialModifiers.rustModifier;
            else if(modifierIndex == 6)
                modifier = appMaterialModifiers.skinModifier;
            else if(modifierIndex == 7)
                modifier = appMaterialModifiers.solidModifier;
            else if(modifierIndex == 8)
                modifier = appMaterialModifiers.woodenModifier;
            else{
                std::cout << "ERROR : Reading material file : Unknown modifier index." << std::endl;
                return false;
            }

            

            int proceduralID;
            int proceduralInvert;
            float proceduralScale;

            //-- Mask texture --
            LGDMATERIAL_READBITS(proceduralID, int, "Procedural ID");
            LGDMATERIAL_READBITS(proceduralInvert, int, "Procedural data - inverted color");
            LGDMATERIAL_READBITS(proceduralScale, float, "Procedural data - scale");

            //Texture data (displaying texture if the procedural texture is used)
            int32_t textureWidth = modifier.maskTexture.getResolution().x;
            LGDMATERIAL_READBITS(textureWidth, int32_t, "Modifier's mask texture - texture width");

            int32_t textureHeight = modifier.maskTexture.getResolution().y;
            LGDMATERIAL_READBITS(textureHeight, int32_t, "Modifier's mask texture - texture height");

            char* pixels = new char[textureWidth * textureHeight * 4];
            if(!rf.read(pixels, textureWidth * textureHeight * 4 * sizeof(char))){
                std::cout << "ERROR : Reading lgdmaterial file. Failed to read at : Modifier's mask texture - texture pixels" << std::endl;
                return false; 
            }
        
            modifier.maskTexture = Texture(pixels, textureWidth, textureHeight);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            modifier.maskTexture.proceduralID = proceduralID;
            modifier.maskTexture.proceduralnverted = proceduralInvert;
            modifier.maskTexture.proceduralScale = proceduralScale;

            delete[] pixels;

            uint32_t propertySize;
            LGDMATERIAL_READBITS(propertySize, uint32_t, "Modifier's properties - property size");

            for (size_t i = 0; i < propertySize; i++)
            {
                uint32_t titleSize;
                LGDMATERIAL_READBITS(titleSize, uint32_t, "Modifier's property - title size");
                
                std::string titleCP;
                for (size_t charI = 0; charI < titleSize; charI++)
                {
                    char c;
                    LGDMATERIAL_READBITS(c, char, "Modifier's property - title");
                    titleCP.push_back(c);
                }

                std::string sectionTitle;
                std::string elementTitle;
                bool interactedWithSeperator = false;
                for (size_t i = 0; i < titleCP.size(); i++)
                {
                    if(titleCP[i] == '-'){
                        interactedWithSeperator = true;
                        i++;
                        if(i == titleSize)
                            break;
                    }

                    if(interactedWithSeperator)
                        elementTitle.push_back(titleCP[i]);
                    else
                        sectionTitle.push_back(titleCP[i]);
                }

                char valueType;
                LGDMATERIAL_READBITS(valueType, char, "Modifier's property - value type");

                int secI = -1;
                int elementI = -1;

                for (size_t ii = 0; ii < modifier.sections.size(); ii++)
                {
                    for (size_t iii = 0; iii < modifier.sections[ii].elements.size(); iii++)
                    {
                        if(valueType == 'f'){
                            if(modifier.sections[ii].header.button.text == sectionTitle && modifier.sections[ii].elements[iii].rangeBar.text == elementTitle){
                                secI = ii;
                                elementI = iii;
                            }
                        }
                        else if(valueType == '3' || valueType == 't'){
                            if(modifier.sections[ii].header.button.text == sectionTitle && modifier.sections[ii].elements[iii].button.text == elementTitle){
                                secI = ii;
                                elementI = iii;
                            }
                        }
                    }
                }
                
                float valF;
                glm::vec3 val3;
                Texture valT;

                //Is a rangebar
                if(valueType == 'f'){
                    LGDMATERIAL_READBITS(valF, float, "Modifier's property - float data");
                }
                
                //Is a button
                else if(valueType == '3'){
                    LGDMATERIAL_READBITS(val3.r, float, "Modifier's property - vec3 data x axis");
                    LGDMATERIAL_READBITS(val3.g, float, "Modifier's property - vec3 data y axis");
                    LGDMATERIAL_READBITS(val3.b, float, "Modifier's property - vec3 data z axis");
                }
                
                else if(valueType == 't'){
                    int proceduralID;
                    int proceduralInvert;
                    float proceduralScale;
                    LGDMATERIAL_READBITS(proceduralID, int, "Modifier's property - texture data - procedural ID");
                    LGDMATERIAL_READBITS(proceduralInvert, int, "Modifier's property - texture data - procedural invert");
                    LGDMATERIAL_READBITS(proceduralScale, float, "Modifier's property - texture data - procedural scale");

                    int32_t textureWidth = valT.getResolution().x;
                    LGDMATERIAL_READBITS(textureWidth, int32_t, "Modifier's property - texture data - texture width");

                    int32_t textureHeight = valT.getResolution().y;
                    LGDMATERIAL_READBITS(textureHeight, int32_t, "Modifier's property - texture data - texture height");

                    char* pixels = new char[textureWidth * textureHeight * 4];
                    if(!rf.read(pixels, textureWidth * textureHeight * 4 * sizeof(char))){
                        std::cout << "ERROR : Reading lgdmaterial file. Failed to read at : Modifier's property - texture data - texture pixels" << std::endl;
                        return false;   
                    }
                
                    valT = Texture(pixels, textureWidth, textureHeight);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    valT.proceduralID = proceduralID;
                    valT.proceduralnverted = proceduralInvert;
                    valT.proceduralScale = proceduralScale;
                }
                else {
                    std::cout << "ERROR! : Reading material file : Unknown value type" << std::endl;
                    return false;
                }
                
                if(secI != -1 && elementI != -1){
                    if(valueType == 'f')
                        modifier.sections[secI].elements[elementI].rangeBar.value = valF;
                    if(valueType == '3'){
                        modifier.sections[secI].elements[elementI].button.color.r = val3.r;
                        modifier.sections[secI].elements[elementI].button.color.g = val3.g;
                        modifier.sections[secI].elements[elementI].button.color.b = val3.b;
                    }
                    if(valueType == 't'){
                        modifier.sections[secI].elements[elementI].button.texture = valT;
                    }
                }
                else{
                    std::cout << "WARNING! : Reading material file : Unknown property." << std::endl;
                }
            }

            material.materialModifiers.push_back(modifier);
        }    
    }

    //Check if there is already a material with the same ID
    bool uniqueIDRepeats = false; 
    for (size_t i = 0; i < materials.size(); i++)
    {
        if(materials[i].uniqueID == material.uniqueID)
            uniqueIDRepeats = true;
    }
    
    //If there is give a new unique id
    if(uniqueIDRepeats){
        UTIL::giveUniqueId(material.uniqueID, materials);
    }

    return true;
}