/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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
                                    LGDLOG::start<< "ERROR : Writing lgdmaterial file. Failed to write at : " << loc << LGDLOG::end;\
                                    try{\
                                        if(std::filesystem::exists(path)){\
                                            std::filesystem::remove(path);\
                                        }\
                                    }\
                                    catch (const std::filesystem::filesystem_error& ex) {\
                                        LGDLOG::start << "ERROR : Filesystem : Location ID 745113 " << ex.what() << LGDLOG::end;\
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
            LGDLOG::start<< "ERROR WHILE WRITING BRUSH FILE! Cannot open file : " << path << LGDLOG::end;
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
        uint32_t versionNumber = 2200; 
        LGDBRUSH_WRITEBITS(versionNumber, uint32_t, "Version number");
    
        //!Brush Data
        uint32_t propSize = 11;
        LGDBRUSH_WRITEBITS(propSize, uint32_t, "propSize");
        
        // Property 0
        std::string scatterStr = "scatter"; 
        uint32_t scatterStrSize = scatterStr.size(); 
        LGDBRUSH_WRITEBITS(scatterStrSize, uint32_t, "scatterStrSize");
        LGDBRUSH_WRITESTR(scatterStr);
        char scatterValueType = 'f';
        LGDBRUSH_WRITEBITS(scatterValueType, char, "scatterValueType");
        LGDBRUSH_WRITEBITS(brush.properties.scatter, float, "scatter");
        
        // Property 1
        std::string spacingStr = "spacing"; 
        uint32_t spacingStrSize = spacingStr.size(); 
        LGDBRUSH_WRITEBITS(spacingStrSize, uint32_t, "spacingStrSize");
        LGDBRUSH_WRITESTR(spacingStr);
        char spacingValueType = 'f';
        LGDBRUSH_WRITEBITS(spacingValueType, char, "spacingValueType");
        LGDBRUSH_WRITEBITS(brush.properties.spacing, float, "spacing");

        // Property 2
        std::string hardnessStr = "hardness"; 
        uint32_t hardnessStrSize = hardnessStr.size(); 
        LGDBRUSH_WRITEBITS(hardnessStrSize, uint32_t, "hardnessStrSize");
        LGDBRUSH_WRITESTR(hardnessStr);
        char hardnessValueType = 'f';
        LGDBRUSH_WRITEBITS(hardnessValueType, char, "hardnessValueType");
        LGDBRUSH_WRITEBITS(brush.properties.hardness, float, "hardness");
        
        // Property 3
        std::string sizeJitterStr = "sizeJitter"; 
        uint32_t sizeJitterStrSize = sizeJitterStr.size(); 
        LGDBRUSH_WRITEBITS(sizeJitterStrSize, uint32_t, "sizeJitterStrSize");
        LGDBRUSH_WRITESTR(sizeJitterStr);
        char sizeJitterValueType = 'f';
        LGDBRUSH_WRITEBITS(sizeJitterValueType, char, "sizeJitterValueType");
        LGDBRUSH_WRITEBITS(brush.properties.sizeJitter, float, "sizeJitter");
        
        // Property 4
        std::string fadeStr = "fade"; 
        uint32_t fadeStrSize = fadeStr.size(); 
        LGDBRUSH_WRITEBITS(fadeStrSize, uint32_t, "fadeStrSize");
        LGDBRUSH_WRITESTR(fadeStr);
        char fadeValueType = 'f';
        LGDBRUSH_WRITEBITS(fadeValueType, char, "fadeValueType");
        LGDBRUSH_WRITEBITS(brush.properties.fade, float, "fade");
        
        // Property 5
        std::string rotationStr = "rotation"; 
        uint32_t rotationStrSize = rotationStr.size(); 
        LGDBRUSH_WRITEBITS(rotationStrSize, uint32_t, "rotationStrSize");
        LGDBRUSH_WRITESTR(rotationStr);
        char rotationValueType = 'f';
        LGDBRUSH_WRITEBITS(rotationValueType, char, "rotationValueType");
        LGDBRUSH_WRITEBITS(brush.properties.rotation, float, "rotation");
        
        // Property 6
        std::string rotationJitterStr = "rotationJitter"; 
        uint32_t rotationJitterStrSize = rotationJitterStr.size(); 
        LGDBRUSH_WRITEBITS(rotationJitterStrSize, uint32_t, "rotationJitterStrSize");
        LGDBRUSH_WRITESTR(rotationJitterStr);
        char rotationJitterValueType = 'f';
        LGDBRUSH_WRITEBITS(rotationJitterValueType, char, "rotationJitterValueType");
        LGDBRUSH_WRITEBITS(brush.properties.rotationJitter, float, "rotationJitter");
        
        // Property 7
        std::string alphaJitterStr = "alphaJitter"; 
        uint32_t alphaJitterStrSize = alphaJitterStr.size(); 
        LGDBRUSH_WRITEBITS(alphaJitterStrSize, uint32_t, "alphaJitterStrSize");
        LGDBRUSH_WRITESTR(alphaJitterStr);
        char alphaJitterValueType = 'f';
        LGDBRUSH_WRITEBITS(alphaJitterValueType, char, "alphaJitterValueType");
        LGDBRUSH_WRITEBITS(brush.properties.alphaJitter, float, "alphaJitter");
        
        // Property 8
        std::string individualTextureStr = "individualTexture"; 
        uint32_t individualTextureStrSize = individualTextureStr.size(); 
        LGDBRUSH_WRITEBITS(individualTextureStrSize, uint32_t, "individualTextureStrSize");
        LGDBRUSH_WRITESTR(individualTextureStr);
        char individualTextureValueType = 'b';
        LGDBRUSH_WRITEBITS(individualTextureValueType, char, "individualTextureValueType");
        LGDBRUSH_WRITEBITS(brush.properties.individualTexture, bool, "individualTexture");
        
        // Property 9
        std::string sinWavePatternStr = "sinWavePattern"; 
        uint32_t sinWavePatternStrSize = sinWavePatternStr.size(); 
        LGDBRUSH_WRITEBITS(sinWavePatternStrSize, uint32_t, "sinWavePatternStrSize");
        LGDBRUSH_WRITESTR(sinWavePatternStr);
        char sinWavePatternValueType = 'b';
        LGDBRUSH_WRITEBITS(sinWavePatternValueType, char, "sinWavePatternValueType");
        LGDBRUSH_WRITEBITS(brush.properties.sinWavePattern, bool, "sinWavePattern");

        // Property 10
        std::string textureStr = "texture"; 
        uint32_t textureStrSize = textureStr.size(); 
        LGDBRUSH_WRITEBITS(textureStrSize, uint32_t, "textureStrSize");
        LGDBRUSH_WRITESTR(textureStr);
        char textureValueType = 't';
        LGDBRUSH_WRITEBITS(textureValueType, char, "textureValueType");
        brush.properties.brushTexture.writeTextureData(wf);
    }

    return true;
}