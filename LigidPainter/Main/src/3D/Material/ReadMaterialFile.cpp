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

void Material::readFile(std::string path,ColorPalette colorPalette ,Shader buttonShader ,AppTextures appTextures, const std::vector<Material> materials){
    if(path.size()){
        std::ifstream rf(path, std::ios::out | std::ios::binary);
        
        ;
        title = UTIL::getLastWordBySeparatingWithChar(path,UTIL::folderDistinguisher());
        title = UTIL::removeExtension(title);
        
        if(!rf) {
            std::cout << "ERROR WHILE READING MATERIAL FILE! Cannot open file : " << path << std::endl;
            return;
        }
        
        //!HEADER
        
        //!Description
        uint64_t h1 = 0xBBF2367A; 
        uint64_t h2 = 0xBB58BC66; 
        uint64_t h3 = 0xBBACB786; 
        
        uint64_t ch1; 
        uint64_t ch2; 
        uint64_t ch3; 

        rf.read(reinterpret_cast<char*>(   &ch1    ),sizeof(uint64_t));
        rf.read(reinterpret_cast<char*>(   &ch2    ),sizeof(uint64_t));
        rf.read(reinterpret_cast<char*>(   &ch3    ),sizeof(uint64_t));
        
        if(ch1 != h1 || ch2 != h2 || ch3 != h3){
            std::cout << "ERROR This is not a material file : " << path << std::endl;
            return;
        }

        //!ID
        rf.read(reinterpret_cast<char*>(   &this->uniqueID    ),sizeof(int));

        //!Modifiers
        uint64_t materialModifierSize;
        rf.read(reinterpret_cast<char*>(   &materialModifierSize     ),sizeof(uint64_t));
        
        for (size_t i = 0; i < materialModifierSize; i++)
        {
            size_t materialModifierModifierIndex;
            rf.read(reinterpret_cast<char*>(   &materialModifierModifierIndex     ),sizeof(uint64_t));
            
            MaterialModifier modifier(colorPalette , buttonShader , appTextures , materialModifierModifierIndex);

            // -- Mask texture --
            rf.read(reinterpret_cast<char*>(&modifier.maskTexture.proceduralID), sizeof(int));
            
            //Write the texture
            //Get the resolution of the texture from the panel of the modifier
            uint64_t txtrWidth;
            uint64_t txtrHeight;
            
            //Write the texture resolution data
            rf.read(reinterpret_cast<char*>(   &txtrWidth     ),sizeof(uint64_t));
            rf.read(reinterpret_cast<char*>(   &txtrHeight     ),sizeof(uint64_t));

            //Get pixels of the texture
            char* pixels = new char[txtrWidth * txtrHeight * 4];

            //Write the texture data
            rf.read(pixels , txtrWidth * txtrHeight * 4 * sizeof(char));
            
            modifier.maskTexture = Texture(pixels,txtrWidth,txtrHeight);

            delete[] pixels;
            //Write the procedural texture properties
            rf.read(reinterpret_cast<char*>(&modifier.maskTexture.proceduralnverted), sizeof(int));
            rf.read(reinterpret_cast<char*>(&modifier.maskTexture.proceduralScale), sizeof(float));

            if(materialModifierModifierIndex == TEXTURE_MATERIAL_MODIFIER){ //Is a texture modifier
                for (size_t channelI = 0; channelI < 6; channelI++) //For each material channel (albedo, roughness, metallic, normal map, etc.)
                {
                    //Get the resolution of the texture from the panel of the modifier
                    uint64_t txtrWidth;
                    uint64_t txtrHeight;
                    
                    //Read the texture resolution data
                    rf.read(reinterpret_cast<char*> (       &txtrWidth          )       ,sizeof(uint64_t));
                    rf.read(reinterpret_cast<char*> (       &txtrHeight         )       ,sizeof(uint64_t));
    
                    //Get pixels of the texture
                    char* pixels = new char[txtrWidth * txtrHeight * 4];

                    //Read the texture data
                    rf.read(pixels , txtrWidth * txtrHeight * 4 * sizeof(char));

                    //Give the texture to the panel of the modifier
                    modifier.sections[0].elements[channelI].button.texture = Texture(pixels,txtrWidth,txtrHeight);

                    delete[] pixels;
                }

                //Write the opacity values
                for (size_t channelI = 0; channelI < 6; channelI++) {
                    rf.read(reinterpret_cast<char*>(&modifier.sections[1].elements[channelI].rangeBar.value), sizeof(float));
                }
            }
            else{
                uint64_t sectionSize;
                rf.read(reinterpret_cast<char*>(sectionSize), sizeof(uint64_t));
                for (size_t secI = 0; secI < sectionSize; secI++)
                {
                    uint64_t elementSize;
                    rf.read(reinterpret_cast<char*>(elementSize), sizeof(uint64_t));
                    for (size_t eI = 0; eI < elementSize; eI++)
                    {
                        //Is rangeBar
                        if(modifier.sections[secI].elements[eI].state == 1)
                            rf.read(reinterpret_cast<char*>(   &modifier.sections[secI].elements[eI].rangeBar.value     ), sizeof(float));
                        
                        //Is button
                        else if(modifier.sections[secI].elements[eI].state == 0){
                            rf.read(reinterpret_cast<char*>(   &modifier.sections[secI].elements[eI].button.color.r     ), sizeof(float));
                            rf.read(reinterpret_cast<char*>(   &modifier.sections[secI].elements[eI].button.color.g     ), sizeof(float));
                            rf.read(reinterpret_cast<char*>(   &modifier.sections[secI].elements[eI].button.color.b     ), sizeof(float));
                        }
                    }
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