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
#include <glm/gtx/string_cast.hpp>

#include "GUI/Elements/Elements.hpp"

#include <string>
#include <iostream>
#include <vector>

Button::Button(){}

/// @brief Manual constructor
/// @param shader 
/// @param text 
/// @param scale 
/// @param color 
/// @param color2 
/// @param outline 
/// @param radius 
/// @param animationStyle 
/// @param textColor 
/// @param textColor2 
/// @param texture 
/// @param textScale 
/// @param panelOffset 
/// @param outlineExtra 
/// @param outlineColor 
/// @param outlineColor2 
/// @param outlineThickness 
/// @param keepPressingState 
Button::Button(
                    Shader shader,
                    std::string text, 
                    glm::vec2 scale, 
                    glm::vec4 color, 
                    glm::vec4 color2, 
                    bool outline, 
                    float radius, 
                    int animationStyle,
                    glm::vec4 textColor,
                    glm::vec4 textColor2,
                    Texture texture,
                    float textScale,
                    float panelOffset,
                    bool outlineExtra,
                    glm::vec3 outlineColor,
                    glm::vec3 outlineColor2,
                    float outlineThickness,
                    bool keepPressingState
                ){

    this->shader = shader;
    this->text = text;
    this->color = color;
    this->color2 = color2;
    this->scale = scale;
    this->outline = outline;
    this->radius = radius;
    this->animationStyle = animationStyle;
    this->textColor = textColor;
    this->textColor2 = textColor2;
    this->texture = texture;
    this->textScale = textScale;
    this->panelOffset = panelOffset;
    this->outlineExtra = outlineExtra;
    this->outlineColor = outlineColor;
    this->outlineColor2 = outlineColor2;
    this->outlineThickness = outlineThickness;
    this->keepPressingState = keepPressingState;
}


/// @brief style constructor
/// @param style is ELEMENT_STYLE_BASIC, ELEMENT_STYLE_SOLID or ELEMENT_STYLE_STYLIZED
/// @param scale scale value of the button in the range of 0 - 100 
/// @param colorPalette color theme of the LigidPainter
/// @param shader buttonShader
/// @param text text of the button
/// @param texture texture of the button (takes texture class)
/// @param panelOffset gap value between the button & the previous button element
/// @param keepPressingState make button work like a checkbox rather than a button
Button::Button(
                    int style,
                    glm::vec2 scale,
                    ColorPalette colorPalette,
                    Shader shader,
                    std::string text,
                    Texture texture,
                    float panelOffset,
                    bool keepPressingState
                ){

    this->shader = shader;
    this->text = text;
    this->scale = scale;
    this->texture = texture;
    this->panelOffset = panelOffset;
    this->keepPressingState = keepPressingState;
    
    if(style == ELEMENT_STYLE_STYLIZED){
        this->color = colorPalette.oppositeColor;
        this->color2 = colorPalette.themeColor;
        this->outline = true;
        this->radius = 0.5f;
        this->animationStyle = 0;
        this->textColor = colorPalette.oppositeColor;
        this->textColor2 = colorPalette.themeColor;
        this->textScale = 0.5f;
        this->outlineExtra = false;
        this->outlineColor = {};
        this->outlineColor2 = {};
        this->outlineThickness = 2.f;
    }
    if(style == ELEMENT_STYLE_SOLID){
        this->color = colorPalette.secondColor;
        this->color2 = colorPalette.themeColor;
        this->outline = false;
        this->radius = 0.1f;
        this->animationStyle = 1;
        this->textColor = colorPalette.oppositeColor;
        this->textColor2 = colorPalette.thirdColor;
        this->textScale = 0.5f;
        this->outlineExtra = true;
        this->outlineColor = colorPalette.oppositeColor;
        this->outlineColor2 = colorPalette.thirdColor;
        this->outlineThickness = 1.f;
        this->textureSizeScale = 1.f;
    }
    if(style == ELEMENT_STYLE_BASIC){
        this->color = colorPalette.secondColor;
        this->color2 = colorPalette.themeColor;
        this->outline = false;
        this->radius = 0.25f;
        this->animationStyle = 1;
        this->textColor = colorPalette.oppositeColor;
        this->textColor2 = colorPalette.thirdColor;
        this->textScale = 0.5f;
        this->outlineExtra = true;
        this->outlineColor = colorPalette.oppositeColor;
        this->outlineColor2 = colorPalette.thirdColor;
        this->outlineThickness = 2.f;
        this->textureSizeScale = 2.f;
    }
}