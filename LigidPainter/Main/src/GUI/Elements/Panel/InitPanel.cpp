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

#include "GUI/Elements.hpp"

#include <string>
#include <iostream>
#include <vector>

Panel::Panel(){}
Panel::Panel(Shader shader,ColorPalette colorPalette,std::vector<Section> sections,glm::vec2 scale,glm::vec3 pos,glm::vec4 color,glm::vec4 color2,bool vertical,bool lockL,bool lockR,bool lockB,bool lockT,
      float outlineThickness,int rowCount,std::vector<Button> barButtons,float maxScaleVal,bool hasSlider){
    this->shader = shader;
    this->vertical = vertical;
    this->scale = scale;
    this->color = color;
    this->color2 = color2;
    this->pos = pos;
    this->sections = sections;
    this->outlineThickness = outlineThickness;
    this->leftSide.locked = lockL;
    this->rightSide.locked = lockR;
    this->bottomSide.locked = lockB;
    this->topSide.locked = lockT;
    this->rowCount = rowCount; 
    this->maxScaleVal = maxScaleVal; 
    this->barButtons = barButtons; 
    this->sliderButton = Button(ELEMENT_STYLE_STYLIZED,glm::vec2(0.25f,20),colorPalette,shader,"",Texture(),0.f,false);
    this->sliderButton.color = colorPalette.mainColor;
    this->sliderButton.color2 = colorPalette.themeColor;
    this->sliderButton.radius = 0.25f;
    this->hasSlider = hasSlider;
}