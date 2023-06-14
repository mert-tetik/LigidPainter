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

#include "GUI/Elements.hpp"

#include <string>
#include <iostream>
#include <vector>



NodeIO::NodeIO(std::string title ,Element element,glm::vec4 color,ColorPalette colorPalette,Shader buttonShader,glm::vec2 videoScale, int state){
    this->title = title;
    this->element = element;
    this->color = color;
    this->state = state;
    
    IOCircle = Button(0,glm::vec2(videoScale.y,videoScale.x)/glm::vec2(videoScale.y*2.5f),colorPalette,buttonShader,"",Texture(),0.f,false);
    IOCircle.radius = 0.25f;
}