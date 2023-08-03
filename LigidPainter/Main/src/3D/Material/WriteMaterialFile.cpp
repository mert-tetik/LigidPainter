/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

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


void Material::writeFile(std::string path){
    //Saves the brush file
    if(path == ""){
        path = showFileSystemObjectSelectionDialog("Select a folder to export the material file", "", {}, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER);
    }
    
    if(path.size()){

        std::ofstream wf(path, std::ios::out | std::ios::binary);

        if(!wf) {
            std::cout << "ERROR WHILE WRITING MATERIAL FILE! Cannot open file : " << path << std::endl;
            return;
        }
        
        //!HEADER
        
        //Description 
        uint64_t h1 = 0x5A7D0C809A22F3B7; 
        uint64_t h2 = 0xE8F691BE0D45C23A; 
        uint64_t h3 = 0x3FD8A9657B101E8C; 
        wf.write(reinterpret_cast<char*>(   &h1    ),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(   &h2    ),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(   &h3    ),sizeof(uint64_t));
        
        //Version number
        uint32_t versionNumber = 0x000007D0; //2000 
        wf.write(reinterpret_cast<char*>(   &versionNumber    ), sizeof(uint32_t));
        
        //ID
        wf.write(reinterpret_cast<char*>(   &this->uniqueID    ), sizeof(int));

        //!Modifiers
        uint64_t materialModifierSize = materialModifiers.size();
        wf.write(reinterpret_cast<char*>(   &materialModifierSize     ), sizeof(uint64_t));

        for (size_t i = 0; i < materialModifierSize; i++)
        {
            //Modifier index
            wf.write(reinterpret_cast<char*>(   &materialModifiers[i].modifierIndex     ), sizeof(int));
            
            //Mask texture
            wf.write(reinterpret_cast<char*>(   &materialModifiers[i].maskTexture.proceduralID     ), sizeof(int));
            wf.write(reinterpret_cast<char*>(   &materialModifiers[i].maskTexture.proceduralnverted     ), sizeof(int));
            wf.write(reinterpret_cast<char*>(   &materialModifiers[i].maskTexture.proceduralScale     ), sizeof(float));

            if(materialModifiers[i].maskTexture.proceduralID == -1){
                int32_t textureWidth = materialModifiers[i].maskTexture.getResolution().x;
                wf.write(reinterpret_cast<char*>(   &textureWidth     ), sizeof(int32_t));

                int32_t textureHeight = materialModifiers[i].maskTexture.getResolution().y;
                wf.write(reinterpret_cast<char*>(   &textureHeight     ), sizeof(int32_t));

                char* pixels = new char[textureWidth * textureHeight * 4];
                materialModifiers[i].maskTexture.getData(pixels);
                wf.write(pixels, textureWidth * textureHeight * 4 * sizeof(char));
            
                delete[] pixels;
            }

            uint32_t propertySize = 0;
            std::vector<std::string> propTitles;
            std::vector<Element> propElements;

            for (size_t sI = 0; sI < materialModifiers[i].sections.size(); sI++)
            {
                propertySize += materialModifiers[i].sections[sI].elements.size();
                for (size_t eI = 0; eI < materialModifiers[i].sections[sI].elements.size(); eI++)
                {
                    propElements.push_back(materialModifiers[i].sections[sI].elements[eI]);
                    
                    if(materialModifiers[i].sections[sI].elements[eI].state == 0)
                        propTitles.push_back(materialModifiers[i].sections[sI].header.button.text + "-" + materialModifiers[i].sections[sI].elements[eI].button.text);
                    else if(materialModifiers[i].sections[sI].elements[eI].state == 1)
                        propTitles.push_back(materialModifiers[i].sections[sI].header.button.text + "-" + materialModifiers[i].sections[sI].elements[eI].rangeBar.text);
                    else{
                        std::cout << "ERROR : Unknown GUI element type : " << materialModifiers[i].sections[sI].elements[eI].state << ". Failed to write the material file" << std::endl;
                        wf.close();
                        
                        if(std::filesystem::exists(path))
                            std::filesystem::remove(path);

                        return;
                    }
                }
            }
            
            wf.write(reinterpret_cast<char*>(   &propertySize     ), sizeof(uint32_t));

            for (size_t i = 0; i < propertySize; i++)
            {
                uint32_t titleSize = propTitles[i].size();
                wf.write(reinterpret_cast<char*>(   &titleSize     ), sizeof(uint32_t));
                
                for (size_t charI = 0; charI < titleSize; charI++)
                {
                    wf.write(reinterpret_cast<char*>(   &propTitles[i][charI]     ), sizeof(char));
                }
                
                //Is a rangebar
                if(propElements[i].state == 1){
                    char valueType = 'f';
                    wf.write(reinterpret_cast<char*>(   &valueType     ), sizeof(char));
                
                    wf.write(reinterpret_cast<char*>(   &propElements[i].rangeBar.value     ), sizeof(float));
                }
                
                //Is a button
                if(propElements[i].state == 0){
                    char valueType = '3';
                    wf.write(reinterpret_cast<char*>(   &valueType     ), sizeof(char));
                    
                    wf.write(reinterpret_cast<char*>(   &propElements[i].button.color.r     ), sizeof(float));
                    wf.write(reinterpret_cast<char*>(   &propElements[i].button.color.g     ), sizeof(float));
                    wf.write(reinterpret_cast<char*>(   &propElements[i].button.color.b     ), sizeof(float));
                }
            }
        }        
    }
}