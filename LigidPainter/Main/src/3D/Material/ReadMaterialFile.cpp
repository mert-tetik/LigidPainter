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

void Material::readFile(std::string path,ColorPalette colorPalette ,Shader buttonShader ,AppTextures appTextures, AppMaterialModifiers appMaterialModifiers, const std::vector<Material> materials){
    if(path.size()){
        std::ifstream rf(path, std::ios::out | std::ios::binary);
        
        title = UTIL::getLastWordBySeparatingWithChar(path,UTIL::folderDistinguisher());
        title = UTIL::removeExtension(title);
        
        if(!rf) {
            std::cout << "ERROR WHILE READING MATERIAL FILE! Cannot open file : " << path << std::endl;
            return;
        }
        
        //!HEADER
        
        //Description 
        uint64_t h1; 
        uint64_t h2; 
        uint64_t h3; 
        uint64_t r1 = 0x5A7D0C809A22F3B7; 
        uint64_t r2 = 0xE8F691BE0D45C23A; 
        uint64_t r3 = 0x3FD8A9657B101E8C; 
        rf.read(reinterpret_cast<char*>(   &h1    ),sizeof(uint64_t));
        rf.read(reinterpret_cast<char*>(   &h2    ),sizeof(uint64_t));
        rf.read(reinterpret_cast<char*>(   &h3    ),sizeof(uint64_t));
        
        if(h1 != r1 || h2 != r2 || h3 != r3){
            std::cout << "ERROR WHILE READING MATERIAL FILE! Description header is not correct." << std::endl;
            return;
        }

        //Version number(
        uint32_t versionNumber = 0x000007D0; //2000 
        rf.read(reinterpret_cast<char*>(   &versionNumber    ), sizeof(uint32_t));
        
        if(versionNumber != 0x000007D0){
            std::cout << "WARNING! : Material version number was : " << versionNumber << ". Results might be unexpected." << std::endl; 
        }

        //ID
        rf.read(reinterpret_cast<char*>(   &this->uniqueID    ), sizeof(int));

        //!Modifiers

        //Read the material modifier size
        uint64_t materialModifierSize;
        rf.read(reinterpret_cast<char*>(   &materialModifierSize     ), sizeof(uint64_t));

        this->materialModifiers.clear();

        for (size_t i = 0; i < materialModifierSize; i++)
        {

            int modifierIndex = NULL;
            //Modifier index
            rf.read(reinterpret_cast<char*>(   &modifierIndex     ), sizeof(int));
            
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
            }
            int proceduralID;
            int proceduralInvert;
            float proceduralScale;

            //Mask texture
            rf.read(reinterpret_cast<char*>(   &proceduralID     ), sizeof(int));
            rf.read(reinterpret_cast<char*>(   &proceduralInvert     ), sizeof(int));
            rf.read(reinterpret_cast<char*>(   &proceduralScale     ), sizeof(float));

            int32_t textureWidth = modifier.maskTexture.getResolution().x;
            rf.read(reinterpret_cast<char*>(   &textureWidth     ), sizeof(int32_t));

            int32_t textureHeight = modifier.maskTexture.getResolution().y;
            rf.read(reinterpret_cast<char*>(   &textureHeight     ), sizeof(int32_t));

            char* pixels = new char[textureWidth * textureHeight * 4];
            rf.read(pixels, textureWidth * textureHeight * 4 * sizeof(char));
        
            modifier.maskTexture = Texture(pixels, textureWidth, textureHeight);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            modifier.maskTexture.proceduralID = proceduralID;
            modifier.maskTexture.proceduralnverted = proceduralInvert;
            modifier.maskTexture.proceduralScale = proceduralScale;

            delete[] pixels;

            uint32_t propertySize;
            rf.read(reinterpret_cast<char*>(   &propertySize     ), sizeof(uint32_t));

            for (size_t i = 0; i < propertySize; i++)
            {
                uint32_t titleSize;
                rf.read(reinterpret_cast<char*>(   &titleSize     ), sizeof(uint32_t));
                
                std::string titleCP;
                for (size_t charI = 0; charI < titleSize; charI++)
                {
                    char c;
                    rf.read(reinterpret_cast<char*>(   &c     ), sizeof(char));
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
                rf.read(reinterpret_cast<char*>(   &valueType     ), sizeof(char));

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
                        else if(valueType == '3'){
                            if(modifier.sections[ii].header.button.text == sectionTitle && modifier.sections[ii].elements[iii].button.text == elementTitle){
                                secI = ii;
                                elementI = iii;
                            }
                        }
                    }
                }
                
                float valF;
                glm::vec3 val3;

                //Is a rangebar
                if(valueType == 'f'){
                    rf.read(reinterpret_cast<char*>(   &valF     ), sizeof(float));
                }
                
                //Is a button
                else if(valueType == '3'){
                    rf.read(reinterpret_cast<char*>(   &val3.r     ), sizeof(float));
                    rf.read(reinterpret_cast<char*>(   &val3.g     ), sizeof(float));
                    rf.read(reinterpret_cast<char*>(   &val3.b     ), sizeof(float));
                }
                else {
                    std::cout << "ERROR! : Reading material file : Unknown value type" << std::endl;
                    return;
                }
                
                if(secI != -1 && elementI != -1){
                    if(valueType == 'f')
                        modifier.sections[secI].elements[elementI].rangeBar.value = valF;
                    if(valueType == '3'){
                        modifier.sections[secI].elements[elementI].button.color.r = val3.r;
                        modifier.sections[secI].elements[elementI].button.color.g = val3.g;
                        modifier.sections[secI].elements[elementI].button.color.b = val3.b;
                    }
                }
                else{
                    std::cout << "WARNING! : Reading material file : Unknown property." << std::endl;
                }
            }

            this->materialModifiers.push_back(modifier);
        }    
    }

    //Check if there is already a material with the same ID
    bool uniqueIDRepeats = false; 
    for (size_t i = 0; i < materials.size(); i++)
    {
        if(materials[i].uniqueID == this->uniqueID)
            uniqueIDRepeats = true;
    }
    
    //If there is give a new unique id
    if(uniqueIDRepeats){
        UTIL::giveUniqueId(this->uniqueID, materials);
    }
}