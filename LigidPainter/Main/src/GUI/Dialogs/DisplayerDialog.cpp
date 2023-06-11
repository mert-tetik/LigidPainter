
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 Copyright (c) 2022-2023, LigidTools 

 All rights reserved.

 Official GitHub Link : https:github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------
 */
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"

#include "3D/ThreeD.hpp"

#include <string>
#include <iostream>
#include <vector>

#include "tinyfiledialogs.h"

DisplayerDialog::DisplayerDialog(){}
DisplayerDialog::DisplayerDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures,Shader prefilteringShader,Shader skyboxBallShader,Model &sphereModel){
    this->context = context;
    this->prefilteringShader = prefilteringShader;
    this->skyboxBallShader = skyboxBallShader;
    this->sphereModel = sphereModel;
    
    this->panel = Panel(buttonShader,colorPalette,{
        {
            Section(
                Element(Button()),
                {
                    Element(Button(1,glm::vec2(2,6),colorPalette,buttonShader, ""  , appTextures.greetingDialogImage, 1.f,true)),
                    Element(RangeBar(0,glm::vec2(2,1),colorPalette,buttonShader, "Rotation"  , Texture(), 1.f,0.f,360.f,0.f)), 
                    Element(RangeBar(0,glm::vec2(2,1),colorPalette,buttonShader, "Blur"  , Texture(), 1.f,0.f,100.f,0.f)), 
                    
                    Element(RangeBar(0,glm::vec2(2,1),colorPalette,buttonShader, "Opacity"  , Texture(), 1.f,0.f,100.f,0.f)), 
                    Element(Button(2,glm::vec2(2,2),colorPalette,buttonShader, "Color"  , Texture(), 1.f, false)),
                }
            )
        }
    },glm::vec2(15.f),glm::vec3(50.f,50.f,0.8f),colorPalette.mainColor,colorPalette.thirdColor,true,true,true,true,true,1.f,1.f,{},0.25f);
    
    for (size_t i = 0; i < 6; i++)
    {
        Element btn;
        btn = Element(Button(1,glm::vec2(1,1),colorPalette,buttonShader,""    ,Texture(), 0.f,false));
        if(i == 0)
            btn.button.color = glm::vec4(glm::vec3(0.56,0.64,0.73),1.);
        if(i == 1)
            btn.button.color = glm::vec4(glm::vec3(0.79,0.76,0.88),1.);
        if(i == 2)
            btn.button.color = glm::vec4(glm::vec3(0.41,0.48,0.56),1.);
        if(i == 3)
            btn.button.color = glm::vec4(glm::vec3(0.37,0.60,0.75),1.);
        if(i == 4)
            btn.button.color = glm::vec4(glm::vec3(0.8,0.8,0.8),1.);
        if(i == 5)
            btn.button.color = glm::vec4(glm::vec3(0.27,0.27,0.12),1.);
        
        skyboxes.push_back(btn);
    }
}

void DisplayerDialog::render(GLFWwindow* originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,
            Library &library,glm::vec2 videoScale,Skybox &skybox){
    panel.render(videoScale,mouse,timer,textRenderer,true);
    panel.sections[0].elements[0].button.texture = Texture(skybox.displayingTexture);
    
    if(panel.sections[0].elements[0].button.clickState1){
        for (size_t i = 0; i < skyboxes.size(); i++)
        {
            skyboxes[i].pos = panel.sections[0].elements[0].button.pos;
            skyboxes[i].pos.y -= panel.sections[0].elements[0].button.scale.y;
            skyboxes[i].pos.x -= panel.sections[0].elements[0].button.scale.x;
            skyboxes[i].pos.y -= skyboxes[i].scale.y;
            skyboxes[i].pos.x += skyboxes[i].scale.x;
            skyboxes[i].pos.x += skyboxes[i].scale.x * 2 * i;
            
            skyboxes[i].scale.x = panel.sections[0].elements[0].button.scale.x/skyboxes.size();
            skyboxes[i].render(videoScale,mouse,timer,textRenderer,true);
            
            if(skyboxes[i].button.hover && mouse.LClick){
                skybox.load("./LigidPainter/Resources/Cubemap/Skybox/sky" + std::to_string(i+1));
                skybox.createPrefilterMap(prefilteringShader,videoScale);
                skybox.createDisplayingTxtr(skyboxBallShader,sphereModel,context.windowScale);
                panel.sections[0].elements[0].button.clickState1 = false;
            }
        }
    }
    panel.sections[0].elements[4].button.color = glm::vec4(skybox.bgColor,1);
    if(panel.sections[0].elements[4].button.hover && mouse.LClick){
        unsigned char defRGB[4] = {0, 0, 0, 0}; // Black color (RGB = 0, 0, 0), alpha = 0
        const char* hex0Val = "#000000";
        auto check = tinyfd_colorChooser("Select a color",hex0Val,defRGB,defRGB);
        Color clr;
        if(check)
            skybox.bgColor = clr.hexToRgb(check)/glm::vec3(255.f);
    }
    skybox.transformMatrix = glm::mat4(1);
    skybox.transformMatrix = glm::rotate(skybox.transformMatrix,glm::radians(panel.sections[0].elements[1].rangeBar.value),glm::vec3(0,1,0));

    skybox.lod = panel.sections[0].elements[2].rangeBar.value/25.f;
    
    skybox.opacity = panel.sections[0].elements[3].rangeBar.value/100.f;
    //End the dialog
    if((panel.sections[0].elements[2].button.hover && mouse.LClick) || glfwGetKey(context.window,GLFW_KEY_ESCAPE) == GLFW_PRESS || (!panel.hover && mouse.LClick)){
        if(!firstActivation){
            panel.sections[0].elements[0].button.clickState1 = false;
            active = false;
        }
    }
    firstActivation = false;
}
void DisplayerDialog::activate(){
    active = true;
    firstActivation = true;
}