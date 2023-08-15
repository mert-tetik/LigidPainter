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

#include "GUI/GUI.hpp"
#include "ShaderSystem/Shader.hpp"
#include "MouseSystem/Mouse.hpp"

#include <iostream>
#include <string>
#include <vector>

//Renders a box using button shader
void TextBox::render(glm::vec3 resultPos,glm::vec2 resultScale,float resultRadius,float resultOutlineThickness){
    
    //Set the transform data (used by vertex shader)
    ShaderSystem::buttonShader().setVec3("pos"    ,     resultPos );
    ShaderSystem::buttonShader().setVec2("scale"  ,     resultScale);
        
    ShaderSystem::buttonShader().setVec4("properties.color"  ,     color * glm::vec4(glm::vec3(1. - (1.f*(hoverMixVal/2.f))),1.f)     ); //Button pressing color
    
    ShaderSystem::buttonShader().setVec4("properties.color2"  ,     color2     ); //Second color that is used by hover or click animations
    
    if(animationStyle == 1) //If hover or clicked change the color of the button
        ShaderSystem::buttonShader().setFloat("properties.colorMixVal"  ,     (clickedMixVal + hoverMixVal)/2.f   );
    else //If clicked change the color of the button
        ShaderSystem::buttonShader().setFloat("properties.colorMixVal"  ,     (clickedMixVal)   );
    //Properties
    ShaderSystem::buttonShader().setFloat("properties.radius",     resultRadius    );
    
    ShaderSystem::buttonShader().setInt("properties.outline.state" ,     1      ); 
    //Outline extra color (affected by the colorMixVal)
    ShaderSystem::buttonShader().setVec3("properties.outline.color" ,     outlineColor     );  
    ShaderSystem::buttonShader().setVec3("properties.outline.color2" ,     outlineColor2     );   
    if(animationStyle == 0) //Increase the thicness of the button if hover
        ShaderSystem::buttonShader().setFloat("properties.outline.thickness" ,    resultOutlineThickness + clickedMixVal*2.f ); 
    else  //Set the thickness value of the button
        ShaderSystem::buttonShader().setFloat("properties.outline.thickness" ,    resultOutlineThickness); 
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

TextBox::TextBox(){}

TextBox::TextBox(std::string text, glm::vec2 scale, glm::vec4 color, glm::vec4 color2, int animationStyle,glm::vec4 textColor,glm::vec4 textColor2,
       float textScale,float panelOffset,glm::vec3 outlineColor,glm::vec3 outlineColor2,int openSelectFolderDialog){
    
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
    activeChar = text.size()-1;
    activeChar2 = text.size()-1;
}

//Style constructor
TextBox::TextBox(int style,glm::vec2 scale,ColorPalette colorPalette,std::string text,float panelOffset,int openSelectFolderDialog){
    this->text = text;
    this->scale = scale;
    this->panelOffset = panelOffset;
    this->openSelectFolderDialog = openSelectFolderDialog;
    
    activeChar = text.size()-1;
    activeChar2 = text.size()-1;
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



void TextBox::render(
                        glm::vec2 videoScale, //Resolution of the monitor
                        //Mouse class to access mouse events
                        Timer &timer, //Timer that handles the animations
                        TextRenderer &textRenderer, //TextRenderer that handles text rendering
                        bool doMouseTracking, //If there is need to check if mouse hover
                        LigidWindow window //To take the key inputs
                    ){
    ;
    
    this->doMouseTracking = doMouseTracking;
    
    glm::vec3 resultPos = glm::vec3( 
                          UTIL::getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value
    
    glm::vec2 resultScale = UTIL::getPercent(videoScale,scale);
    resultScale.x /= 1.2f;

    float resultRadius = UTIL::getPercent(videoScale.x,radius);

    float resultScaleText = videoScale.x/1920/2*textScale;
    
    float resultOutlineThickness = videoScale.x/1920/2 * (thickness);

    //Check if mouse on top of the text box
    if(doMouseTracking)
        hover = Mouse::isMouseHover(resultScale,glm::vec2(resultPos.x,resultPos.y));
    else 
        hover = false;

    if(hover)//Set the cursor as pointer
        Mouse::setCursor(*Mouse::pointerCursor());// mouse.activeCursor = *Mouse::pointerCursor()
    
    if(hover && *Mouse::LClick()){
        //Mouse left button pressed on top of the button
        active = !active;
    }
    
    //Deactivate the textbox
    if((!hover && *Mouse::LClick()) || window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || window.isKeyPressed(LIGIDGL_KEY_ENTER) == LIGIDGL_PRESS){
        active = false;
    }
    
    //Color transition animations
    timer.transition(hover,hoverMixVal,0.2f); 
    timer.transition(active,clickedMixVal,0.15f); 
    
    //---Get the input
    if(active){
        if(openSelectFolderDialog == 1){
            std::string test = showFileSystemObjectSelectionDialog("Select a folder.", "", {}, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER);

            if(test.size()){
                text = test;
            }
            active = false;
        }
        else if(openSelectFolderDialog == 2){
            std::string test = showFileSystemObjectSelectionDialog("Select a 3D model file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MODEL, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);
            
            if(test.size()){
                text = test;
            }
            
            active = false;
        }   
        else if(openSelectFolderDialog == 3){
            //Select Texture
            std::string test = showFileSystemObjectSelectionDialog("Select a texture file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_TEXTURE, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);
            if(test.size()){
                text = test;
            }
            active = false;
        }
        else{
            //Take key input
            textRenderer.processTextInput(text,window,activeChar,activeChar2,textPosCharIndex);
        }
    }

    //Render the button
    render(resultPos,resultScale,resultRadius,resultOutlineThickness);

    //Color of the text
    ShaderSystem::buttonShader().setVec4("properties.color"  ,     textColor     );
    ShaderSystem::buttonShader().setVec4("properties.color2"  ,     textColor2     );
    
    //Render the text

    textRenderer.loadTextData(  
                                text,
                                glm::vec3(resultPos.x - resultScale.x / 1.1,resultPos.y,resultPos.z),
                                false,
                                resultScaleText,
                                resultPos.x - resultScale.x,
                                resultPos.x + resultScale.x / 1.1,
                                TEXTRENDERER_ALIGNMENT_LEFT,
                                active,
                                activeChar,
                                activeChar2,
                                textPosCharIndex
                            );

    textRenderer.renderText(textPosCharIndex,textColor2);
}