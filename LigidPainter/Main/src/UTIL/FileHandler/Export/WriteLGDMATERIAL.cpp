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

#define LGDMATERIAL_WRITEBITS(var, type, loc) if(!wf.write(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                    LGDLOG::start<< "ERROR : Writing lgdmaterial file. Failed to write at : " << loc << LGDLOG::end;\
                                    return false; \
                                }

bool FileHandler::writeLGDMATERIALFile(
                                        std::string path, 
                                        Material &material)
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

        std::ofstream wf(path, std::ios::out | std::ios::binary);

        if(!wf) {
            LGDLOG::start<< "ERROR WHILE WRITING MATERIAL FILE! Cannot open file : " << path << LGDLOG::end;
            
            return false;
        }
        
        //!HEADER
        
        //Description 
        uint64_t h1 = 0x5A7D0C809A22F3B7; 
        uint64_t h2 = 0xE8F691BE0D45C23A; 
        uint64_t h3 = 0x3FD8A9657B101E8C; 
        LGDMATERIAL_WRITEBITS(h1, uint64_t, "Description 1st");
        LGDMATERIAL_WRITEBITS(h2, uint64_t, "Description 2nd");
        LGDMATERIAL_WRITEBITS(h3, uint64_t, "Description 3rd");
        
        //Version number
        uint32_t versionNumber = 0x000007D0; //2000 
        LGDMATERIAL_WRITEBITS(versionNumber, uint32_t, "Version number");
        
        //ID
        LGDMATERIAL_WRITEBITS(material.uniqueID, int, "Material ID");

        //!Modifiers
        uint64_t materialModifierSize = material.materialModifiers.size();
        LGDMATERIAL_WRITEBITS(materialModifierSize, uint64_t, "Modifier size");

        for (size_t i = 0; i < materialModifierSize; i++)
        {
            //Modifier index
            LGDMATERIAL_WRITEBITS(material.materialModifiers[i].modifierIndex, int, "Modifier index");
            
            //Mask texture
            LGDMATERIAL_WRITEBITS(material.materialModifiers[i].maskTexture.proceduralID, int, "Mask texture - procedural ID");
            LGDMATERIAL_WRITEBITS(material.materialModifiers[i].maskTexture.proceduralnverted, int, "Mask texture - procedural inverted");
            LGDMATERIAL_WRITEBITS(material.materialModifiers[i].maskTexture.proceduralScale, float, "Mask texture - procedural scale");

            int32_t textureWidth = material.materialModifiers[i].maskTexture.getResolution().x;
            LGDMATERIAL_WRITEBITS(textureWidth, int32_t, "Mask texture - texture width");

            int32_t textureHeight = material.materialModifiers[i].maskTexture.getResolution().y;
            LGDMATERIAL_WRITEBITS(textureHeight, int32_t, "Mask texture - texture height");

            char* pixels = new char[textureWidth * textureHeight * 4];
            material.materialModifiers[i].maskTexture.getData(pixels);
            if(!wf.write(pixels, textureWidth * textureHeight * 4 * sizeof(char))){
                LGDLOG::start<< "ERROR : Writing lgdmaterial file. Failed to write at : " << "Mask texture - texture pixels" << LGDLOG::end;
                return false; 
            }
        
            delete[] pixels;

            uint32_t propertySize = 0;
            std::vector<std::string> propTitles;
            std::vector<Element> propElements;

            for (size_t sI = 0; sI < material.materialModifiers[i].sections.size(); sI++)
            {
                propertySize += material.materialModifiers[i].sections[sI].elements.size();
                for (size_t eI = 0; eI < material.materialModifiers[i].sections[sI].elements.size(); eI++)
                {
                    propElements.push_back(material.materialModifiers[i].sections[sI].elements[eI]);
                    
                    if(material.materialModifiers[i].sections[sI].elements[eI].state == 0)
                        propTitles.push_back(material.materialModifiers[i].sections[sI].header.button.text + "-" + material.materialModifiers[i].sections[sI].elements[eI].button.text);
                    else if(material.materialModifiers[i].sections[sI].elements[eI].state == 1)
                        propTitles.push_back(material.materialModifiers[i].sections[sI].header.button.text + "-" + material.materialModifiers[i].sections[sI].elements[eI].rangeBar.text);
                    else if(material.materialModifiers[i].sections[sI].elements[eI].state == 2)
                        propTitles.push_back(material.materialModifiers[i].sections[sI].header.button.text + "-" + material.materialModifiers[i].sections[sI].elements[eI].checkBox.text);
                    else{
                        LGDLOG::start<< "ERROR : Unknown GUI element type : " << material.materialModifiers[i].sections[sI].elements[eI].state << ". Failed to write the material file" << LGDLOG::end;
                        wf.close();
                        
                        return false;
                    }
                }
            }
            
            LGDMATERIAL_WRITEBITS(propertySize, uint32_t, "Property size");

            for (size_t pI = 0; pI < propertySize; pI++)
            {
                uint32_t titleSize = propTitles[pI].size();
                LGDMATERIAL_WRITEBITS(titleSize, uint32_t, "Property title size");
                
                for (size_t charI = 0; charI < titleSize; charI++)
                {
                    LGDMATERIAL_WRITEBITS(propTitles[pI][charI], char, "Property title");
                }

                //Is a rangebar
                if(propElements[pI].state == 1){
                    char valueType = 'f';
                    LGDMATERIAL_WRITEBITS(valueType, char, "Property value type");
                
                    LGDMATERIAL_WRITEBITS(propElements[pI].rangeBar.value, float, "Property value");
                }
                
                if(propElements[pI].state == 2){
                    char valueType = 'b';
                    LGDMATERIAL_WRITEBITS(valueType, char, "Property value type");
                
                    LGDMATERIAL_WRITEBITS(propElements[pI].checkBox.clickState1, bool, "Property value");
                }
                
                //Is a button
                if(propElements[pI].state == 0){
                    if(material.materialModifiers[i].modifierIndex == TEXTURE_MATERIAL_MODIFIER){
                        char valueType = 't';
                        LGDMATERIAL_WRITEBITS(valueType, char, "Property type");

                        LGDMATERIAL_WRITEBITS(propElements[pI].button.texture.proceduralID, int, "Property texture - procedural ID");
                        LGDMATERIAL_WRITEBITS(propElements[pI].button.texture.proceduralnverted, int, "Property texture - procedural inverted");
                        LGDMATERIAL_WRITEBITS(propElements[pI].button.texture.proceduralScale, float, "Property texture - procedural scale");

                        int32_t textureWidth = propElements[pI].button.texture.getResolution().x;
                        LGDMATERIAL_WRITEBITS(textureWidth, int32_t, "Property texture - texture width");

                        int32_t textureHeight = propElements[pI].button.texture.getResolution().y;
                        LGDMATERIAL_WRITEBITS(textureHeight, int32_t, "Property texture - texture height");

                        char* pixels = new char[textureWidth * textureHeight * 4];
                        propElements[pI].button.texture.getData(pixels);
                        
                        if(!wf.write(pixels, textureWidth * textureHeight * 4 * sizeof(char))){
                            LGDLOG::start<< "ERROR : Writing lgdmaterial file. Failed to write at : " << "Property texture - texture pixels" << LGDLOG::end;
                            return false; 
                        }
                    
                        delete[] pixels;
                    }
                    else{
                        char valueType = '3';
                        LGDMATERIAL_WRITEBITS(valueType, char, "Property type");

                        LGDMATERIAL_WRITEBITS(propElements[pI].button.color.r, float, "Property value axis x");
                        LGDMATERIAL_WRITEBITS(propElements[pI].button.color.g, float, "Property value axis y");
                        LGDMATERIAL_WRITEBITS(propElements[pI].button.color.b, float, "Property value axis z");
                    }
                }
            }
        }        
    }

    return true;
}