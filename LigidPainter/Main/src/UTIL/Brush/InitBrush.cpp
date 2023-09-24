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
}

Brush::Brush(
        float spacing,
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
    this->spacing = spacing;
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

    this->initDisplayingTexture();
    this->updateDisplayTexture(0.1, 1.f);
}




//-------- INIT UTIL ---------

void Brush::initDisplayingTexture(){
    this->displayingTexture = Texture(nullptr, 100, 100, GL_LINEAR);
    this->displayingTexture.title = "BrushDisplayingTexture";
}