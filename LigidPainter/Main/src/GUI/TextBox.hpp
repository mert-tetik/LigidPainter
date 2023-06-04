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

#ifndef LGDUITEXTBOX_HPP
#define LGDUITEXTBOX_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "tinyfiledialogs.h"

#include "Shader.hpp"
#include "Box.hpp"
#include "Renderer.h"
#include "Util.hpp"
#include "Timer.hpp"
#include "Texture.hpp"

#include "GUI/Button.hpp"
#include "Mouse.hpp"


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

class TextBox
{
private:
    void render(glm::vec3 resultPos,glm::vec2 resultScale,float resultRadius,float resultOutlineThickness){
        
        //Set the transform data (used by vertex shader)
        shader.setVec3("pos"    ,     resultPos );
        shader.setVec2("scale"  ,     resultScale);
            
        shader.setVec4("color"  ,     color * glm::vec4(glm::vec3(1. - (1.f*(hoverMixVal/2.f))),1.f)     ); //Button pressing color
        
        shader.setVec4("color2"  ,     color2     ); //Second color that is used by hover or click animations
        
        if(animationStyle == 1) //If hover or clicked change the color of the button
            shader.setFloat("colorMixVal"  ,     (clickedMixVal + hoverMixVal)/2.f   );
        else //If clicked change the color of the button
            shader.setFloat("colorMixVal"  ,     (clickedMixVal)   );

        //Set the resolution of the button (used by fragment shader)
        shader.setFloat("width" ,     resultScale.x   );
        shader.setFloat("height",     resultScale.y   );

        //Properties
        shader.setFloat("radius",     resultRadius    );
        shader.setInt("outline" ,     true      ); 
        shader.setInt("outlineExtra" ,     false     ); 

        //Outline extra color (affected by the colorMixVal)
        shader.setVec3("outlineColor" ,     outlineColor     );  
        shader.setVec3("outlineColor2" ,     outlineColor2     );   


        if(animationStyle == 0) //Increase the thicness of the button if hover
            shader.setFloat("thickness" ,    resultOutlineThickness + clickedMixVal*4.f ); 
        else  //Set the thickness value of the button
            shader.setFloat("thickness" ,    resultOutlineThickness); 
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

public:
    //For now buttons can only be usable with panels

    Shader shader;

    bool active = false;

    float panelOffset = 0.f; //While rendering in the panel put that much space in % between the next button element

    //---Button properties
    std::string text; //Text of the button

    glm::vec4 color; //Original color
    glm::vec4 color2; //Hover or clicked transition color
    glm::vec4 textColor; //Original color of the text
    glm::vec4 textColor2; //Hover or clicked transition color of the text

    float textScale;

    glm::vec3 outlineColor;
    glm::vec3 outlineColor2;

    int activeChar = 0;
    int activeChar2 = 0;

    int animationStyle; //determines what type of mouse hover or click animation will be used
    //0 = Change thickness for mousehover
    //1 = Change color for mousehover

    //Will be display on top of the button
    //Left side if text

    //Those values are adepting to the panel if button is attached to one
    //Values are percentage of the monitor size
    glm::vec2 scale;  //Example : w : 20, h : 30 means cover %20 of the window in x axis & cover 30% of the window in y axis
    glm::vec3 pos; //Same here
    
    bool doMouseTracking;

    bool hover = false;

    float hoverMixVal = 0.f;
    float clickedMixVal = 0.f;

    int buttonImageScaleDivider = 1.5f;

    bool openSelectFolderDialog;

    TextBox(){}




    //----Constructors 

    //Manual constructor
    TextBox(Shader shader,std::string text, glm::vec2 scale, glm::vec4 color, glm::vec4 color2, int animationStyle,glm::vec4 textColor,glm::vec4 textColor2,
           float textScale,float panelOffset,glm::vec3 outlineColor,glm::vec3 outlineColor2,bool openSelectFolderDialog){
        
        this->shader = shader;
        this->text = text;
        this->color = color;
        this->color2 = color2;
        this->scale = scale;
        this->animationStyle = animationStyle;
        this->textColor = textColor;
        this->textColor2 = textColor2;
        this->textScale = textScale;
        this->panelOffset = panelOffset;
        this->outlineColor = outlineColor;
        this->outlineColor2 = outlineColor2;
        this->openSelectFolderDialog = openSelectFolderDialog;

        activeChar = text.size();
        activeChar2 = text.size();
    }

    //Style constructor
    TextBox(int style,glm::vec2 scale,ColorPalette colorPalette,Shader shader,std::string text,float panelOffset,bool openSelectFolderDialog){
        this->shader = shader;
        this->text = text;
        this->scale = scale;
        this->panelOffset = panelOffset;
        this->openSelectFolderDialog = openSelectFolderDialog;
        
        activeChar = text.size();
        activeChar2 = text.size();

        if(style == 0){
            this->color = colorPalette.oppositeColor;
            this->color2 = colorPalette.themeColor;
            this->animationStyle = 0;
            this->textColor = colorPalette.oppositeColor;
            this->textColor2 = colorPalette.themeColor;
            this->textScale = 0.7f;
            this->outlineColor = {};
            this->outlineColor2 = {};
        }
    }



    void render(glm::vec2 videoScale,Mouse& mouse, Timer &timer,TextRenderer &textRenderer,bool doMouseTracking,GLFWwindow* window){
        Util util;

        this->doMouseTracking = doMouseTracking;

        glm::vec3 resultPos = glm::vec3( 
                              util.getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                              ,pos.z); //Use the original depth value

        glm::vec2 resultScale = util.getPercent(videoScale,scale);

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
            active = !active;
        }
        if((!hover && mouse.LClick) || glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window,GLFW_KEY_ENTER) == GLFW_PRESS){
            active = false;
        }

        timer.transition(hover,hoverMixVal,0.2f); 
        timer.transition(active,clickedMixVal,0.15f); 
        
        //---Get the input
        if(active){
            if(openSelectFolderDialog){
                char* test = tinyfd_selectFolderDialog("Select A Folder", "");
                if(test){
                    text = test;
                }
                active = false;
            }   
            else{
                textRenderer.processTextInput(text,activeChar,activeChar2);
            }
        }

        //Render the button
        render(resultPos,resultScale,resultRadius,resultOutlineThickness);

        //Render the text
        shader.setVec4("color"  ,     textColor     );
        shader.setVec4("color2"  ,     textColor2     );
        //Update the parameters of the renderText function in the renderTheTexture function if this function's parameters are changed
        textRenderer.renderText(shader,text,resultPos.x ,resultPos.y,resultPos.z+0.02f,resultPos.x + resultPos.x,false,resultScaleText,resultPos.x-resultScale.x,active,activeChar,activeChar2,timer);
    }
};
#endif