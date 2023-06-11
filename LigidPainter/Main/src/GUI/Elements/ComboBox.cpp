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
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

    void ComboBox::render(glm::vec3 resultPos,glm::vec2 resultScale,float resultRadius,float resultOutlineThickness,float aClickedMixVal,float aHoverMixVal,
                bool outline,glm::vec4 aColor){
        
        //Set the transform data (used by vertex shader)
        shader.setVec3("pos"    ,     resultPos );
        shader.setVec2("scale"  ,     resultScale);
        
        if(pressed)//If button is pressed
            shader.setVec4("color"  ,     aColor * glm::vec4(2.f,2.f,2.f,1.f)     ); //Button pressing color
        else
            shader.setVec4("color"  ,     aColor     ); //Default button color
        
        shader.setVec4("color2"  ,     color2     ); //Second color that is used by hover or click animations
        
        shader.setFloat("colorMixVal"  ,     (aClickedMixVal + aHoverMixVal)/2.f   );

        //Properties
        shader.setFloat("radius",     resultRadius    );
        
        if(outline)
            shader.setInt("outlineState" ,     1      ); 
        else
            shader.setInt("outlineState" ,    0      ); 

        //Outline extra color (affected by the colorMixVal)
        shader.setVec3("outlineColor" ,     outlineColor     );  
        shader.setVec3("outlineColor2" ,     outlineColor2     );   

        shader.setFloat("thickness" ,    resultOutlineThickness + aClickedMixVal*4.f ); 
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    ComboBox::ComboBox(){}
    
    //Manual constructor
    ComboBox::ComboBox(Shader shader,std::vector<std::string> texts, glm::vec2 scale, glm::vec4 color, glm::vec4 color2,glm::vec4 textColor,glm::vec4 textColor2,
           float textScale,float panelOffset,glm::vec3 outlineColor,glm::vec3 outlineColor2){
        
        this->shader = shader;
        this->texts = texts;
        this->color = color;
        this->color2 = color2;
        this->scale = scale;
        this->textColor = textColor;
        this->textColor2 = textColor2;
        this->textScale = textScale;
        this->panelOffset = panelOffset;
        this->outlineColor = outlineColor;
        this->outlineColor2 = outlineColor2;

        for (size_t i = 0; i < texts.size(); i++)
        {
            hover.push_back(0);
            hoverMixVal.push_back(0);
            clickedMixVal.push_back(0);
        }
    }

    //Style constructor
    ComboBox::ComboBox(int style,glm::vec2 scale,ColorPalette colorPalette,Shader shader,std::vector<std::string> texts,float panelOffset){
        this->shader = shader;
        this->texts = texts;
        this->scale = scale;
        this->panelOffset = panelOffset;
        
        if(style == 0){
            this->bgColor = colorPalette.mainColor;
            this->color = colorPalette.oppositeColor;
            this->color2 = colorPalette.themeColor;
            this->textColor = colorPalette.oppositeColor;
            this->textColor2 = colorPalette.themeColor;
            this->textScale = 0.5f;
            this->outlineColor = {};
            this->outlineColor2 = {};
        }

        for (size_t i = 0; i < texts.size(); i++)
        {
            hover.push_back(0);
            hoverMixVal.push_back(0);
            clickedMixVal.push_back(0);
        }
    }



void ComboBox::render(
                        glm::vec2 videoScale, //Resolution of the monitor
                        Mouse& mouse, //Mouse class to access mouse events
                        Timer &timer, //Timer that handles the animations
                        TextRenderer &textRenderer, //TextRenderer that handles text rendering
                        bool doMouseTracking, //If there is need to check if mouse hover
                        GLFWwindow* window //To take the key inputs
                    ){
    Util util;
    
    this->doMouseTracking = doMouseTracking;
    
    pos.z = 0.95f;
    
    glm::vec3 orgResultPos = glm::vec3( 
                          util.getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value
    
    glm::vec3 resultTextPos = glm::vec3( 
                          util.getPercent(videoScale,glm::vec2(pos.x+2.5,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value
    
    glm::vec2 orgResultScale = util.getPercent(videoScale,scale);
    
    glm::vec2 resultScale = util.getPercent(videoScale,scale);

    float resultRadius = util.getPercent(videoScale.x,0.35f); //0.25F = radius val
    
    float resultScaleText = videoScale.x/1920/2*textScale;
    
    float resultOutlineThickness = videoScale.x/1920/2 * (2.f);//2.f = outline thickness

    glm::vec3 bgResultPos = glm::vec3( 
              util.getPercent(videoScale,glm::vec2(pos.x,pos.y + (texts.size() * scale.y*2.f * clickedMixVal[0])/2.f - scale.y)) //Don't include the depth
              ,pos.z); //Use the original depth value
    
    glm::vec2 bgResultScale = util.getPercent(videoScale,glm::vec2(scale.x,(texts.size() * scale.y * clickedMixVal[0])));
    
    render(bgResultPos,bgResultScale,resultRadius,resultOutlineThickness,0.f,0.f,false,bgColor);
    
    //Render the buttons
    for (size_t i = 0; i < texts.size(); i++)
    {
        glm::vec3 resultPos = glm::vec3( 
                  util.getPercent(videoScale,glm::vec2(pos.x,pos.y + i * scale.y*2.f * clickedMixVal[0])) //Don't include the depth
                  ,pos.z); //Use the original depth value
        //Check if mouse on top of the button
        if(doMouseTracking)
            hover[i] = mouse.isMouseHover(resultScale,glm::vec2(resultPos.x,resultPos.y));
        else 
            hover[i] = false;
        if(hover[i])
        //Set the cursor as pointer
            mouse.setCursor(mouse.pointerCursor);// mouse.activeCursor = mouse.pointerCursor
        if(hover[i] && mouse.LClick){
            if(i != 0){
                if(clickedMixVal[0] > 0.8f){
                    //Mouse left button pressed on top of the button
                    pressed = !pressed;
                    selectedIndex = i;
                }
            }
            else{
                if(selectedIndex != 0)
                    selectedIndex = i;
                pressed = !pressed;
            }
        }
        timer.transition(hover[i],hoverMixVal[i],0.2f); 
        timer.transition(pressed,clickedMixVal[i],0.15f);
        render(resultPos,resultScale,resultRadius,resultOutlineThickness,hoverMixVal[i],clickedMixVal[i],true,color);
        
        if(!pressed){
            textRenderer.renderText(shader,texts[selectedIndex],resultPos.x,resultPos.y,resultPos.z+0.02f,resultPos.x + resultScale.x,false,resultScaleText,resultPos.x-resultScale.x,false,false);
        }
        else{
            textRenderer.renderText(shader,texts[i],resultPos.x,resultPos.y,resultPos.z+0.02f,resultPos.x + resultScale.x,false,resultScaleText,resultPos.x-resultScale.x,false,false);
        }
        if(i == 0 && !pressed && clickedMixVal[0] < 0.2f)
            break;
    }
    
    if((!hover[0] && mouse.LClick) || glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window,GLFW_KEY_ENTER) == GLFW_PRESS){
        pressed = false;
    }
    
    //Render the text
    //Update the parameters of the renderText function in the renderTheTexture function if this function's parameters are changed
}