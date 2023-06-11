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

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "tinyfiledialogs.h"

#include "GUI/Elements.hpp"

#include <iostream>
#include <string>
#include <vector>

//Renders a box using button shader
void TextBox::render(glm::vec3 resultPos,glm::vec2 resultScale,float resultRadius,float resultOutlineThickness){
    
    //Set the transform data (used by vertex shader)
    shader.setVec3("pos"    ,     resultPos );
    shader.setVec2("scale"  ,     resultScale);
        
    shader.setVec4("color"  ,     color * glm::vec4(glm::vec3(1. - (1.f*(hoverMixVal/2.f))),1.f)     ); //Button pressing color
    
    shader.setVec4("color2"  ,     color2     ); //Second color that is used by hover or click animations
    
    if(animationStyle == 1) //If hover or clicked change the color of the button
        shader.setFloat("colorMixVal"  ,     (clickedMixVal + hoverMixVal)/2.f   );
    else //If clicked change the color of the button
        shader.setFloat("colorMixVal"  ,     (clickedMixVal)   );
    //Properties
    shader.setFloat("radius",     resultRadius    );
    
    shader.setInt("outlineState" ,     1      ); 
    //Outline extra color (affected by the colorMixVal)
    shader.setVec3("outlineColor" ,     outlineColor     );  
    shader.setVec3("outlineColor2" ,     outlineColor2     );   
    if(animationStyle == 0) //Increase the thicness of the button if hover
        shader.setFloat("thickness" ,    resultOutlineThickness + clickedMixVal*2.f ); 
    else  //Set the thickness value of the button
        shader.setFloat("thickness" ,    resultOutlineThickness); 
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

TextBox::TextBox(){}

TextBox::TextBox(Shader shader,std::string text, glm::vec2 scale, glm::vec4 color, glm::vec4 color2, int animationStyle,glm::vec4 textColor,glm::vec4 textColor2,
       float textScale,float panelOffset,glm::vec3 outlineColor,glm::vec3 outlineColor2,int openSelectFolderDialog){
    
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
TextBox::TextBox(int style,glm::vec2 scale,ColorPalette colorPalette,Shader shader,std::string text,float panelOffset,int openSelectFolderDialog){
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



void TextBox::render(
                        glm::vec2 videoScale, //Resolution of the monitor
                        Mouse& mouse, //Mouse class to access mouse events
                        Timer &timer, //Timer that handles the animations
                        TextRenderer &textRenderer, //TextRenderer that handles text rendering
                        bool doMouseTracking, //If there is need to check if mouse hover
                        GLFWwindow* window //To take the key inputs
                    ){
    Util util;
    
    this->doMouseTracking = doMouseTracking;
    
    glm::vec3 resultPos = glm::vec3( 
                          util.getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value
    
    glm::vec2 resultScale = util.getPercent(videoScale,scale);
    resultScale.x /= 1.2f;

    float resultRadius = util.getPercent(videoScale.x,radius);

    float resultScaleText = videoScale.x/1920/2*textScale;
    
    float resultOutlineThickness = videoScale.x/1920/2 * (thickness);

    //Check if mouse on top of the text box
    if(doMouseTracking)
        hover = mouse.isMouseHover(resultScale,glm::vec2(resultPos.x,resultPos.y));
    else 
        hover = false;

    if(hover)//Set the cursor as pointer
        mouse.setCursor(mouse.pointerCursor);// mouse.activeCursor = mouse.pointerCursor
    
    if(hover && mouse.LClick){
        //Mouse left button pressed on top of the button
        active = !active;
    }
    
    //Deactivate the textbox
    if((!hover && mouse.LClick) || glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window,GLFW_KEY_ENTER) == GLFW_PRESS){
        active = false;
    }
    
    //Color transition animations
    timer.transition(hover,hoverMixVal,0.2f); 
    timer.transition(active,clickedMixVal,0.15f); 
    
    //---Get the input
    if(active){
        if(openSelectFolderDialog == 1){
            char* test = tinyfd_selectFolderDialog("Select A Folder", "");
            if(test){
                text = test;
            }
            active = false;
        }
        else if(openSelectFolderDialog == 2){
            //Select 3D Model
            char const* lFilterPatterns[11] = { "*.obj","*.gltf", "*.fbx", "*.stp", "*.max","*.x3d","*.obj","*.vrml","*.3ds","*.stl","*.dae" };

            char* test = tinyfd_openFileDialog("Select a 3D model","",11, lFilterPatterns,"",false);
            if(test){
                text = test;
            }
            active = false;
        }   
        else if(openSelectFolderDialog == 3){
            //Select Texture
            char const* lFilterPatterns[12] = 
                                            { 
                                                "*.png",                     //(Portable Network Graphics)
                                                "*.jpeg",                    //(Joint Photographic Experts Group)
                                                "*.jpg",                    //(Joint Photographic Experts Group but 3 letters)
                                                "*.bmp",                     //(Bitmap)
                                                "*.gif",                     //(Graphics Interchange Format)
                                                "*.tga",                     //(Truevision Targa)
                                                "*.hdr",                     //(High Dynamic Range)
                                                "*.pic",                     //(Softimage PIC)
                                                "*.pnm",                     //(Portable Any Map)
                                                "*.ppm",                     //(Portable Pixel Map)
                                                "*.pgm",                     //(Portable Gray Map)
                                                "*.pbm"                     //(Portable Bitmap) };
                                            };   
            char* test = tinyfd_openFileDialog("Select a texture","", 12, lFilterPatterns,"",false);
            if(test){
                text = test;
            }
            active = false;
        }
        else{
            //Take key input
            textRenderer.processTextInput(text,activeChar,activeChar2);
        }
    }

    //Render the button
    render(resultPos,resultScale,resultRadius,resultOutlineThickness);

    //Color of the text
    shader.setVec4("color"  ,     textColor     );
    shader.setVec4("color2"  ,     textColor2     );
    
    //Render the text
    //!Update the parameters of the renderText function in the renderTheTexture function if this function's parameters are changed
    textRenderer.renderText(shader,text,resultPos.x ,resultPos.y,resultPos.z+0.02f,resultPos.x + resultPos.x,false,resultScaleText,resultPos.x-resultScale.x,active,activeChar,activeChar2,timer,true);
}