/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    * write the *.lgdbrush file
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
    
#define LGDBRUSH_WRITEBITS(var, type, loc) if(!wf.write(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                    std::cout << "ERROR : Writing lgdmaterial file. Failed to write at : " << loc << std::endl;\
                                    if(std::filesystem::exists(path)){\
                                        std::filesystem::remove(path);\
                                    }\
                                    return false; \
                                }

#define LGDBRUSH_WRITESTR(str)for (size_t charI = 0; charI < str.size(); charI++)\
                {\
                    LGDBRUSH_WRITEBITS(str[charI], char, str);\
                }\

bool FileHandler::writeLGDBRUSHFile(std::string path, Brush brush){
    
    if(path == ""){
        path = showFileSystemObjectSelectionDialog(
                                                    "Select a folder to export the brush file.", 
                                                    "", 
                                                    {}, 
                                                    false, 
                                                    FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER
                                                );
    }
    
    if(path.size()){

        std::ofstream wf(path + UTIL::folderDistinguisher() + brush.title + ".lgdbrush", std::ios::out | std::ios::binary);

        if(!wf) {
            std::cout << "ERROR WHILE WRITING BRUSH FILE! Cannot open file : " << path << std::endl;
            return false;
        }
        
        //!HEADER
        
        //!Description
        uint64_t h1 = 0x8A3F9B0E5CFA2D71; 
        uint64_t h2 = 0xE6724F8D319BCA06; 
        uint64_t h3 = 0x597AEC82D60491B9; 
        LGDBRUSH_WRITEBITS(h1, uint64_t, "Description 1st");
        LGDBRUSH_WRITEBITS(h2, uint64_t, "Description 2nd");
        LGDBRUSH_WRITEBITS(h3, uint64_t, "Description 3rd");

        //Version number
        uint32_t versionNumber = 0x000007D0; //2000 
        LGDBRUSH_WRITEBITS(versionNumber, uint32_t, "Version number");
    
        //!Brush Data
        uint32_t propSize = 9;
        LGDBRUSH_WRITEBITS(propSize, uint32_t, "propSize");
        
        std::string scatterStr = "scatter"; 
        uint32_t scatterStrSize = scatterStr.size(); 
        LGDBRUSH_WRITEBITS(scatterStrSize, uint32_t, "scatterStrSize");
        LGDBRUSH_WRITESTR(scatterStr);
        char scatterValueType = 'f';
        LGDBRUSH_WRITEBITS(scatterValueType, char, "scatterValueType");
        LGDBRUSH_WRITEBITS(brush.scatter, float, "scatter");
        
        std::string sizeJitterStr = "sizeJitter"; 
        uint32_t sizeJitterStrSize = sizeJitterStr.size(); 
        LGDBRUSH_WRITEBITS(sizeJitterStrSize, uint32_t, "sizeJitterStrSize");
        LGDBRUSH_WRITESTR(sizeJitterStr);
        char sizeJitterValueType = 'f';
        LGDBRUSH_WRITEBITS(sizeJitterValueType, char, "sizeJitterValueType");
        LGDBRUSH_WRITEBITS(brush.sizeJitter, float, "sizeJitter");
        
        std::string fadeStr = "fade"; 
        uint32_t fadeStrSize = fadeStr.size(); 
        LGDBRUSH_WRITEBITS(fadeStrSize, uint32_t, "fadeStrSize");
        LGDBRUSH_WRITESTR(fadeStr);
        char fadeValueType = 'f';
        LGDBRUSH_WRITEBITS(fadeValueType, char, "fadeValueType");
        LGDBRUSH_WRITEBITS(brush.fade, float, "fade");
        
        std::string rotationStr = "rotation"; 
        uint32_t rotationStrSize = rotationStr.size(); 
        LGDBRUSH_WRITEBITS(rotationStrSize, uint32_t, "rotationStrSize");
        LGDBRUSH_WRITESTR(rotationStr);
        char rotationValueType = 'f';
        LGDBRUSH_WRITEBITS(rotationValueType, char, "rotationValueType");
        LGDBRUSH_WRITEBITS(brush.rotation, float, "rotation");
        
        std::string rotationJitterStr = "rotationJitter"; 
        uint32_t rotationJitterStrSize = rotationJitterStr.size(); 
        LGDBRUSH_WRITEBITS(rotationJitterStrSize, uint32_t, "rotationJitterStrSize");
        LGDBRUSH_WRITESTR(rotationJitterStr);
        char rotationJitterValueType = 'f';
        LGDBRUSH_WRITEBITS(rotationJitterValueType, char, "rotationJitterValueType");
        LGDBRUSH_WRITEBITS(brush.rotationJitter, float, "rotationJitter");
        
        std::string alphaJitterStr = "alphaJitter"; 
        uint32_t alphaJitterStrSize = alphaJitterStr.size(); 
        LGDBRUSH_WRITEBITS(alphaJitterStrSize, uint32_t, "alphaJitterStrSize");
        LGDBRUSH_WRITESTR(alphaJitterStr);
        char alphaJitterValueType = 'f';
        LGDBRUSH_WRITEBITS(alphaJitterValueType, char, "alphaJitterValueType");
        LGDBRUSH_WRITEBITS(brush.alphaJitter, float, "alphaJitter");
        
        std::string individualTextureStr = "individualTexture"; 
        uint32_t individualTextureStrSize = individualTextureStr.size(); 
        LGDBRUSH_WRITEBITS(individualTextureStrSize, uint32_t, "individualTextureStrSize");
        LGDBRUSH_WRITESTR(individualTextureStr);
        char individualTextureValueType = 'b';
        LGDBRUSH_WRITEBITS(individualTextureValueType, char, "individualTextureValueType");
        LGDBRUSH_WRITEBITS(brush.individualTexture, bool, "individualTexture");
        
        std::string sinWavePatternStr = "sinWavePattern"; 
        uint32_t sinWavePatternStrSize = sinWavePatternStr.size(); 
        LGDBRUSH_WRITEBITS(sinWavePatternStrSize, uint32_t, "sinWavePatternStrSize");
        LGDBRUSH_WRITESTR(sinWavePatternStr);
        char sinWavePatternValueType = 'b';
        LGDBRUSH_WRITEBITS(sinWavePatternValueType, char, "sinWavePatternValueType");
        LGDBRUSH_WRITEBITS(brush.sinWavePattern, bool, "sinWavePattern");

        std::string textureStr = "texture"; 
        uint32_t textureStrSize = textureStr.size(); 
        LGDBRUSH_WRITEBITS(textureStrSize, uint32_t, "textureStrSize");
        LGDBRUSH_WRITESTR(textureStr);
        char textureValueType = 't';
        LGDBRUSH_WRITEBITS(textureValueType, char, "textureValueType");
        
        LGDBRUSH_WRITEBITS(brush.texture.proceduralID, int, "Property texture - procedural ID");
        LGDBRUSH_WRITEBITS(brush.texture.proceduralnverted, int, "Property texture - procedural inverted");
        LGDBRUSH_WRITEBITS(brush.texture.proceduralScale, float, "Property texture - procedural scale");

        int32_t textureWidth = brush.texture.getResolution().x;
        LGDBRUSH_WRITEBITS(textureWidth, int32_t, "Property texture - texture width");

        int32_t textureHeight = brush.texture.getResolution().y;
        LGDBRUSH_WRITEBITS(textureHeight, int32_t, "Property texture - texture height");

        char* pixels = new char[textureWidth * textureHeight * 4];
        brush.texture.getData(pixels);
        
        if(!wf.write(pixels, textureWidth * textureHeight * 4 * sizeof(char))){
            std::cout << "ERROR : Writing lgdmaterial file. Failed to write at : " << "Property texture - texture pixels" << std::endl;
            if(std::filesystem::exists(path)){
                std::filesystem::remove(path);
            }
            return false; 
        }
    
        delete[] pixels;
    }

    return true;
}