/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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
    //Init and load the displaying texture
    this->displayingTexture.load(path.c_str());

    //Get the title of the path
    this->ID = UTIL::removeExtension(UTIL::getLastWordBySeparatingWithChar(path, UTIL::folderDistinguisher()));
}

Texture SourceLibTexture::getTexture(){
    const std::string folderPath = "./LigidPainter/Resources/Texture Library";

    std::string filePath = folderPath + "/" + this->ID + ".jpg"; //Assuming the texture is in JPEG format

    if(this->fullTexture.ID == 0 || glIsTexture(this->fullTexture.ID) == GL_FALSE)
        this->fullTexture.load(filePath.c_str());

    return this->fullTexture; 
} 