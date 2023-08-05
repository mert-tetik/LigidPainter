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
    
#define LGDBRUSH_READBITS(var, type, loc) if(!rf.read(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                std::cout << "ERROR : Reading lgdmaterial file. Failed to read at : " << loc << std::endl;\
                                return false; \
                            }

struct LGDBRUSHProp{
    std::string title;
    char valueType;

    bool boolVal;
    float floatVal;
    Texture texture;
};

static bool readFile(std::ifstream& rf, std::vector<LGDBRUSHProp>& properties);
static bool parseLGDBRUSHProperties(const std::vector<LGDBRUSHProp> properties, Brush& brush);

bool FileHandler::readLGDBRUSHFile(std::string path, Brush& brush){
    
    std::ifstream rf(path, std::ios::in | std::ios::binary);

    brush.title = UTIL::getLastWordBySeparatingWithChar(path, UTIL::folderDistinguisher());
    brush.title = UTIL::removeExtension(brush.title);

    if(!rf) {
        std::cout << "ERROR WHILE WRITING BRUSH FILE! Cannot open file : " << path << std::endl;
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

    if(c1 != h1 || c2 != h1 || c3 != h1 ){
        std::cout << "ERROR WHILE READING BRUSH FILE! Description header is not correct." << std::endl;
        return false;
    }
    
    //Version number
    uint32_t versionNumber; //2000 
    LGDBRUSH_READBITS(versionNumber, uint32_t, "Version number");

    if(versionNumber != 0x000007D0){
        std::cout << "WARNING! : Brush version number was : " << versionNumber << ". Results might be unexpected." << std::endl; 
    }

    std::vector<LGDBRUSHProp> properties;
    if(!readFile(rf, properties))
        return false;

    if(!parseLGDBRUSHProperties(properties, brush))
        return false;

    return true;   
}










static bool parseLGDBRUSHProperties(const std::vector<LGDBRUSHProp> properties, Brush& brush){
    for (size_t i = 0; i < properties.size(); i++)
    {
        
        if(properties[i].title == "sizeJitter"){
            if(properties[i].valueType == 'f')
                brush.sizeJitter = properties[i].floatVal;
            else if(properties[i].valueType == 'b')
                brush.sizeJitter = properties[i].boolVal;
            else
                std::cout << "ERROR! Parsing brush file. Invalid value type for the sizeJitter property : " << properties[i].valueType << "." << std::endl;
        }
        else if(properties[i].title == "scatter"){
            if(properties[i].valueType == 'f')
                brush.scatter = properties[i].floatVal;
            else if(properties[i].valueType == 'b')
                brush.scatter = properties[i].boolVal;
            else
                std::cout << "ERROR! Parsing brush file. Invalid value type for the scatter property : " << properties[i].valueType << "." << std::endl;
        }
        else if(properties[i].title == "fade"){
            if(properties[i].valueType == 'f')
                brush.fade = properties[i].floatVal;
            else if(properties[i].valueType == 'b')
                brush.fade = properties[i].boolVal;
            else
                std::cout << "ERROR! Parsing brush file. Invalid value type for the fade property : " << properties[i].valueType << "." << std::endl;
        }
        else if(properties[i].title == "rotation"){
            if(properties[i].valueType == 'f')
                brush.rotation = properties[i].floatVal;
            else if(properties[i].valueType == 'b')
                brush.rotation = properties[i].boolVal;
            else
                std::cout << "ERROR! Parsing brush file. Invalid value type for the rotation property : " << properties[i].valueType << "." << std::endl;
        }
        else if(properties[i].title == "rotationJitter"){
            if(properties[i].valueType == 'f')
                brush.rotationJitter = properties[i].floatVal;
            else if(properties[i].valueType == 'b')
                brush.rotationJitter = properties[i].boolVal;
            else
                std::cout << "ERROR! Parsing brush file. Invalid value type for the rotationJitter property : " << properties[i].valueType << "." << std::endl;
        }
        else if(properties[i].title == "alphaJitter"){
            if(properties[i].valueType == 'f')
                brush.alphaJitter = properties[i].floatVal;
            else if(properties[i].valueType == 'b')
                brush.alphaJitter = properties[i].boolVal;
            else
                std::cout << "ERROR! Parsing brush file. Invalid value type for the alphaJitter property : " << properties[i].valueType << "." << std::endl;
        }
        else if(properties[i].title == "individualTexture"){
            if(properties[i].valueType == 'f')
                brush.individualTexture = properties[i].floatVal;
            else if(properties[i].valueType == 'b')
                brush.individualTexture = properties[i].boolVal;
            else
                std::cout << "ERROR! Parsing brush file. Invalid value type for the individualTexture property : " << properties[i].valueType << "." << std::endl;
        }
        else if(properties[i].title == "sinWavePattern"){
            if(properties[i].valueType == 'f')
                brush.sinWavePattern = properties[i].floatVal;
            else if(properties[i].valueType == 'b')
                brush.sinWavePattern = properties[i].boolVal;
            else
                std::cout << "ERROR! Parsing brush file. Invalid value type for the sinWavePattern property : " << properties[i].valueType << "." << std::endl;
        }
        else if(properties[i].title == "texture"){
            if(properties[i].valueType == 't')
                brush.texture = properties[i].texture;
            else
                std::cout << "ERROR! Parsing brush file. Invalid value type for the texture property : " << properties[i].valueType << "." << std::endl;
        }
        else{
            std::cout << "WARNING! Parning brush file. Unknown property : " << properties[i].title << std::endl;
        }
    }

    return true;
}

static bool readFile(std::ifstream& rf, std::vector<LGDBRUSHProp>& properties){
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
            int proceduralID;
            int proceduralInvert;
            float proceduralScale;
            LGDBRUSH_READBITS(proceduralID, int, "property - texture data - procedural ID");
            LGDBRUSH_READBITS(proceduralInvert, int, "property - texture data - procedural invert");
            LGDBRUSH_READBITS(proceduralScale, float, "property - texture data - procedural scale");

            int32_t textureWidth = prop.texture.getResolution().x;
            LGDBRUSH_READBITS(textureWidth, int32_t, "property - texture data - texture width");

            int32_t textureHeight = prop.texture.getResolution().y;
            LGDBRUSH_READBITS(textureHeight, int32_t, "property - texture data - texture height");

            char* pixels = new char[textureWidth * textureHeight * 4];
            if(!rf.read(pixels, textureWidth * textureHeight * 4 * sizeof(char))){
                std::cout << "ERROR : Reading lgdbrush file. Failed to read at : property - texture data - texture pixels" << std::endl;
                return false;   
            }
        
            prop.texture = Texture(pixels, textureWidth, textureHeight);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            prop.texture.proceduralID = proceduralID;
            prop.texture.proceduralnverted = proceduralInvert;
            prop.texture.proceduralScale = proceduralScale;
        }
        else{
            std::cout << "ERROR! Reading lgdbrush file. Unknown property value type!" << std::endl;
            return false;
        }
        
        properties.push_back(prop);
    }

    return true;
}