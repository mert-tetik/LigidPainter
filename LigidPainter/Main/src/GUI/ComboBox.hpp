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

#ifndef LGDUICOMBOBOX_HPP
#define LGDUICOMBOBOX_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Shader.hpp"
#include "Box.hpp"
#include "Renderer.h"
#include "Util.hpp"
#include "Timer.hpp"
#include "Texture.hpp"

#include "GUI/Button.hpp"
#include "Mouse.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

class ComboBox
{
private:
    void render(glm::vec3 resultPos,glm::vec2 resultScale,float resultRadius,float resultOutlineThickness,float aClickedMixVal,float aHoverMixVal,bool outline,glm::vec4 aColor){
        
        //Set the transform data (used by vertex shader)
        shader.setVec3("pos"    ,     resultPos );
        shader.setVec2("scale"  ,     resultScale);
        
        if(pressed)//If button is pressed
            shader.setVec4("color"  ,     aColor * glm::vec4(2.f,2.f,2.f,1.f)     ); //Button pressing color
        else
            shader.setVec4("color"  ,     aColor     ); //Default button color
        
        shader.setVec4("color2"  ,     color2     ); //Second color that is used by hover or click animations
        
        shader.setFloat("colorMixVal"  ,     (aClickedMixVal + aHoverMixVal)/2.f   );

        //Set the resolution of the button (used by fragment shader)
        shader.setFloat("width" ,     resultScale.x   );
        shader.setFloat("height",     resultScale.y   );

        //Properties
        shader.setFloat("radius",     resultRadius    );
        shader.setInt("outline" ,     outline      ); 
        shader.setInt("outlineExtra" ,     false     ); 

        //Outline extra color (affected by the colorMixVal)
        shader.setVec3("outlineColor" ,     outlineColor     );  
        shader.setVec3("outlineColor2" ,     outlineColor2     );   

        shader.setFloat("thickness" ,    resultOutlineThickness + aClickedMixVal*4.f ); 
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

public:
    Shader shader;

    float panelOffset = 0.f; //While rendering in the panel put that much space in % between the next button element

    std::vector<std::string> texts; 

    glm::vec4 color; //Original color
    glm::vec4 color2; //Hover or clicked transition color
    glm::vec4 textColor; //Original color of the text
    glm::vec4 textColor2; //Hover or clicked transition color of the text

    glm::vec4 bgColor; //Hover or clicked transition color

    bool pressed = false;

    float textScale;

    glm::vec3 outlineColor;
    glm::vec3 outlineColor2;

    //Will be display on top of the button
    //Left side if text

    //Those values are adepting to the panel if button is attached to one
    //Values are percentage of the monitor size
    glm::vec2 scale;  //Example : w : 20, h : 30 means cover %20 of the window in x axis & cover 30% of the window in y axis
    glm::vec3 pos; //Same here
    
    bool doMouseTracking;

    std::vector<bool> hover;

    int selectedIndex = 0;

    std::vector<float> hoverMixVal;
    std::vector<float> clickedMixVal;

    ComboBox(){}




    //----Constructors 

    //Manual constructor
    ComboBox(Shader shader,std::vector<std::string> texts, glm::vec2 scale, glm::vec4 color, glm::vec4 color2,glm::vec4 textColor,glm::vec4 textColor2,
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
    ComboBox(int style,glm::vec2 scale,ColorPalette colorPalette,Shader shader,std::vector<std::string> texts,float panelOffset){
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



    void render(glm::vec2 videoScale,Mouse& mouse, Timer &timer,TextRenderer &textRenderer,bool doMouseTracking,GLFWwindow* window){
        Util util;

        this->doMouseTracking = doMouseTracking;

        pos.z = 0.95f;

        // pos value % of the video scale
        glm::vec3 orgResultPos = glm::vec3( 
                              util.getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                              ,pos.z); //Use the original depth value
        

        glm::vec3 resultTextPos = glm::vec3( 
                              util.getPercent(videoScale,glm::vec2(pos.x+2.5,pos.y)) //Don't include the depth
                              ,pos.z); //Use the original depth value

        // scale value % of the video scale
        glm::vec2 orgResultScale = util.getPercent(videoScale,scale);
        
        glm::vec2 resultScale = util.getPercent(videoScale,scale);

        // scale value % of the video scale
        float resultRadius = util.getPercent(videoScale.x,0.35f); //0.25F = radius val
        
        // Real scale value of the text
        float resultScaleText = videoScale.x/1920/2*textScale;
        
        // Real outline value of the text
        float resultOutlineThickness = videoScale.x/1920/2 * (2.f);//2.f = outline thickness



        glm::vec3 bgResultPos = glm::vec3( 
                  util.getPercent(videoScale,glm::vec2(pos.x,pos.y + (texts.size() * scale.y*2.f * clickedMixVal[0])/2.f - scale.y)) //Don't include the depth
                  ,pos.z); //Use the original depth value
        
        glm::vec2 bgResultScale = util.getPercent(videoScale,glm::vec2(scale.x,(texts.size() * scale.y * clickedMixVal[0])));
        
        render(bgResultPos,bgResultScale,resultRadius,resultOutlineThickness,0.f,0.f,false,bgColor);

        
        //Render the button
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
                textRenderer.renderText(shader,texts[selectedIndex],resultPos.x,resultPos.y,resultPos.z+0.02f,resultPos.x + resultScale.x,false,resultScaleText,resultPos.x-resultScale.x);
            }
            else{
                textRenderer.renderText(shader,texts[i],resultPos.x,resultPos.y,resultPos.z+0.02f,resultPos.x + resultScale.x,false,resultScaleText,resultPos.x-resultScale.x);
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
};
#endif