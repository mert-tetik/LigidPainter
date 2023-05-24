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

#ifndef LGDUIRANGEBAR_HPP
#define LGDUIRANGEBAR_HPP

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

#include "Mouse.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

class RangeBar
{
private:
    //Unlike the button class this function takes more parameters to draw individual parts of the range bar 
    void render(glm::vec3 posVal,glm::vec2 scaleVal,float radiusVal,glm::vec4 color1Val, glm::vec4 color2Val,float mixVal,bool outlineExtra,float resultOutlineThickness){
        
        //Set the transform data (used by vertex shader)
        shader.setVec3("pos"    ,     posVal );
        shader.setVec2("scale"  ,     scaleVal);
        
        shader.setVec4("color"  ,     color1Val     ); //Default button color
        shader.setVec4("color2"  ,     color2Val     ); //Second color that is used by hover or click animations
        
        shader.setFloat("colorMixVal"  ,     mixVal   );

        //Set the resolution of the button (used by fragment shader)
        shader.setFloat("width" ,     scaleVal.x   );
        shader.setFloat("height",     scaleVal.y   );

        //Properties
        shader.setFloat("radius",     radiusVal    );
        shader.setInt("outline" ,     false      ); //Outline is not used 
        shader.setInt("outlineExtra" ,     outlineExtra     );  
        
        //Outline extra color (affected by the colorMixVal)
        shader.setVec3("outlineColor" ,     outlineColor     );  
        shader.setVec3("outlineColor2" ,     outlineColor2     );  

        shader.setFloat("thickness" ,    resultOutlineThickness);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

public:
    Shader shader;

    bool pointerPressed = false;//Range bar is pressed and now is changing value via mouse offset

    float panelOffset = 0.f; //While rendering in the panel put that much space in % between the next button element


    //---RangeBar properties
    std::string text; //Text of the button

    glm::vec4 color; //Original color of the back side
    glm::vec4 color2; //Hover or clicked transition color of the back side
    glm::vec4 pointerColor; //Original color of the pointer / front side 
    glm::vec4 pointerColor2; //Hover or clicked transition color of the pointer / front side
    glm::vec4 textColor; //Original color of the text
    glm::vec4 textColor2; //Hover or clicked transition color of the text

    float textScale; //Scale of the text

    //Outline Extra does the same with outline. But unlike the outline doesn't remove the center
    bool outlineExtra; 
    glm::vec3 outlineColor;
    glm::vec3 outlineColor2;
    float outlineThickness;

    float radius; //% Radius of the corners 
    //0 = Change thickness for mousehover
    //1 = Change color for mousehover

    //Will be display on top of the button
    //Left side if text
    Texture texture; 

    //Those values are adepting to the panel if button is attached to one
    //Values are percentage of the monitor size
    glm::vec2 scale;  //Example : w : 20, h : 30 means cover %20 of the window in x axis & cover 30% of the window in y axis
    glm::vec3 pos; //Same here
    
    bool hover = false; //The slider's itself mouse hover

    float hoverMixVal = 0.f;
    float clickedMixVal = 0.f;

    //Keep the value between these variables
    float minValue = -50.f; 
    float maxValue = 50.f; 
    float value = (maxValue+minValue)/2.f; //Value of the range bar

    RangeBar(){}

    //Manual constructor
    RangeBar(Shader shader,std::string text, glm::vec2 scale, glm::vec4 color, glm::vec4 color2,glm::vec4 pointerColor,glm::vec4 pointerColor2,bool outlineExtra,glm::vec3 outlineColor,
             glm::vec3 outlineColor2, float radius,glm::vec4 textColor,glm::vec4 textColor2,Texture texture,float textScale,float panelOffset,float outlineThickness,float minValue,float maxValue
             ,float value){
        //animationStyle determines what type of mouse hover or click animation will be used
        //0 = Change thickness for mousehover
        //1 = Change color for mousehover

        this->shader = shader;
        this->text = text;
        this->color = color;
        this->color2 = color2;
        this->pointerColor = pointerColor;
        this->pointerColor2 = pointerColor2;
        this->scale = scale;
        this->radius = radius;
        this->textColor = textColor;
        this->textColor2 = textColor2;
        this->texture = texture;
        this->textScale = textScale;
        this->panelOffset = panelOffset;
        this->outlineExtra = outlineExtra;
        this->outlineColor = outlineColor;
        this->outlineColor2 = outlineColor2;
        this->outlineThickness = outlineThickness;
        this->minValue = minValue;
        this->maxValue = maxValue;
        this->value = value;
    }

    //Style constructor
    RangeBar(int style,glm::vec2 scale,ColorPalette colorPalette,Shader shader,std::string text,Texture texture,float panelOffset,float minValue,float maxValue,float value){
        //For now there is only 1 style
        
        this->shader = shader;
        this->text = text;

        this->color = colorPalette.secondColor;
        this->color2 = colorPalette.mainColor;
        
        this->pointerColor = colorPalette.thirdColor;
        this->pointerColor2 = colorPalette.themeColor;

        this->scale = scale;
        this->radius = 0.2f;
        this->textColor = colorPalette.oppositeColor;
        this->textColor2 = colorPalette.secondColor;
        this->texture = texture;
        this->textScale = 0.5f;

        this->panelOffset = panelOffset;
        
        this->outlineExtra = true;
        
        this->outlineColor = colorPalette.thirdColor;
        this->outlineColor2 = colorPalette.themeColor;
        this->outlineThickness = 2.f;

        this->minValue = minValue;
        this->maxValue = maxValue;
        this->value = value;
    }

    void render(glm::vec2 videoScale,Mouse& mouse, Timer &timer,TextRenderer &textRenderer){
        Util util;

        // pos value % of the video scale
        glm::vec3 resultPos = glm::vec3( 
                              util.getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                              ,pos.z); //Use the original depth value

        // scale value % of the video scale
        glm::vec2 resultScale = util.getPercent(videoScale,scale);
        resultScale.x /= 1.2f;
        
        // scale value % of the video scale
        float resultRadius = util.getPercent(videoScale.x,radius);

        // Real value of the thickness
        float resultOutlineThickness = videoScale.x/1920/2*outlineThickness;



        const float buttonPointerWidthRatio = 30.f;

        float normalizedVal = ((value+0.000001f) - minValue) / (maxValue-minValue);//0-1
        float resultVal = ((normalizedVal*2.f)-1.f)*50.f; //-50,50
        
        float displayValue = util.getPercent(resultScale.x,resultVal);

        //Check if mouse on top of the slider
        hover = mouse.isMouseHover(resultScale,glm::vec2(resultPos.x,resultPos.y));
        
        if(hover)
            //Set the cursor as horizontal slide
            mouse.setCursor(mouse.hSlideCursor);// mouse.activeCursor = mouse.pointerCursor

        if(hover && mouse.LClick){
            //Set pointer as pressed
            pointerPressed = true;
        }
        if(pointerPressed){
            mouse.setCursor(mouse.hSlideCursor);// mouse.activeCursor = mouse.pointerCursor
            value += mouse.mouseOffset.x;
            if(value < minValue)
                value = minValue;
            if(value > maxValue)
                value = maxValue;
        }

        if(!mouse.LPressed){ //If left click is released
            pointerPressed = false;
        }


        timer.transition(hover,hoverMixVal,0.2f); 
        timer.transition(pointerPressed,clickedMixVal,0.2f); 

        
        //Render the range bar
        render(resultPos,resultScale,resultRadius,color,color2,hoverMixVal,true,resultOutlineThickness); //Back side

        render( glm::vec3(resultPos.x + displayValue - resultScale.x/2.f, resultPos.y,resultPos.z),
                glm::vec2(resultScale.x/2.f + displayValue ,resultScale.y),
                resultRadius,
                pointerColor,
                pointerColor2,
                (clickedMixVal+hoverMixVal)/2.f,
                false,
                resultOutlineThickness); //Pointer

        ////Render the text
        float resultScaleText = videoScale.x/1920/2*textScale;

        shader.setVec4("color"  ,     textColor     );
        shader.setVec4("color2"  ,     textColor2     );
        
        std::string textResult; 
        textResult = text;
        textResult += "(" + std::to_string(value) + ")";
        textRenderer.renderText(shader,textResult,resultPos.x ,resultPos.y,1,resultPos.x + resultScale.x ,false,resultScaleText,resultPos.x-resultScale.x);
    }
};
#endif