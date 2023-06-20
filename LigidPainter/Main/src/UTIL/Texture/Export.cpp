/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include<GLFW/glfw3.h>

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

/// @brief 
/// @param path is folder path 
/// @param format PNG , JPEG , BMP , TGA
void Texture::exportTexture(std::string path,const std::string format){
    glm::ivec2 scale;
    scale = getResolution();
    
    unsigned char* pixels = new unsigned char[scale.x * scale.y * 4];;
    
    //getData function with unsigned byte
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,ID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
    const int channels = 4;

    if(format == "PNG"){
        if(!stbi_write_png((path + folderDistinguisher + title + ".png").c_str(), scale.x, scale.y, channels, pixels, scale.x * channels)){
            std::cout << "ERROR Failed to write texture file : " << path << std::endl;
        }
    }
    else if(format == "JPEG"){
        if(!stbi_write_jpg((path + folderDistinguisher + title + ".jpeg").c_str(), scale.x, scale.y, channels, pixels, scale.x * channels)){
            std::cout << "ERROR Failed to write texture file : " << path << std::endl;
        }
    }
    else if(format == "BMP"){
        if(!stbi_write_bmp((path + folderDistinguisher + title + ".bmp").c_str(), scale.x, scale.y, channels, pixels)){
            std::cout << "ERROR Failed to write texture file : " << path << std::endl;
        }
    }
    else if(format == "TGA"){
        if(!stbi_write_tga((path + folderDistinguisher + title + ".tga").c_str(), scale.x, scale.y, channels, pixels)){
            std::cout << "ERROR Failed to write texture file : " << path << std::endl;
        }
    }

    delete[] pixels;
}