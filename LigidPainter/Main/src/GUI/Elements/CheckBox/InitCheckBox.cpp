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

#include "GUI/Elements/Elements.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

CheckBox::CheckBox(){}

//Manual constructor
CheckBox::CheckBox(std::string text, glm::vec2 scale, glm::vec4 color, glm::vec4 color2, int animationStyle,glm::vec4 textColor,glm::vec4 textColor2,
        float textScale,float panelOffset,glm::vec3 outlineColor,glm::vec3 outlineColor2){
    
    this->text = text;
    this->color = color;
    this->color2 = color2;
    this->scale = scale;
    this->animationStyle = animationStyle;
    this->textColor = textColor;
    this->textColor2 = textColor2;
    this->textScale = textScale;
    this->panelOffset = panelOffset;
    this->outlineColor = outlineColor;
    this->outlineColor2 = outlineColor2;
}

//Style constructor
CheckBox::CheckBox(int style,glm::vec2 scale,std::string text,float panelOffset){
    
    this->text = text;
    this->scale = scale;
    this->panelOffset = panelOffset;
    
    if(style == ELEMENT_STYLE_BASIC){
        this->color = ColorPalette::oppositeColor;
        this->color2 = ColorPalette::themeColor;
        this->animationStyle = 0;
        this->textColor = ColorPalette::oppositeColor;
        this->textColor2 = ColorPalette::themeColor;
        this->textScale = 0.5f;
        this->outlineColor = {};
        this->outlineColor2 = {};
    }
}

CheckBox::CheckBox(int style, glm::vec2 scale, std::string text, float panelOffset, bool (*shortcutFunc)()){
    
    this->shortcutFunc = shortcutFunc;

    *this = CheckBox(style, scale, text, panelOffset);
}