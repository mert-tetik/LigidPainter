/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../thirdparty/stb_image_write.h"

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"

bool Texture::exportTexture(std::string path, const std::string format){
    if(!this->getResolution().x || !this->getResolution().y){
        LGDLOG::start << "ERROR : Can't write texture data : Resolution 0" << LGDLOG::end;
        return false;
    }
    
    if(this->getResolution().x > 9000 || this->getResolution().y > 9000){
        LGDLOG::start << "ERROR : Can't write texture data : Resolution is : " << this->getResolution().x << "x" << this->getResolution().y << LGDLOG::end;
        return false;
    }
    
    unsigned char* pixels = new unsigned char[this->getResolution().x * this->getResolution().y * 4];
    this->getData(pixels);

    const int channels = 4;

    stbi_flip_vertically_on_write(1);  // Enable flipping

    if(format == "PNG"){
        if(!stbi_write_png((path + UTIL::folderDistinguisher() + title + ".png").c_str(), this->getResolution().x, this->getResolution().y, channels, pixels, this->getResolution().x * channels)){
            LGDLOG::start<< "ERROR Failed to write texture file : " << path + UTIL::folderDistinguisher() + title + ".png" << LGDLOG::end;
        }
    }
    else if(format == "JPEG"){
        if(!stbi_write_jpg((path + UTIL::folderDistinguisher() + title + ".jpeg").c_str(), this->getResolution().x, this->getResolution().y, channels, pixels, this->getResolution().x * channels)){
            LGDLOG::start<< "ERROR Failed to write texture file : " << path + UTIL::folderDistinguisher() + title + ".jpeg" << LGDLOG::end;
        }
    }
    else if(format == "BMP"){
        if(!stbi_write_bmp((path + UTIL::folderDistinguisher() + title + ".bmp").c_str(), this->getResolution().x, this->getResolution().y, channels, pixels)){
            LGDLOG::start<< "ERROR Failed to write texture file : " << path + UTIL::folderDistinguisher() + title + ".bmp" << LGDLOG::end;
        }
    }
    else if(format == "TGA"){
        if(!stbi_write_tga((path + UTIL::folderDistinguisher() + title + ".tga").c_str(), this->getResolution().x, this->getResolution().y, channels, pixels)){
            LGDLOG::start<< "ERROR Failed to write texture file : " << path + UTIL::folderDistinguisher() + title + ".tga" << LGDLOG::end;
        }
    }

    delete[] pixels;

    return true;
}