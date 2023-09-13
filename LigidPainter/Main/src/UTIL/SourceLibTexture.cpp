/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/


#include <glad/glad.h>

#include <glm/glm.hpp>

#include <string>
#include <iostream>

#include "UTIL/Util.hpp"

void SourceLibTexture::load(const std::string path){
    const glm::ivec2 displayTextureResolution = glm::ivec2(128);
    
    std::cout << "B" << std::endl;
    //Init and load the displaying texture
    this->displayingTexture.load(path.c_str(), displayTextureResolution);
    std::cout << "C" << std::endl;

    //Get the title of the path
    this->ID = UTIL::removeExtension(UTIL::getLastWordBySeparatingWithChar(path, UTIL::folderDistinguisher()));
    std::cout << "D" << std::endl;
}

Texture SourceLibTexture::getTexture(){
    const std::string folderPath = "/LigidPainter/Resources/Texture Library";

    std::string filePath = folderPath + "/" + this->ID + ".jpg"; //Assuming the texture is in JPEG format

    Texture txtr;
    txtr.load(filePath.c_str());

    return txtr; 
} 