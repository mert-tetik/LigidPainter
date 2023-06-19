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


void CheckBox::render(glm::vec2 videoScale,Mouse& mouse, Timer &timer,TextRenderer &textRenderer,bool doMouseTracking){
    Util util;
    this->doMouseTracking = doMouseTracking;
    // pos value % of the video scale
    glm::vec3 orgResultPos = glm::vec3( 
                          util.getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value
    
    glm::vec3 resultPos = glm::vec3( 
                          util.getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value
    glm::vec3 resultTextPos = glm::vec3( 
                          util.getPercent(videoScale,glm::vec2(pos.x+2.5,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value
    // scale value % of the video scale
    glm::vec2 orgResultScale = util.getPercent(videoScale,scale);
    
    glm::vec2 resultScale = util.getPercent(videoScale,scale);
    resultScale = glm::vec2(std::min(resultScale.x,resultScale.y)); //Make it a square
    resultScale /= 2.f; //Make the box smaller
    resultPos.x -= orgResultScale.x/4.f; //Take the checkbox to the left side
    // scale value % of the video scale
    float resultRadius = util.getPercent(videoScale.x,0.35f); //0.25F = radius val
    
    // Real scale value of the text
    float resultScaleText = videoScale.x/1920/2*textScale;
    
    // Real outline value of the text
    float resultOutlineThickness = videoScale.x/1920/2 * (2.f);//2.f = outline thickness
    //Check if mouse on top of the button
    if(doMouseTracking)
        hover = mouse.isMouseHover(resultScale,glm::vec2(resultPos.x,resultPos.y));
    else 
        hover = false;
    if(hover)
        //Set the cursor as pointer
        mouse.setCursor(mouse.pointerCursor);// mouse.activeCursor = mouse.pointerCursor
    if(hover && mouse.LClick){
        //Mouse left button pressed on top of the button
        clickState1 = !clickState1;
    }
    timer.transition(hover,hoverMixVal,0.2f); 
    timer.transition(clickState1,clickedMixVal,0.15f); 
    
    //Render the button
    render(resultPos,resultScale,resultRadius,resultOutlineThickness);
    //Render the text
    shader.setVec4("color"  ,     textColor     );
    shader.setVec4("color2"  ,     textColor2     );
    
    textRenderer.loadTextData(
                                shader,
                                text,
                                glm::vec3(resultTextPos.x,resultTextPos.y,resultTextPos.z + 0.02f),
                                false,
                                resultScaleText,
                                resultTextPos.x-resultScale.x,
                                orgResultPos.x + orgResultScale.x,
                                TEXTRENDERER_ALIGNMENT_MID
                             );

    textRenderer.renderText(shader);
}