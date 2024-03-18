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

#include "GUI/Elements.hpp"
#include "GUI/Dialogs.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

PainterColorSelection::PainterColorSelection(glm::vec2 scale, float panelOffset){
    this->scale = scale;
    this->panelOffset = panelOffset;
    
    clr1_Btn = Button(ELEMENT_STYLE_BASIC, scale, "Color 1", Texture(), 0.f, true);
    clr2_Btn = Button(ELEMENT_STYLE_BASIC, scale, "Color 2", Texture(), 0.f, true);
    clr3_Btn = Button(ELEMENT_STYLE_BASIC, scale, "Color 3", Texture(), 0.f, true);
    dropper_btn = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.f), "", appTextures.dropperIcon, 0.f, false);

    clr1_Btn.color = glm::vec4(glm::vec3(226,212,186) / glm::vec3(255.f), 1.f);
    clr2_Btn.color = glm::vec4(glm::vec3(175,122,109) / glm::vec3(255.f), 1.f);
    clr3_Btn.color = glm::vec4(glm::vec3(101,50,57) / glm::vec3(255.f), 1.f);
    dropper_btn.color = glm::vec4(0.f);
    dropper_btn.outline = false;

    clr1_Btn.colorSelection = true;
    clr2_Btn.colorSelection = true;
    clr3_Btn.colorSelection = true;
}

void PainterColorSelection::render(
                                    Timer &timer, //Timer that handles the animations
                                    bool doMouseTracking //If there is need to check if mouse hover
                                ){
    
    this->doMouseTracking = doMouseTracking;

    clr1_Btn.pos = glm::vec3(this->pos.x - clr1_Btn.scale.x * 2.f, this->pos.y, this->pos.z);
    clr2_Btn.pos = glm::vec3(this->pos.x, this->pos.y, this->pos.z);
    clr3_Btn.pos = glm::vec3(this->pos.x + clr1_Btn.scale.x * 2.f, this->pos.y, this->pos.z);                    

    clr1_Btn.scale.x = this->scale.x / 3.f;
    clr2_Btn.scale.x = this->scale.x / 3.f;
    clr3_Btn.scale.x = this->scale.x / 3.f;       
    
    clr1_Btn.scale.y = this->scale.y - dropper_btn.scale.y;
    clr2_Btn.scale.y = this->scale.y - dropper_btn.scale.y;
    clr3_Btn.scale.y = this->scale.y - dropper_btn.scale.y;

    dropper_btn.pos = clr3_Btn.pos;       
    dropper_btn.pos.x += clr3_Btn.scale.x - dropper_btn.scale.x;
    dropper_btn.pos.y += clr3_Btn.scale.y + dropper_btn.scale.y;

    clr1_Btn.color2 = clr1_Btn.color;
    clr2_Btn.color2 = clr2_Btn.color;
    clr3_Btn.color2 = clr3_Btn.color;
    
    clr1_Btn.textColor = (glm::vec4(1.f) - clr1_Btn.color) * (1.f + clr1_Btn.clickedMixVal / 4.f);
    clr2_Btn.textColor = (glm::vec4(1.f) - clr2_Btn.color) * (1.f + clr2_Btn.clickedMixVal / 4.f);
    clr3_Btn.textColor = (glm::vec4(1.f) - clr3_Btn.color) * (1.f + clr3_Btn.clickedMixVal / 4.f);
    clr1_Btn.textColor.a = 1.f;
    clr2_Btn.textColor.a = 1.f;
    clr3_Btn.textColor.a = 1.f;
    
    clr1_Btn.textColor2 = clr1_Btn.textColor;
    clr2_Btn.textColor2 = clr2_Btn.textColor;
    clr3_Btn.textColor2 = clr3_Btn.textColor;

    clr1_Btn.outlineColor = glm::vec4(1.f) - clr1_Btn.color;
    clr2_Btn.outlineColor = glm::vec4(1.f) - clr2_Btn.color;
    clr3_Btn.outlineColor = glm::vec4(1.f) - clr3_Btn.color;             

    clr1_Btn.outlineThickness = 1.f + clr1_Btn.clickedMixVal * 5.f;
    clr2_Btn.outlineThickness = 1.f + clr2_Btn.clickedMixVal * 5.f;
    clr3_Btn.outlineThickness = 1.f + clr3_Btn.clickedMixVal * 5.f;
    
    clr1_Btn.render(timer, doMouseTracking);
    clr2_Btn.render(timer, doMouseTracking);
    clr3_Btn.render(timer, doMouseTracking);
    dropper_btn.render(timer, doMouseTracking);
    
    if(dropper_btn.clicked){
        Color clr;
        clr.loadRGB_normalized(this->getSelectedButton()->color);
        dialog_dropper.show(timer, &clr);
        this->getSelectedButton()->color.r = clr.getRGB_normalized().r;
        this->getSelectedButton()->color.g = clr.getRGB_normalized().g;
        this->getSelectedButton()->color.b = clr.getRGB_normalized().b;
    }       

    if(clr1_Btn.hover && *Mouse::LClick())
        this->selectedI = 0;
    if(clr2_Btn.hover && *Mouse::LClick())
        this->selectedI = 1;
    if(clr3_Btn.hover && *Mouse::LClick())
        this->selectedI = 2;

    if(this->selectedI != 0)
        clr1_Btn.clickState1 = false;
    else
        clr1_Btn.clickState1 = true;

    if(this->selectedI != 1)
        clr2_Btn.clickState1 = false;
    else
        clr2_Btn.clickState1 = true;

    if(this->selectedI != 2)
        clr3_Btn.clickState1 = false;
    else
        clr3_Btn.clickState1 = true;
}

bool PainterColorSelection::isInteracted(){
    return this->clr1_Btn.clicked || this->clr2_Btn.clicked || this->clr3_Btn.clicked;
}

Button* PainterColorSelection::getSelectedButton(){
    if(this->selectedI == 0)
        return &this->clr1_Btn;
    if(this->selectedI == 1)
        return &this->clr2_Btn;
    if(this->selectedI == 2)
        return &this->clr3_Btn;

    return nullptr;
}