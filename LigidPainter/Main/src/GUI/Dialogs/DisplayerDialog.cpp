
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 Copyright (c) 2022-2023, LigidTools 

 All rights reserved.

 Official GitHub Link : https:github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------

    Display settings dialog.
    (Mostly manipulates the skybox )

 */
#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

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

DisplayerDialog::DisplayerDialog(
                                    Context context, //Window and stuff
                                    glm::vec2 videoScale, //Monitor resolution
                                    ColorPalette colorPalette, //LigidPainter's theme
                                    Shader buttonShader, //Button shader
                                    AppTextures appTextures, //Textures used in GUI
                                    Shader prefilteringShader, 
                                    Shader skyboxBallShader,
                                    Model &sphereModel //Skybox's displaying texture is rendered using this sphere model
                                ){
                                    
    //Take the parameters to the class member variables
    this->buttonShader = buttonShader;
    this->context = context;
    this->prefilteringShader = prefilteringShader;
    this->skyboxBallShader = skyboxBallShader;
    this->sphereModel = sphereModel;
    
    //Create the panel
    this->panel = Panel(
        buttonShader,
        colorPalette,
        {
            {
                Section(
                    Element(Button()),
                    {
                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,6),colorPalette,buttonShader, ""  , appTextures.greetingDialogImage, 1.f,true)),
                        Element(RangeBar(0,glm::vec2(2,1),colorPalette,buttonShader, "Rotation"  , Texture(), 1.f,0.f,360.f,0.f)), 
                        Element(RangeBar(0,glm::vec2(2,1),colorPalette,buttonShader, "Blur"  , Texture(), 1.f,0.f,100.f,0.f)), 

                        Element(RangeBar(0,glm::vec2(2,1),colorPalette,buttonShader, "Opacity"  , Texture(), 1.f,0.f,100.f,0.f)), 
                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette,buttonShader, "Color"  , Texture(), 1.f, false)),
                    }
                )
            }
        },
        glm::vec2(15.f),
        glm::vec3(50.f,50.f,0.8f),
        colorPalette.mainColor,
        colorPalette.thirdColor,
        true,
        true,
        true,
        true,
        true,
        1.f,
        1.f,
        {},
        0.25f,
        false
    );
    
    //For each default skybox generate a button with a unique color (there are 6 default skyboxes)
    for (size_t i = 0; i < 6; i++)
    {
        //Create the button
        Element btn;
        btn = Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1),colorPalette,buttonShader,""    ,Texture(), 0.f,false));
        
        //Unique color for the button (color represents the skybox)
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
        
        //Add the button to the skyboxes array (is a button array for each skybox)
        skyboxes.push_back(btn);
    }
}

void DisplayerDialog::render(LigidWindow originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,
            Library &library,glm::vec2 videoScale,Skybox &skybox){
    
    dialogControl.updateStart(buttonShader);

    //Render the panel
    panel.render(videoScale,mouse,timer,textRenderer,true);
    
    //Update the texture of the skybox displayer button
    panel.sections[0].elements[0].button.texture = Texture(skybox.displayingTexture);
    
    //Render the skybox buttons if the skybox displayer is pressed
    if(panel.sections[0].elements[0].button.clickState1){
        for (size_t i = 0; i < skyboxes.size(); i++)
        {
            //Positioning the buttons
            skyboxes[i].pos = panel.sections[0].elements[0].button.pos;
            skyboxes[i].pos.y -= panel.sections[0].elements[0].button.scale.y;
            skyboxes[i].pos.x -= panel.sections[0].elements[0].button.scale.x;
            skyboxes[i].pos.y -= skyboxes[i].scale.y;
            skyboxes[i].pos.x += skyboxes[i].scale.x;
            skyboxes[i].pos.x += skyboxes[i].scale.x * 2 * i;
            skyboxes[i].scale.x = panel.sections[0].elements[0].button.scale.x/skyboxes.size();

            //Render the button
            skyboxes[i].render(videoScale,mouse,timer,textRenderer,true);
            
            //If pressed to the skybox load the pressed skybox
            if(skyboxes[i].button.hover && mouse.LClick){
                skybox.load("./LigidPainter/Resources/Cubemap/Skybox/sky" + std::to_string(i+1));
                skybox.createPrefilterMap(prefilteringShader,videoScale);
                skybox.createDisplayingTxtr(skyboxBallShader,sphereModel,context.windowScale);

                //Unpress the skybox displayer
                panel.sections[0].elements[0].button.clickState1 = false;
            }
        }
    }
    
    //Set the bg color element to the bg color of the skybox
    panel.sections[0].elements[4].button.color = glm::vec4(skybox.bgColor,1);
    
    //If pressed to the bg color element show color picker dialog
    if(panel.sections[0].elements[4].button.hover && mouse.LClick){
        unsigned char defRGB[4] = {0, 0, 0, 0}; // Black color (RGB = 0, 0, 0), alpha = 0
        const char* hex0Val = "#000000";
        auto check = tinyfd_colorChooser("Select a color",hex0Val,defRGB,defRGB);
        Color clr(check);

        //If a color is selected change the color of the skybox bgcolor (than the bg color element's color will be set to skybox bgcolor)
        if(check)
            skybox.bgColor = clr.getRGB_normalized();
    }
    
    //Change the transform matrix of the skybox (rotate it using the rotation range bar element)
    skybox.transformMatrix = glm::mat4(1);
    skybox.transformMatrix = glm::rotate(skybox.transformMatrix,glm::radians(panel.sections[0].elements[1].rangeBar.value),glm::vec3(0,1,0));

    //Change the skybox lod using the blur range bar element
    skybox.lod = panel.sections[0].elements[2].rangeBar.value/25.f;
    
    //Change the skybox opacity using the opacity range bar element
    skybox.opacity = panel.sections[0].elements[3].rangeBar.value/100.f;
    
    //End the dialog
    if((panel.sections[0].elements[2].button.hover && mouse.LClick) || context.window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!panel.hover && mouse.LClick)){
        if(!dialogControl.firstFrameActivated){
            panel.sections[0].elements[0].button.clickState1 = false;
            dialogControl.unActivate();
        }
    }

    dialogControl.updateEnd(timer,buttonShader,0.3);
}