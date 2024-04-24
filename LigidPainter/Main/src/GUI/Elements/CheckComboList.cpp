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
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <map>

static std::map<std::string, CheckComboList*> all_checkcombolists;

CheckComboList::CheckComboList(std::string info, std::string title, Texture texture, glm::vec3 pos, float panelOffset, Section& section){
    this->info = info;
    this->title = title;
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
                            )
{
                        
    all_checkcombolists[this->info] = this;

    if(this->arrowButton.hover && *Mouse::LClick()){
        for (auto it = all_checkcombolists.begin(); it != all_checkcombolists.end(); ++it) {
            if(it->second->arrowButton.clickState1 && it->first != this->info)
                it->second->arrowButton.clickState1 = false;
        }
    }

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
    panel.scale = glm::vec2(glm::sin(arrowButton.clickedMixVal));
    panel.scale.x *= 10.f;
    panel.scale.y *= panel.get_elements_height() + 2.f;
    panel.pos.y += arrowButton.scale.y + panel.scale.y - 0.2f;
    panel.pos.z = 0.9f;
    if(panel.scale.x > 2.f){
        // Prevent coliding with the title info button
        panel.sections[0].elements[0].panelOffset += 1.f;
        // Render the main panel        
        panel.render(timer, doMouseTracking);
        // Set the panel offset value back
        panel.sections[0].elements[0].panelOffset -= 1.f;
    }

    Button title_info_btn = Button(ELEMENT_STYLE_SOLID, glm::vec2(panel.scale.x, 1.f), this->title, Texture(), 0.f, false);
    title_info_btn.color.a = 1.f;
    title_info_btn.pos = glm::vec3(this->panel.pos.x, this->panel.pos.y - this->panel.scale.y + title_info_btn.scale.y, this->panel.pos.z);
    title_info_btn.render(timer, false);

    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);

    if(hover)
        lastTimeHover = LigidGL::getTime();

    if(LigidGL::getTime() - lastTimeHover > 0.5)
        arrowButton.clickState1 = false;
}