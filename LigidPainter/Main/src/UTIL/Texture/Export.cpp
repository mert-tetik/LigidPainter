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

//Path : folder path
void Texture::exportTexture(const std::string path){
    glm::vec2 scale;
    char* pixels;
    getData(pixels);
    scale = getResolution();
    
    const int channels = 4;
    if(!stbi_write_png((path + folderDistinguisher + title + ".png").c_str(), scale.x, scale.y, channels, pixels, scale.x * channels)){
        std::cout << "ERROR Failed to write texture file : " << path << std::endl;
    }
    delete[] pixels;
}