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

//Unlike the button class this function takes more parameters to draw individual parts of the range bar 
void RangeBar::render(glm::vec3 posVal,glm::vec2 scaleVal,float radiusVal,glm::vec4 color1Val, glm::vec4 color2Val,float mixVal,bool outlineExtra,
            float resultOutlineThickness){
    
    //Set the transform data (used by vertex shader)
    shader.setVec3("pos"    ,     posVal );
    shader.setVec2("scale"  ,     scaleVal);
    
    shader.setVec4("color"  ,     color1Val     ); //Default button color
    shader.setVec4("color2"  ,     color2Val     ); //Second color that is used by hover or click animations
    
    shader.setFloat("colorMixVal"  ,     mixVal   );
    //Properties
    shader.setFloat("radius",     radiusVal    );
    
    if(outlineExtra)
        shader.setInt("outlineState" ,     2      ); //Outline is not used 
    else
        shader.setInt("outlineState" ,     0      ); //Outline is not used 
    
    //Outline extra color (affected by the colorMixVal)
    shader.setVec3("outlineColor" ,     outlineColor     );  
    shader.setVec3("outlineColor2" ,     outlineColor2     );  
    shader.setFloat("thickness" ,    resultOutlineThickness);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

//Manual constructor
RangeBar::RangeBar(){

}

RangeBar::RangeBar(Shader shader,std::string text, glm::vec2 scale, glm::vec4 color, glm::vec4 color2,glm::vec4 pointerColor,glm::vec4 pointerColor2,bool outlineExtra,glm::vec3 outlineColor,
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
RangeBar::RangeBar(int style,glm::vec2 scale,ColorPalette colorPalette,Shader shader,std::string text,Texture texture,float panelOffset,float minValue,float maxValue,float value){
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

void RangeBar::render(
                        glm::vec2 videoScale, //Resolution of the monitor
                        Mouse& mouse, //Mouse class to access mouse events
                        Timer &timer, //Timer that handles the animations
                        TextRenderer &textRenderer, //TextRenderer that handles text rendering
                        bool doMouseTracking //If there is need to check if mouse hover
                    ){
    Util util;

    this->doMouseTracking = doMouseTracking;

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

    float normalizedVal = ((value+0.000001f) - minValue) / (maxValue-minValue);//0-1
    float resultVal = ((normalizedVal*2.f)-1.f)*50.f; //-50,50
    float displayValue = util.getPercent(resultScale.x,resultVal);

        
    //Check if mouse on top of the slider
    if(doMouseTracking)
        hover = mouse.isMouseHover(resultScale,glm::vec2(resultPos.x,resultPos.y));
    else 
        hover = false;


    if(hover) //Set the cursor as horizontal slide if hover
        mouse.setCursor(mouse.hSlideCursor);// mouse.activeCursor = mouse.pointerCursor

    if(hover && mouse.LClick){ //If clicked to the pointer
        //Set pointer as pressed
        pointerPressed = true;
    }

    if(pointerPressed){
        //Move the pointer (change the value of the slider)
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

    //Handle transition animation
    timer.transition(hover,hoverMixVal,0.2f); 
    timer.transition(pointerPressed,clickedMixVal,0.2f); 

    //Render the range bar
    render(resultPos,resultScale,resultRadius,color,color2,hoverMixVal,true,resultOutlineThickness); //Back side

    //Render the pointer
    render( glm::vec3(resultPos.x + displayValue - resultScale.x/2.f, resultPos.y,resultPos.z),
            glm::vec2(resultScale.x/2.f + displayValue ,resultScale.y),
            resultRadius,
            pointerColor,
            pointerColor2,
            (clickedMixVal+hoverMixVal)/2.f,
            false,
            resultOutlineThickness); 

    float resultScaleText = videoScale.x/1920/2*textScale;

    //Set color of the text
    shader.setVec4("color"  ,     textColor     );
    shader.setVec4("color2"  ,     textColor2     );
        
    //Render the text
    std::string textResult; 
    textResult = text;
    textResult += "(" + std::to_string(value) + ")";
    textRenderer.renderText(shader,textResult,resultPos.x ,resultPos.y,1,resultPos.x + resultScale.x ,false,resultScaleText,resultPos.x-resultScale.x,false,false);
}