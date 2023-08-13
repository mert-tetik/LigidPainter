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

#include "NodeSystem/Node/Node.hpp"

#include <string>
#include <iostream>
#include <vector>

NodeIO::NodeIO(std::string title ,Element element,glm::vec4 color,ColorPalette colorPalette,glm::vec2 videoScale, int state){
    this->title = title;
    this->element = element;
    this->color = color;
    this->state = state;
    
    IOCircle = Button(ELEMENT_STYLE_STYLIZED,glm::vec2(videoScale.y,videoScale.x)/glm::vec2(videoScale.y*2.5f),colorPalette,"",Texture(),0.f,false);
    IOCircle.radius = 0.25f;
}