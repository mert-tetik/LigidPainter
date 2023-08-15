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

#include "GUI/Elements/Elements.hpp"
#include "ShaderSystem/Shader.hpp"
#include "MouseSystem/Mouse.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

void ComboBox::render(
                        glm::vec2 videoScale, //Resolution of the monitor
                        //Mouse class to access mouse events
                        Timer &timer, //Timer that handles the animations
                        TextRenderer &textRenderer, //TextRenderer that handles text rendering
                        bool doMouseTracking, //If there is need to check if mouse hover
                        LigidWindow window //To take the key inputs
                    ){
    
    if(this->pressed)
        glDepthFunc(GL_ALWAYS);
    
    //Take the doMouseTracking param to the public class member variable
    this->doMouseTracking = doMouseTracking;
    
    //ComboBox's depth value is always 0.95f (idk why might remove that line later)
    pos.z = 0.95f;
    
    //Original position in the screen coordinates
    glm::vec3 orgResultPos = glm::vec3( 
                          UTIL::getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value
    
    //Modified position in the screen coordinates
    glm::vec3 resultTextPos = glm::vec3( 
                          UTIL::getPercent(videoScale,glm::vec2(pos.x+2.5,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value
    

    //Original scale in the screen coordinates
    glm::vec2 orgResultScale = UTIL::getPercent(videoScale,scale);
    
    //Modified scale in the screen coordinates
    glm::vec2 resultScale = UTIL::getPercent(videoScale,scale);

    //Radius in the screen coordinates
    float resultRadius = UTIL::getPercent(videoScale.x,0.35f); //0.25F = radius val
    
    //Text scale value in the screen coordinates
    float resultScaleText = videoScale.x/1920/2*textScale;
    
    //Thickness in the screen coordinates
    float resultOutlineThickness = videoScale.x/1920/2 * (3.3f);//3.3f = outline thickness

    //Background position in the screen coordinates (background that covers the back when the combobox is pressed) (beneath the elements)
    glm::vec3 bgResultPos = glm::vec3( 
              UTIL::getPercent(videoScale,glm::vec2(pos.x,pos.y + (texts.size() * scale.y*2.f * clickedMixVal[0])/2.f - scale.y)) //Don't include the depth
              ,pos.z); //Use the original depth value
    
    //Background scale in the screen coordinates (background that covers the back when the combobox is pressed) (beneath the elements)
    glm::vec2 bgResultScale = UTIL::getPercent(videoScale,glm::vec2(scale.x,(texts.size() * scale.y * clickedMixVal[0])));
    
    //Render the background
    render(bgResultPos,bgResultScale,resultRadius,resultOutlineThickness,0.f,0.f,false,bgColor);
    
    glm::vec3 resultPos = glm::vec3( 
      UTIL::getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
      ,pos.z); //Use the original depth value
    
    
    //Render the combobox title
    textRenderer.loadTextData(
                                text,
                                glm::vec3(resultPos.x ,resultPos.y - resultScale.y,resultPos.z+0.002f),
                                false,
                                resultScaleText,
                                resultPos.x - resultScale.x,
                                resultPos.x + resultScale.x,
                                TEXTRENDERER_ALIGNMENT_MID
                             );
    
    //Set the color of the title
    if(pressed)//If button is pressed
        ShaderSystem::buttonShader().setVec4("properties.color"  ,     color * glm::vec4(2.f,2.f,2.f,1.f)     ); //Button pressing color
    else
        ShaderSystem::buttonShader().setVec4("properties.color"  ,     color     ); //Default button color
    ShaderSystem::buttonShader().setVec4("properties.color2"  ,     color2     ); //Second color that is used by hover or click animations

    textRenderer.renderText();

    //Render the title barrier
    glm::vec2 textBoxScale = resultScale;
    textBoxScale.x = textRenderer.getTextLastCharOffset();
    render(glm::vec3(resultPos.x ,resultPos.y - resultScale.y,1.),textBoxScale,0.f,0.f,0.f,0.f,true,glm::vec4(0));

    //Render the buttons (elements)
    for (size_t i = 0; i < texts.size(); i++)
    {
        //Position value in the screen coordinates
        glm::vec3 resultPos = glm::vec3( 
                  UTIL::getPercent(videoScale,glm::vec2(pos.x,pos.y + i * scale.y*2.f * clickedMixVal[0])) //Don't include the depth
                  ,pos.z); //Use the original depth value
        
        //Check if mouse on top of the element
        if(doMouseTracking)
            hover[i] = Mouse::isMouseHover(resultScale,glm::vec2(resultPos.x,resultPos.y));
        else 
            hover[i] = false;
        
        //Set the cursor as pointer if hovers the element
        if(hover[i])
            Mouse::setCursor(*Mouse::pointerCursor()); //mouse.activeCursor = *Mouse::pointerCursor()
        
        //If clicked to the element
        if(hover[i] && *Mouse::LClick()){
            if(clickedMixVal[0] > 0.5f){
                //Mouse left button pressed on top of the button
                selectedIndex = i;
                pressed = false;
                *Mouse::LClick() = false;
            }
            else
                pressed = true;
        }

        //Element hover animation
        timer.transition(hover[i],hoverMixVal[i],0.2f); 
        
        //Element pressing animation 
        timer.transition(pressed,clickedMixVal[i],0.15f);
        
        //Render the element
        render(resultPos,resultScale,resultRadius,resultOutlineThickness,hoverMixVal[i],clickedMixVal[i],true,color);

        //Determine the text of the element
        std::string boxText;
        if(!pressed){
            boxText = texts[selectedIndex];
        }
        else{
            boxText = texts[i];
        }

        //Render the text
        textRenderer.loadTextData(
                                    boxText,
                                    glm::vec3(resultPos.x,resultPos.y,resultPos.z + 0.02f),
                                    false,
                                    resultScaleText,
                                    resultPos.x - resultScale.x,
                                    resultPos.x + resultScale.x,
                                    TEXTRENDERER_ALIGNMENT_MID
                                );
        glDisable(GL_DEPTH_TEST);
        textRenderer.renderText();
        glEnable(GL_DEPTH_TEST);

        //Just render the first element if not pressed
        if(i == 0 && !pressed && clickedMixVal[0] < 0.2f)
            break;
    }
    
    if(this->pressed)
        glDepthFunc(GL_LEQUAL);
    
    //Unpress
    if(window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || window.isKeyPressed(LIGIDGL_KEY_ENTER) == LIGIDGL_PRESS || *Mouse::LClick() && !hover[0]){
        pressed = false;
    }
}