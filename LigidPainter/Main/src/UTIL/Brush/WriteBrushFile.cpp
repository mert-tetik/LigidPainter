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
#include "GUI/GUI.hpp"
    

void Brush::saveFile(std::string path){
    //Saves the brush file
    if(path == "")
        path = showFileSystemObjectSelectionDialog("Select a folder to export the brush file.", "", {}, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER);
    
    if(path.size()){

        std::ofstream wf(path + UTIL::folderDistinguisher() + title + ".lgdbrush", std::ios::out | std::ios::binary);

        if(!wf) {
            std::cout << "ERROR WHILE WRITING BRUSH FILE! Cannot open file : " << path << std::endl;
            return;
        }
        
        //!HEADER
        
        //!Description
        uint64_t h1 = 0x4B4B9AAA; 
        uint64_t h2 = 0xABAB9ACC; 
        uint64_t h3 = 0x334A9FFF; 

        wf.write(reinterpret_cast<char*>(   &h1    ),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(   &h2    ),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(   &h3    ),sizeof(uint64_t));
    
        //!Brush Data
        wf.write(reinterpret_cast<char*>(   &sizeJitter         ),sizeof(float));
        wf.write(reinterpret_cast<char*>(   &scatter            ),sizeof(float));
        wf.write(reinterpret_cast<char*>(   &fade               ),sizeof(float));
        wf.write(reinterpret_cast<char*>(   &rotation           ),sizeof(float));
        wf.write(reinterpret_cast<char*>(   &rotationJitter     ),sizeof(float));
        wf.write(reinterpret_cast<char*>(   &alphaJitter        ),sizeof(float));
        wf.write(reinterpret_cast<char*>(   &individualTexture  ),sizeof(bool));
        wf.write(reinterpret_cast<char*>(   &sinWavePattern     ),sizeof(bool));

        bool haveTexture = texture.ID; 
        
        wf.write(reinterpret_cast<char*>(   &haveTexture     ),sizeof(bool));

        if(haveTexture){
            //Write texture resolution
            uint64_t txtrWidth = texture.getResolution().x;
            uint64_t txtrHeight = texture.getResolution().y;
            wf.write(reinterpret_cast<char*>(   &txtrWidth     ),sizeof(uint64_t));
            wf.write(reinterpret_cast<char*>(   &txtrHeight     ),sizeof(uint64_t));

            //Get pixels of the texture
            char* pixels = new char[txtrWidth * txtrHeight * 4];
            texture.getData(pixels);
            
            //Write texture data
            wf.write(pixels , txtrWidth * txtrHeight * 4 * sizeof(char));

            delete[] pixels;
        }
    }
}