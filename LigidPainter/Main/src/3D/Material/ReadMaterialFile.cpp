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
 
            //Mask texture
            rf.read(reinterpret_cast<char*>(   &modifier.maskTexture.proceduralID     ), sizeof(int));
            rf.read(reinterpret_cast<char*>(   &modifier.maskTexture.proceduralnverted     ), sizeof(int));
            rf.read(reinterpret_cast<char*>(   &modifier.maskTexture.proceduralScale     ), sizeof(float));

            if(modifier.maskTexture.proceduralID == -1){
                int32_t textureWidth = modifier.maskTexture.getResolution().x;
                rf.read(reinterpret_cast<char*>(   &textureWidth     ), sizeof(int32_t));

                int32_t textureHeight = modifier.maskTexture.getResolution().y;
                rf.read(reinterpret_cast<char*>(   &textureHeight     ), sizeof(int32_t));

                char* pixels = new char[textureWidth * textureHeight * 4];
                rf.read(pixels, textureWidth * textureHeight * 4 * sizeof(char));
            
                modifier.maskTexture = Texture(pixels, textureWidth, textureHeight);

                delete[] pixels;
            }

            uint32_t propertySize = 0;
            std::vector<std::string> propTitles;
            std::vector<Element> propElements;

            for (size_t sI = 0; sI < modifier.sections.size(); sI++)
            {
                propertySize += modifier.sections[sI].elements.size();
                for (size_t eI = 0; eI < modifier.sections[sI].elements.size(); eI++)
                {
                    propElements.push_back(modifier.sections[sI].elements[eI]);
                    
                    if(modifier.sections[sI].elements[eI].state == 0)
                        propTitles.push_back(modifier.sections[sI].header.button.text + "-" + modifier.sections[sI].elements[eI].button.text);
                    else if(modifier.sections[sI].elements[eI].state == 1)
                        propTitles.push_back(modifier.sections[sI].header.button.text + "-" + modifier.sections[sI].elements[eI].rangeBar.text);
                    else{
                        std::cout << "ERROR : Unknown GUI element type : " << modifier.sections[sI].elements[eI].state << ". Failed to write the material file" << std::endl;
                        wf.close();
                        
                        if(std::filesystem::exists(path))
                            std::filesystem::remove(path);

                        return;
                    }
                }
            }
            
            rf.read(reinterpret_cast<char*>(   &propertySize     ), sizeof(uint32_t));

            for (size_t i = 0; i < propertySize; i++)
            {
                uint32_t titleSize = propTitles[i].size();
                rf.read(reinterpret_cast<char*>(   &titleSize     ), sizeof(uint32_t));
                
                for (size_t charI = 0; charI < titleSize; charI++)
                {
                    rf.read(reinterpret_cast<char*>(   &propTitles[i][charI]     ), sizeof(char));
                }
                
                //Is a rangebar
                if(propElements[i].state == 1){
                    char valueType = 'f';
                    rf.read(reinterpret_cast<char*>(   &valueType     ), sizeof(char));
                
                    rf.read(reinterpret_cast<char*>(   &propElements[i].rangeBar.value     ), sizeof(float));
                }
                
                //Is a button
                if(propElements[i].state == 0){
                    char valueType = '3';
                    rf.read(reinterpret_cast<char*>(   &valueType     ), sizeof(char));
                    
                    rf.read(reinterpret_cast<char*>(   &propElements[i].button.color.r     ), sizeof(float));
                    rf.read(reinterpret_cast<char*>(   &propElements[i].button.color.g     ), sizeof(float));
                    rf.read(reinterpret_cast<char*>(   &propElements[i].button.color.b     ), sizeof(float));
                }
            }
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