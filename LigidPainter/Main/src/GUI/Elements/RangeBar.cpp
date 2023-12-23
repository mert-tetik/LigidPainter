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
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

//Unlike the button class this function takes more parameters to draw individual parts of the range bar 
void RangeBar::render(
                        glm::vec3 posVal,
                        glm::vec2 scaleVal,
                        float radiusVal,
                        glm::vec4 color1Val, 
                        glm::vec4 color2Val,
                        float mixVal,
                        bool outlineExtra,
                        float resultOutlineThickness
                    )
{
    
    //Set the transform data (used by vertex shader)
    ShaderSystem::buttonShader().setVec3("pos"    ,     posVal );
    ShaderSystem::buttonShader().setVec2("scale"  ,     scaleVal);
    
    ShaderSystem::buttonShader().setVec4("properties.color"  ,     color1Val     ); //Default button color
    ShaderSystem::buttonShader().setVec4("properties.color2"  ,     color2Val     ); //Second color that is used by hover or click animations
    
    ShaderSystem::buttonShader().setFloat("properties.colorMixVal"  ,     mixVal   );
    //Properties
    ShaderSystem::buttonShader().setFloat("properties.radius",     radiusVal    );
    
    if(outlineExtra)
        ShaderSystem::buttonShader().setInt("properties.outline.state" ,     2      ); //Outline is not used 
    else
        ShaderSystem::buttonShader().setInt("properties.outline.state" ,     0      ); //Outline is not used 
    
    //Outline extra color (affected by the colorMixVal)
    ShaderSystem::buttonShader().setVec3("properties.outline.color" ,     outlineColor     );  
    ShaderSystem::buttonShader().setVec3("properties.outline.color2" ,     outlineColor2     );  
    ShaderSystem::buttonShader().setFloat("properties.outline.thickness" ,    resultOutlineThickness);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

//Manual constructor
RangeBar::RangeBar(){

}

RangeBar::RangeBar(std::string text, glm::vec2 scale, glm::vec4 color, glm::vec4 color2,glm::vec4 pointerColor,glm::vec4 pointerColor2,bool outlineExtra,glm::vec3 outlineColor,
         glm::vec3 outlineColor2, float radius,glm::vec4 textColor,glm::vec4 textColor2,Texture texture,float textScale,float panelOffset,float outlineThickness,float minValue,float maxValue
         ,float value){
    //animationStyle determines what type of mouse hover or click animation will be used
    //0 = Change thickness for mousehover
    //1 = Change color for mousehover
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
    this->constructValue = value;
}

//Style constructor
RangeBar::RangeBar(int style,glm::vec2 scale,std::string text,Texture texture,float panelOffset,float minValue,float maxValue,float value){
    
    if(style == ELEMENT_STYLE_STYLIZED){
        this->isNumeric = true;
    }

    this->text = text;
    this->color = ColorPalette::secondColor;
    this->color2 = ColorPalette::mainColor;
    
    this->pointerColor = ColorPalette::thirdColor;
    this->pointerColor2 = ColorPalette::themeColor;
    this->scale = scale;
    this->radius = 0.2f;
    this->textColor = ColorPalette::oppositeColor;
    this->textColor2 = ColorPalette::secondColor;
    this->texture = texture;
    this->textScale = 0.5f;
    this->panelOffset = panelOffset;
    
    this->outlineExtra = true;
    
    this->outlineColor = ColorPalette::thirdColor;
    this->outlineColor2 = ColorPalette::themeColor;
    this->outlineThickness = 2.4f;
    this->minValue = minValue;
    this->maxValue = maxValue;
    this->value = value;
    this->constructValue = value;
    
    
}

void RangeBar::render(
                        Timer &timer, //Timer that handles the animations
                        bool doMouseTracking //If there is need to check if mouse hover
                    ){
    this->doMouseTracking = doMouseTracking;

    // pos value % of the video scale
    glm::vec3 resultPos = glm::vec3( 
                          UTIL::getPercent(*Settings::videoScale(), glm::vec2(pos.x,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value


    // scale value % of the video scale
    glm::vec2 resultScale = UTIL::getPercent(*Settings::videoScale(), scale);
    glm::vec2 arrowBtnScale = glm::vec2(resultScale.y, resultScale.y);  
    resultScale.x /= 1.2f;
        
    // scale value % of the video scale
    float resultRadius = UTIL::getPercent(Settings::videoScale()->x,radius);

    // Real value of the thickness
    float resultOutlineThickness = Settings::videoScale()->x/1920/2*outlineThickness;

    float normalizedVal = ((value+0.000001f) - minValue) / (maxValue-minValue);//0-1
    float resultVal = ((normalizedVal*2.f)-1.f)*50.f; //-50,50
    float displayValue = UTIL::getPercent(resultScale.x, resultVal);

    float normalizedValConstruct = ((constructValue+0.000001f) - minValue) / (maxValue-minValue);//0-1
    float resultValConstruct = ((normalizedValConstruct*2.f)-1.f)*50.f; //-50,50
    float displayValueConstruct = UTIL::getPercent(resultScale.x, resultValConstruct);

        
    //Check if mouse on top of the slider
    if(doMouseTracking)
        hover = Mouse::isMouseHover(resultScale, glm::vec2(resultPos.x,resultPos.y));
    else 
        hover = false;


    if(hover) //Set the cursor as horizontal slide if hover
        Mouse::setCursor(*Mouse::hSlideCursor());// mouse.activeCursor = *Mouse::pointerCursor()

    if(hover && !leftArrowHover && !rightArrowHover && *Mouse::LClick()){ //If clicked to the pointer
        //Set pointer as pressed
        pointerPressed = true;
    }

    if(pointerPressed){
        //Move the pointer (change the value of the slider)
        Mouse::setCursor(*Mouse::hSlideCursor());// mouse.activeCursor = *Mouse::pointerCursor()
        if(isNumeric){
            if(Mouse::mouseOffset()->x > 0 || (rightArrowHover && *Mouse::LClick()))
                value++;
            else if(Mouse::mouseOffset()->x < 0 || (leftArrowHover && *Mouse::LClick()))
                value--;
        }
        else{
            float v = Mouse::mouseOffset()->x / Settings::videoScale()->x * (this->maxValue - this->minValue) * (50.f / this->scale.x); 
            if(getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT))
                v /= 50.f;
            value += v;
        }
        
        if(value < minValue)
            value = minValue;
        if(value > maxValue)
            value = maxValue;
    }

    this->valueDoneChanging = false;
    if(!*Mouse::LPressed()){ //If left click is released
        if(pointerPressed)
            this->valueDoneChanging = true;
        pointerPressed = false;
    }

    //Handle transition animation
    timer.transition(hover,hoverMixVal,0.2f); 
    timer.transition(pointerPressed,clickedMixVal,0.2f); 
    
    //Render the range bar
    render(resultPos,resultScale,resultRadius,color,color2,hoverMixVal,true,resultOutlineThickness); //Back side
    
    this->defaultPointBtnPressed = false;
    bool defaultPointHover = false;
    if(!this->isNumeric && this->defaultPointMode){
        glm::vec2 defPointScale = UTIL::getPercent(glm::vec2(Settings::videoScale()->y), glm::vec2(0.6f));
        if(Mouse::isMouseHover(defPointScale, glm::vec2(resultPos.x + (displayValueConstruct*2.f), resultPos.y + resultScale.y + defPointScale.y * 1.5f))){
            defaultPointHover = true;
            
            if(*Mouse::LClick()){
                this->defaultPointBtnPressed = true;
                this->value = this->constructValue;
            }
        }

        timer.transition(defaultPointHover, this->defPointMixVal, 0.2f);

        //Render the default point
        render(glm::vec3(resultPos.x + (displayValueConstruct*2.f), resultPos.y + resultScale.y + defPointScale.y * 1.5f, resultPos.z), defPointScale, resultRadius, color, color2, defPointMixVal,true,resultOutlineThickness); //Back side
    }
    
    //Render the pointer
    render( 
            glm::vec3(resultPos.x + displayValue - resultScale.x/2.f, resultPos.y,resultPos.z),
            glm::vec2(resultScale.x/2.f + displayValue ,resultScale.y),
            resultRadius,
            pointerColor,
            pointerColor2,
            (clickedMixVal+hoverMixVal)/2.f,
            false,
            resultOutlineThickness
        ); 

    ShaderSystem::buttonShader().setInt("states.renderTexture"  ,     1    );
    ShaderSystem::buttonShader().setInt("properties.txtr"  ,     0    );


    //Render the left arrow

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, appTextures.arrowL.ID);
    render(
            glm::vec3(resultPos.x - resultScale.x + (arrowBtnScale.x), resultPos.y, resultPos.z), 
            arrowBtnScale * (0.6f + (leftArrowMixVal + leftArrowClickedMixVal) / 10.f),
            resultRadius, color, color2, 0.f, true, resultOutlineThickness); //Back side

    if(doMouseTracking)
        leftArrowHover = Mouse::isMouseHover(arrowBtnScale, glm::vec2(resultPos.x - resultScale.x + (arrowBtnScale.x), resultPos.y));
    else 
        leftArrowHover = false;

    timer.transition(leftArrowHover, leftArrowMixVal,0.2f); 
    timer.transition(false, leftArrowClickedMixVal,0.2f); 

    if(leftArrowHover && *Mouse::LClick()){
        leftArrowClickedMixVal = 1.f;
        if(this->isNumeric)
            this->value--;
        else
            this->value -= (this->maxValue - this->minValue) / 100.f;
    
        if(this->value < minValue)
            this->value = minValue;
    }
    
    //Render the right arrow
    
    glBindTexture(GL_TEXTURE_2D, appTextures.arrowR.ID);
    render(
            glm::vec3(resultPos.x + resultScale.x - (arrowBtnScale.x), resultPos.y, resultPos.z), 
            arrowBtnScale * (0.6f + (rightArrowMixVal + rightArrowClickedMixVal) / 10.f),
            resultRadius, color, color2, 0.f, true, resultOutlineThickness); //Back side
    
    if(doMouseTracking)
        rightArrowHover = Mouse::isMouseHover(arrowBtnScale, glm::vec2(resultPos.x + resultScale.x - (arrowBtnScale.x), resultPos.y));
    else 
        rightArrowHover = false;

    timer.transition(rightArrowHover, rightArrowMixVal, 0.2f); 
    timer.transition(false, rightArrowClickedMixVal, 0.2f); 

    if(rightArrowHover && *Mouse::LClick()){
        rightArrowClickedMixVal = 1.f;

        if(this->isNumeric)
            this->value++;
        else
            this->value += (this->maxValue - this->minValue) / 100.f;

        if(this->value > maxValue)
            this->value = maxValue;
    }

    if(leftArrowHover || rightArrowHover)
        Mouse::setCursor(*Mouse::pointerCursor());// mouse.activeCursor = *Mouse::pointerCursor()

    ShaderSystem::buttonShader().setInt("states.renderTexture"  ,     0    );



    float resultScaleText = Settings::videoScale()->x/1920/2*textScale;

    //Set color of the text
    ShaderSystem::buttonShader().setVec4("properties.color"  ,     textColor     );
    ShaderSystem::buttonShader().setVec4("properties.color2"  ,     textColor2     );
        
    //Render the text
    std::string textResult; 
    textResult = text;
    if(this->isNumeric)
        textResult += "(" + std::to_string((int)value) + ")";
    else
        textResult += "(" + std::to_string(value) + ")";
    
    textRenderer.loadTextData(
                                textResult,
                                glm::vec3(resultPos.x,resultPos.y,resultPos.z+0.001f),
                                false,
                                resultScaleText,
                                resultPos.x - resultScale.x,
                                resultPos.x + resultScale.x,
                                TEXTRENDERER_ALIGNMENT_MID
                             );

    textRenderer.renderText();

    if(leftArrowHover || rightArrowHover || defaultPointHover)
        Mouse::setCursor(*Mouse::pointerCursor());
        
    if(this->pointerPressed)
        Mouse::setCursor(*Mouse::hSlideCursor());
}