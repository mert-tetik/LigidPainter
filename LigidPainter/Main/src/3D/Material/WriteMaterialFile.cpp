/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Write *.lgdmaterial file

*/

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "3D/ThreeD.hpp"
#include "GUI/GUI.hpp"


void Material::writeFile(std::string path){
    //Saves the brush file
    if(path == ""){
        path = showFileSystemObjectSelectionDialog("Select a folder to export the material file", "", {}, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER);
    }
    
    if(path.size()){

        std::ofstream wf(path + folderDistinguisher + title + ".lgdmaterial", std::ios::out | std::ios::binary);

        if(!wf) {
            std::cout << "ERROR WHILE WRITING MATERIAL FILE! Cannot open file : " << path << std::endl;
            return;
        }
        
        //!HEADER
        
        //!Description
        uint64_t h1 = 0xBBF2367A; 
        uint64_t h2 = 0xBB58BC66; 
        uint64_t h3 = 0xBBACB786; 

        wf.write(reinterpret_cast<char*>(   &h1    ),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(   &h2    ),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(   &h3    ),sizeof(uint64_t));
        
        //!ID
        wf.write(reinterpret_cast<char*>(   &this->uniqueID    ),sizeof(int));

        //!Modifiers
        uint64_t materialModifierSize = materialModifiers.size();
        wf.write(reinterpret_cast<char*>(   &materialModifierSize     ),sizeof(uint64_t));
        
        for (size_t i = 0; i < materialModifierSize; i++)
        {

            size_t materialModifierModifierIndex = materialModifiers[i].modifierIndex;
            wf.write(reinterpret_cast<char*>(   &materialModifierModifierIndex     ),sizeof(uint64_t));

            if(materialModifierModifierIndex == TEXTURE_MATERIAL_MODIFIER){ //Is a texture modifier
                for (size_t channelI = 0; channelI < 6; channelI++) //For each material channel (albedo, roughness, metallic, normal map, etc.)
                {
                    //Get the resolution of the texture from the panel of the modifier
                    uint64_t txtrWidth = this->materialModifiers[i].sections[0].elements[channelI].button.texture.getResolution().x;
                    uint64_t txtrHeight = this->materialModifiers[i].sections[0].elements[channelI].button.texture.getResolution().y;
                    
                    //Write the texture resolution data
                    wf.write(reinterpret_cast<char*>(   &txtrWidth     ),sizeof(uint64_t));
                    wf.write(reinterpret_cast<char*>(   &txtrHeight     ),sizeof(uint64_t));
    
                    //Get pixels of the texture
                    char* pixels = new char[txtrWidth * txtrHeight * 4];
                    this->materialModifiers[i].sections[0].elements[channelI].button.texture.getData(pixels);

                    //Write the texture data
                    wf.write(pixels , txtrWidth * txtrHeight * 4 * sizeof(char));

                    delete[] pixels;
                }
            }
        }
    }
}