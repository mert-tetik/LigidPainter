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
#include "UTIL/GL/GL.hpp"

#include <string>
#include <iostream>
#include <vector>

float Slider::render(bool doMouseTracking, glm::vec3 pos, float height, float panel_height, float element_max_height, bool panel_hover){

    if(!gui_initialized){

        slide_btn = Button(ELEMENT_STYLE_BASIC, glm::vec2(1.f, height), "", Texture(), 0.f, false);
        
        bg_btn = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.f, height), "", Texture(), 0.f, false);
        bg_btn.color2 = bg_btn.color;
        
        up_btn = Button(ELEMENT_STYLE_BASIC, glm::vec2(2.f), "", appTextures.arrowT, 0.f, false);
        up_btn.color = glm::vec4(0.f);
        up_btn.outline = false;
        up_btn.outlineExtra = false;
        
        down_btn = Button(ELEMENT_STYLE_BASIC, glm::vec2(2.f), "", appTextures.arrowB, 0.f, false);
        down_btn.color = glm::vec4(0.f);
        down_btn.outline = false;
        down_btn.outlineExtra = false;

        gui_initialized = true;
    }

    // Background button
    this->bg_btn.pos = pos;
    this->bg_btn.scale = glm::vec2(0.5f, height);
    this->bg_btn.render(*getTimer(), true);

    
    float slideRatio = panel_height / element_max_height;

    float available_height = height;
    if(slideRatio < 1){
        // Down button
        this->down_btn.scale.x = this->bg_btn.scale.x;
        this->down_btn.pos = glm::vec3(this->bg_btn.pos.x, this->bg_btn.pos.y + this->bg_btn.scale.y - this->down_btn.scale.y, this->bg_btn.pos.z);
        this->down_btn.render(*getTimer(), doMouseTracking);
        
        // Up button
        this->up_btn.scale.x = this->bg_btn.scale.x;
        this->up_btn.pos = glm::vec3(this->bg_btn.pos.x, this->bg_btn.pos.y - this->bg_btn.scale.y + this->up_btn.scale.y, this->bg_btn.pos.z);
        this->up_btn.render(*getTimer(), doMouseTracking);

        available_height = height - this->up_btn.scale.y - this->down_btn.scale.y;
        
        if(down_btn.clicked)
            this->scroll += 1.f;
        if(up_btn.clicked)
            this->scroll -= 1.f;
        
        if(this->slide_btn.clickState1){ //Pressed
            //Move the slidebar
            this->scroll += Mouse::mouseOffset()->y / Settings::videoScale()->y * 100.f;
        } 
        
        if(this->bg_btn.hover || panel_hover){
            if(*Mouse::mouseScroll() > 100)
                *Mouse::mouseScroll() = 100;
            
            if(*Mouse::mouseScroll() < -100)
                *Mouse::mouseScroll() = -100;
            
            this->scroll -= *Mouse::mouseScroll() / Settings::videoScale()->y * 100.f * slideRatio;
        }
        
        if(this->scroll < 0) //If the slider is out of boundaries
            this->scroll = 0; //Get the slide bar back

        if(((pos.y - available_height) + this->slide_btn.scale.y + this->scroll) + (available_height * slideRatio) > this->bg_btn.pos.y + available_height){
            this->scroll = (this->bg_btn.pos.y + available_height) - (((pos.y - available_height) + this->slide_btn.scale.y) + (available_height * slideRatio));
        }

        this->slide_btn.scale.x = this->bg_btn.scale.x;
        this->slide_btn.scale.y = available_height * slideRatio;
        this->slide_btn.pos = this->bg_btn.pos;  
        this->slide_btn.pos.y = (pos.y - available_height) + this->slide_btn.scale.y + this->scroll;  
        this->slide_btn.pos.z += 0.02f;

        this->slide_btn.render(*getTimer(), doMouseTracking);
    }
    else{
        this->scroll = 0.f;
    }

    return this->scroll / slideRatio * (height / available_height);
}