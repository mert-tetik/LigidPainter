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
    
Brush::Brush(){
    initDisplayingTexture();
}

Brush::Brush(
        float sizeJitter,
        float scatter,
        float fade,
        float rotation,
        float rotationJitter,
        float alphaJitter,
        bool individualTexture,
        bool sinWavePattern,
        std::string title,
        Texture texture
    )
{
    this->sizeJitter = sizeJitter;
    this->scatter = scatter;
    this->fade = fade;
    this->rotation = rotation;
    this->rotationJitter = rotationJitter;
    this->alphaJitter = alphaJitter;
    this->individualTexture = individualTexture;
    this->sinWavePattern = sinWavePattern;
    this->title = title;
    this->texture = texture;

    initDisplayingTexture();
}




//-------- INIT UTIL ---------

void Brush::initDisplayingTexture(){
    glGenTextures(1,&displayingTexture);
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D,displayingTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 100, 100, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
}