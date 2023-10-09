/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    * read the *.lgdbrush file 

    Description header
    64bit 64bit 64bit
    0x4B4B9AAA
    0xABAB9ACC
    0x334A9FFF
        |
    spacing(sizeof(float))
        |   
    sizeJitter(sizeof(float))
        |   
    scatter(sizeof(float))
        |    
    fade(sizeof(float))
        |    
    rotation(sizeof(float))
        |    
    rotationJitter(sizeof(float))
        |    
    alphaJitter(sizeof(float))
        |
    individualTexture(sizeof(bool))
        |
    sinWavePattern(sizeof(bool))
        |
    readTexture(sizeof(bool)) //If that bit is set to 1 prep to read raw texture data otherwise ignore the rest
        |(if set to 1)
    textureWidth(64bit)
        |
    textureHeight(64bit) (Texture always has 4 channels for now)
        |
    textureData(8bit * textureWidth * textureHeight * 4)

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
    
#define LGDBRUSH_READBITS(var, type, loc) if(!rf.read(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                LGDLOG::start<< "ERROR : Reading lgdmaterial file. Failed to read at : " << loc << LGDLOG::end;\
                                return false; \
                            }

struct LGDBRUSHProp{
    std::string title;
    char valueType;

    bool boolVal;
    float floatVal;
    Texture texture;
};

static bool readProperties(std::ifstream& rf, std::vector<LGDBRUSHProp>& properties);
static bool parseLGDBRUSHProperties(const std::vector<LGDBRUSHProp> properties, Brush& brush);

bool FileHandler::readLGDBRUSHFile(std::string path, Brush& brush){
    
    if(!std::filesystem::is_regular_file(path)){
        LGDLOG::start << "ERROR : Loading brush : " << path << " is not a regular file!" << LGDLOG::end;
        return false;
    }

    std::ifstream rf(path, std::ios::in | std::ios::binary);

    if(!rf) {
        LGDLOG::start<< "ERROR WHILE WRITING BRUSH FILE! Cannot open file : " << path << LGDLOG::end;
        return false;
    }
        
    //!HEADER
        
    //!Description
    uint64_t h1 = 0x8A3F9B0E5CFA2D71; 
    uint64_t h2 = 0xE6724F8D319BCA06; 
    uint64_t h3 = 0x597AEC82D60491B9; 
    uint64_t c1; 
    uint64_t c2; 
    uint64_t c3; 
    LGDBRUSH_READBITS(c1, uint64_t, "Description 1st");
    LGDBRUSH_READBITS(c2, uint64_t, "Description 2nd");
    LGDBRUSH_READBITS(c3, uint64_t, "Description 3rd");

    if(c1 != h1 || c2 != h2 || c3 != h3){
        LGDLOG::start<< "ERROR WHILE READING BRUSH FILE! Description header is not correct." << LGDLOG::end;
        return false;
    }
    
    //Version number
    uint32_t versionNumber; //2000 
    LGDBRUSH_READBITS(versionNumber, uint32_t, "Version number");

    if(versionNumber != 0x000007D0){
        LGDLOG::start<< "WARNING! : Brush version number was : " << versionNumber << ". Results might be unexpected." << LGDLOG::end; 
    }

    std::vector<LGDBRUSHProp> properties;
    if(!readProperties(rf, properties))
        return false;

    if(!parseLGDBRUSHProperties(properties, brush))
        return false;

    brush.title = UTIL::getLastWordBySeparatingWithChar(path, UTIL::folderDistinguisher());
    brush.title = UTIL::removeExtension(brush.title);

    return true;   
}










static bool parseLGDBRUSHProperties(const std::vector<LGDBRUSHProp> properties, Brush& brush){
    
    Brush dummyBrush;

    for (size_t i = 0; i < properties.size(); i++)
    {
        if(properties[i].title == "sizeJitter"){
            if(properties[i].valueType == 'f')
                dummyBrush.sizeJitter = properties[i].floatVal;
            else if(properties[i].valueType == 'b')
                dummyBrush.sizeJitter = properties[i].boolVal;
            else
                LGDLOG::start<< "ERROR! Parsing brush file. Invalid value type for the sizeJitter property : " << properties[i].valueType << "." << LGDLOG::end;
        }
        else if(properties[i].title == "hardness"){
            if(properties[i].valueType == 'f')
                dummyBrush.hardness = properties[i].floatVal;
            else if(properties[i].valueType == 'b')
                dummyBrush.hardness = properties[i].boolVal;
            else
                LGDLOG::start<< "ERROR! Parsing brush file. Invalid value type for the hardness property : " << properties[i].valueType << "." << LGDLOG::end;
        }
        else if(properties[i].title == "spacing"){
            if(properties[i].valueType == 'f')
                dummyBrush.spacing = properties[i].floatVal;
            else if(properties[i].valueType == 'b')
                dummyBrush.spacing = properties[i].boolVal;
            else
                LGDLOG::start<< "ERROR! Parsing brush file. Invalid value type for the spacing property : " << properties[i].valueType << "." << LGDLOG::end;
        }
        else if(properties[i].title == "scatter"){
            if(properties[i].valueType == 'f')
                dummyBrush.scatter = properties[i].floatVal;
            else if(properties[i].valueType == 'b')
                dummyBrush.scatter = properties[i].boolVal;
            else
                LGDLOG::start<< "ERROR! Parsing brush file. Invalid value type for the scatter property : " << properties[i].valueType << "." << LGDLOG::end;
        }
        else if(properties[i].title == "fade"){
            if(properties[i].valueType == 'f')
                dummyBrush.fade = properties[i].floatVal;
            else if(properties[i].valueType == 'b')
                dummyBrush.fade = properties[i].boolVal;
            else
                LGDLOG::start<< "ERROR! Parsing brush file. Invalid value type for the fade property : " << properties[i].valueType << "." << LGDLOG::end;
        }
        else if(properties[i].title == "rotation"){
            if(properties[i].valueType == 'f')
                dummyBrush.rotation = properties[i].floatVal;
            else if(properties[i].valueType == 'b')
                dummyBrush.rotation = properties[i].boolVal;
            else
                LGDLOG::start<< "ERROR! Parsing brush file. Invalid value type for the rotation property : " << properties[i].valueType << "." << LGDLOG::end;
        }
        else if(properties[i].title == "rotationJitter"){
            if(properties[i].valueType == 'f')
                dummyBrush.rotationJitter = properties[i].floatVal;
            else if(properties[i].valueType == 'b')
                dummyBrush.rotationJitter = properties[i].boolVal;
            else
                LGDLOG::start<< "ERROR! Parsing brush file. Invalid value type for the rotationJitter property : " << properties[i].valueType << "." << LGDLOG::end;
        }
        else if(properties[i].title == "alphaJitter"){
            if(properties[i].valueType == 'f')
                dummyBrush.alphaJitter = properties[i].floatVal;
            else if(properties[i].valueType == 'b')
                dummyBrush.alphaJitter = properties[i].boolVal;
            else
                LGDLOG::start<< "ERROR! Parsing brush file. Invalid value type for the alphaJitter property : " << properties[i].valueType << "." << LGDLOG::end;
        }
        else if(properties[i].title == "individualTexture"){
            if(properties[i].valueType == 'f')
                dummyBrush.individualTexture = properties[i].floatVal;
            else if(properties[i].valueType == 'b')
                dummyBrush.individualTexture = properties[i].boolVal;
            else
                LGDLOG::start<< "ERROR! Parsing brush file. Invalid value type for the individualTexture property : " << properties[i].valueType << "." << LGDLOG::end;
        }
        else if(properties[i].title == "sinWavePattern"){
            if(properties[i].valueType == 'f')
                dummyBrush.sinWavePattern = properties[i].floatVal;
            else if(properties[i].valueType == 'b')
                dummyBrush.sinWavePattern = properties[i].boolVal;
            else
                LGDLOG::start<< "ERROR! Parsing brush file. Invalid value type for the sinWavePattern property : " << properties[i].valueType << "." << LGDLOG::end;
        }
        else if(properties[i].title == "texture"){
            if(properties[i].valueType == 't')
                dummyBrush.texture = properties[i].texture;
            else
                LGDLOG::start<< "ERROR! Parsing brush file. Invalid value type for the texture property : " << properties[i].valueType << "." << LGDLOG::end;
        }
        else{
            LGDLOG::start<< "WARNING! Parning brush file. Unknown property : " << properties[i].title << LGDLOG::end;
        }
    }

    // Set the properties and generate neccessary textures
    brush = Brush(
                    0.1f,
                    dummyBrush.spacing, 
                    dummyBrush.hardness, 
                    dummyBrush.sizeJitter, 
                    dummyBrush.scatter, 
                    dummyBrush.fade, 
                    dummyBrush.rotation, 
                    dummyBrush.rotationJitter, 
                    dummyBrush.alphaJitter, 
                    dummyBrush.individualTexture, 
                    dummyBrush.sinWavePattern, 
                    dummyBrush.title, 
                    dummyBrush.texture
                );

    return true;
}

static bool readProperties(std::ifstream& rf, std::vector<LGDBRUSHProp>& properties){
    //!Brush Data
    uint32_t propSize;
    LGDBRUSH_READBITS(propSize, uint32_t, "propSize");

    for (size_t i = 0; i < propSize; i++)
    {
        LGDBRUSHProp prop;
        
        uint32_t scatterStrSize; 
        LGDBRUSH_READBITS(scatterStrSize, uint32_t, "scatterStrSize");

        for (size_t charI = 0; charI < scatterStrSize; charI++)
        {
            char c;
            LGDBRUSH_READBITS(c, char, "Modifier's property - title");
            prop.title.push_back(c);
        }

        LGDBRUSH_READBITS(prop.valueType, char, "scatterValueType");
        
        if(prop.valueType == 'f'){
            LGDBRUSH_READBITS(prop.floatVal, float, "scatter");
        }
        else if(prop.valueType == 'b'){
            LGDBRUSH_READBITS(prop.boolVal, bool, "scatter");
        }
        else if(prop.valueType == 't'){
            prop.texture.readTextureData(rf);
        }
        else{
            LGDLOG::start<< "ERROR! Reading lgdbrush file. Unknown property value type!" << LGDLOG::end;
            return false;
        }
        
        properties.push_back(prop);
    }

    return true;
}