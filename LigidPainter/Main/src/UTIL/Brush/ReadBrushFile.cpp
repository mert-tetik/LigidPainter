/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

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
#include<GLFW/glfw3.h>

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
    

void Brush::readFile(std::string path){
    std::ifstream rf(path, std::ios::in | std::ios::binary);

    //Get the title(name) of the brush
    ;
    title = UTIL::getLastWordBySeparatingWithChar(path,UTIL::folderDistinguisher());
    title = UTIL::removeExtension(title);

    if(!rf) {
        std::cout << "ERROR WHILE WRITING BRUSH FILE! Cannot open file : " << path << std::endl;
        return;
    }
        
    //!HEADER
        
    //!Description
    uint64_t h1; 
    uint64_t h2; 
    uint64_t h3; 

    rf.read(reinterpret_cast<char*>(   &h1    ),sizeof(uint64_t));
    rf.read(reinterpret_cast<char*>(   &h2    ),sizeof(uint64_t));
    rf.read(reinterpret_cast<char*>(   &h3    ),sizeof(uint64_t));

    if(
            h1 == 0x4B4B9AAA &&
            h2 == 0xABAB9ACC &&
            h3 == 0x334A9FFF 
        )
    {
        //!Brush Data
        rf.read(reinterpret_cast<char*>(   &sizeJitter         ),sizeof(float));
        rf.read(reinterpret_cast<char*>(   &scatter            ),sizeof(float));
        rf.read(reinterpret_cast<char*>(   &fade               ),sizeof(float));
        rf.read(reinterpret_cast<char*>(   &rotation           ),sizeof(float));
        rf.read(reinterpret_cast<char*>(   &rotationJitter     ),sizeof(float));
        rf.read(reinterpret_cast<char*>(   &alphaJitter        ),sizeof(float));
        rf.read(reinterpret_cast<char*>(   &individualTexture  ),sizeof(bool));
        rf.read(reinterpret_cast<char*>(   &sinWavePattern     ),sizeof(bool));

        bool haveTexture = false; 

        rf.read(reinterpret_cast<char*>(   &haveTexture     ),sizeof(bool));

        if(haveTexture){
            uint64_t w = 0,h = 0;
            //read texture resolution
            rf.read(reinterpret_cast<char*>(   &w     ),sizeof(uint64_t));
            rf.read(reinterpret_cast<char*>(   &h     ),sizeof(uint64_t));

            //Get pixels of the texture
            char* pixels = new char[w * h * 4];

            //read texture data
            rf.read(pixels , w * h * 4 * sizeof(char));

            glGenTextures(1,&texture.ID);
            glActiveTexture(GL_TEXTURE0);

            glBindTexture(GL_TEXTURE_2D,texture.ID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_BYTE, pixels);
            glGenerateMipmap(GL_TEXTURE_2D);

            delete[] pixels;
        }
    }
    else{
        std::cout << "ERROR WHILE WRITING BRUSH FILE! File is not recognised : " << path << std::endl;
        return;
    }
}