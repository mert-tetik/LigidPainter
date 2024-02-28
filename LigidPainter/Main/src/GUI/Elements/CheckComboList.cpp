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
#include "ShaderSystem/Shader.hpp"
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

CheckComboList::CheckComboList(std::string info, Texture texture, glm::vec3 pos, float panelOffset, Section& section){
    this->info = info;
    this->pos = pos;
    this->panelOffset = panelOffset;
    this->checkButton = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.f, Settings::videoScale()->x / Settings::videoScale()->y), "", texture, 0.f, true);
    this->arrowButton = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.f, Settings::videoScale()->x / Settings::videoScale()->y), "", appTextures.arrowB, 0.f, true);
    this->panel = Panel(
        {
            section
        },
        glm::vec2(24.f, 40.f),
        glm::vec3(50.f,50.f,0.8f),
        ColorPalette::mainColor,
        ColorPalette::thirdColor,
        true,
        true,
        true,
        true,
        true,
        1.f,
        1.f,
        {},
        0.25f,
        false
    );
    this->checkButton.scale /= 1.6f;
    this->arrowButton.scale /= 1.6f;

    checkButton.color = ColorPalette::secondColor;
    arrowButton.color = ColorPalette::secondColor;
    arrowButton.color2 = glm::vec4(0,0,0,1);
    arrowButton.textureSizeScale = 1.6f;
}

void CheckComboList::render(
                        Timer &timer, //Timer that handles the animations
                        bool doMouseTracking //If there is need to check if mouse hover
                    ){
                        
    this->doMouseTracking = doMouseTracking;

    // pos value % of the video scale
    glm::vec3 resultPos = glm::vec3( 
                          UTIL::getPercent(*Settings::videoScale(), glm::vec2(pos.x,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value


    // scale value % of the video scale
    glm::vec2 resultScale = UTIL::getPercent(*Settings::videoScale(), scale);

    checkButton.pos = pos;
    checkButton.pos.x -= checkButton.scale.x;
    checkButton.infoText = info;
    arrowButton.pos = pos;
    arrowButton.pos.x += arrowButton.scale.x;
    this->checkButton.render(timer, doMouseTracking);
    this->arrowButton.render(timer, doMouseTracking);

    this->scale.x = checkButton.scale.x + arrowButton.scale.x;
    this->scale.y = checkButton.scale.y;

    this->hover = checkButton.hover || arrowButton.hover || panel.hover;

    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", arrowButton.clickedMixVal);
    panel.pos = arrowButton.pos;
    panel.scale = glm::vec2(glm::sin(arrowButton.clickedMixVal) * 10.f);
    panel.scale.y *= 2.f;
    panel.pos.y += arrowButton.scale.y + panel.scale.y - 0.2f;
    panel.pos.z = 0.9f;
    if(panel.scale.x > 2.f)
        panel.render(timer, doMouseTracking);
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);

    if(hover)
        lastTimeHover = LigidGL::getTime();

    if(LigidGL::getTime() - lastTimeHover > 0.5)
        arrowButton.clickState1 = false;

}