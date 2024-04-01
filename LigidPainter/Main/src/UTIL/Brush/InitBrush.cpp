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
        float displayRadius,
        float spacing,
        float hardness,
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
    this->properties.spacing = spacing;
    this->properties.hardness = hardness;
    this->properties.sizeJitter = sizeJitter;
    this->properties.scatter = scatter;
    this->properties.fade = fade;
    this->properties.rotation = rotation;
    this->properties.rotationJitter = rotationJitter;
    this->properties.alphaJitter = alphaJitter;
    this->properties.individualTexture = individualTexture;
    this->properties.sinWavePattern = sinWavePattern;
    this->title = title;

    this->properties.brushTexture.proceduralProps = texture.proceduralProps;
    this->properties.brushTexture.ID = texture.duplicateTexture();

    this->displayingTexture = Texture(nullptr, 100, 100, GL_LINEAR);
    this->displayingTexture.title = "BrushDisplayingTexture";    
    
    this->updateDisplayTexture(displayRadius);
}

Brush::Brush(BrushProperties brushProperties, std::string title){
    this->properties = brushProperties;
    this->title = title;
}


void Brush::update(
        float displayRadius,
        float spacing,
        float hardness,
        float sizeJitter,
        float scatter,
        float fade,
        float rotation,
        float rotationJitter,
        float alphaJitter,
        bool individualTexture,
        bool sinWavePattern,
        Texture texture
    )
{
    this->properties.spacing = spacing;
    this->properties.hardness = hardness;
    this->properties.sizeJitter = sizeJitter;
    this->properties.scatter = scatter;
    this->properties.fade = fade;
    this->properties.rotation = rotation;
    this->properties.rotationJitter = rotationJitter;
    this->properties.alphaJitter = alphaJitter;
    this->properties.individualTexture = individualTexture;
    this->properties.sinWavePattern = sinWavePattern;
    
    this->properties.brushTexture.proceduralProps = texture.proceduralProps;
    this->properties.brushTexture.ID = texture.duplicateTexture();

    this->updateDisplayTexture(displayRadius);
}