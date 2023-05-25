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

#ifndef LGDUIMATERIALEDITORDIALOG_HPP
#define LGDUIMATERIALEDITORDIALOG_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.hpp"
#include "Box.hpp"
#include "Renderer.h"
#include "Util.hpp"

#include "GUI/UI.hpp"
#include "GUI/Button.hpp"
#include "Mouse.hpp"
#include "Timer.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

class MaterialEditorDialog
{
private:
    /* data */
public:
    bool active = true; //Render the dialog
    glm::vec3 pos = glm::vec3(50.f,50.f,0.8f); ///Position of the dialog
    glm::vec2 scale = glm::vec2(40,40); ///Scale of the dialog
    
    glm::vec3 posLayer = glm::vec3(20.f,50.f,0.8f); ///Position of the layer panel
    glm::vec2 scaleLayer = glm::vec2(10,40); ///Scale of the layer panel
    
    glm::vec3 posModifier = glm::vec3(80.f,50.f,0.8f); ///Position of the layer panel
    glm::vec2 scaleModifier = glm::vec2(10,40); ///Scale of the layer panel

    Button materialDisplayer; //That buttons displays the material with a texture

    Panel bgPanel; //To cover the bg
    Panel layerPanel; //Modifiers will be displayed in there
    Panel modifiersPanel; //Modifiers will be displayed in there

    MaterialEditorDialog(){}
    
    MaterialEditorDialog(Shader buttonShader,ColorPalette colorPalette,AppTextures appTextures){
        bgPanel = Panel
        (
            buttonShader,
            colorPalette,
            {
                // {
                //     Section(
                //             Element(Button()),
                //             {
                //                 Element(Button(1,glm::vec2(30,30),colorPalette,buttonShader,"Add"        , appTextures.greetingDialogImage, 0.f,false))
                //             }
                //         )
                // }
            },
            scale,pos,colorPalette.mainColor,colorPalette.thirdColor,true,true,true,true,true,1.f,1.f,{});
        
        layerPanel = Panel(buttonShader,colorPalette,{},scaleLayer,posLayer,colorPalette.mainColor,colorPalette.thirdColor,false,true,false,true,true,1.f,1.f,                                
                                {
                                    Button(1,glm::vec2(2,1.5f),colorPalette,buttonShader,"Add"        , Texture(), 0.f,false),
                                    Button(1,glm::vec2(2,1.5f),colorPalette,buttonShader,"Del"        , Texture(), 0.f,false)
                                }
                          );
        modifiersPanel = Panel(buttonShader,colorPalette,{},scaleModifier,posModifier,colorPalette.mainColor,colorPalette.thirdColor,false,false,true,true,true,1.f,1.f,                                
                                {
                                    Button(1,glm::vec2(2,1.5f),colorPalette,buttonShader,"Add"        , Texture(), 0.f,false),
                                    Button(1,glm::vec2(2,1.5f),colorPalette,buttonShader,"Del"        , Texture(), 0.f,false)
                                }
                          );
        materialDisplayer = Button(1,glm::vec2(45,45),colorPalette,buttonShader,"Material"        , appTextures.greetingDialogImage, 0.f,false);
    }

    void render(glm::vec2 videoScale,Mouse& mouse,Timer &timer,TextRenderer &textRenderer){
        bgPanel.render(videoScale,mouse,timer,textRenderer);
        layerPanel.render(videoScale,mouse,timer,textRenderer);
        modifiersPanel.render(videoScale,mouse,timer,textRenderer);
        materialDisplayer.pos = bgPanel.pos;
        materialDisplayer.scale = glm::vec2(35.f); 
        materialDisplayer.scale.x = (modifiersPanel.pos.x - modifiersPanel.scale.x) - (layerPanel.pos.x + layerPanel.scale.x); 
        materialDisplayer.scale.x /= 2.f;
        materialDisplayer.pos.x = modifiersPanel.pos.x - modifiersPanel.scale.x - materialDisplayer.scale.x;

        materialDisplayer.render(videoScale,mouse,timer,textRenderer);
    }
};

#endif