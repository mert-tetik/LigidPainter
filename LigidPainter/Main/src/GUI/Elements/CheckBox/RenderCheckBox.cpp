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
#include "UTIL/Settings/Settings.hpp"

#include <string>
#include <iostream>
#include <vector>


void CheckBox::render(Timer &timer,bool doMouseTracking){
    
    this->doMouseTracking = doMouseTracking;

    //Original position in the screen coordinates
    glm::vec3 orgResultPos = glm::vec3( 
                          UTIL::getPercent(*Settings::videoScale(), glm::vec2(pos.x,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value
    
    //Modified position in the screen coordinates
    glm::vec3 resultPos = glm::vec3( 
                          UTIL::getPercent(*Settings::videoScale(), glm::vec2(pos.x,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value
    
    //Text position in the screen coordinates
    glm::vec3 resultTextPos = glm::vec3( 
                          UTIL::getPercent(*Settings::videoScale(), glm::vec2(pos.x+2.5,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value
    
    //Original scale value in the screen coordinates
    glm::vec2 orgResultScale = UTIL::getPercent(*Settings::videoScale(), scale);
    
    //Modified scale value in the screen coordinates
    glm::vec2 resultScale = UTIL::getPercent(*Settings::videoScale(), scale);
    
    //Make the circle a square
    resultScale = glm::vec2(std::min(resultScale.x,resultScale.y));
    
    //Make the circle smaller
    resultScale /= 2.f;
    
    //Take the circle to the left side
    resultPos.x -= orgResultScale.x/4.f;
    
    //Radius value in the screen coordinates
    float resultRadius = UTIL::getPercent(Settings::videoScale()->x,0.35f); //0.25F = radius val
    
    //Text scale value in the screen coordinates
    float resultScaleText = Settings::videoScale()->x/1920/2*textScale;
    
    //Outline thickness value in the screen coordinates
    float resultOutlineThickness = Settings::videoScale()->x/1920/2 * (2.f);//2.f = outline thickness
    
    //Mouse activites
    manageMouseActivity(resultScale,resultPos);

    //Mouse hover animation
    timer.transition(hover,hoverMixVal,0.2f); 
    
    //Pressing animation
    timer.transition(clickState1,clickedMixVal,0.15f); 
    
    //Render the button
    render(resultPos,resultScale,resultRadius,resultOutlineThickness);
    
    //Set the color of the text 
    ShaderSystem::buttonShader().setVec4("properties.color"  ,     textColor     );
    ShaderSystem::buttonShader().setVec4("properties.color2"  ,     textColor2     );

    //Render the text
    textRenderer.loadTextData(
                                text,
                                glm::vec3(resultTextPos.x,resultTextPos.y,resultTextPos.z + 0.02f),
                                false,
                                resultScaleText,
                                orgResultPos.x - orgResultScale.x,
                                orgResultPos.x + orgResultScale.x,
                                TEXTRENDERER_ALIGNMENT_MID
                             );

    textRenderer.renderText();

    if(!this->hover)
        hoverDuration = 0;
    else{
        if(timer.tick){
            hoverDuration++;
        }
    }

    if(hoverDuration > 2 && this->infoText.size()){
        Button infoBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 1.3f), this->infoText, Texture(), 0.f, false);
        infoBtn.pos = this->pos;
        infoBtn.pos.z += 0.03f;
        infoBtn.render(timer, false);
    }

    if(this->shortcutFunc != nullptr){
        if(this->shortcutFunc()){
            this->clickState1 = !this->clickState1;
        }
    }
}