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
#include <glm/gtx/string_cast.hpp>

#include "GUI/Elements.hpp"

#include <string>
#include <iostream>
#include <vector>

/// @brief renders the button element
/// @param videoScale resolution value of the primary monitor
/// @param mouse local mouse class (manage mouse activities)
/// @param timer local timer class (manage the time (for transitions))
/// @param textRenderer local textRenderer class (manage text rendering & keyboard inputs)
/// @param doMouseTracking don't check mouse hover if set to false
void Button::render(
                        glm::vec2 videoScale,
                        Mouse& mouse,
                        Timer &timer,
                        TextRenderer &textRenderer,
                        bool doMouseTracking
                    ){
    
    ;

    this->doMouseTracking = doMouseTracking;

    //Get the real position value (0 - 100 to screen location)
    resultPos = glm::vec3( 
                          UTIL::getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value

    //Get the real scale value
    resultScale = UTIL::getPercent(videoScale,scale);
    
    //Get the real radius value
    float resultRadius = UTIL::getPercent(videoScale.x,radius);
    
    //Get the real text scale value
    float resultScaleText = videoScale.x/1920/2*textScale;
    
    //Get the real outline thickness value
    float resultOutlineThickness = videoScale.x/1920/2*outlineThickness;

    //Manage mouse activites
    manageMouseActivity(mouse);

    //Manage the hover animation of the button 
    timer.transition(hover,hoverMixVal,0.2f); 
    
    //Manage the click animation of the button 
    if(keepPressingState)
        timer.transition(clickState1,clickedMixVal,0.5f); //Change the color by the state of being clicked
    else
        timer.transition(false,clickedMixVal,0.5f); //Take the value down repedatedly (since clickedMixVal will be set to 1. when clicking is done)
    
    //Render the button
    render(resultPos,resultScale,resultRadius,resultOutlineThickness);
    
    //Render the text and the texture
    renderTextAndTexture(textRenderer,videoScale,resultScaleText);
}