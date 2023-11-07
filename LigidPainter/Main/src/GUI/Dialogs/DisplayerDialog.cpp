
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 Copyright (c) 2022-2023, Mert Tetik

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
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

#include "tinyfiledialogs.h"

DisplayerDialog::DisplayerDialog(){
    Texture bgTxtr0;
    bgTxtr0.load("./LigidPainter/Resources/Images/BGTexture0.jpg");
    Texture bgTxtr1;
    bgTxtr1.load("./LigidPainter/Resources/Images/BGTexture1.jpg");
    Texture bgTxtr2;
    bgTxtr2.load("./LigidPainter/Resources/Images/BGTexture2.jpg");
    Texture bgTxtr3;
    bgTxtr3.load("./LigidPainter/Resources/Images/BGTexture3.jpg");

    //Create the panel
    this->panel = Panel(
        
        {
            {
                Section(
                    Element(Button()),
                    {
                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,6),"",  Texture(), 1.f, false)),
                        Element(RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1),"Rotation"  , Texture(), 2.f,0.f,360.f,0.f)), 
                        Element(RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1),"Blur"  , Texture(), 2.f,0.f,100.f,0.f)), 
                        Element(RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1),"Opacity"  , Texture(), 2.f,0.f,100.f,0.f)), 
                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Color"  , Texture(), 2.f, false)),
                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),"",  bgTxtr0, 2.f,false)),
                        Element(RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1),"Image Opacity"  , Texture(), 2.f,0.f,1.f,0.f)), 
                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2),"",  bgTxtr0, 1.f, false)),
                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2),"",  bgTxtr1, 1.f, false)),
                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2),"",  bgTxtr2, 1.f, false)),
                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2),"",  bgTxtr3, 1.f, false)),
                    }
                )
            }
        },
        glm::vec2(20.f, 35.f),
        glm::vec3(50.f,50.f,0.8f),
        ColorPalette::mainColor,
        ColorPalette::thirdColor,
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
        btn = Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1),""    ,Texture(), 0.f,false));
        
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

void DisplayerDialog::render(Timer timer,
            Skybox &skybox){
    
    dialogControl.updateStart();

    //Render the panel
    panel.render(timer, true);
    
    //Update the texture of the skybox displayer button
    panel.sections[0].elements[0].button.texture = Texture(skybox.displayingTexture);
    
    //Render the skybox buttons if the skybox displayer is pressed
    for (size_t i = 0; i < skyboxes.size(); i++)
    {
        //Positioning the buttons
        skyboxes[i].pos = panel.sections[0].elements[0].button.pos;
        skyboxes[i].pos.y -= panel.sections[0].elements[0].button.scale.y;
        skyboxes[i].pos.x -= panel.sections[0].elements[0].button.scale.x;
        skyboxes[i].pos.y += skyboxes[i].scale.y;
        skyboxes[i].pos.x += skyboxes[i].scale.x;
        skyboxes[i].pos.x += skyboxes[i].scale.x * 2 * i;
        skyboxes[i].scale.x = panel.sections[0].elements[0].button.scale.x/skyboxes.size();

        //Render the button
        skyboxes[i].render(timer, true);
        
        //If pressed to the skybox load the pressed skybox
        if(skyboxes[i].button.clicked){
            skybox.load("./LigidPainter/Resources/Cubemap/Skybox/sky" + std::to_string(i+1));
            skybox.createPrefilterMap();
            skybox.createDisplayingTxtr();

            //Unpress the skybox displayer
            panel.sections[0].elements[0].button.clickState1 = false;
        }
    }
    
    //Set the bg color element to the bg color of the skybox
    panel.sections[0].elements[4].button.color = glm::vec4(skybox.bgColor,1);
    
    //If pressed to the bg color element show color picker dialog
    if(panel.sections[0].elements[4].button.clicked){
        unsigned char defRGB[4] = {0, 0, 0, 0}; // Black color (RGB = 0, 0, 0), alpha = 0
        Color clrObj;
        clrObj.loadRGB_normalized(glm::vec3(skybox.bgColor.r, skybox.bgColor.g, skybox.bgColor.b));
        std::string hex0Val = clrObj.getHEX();
        auto check = tinyfd_colorChooser("Select a color",hex0Val.c_str(),defRGB,defRGB);

        //If a color is selected change the color of the skybox bgcolor (than the bg color element's color will be set to skybox bgcolor)
        if(check){
            Color clr(check);
            skybox.bgColor = clr.getRGB_normalized();
        }
    }

    if(panel.sections[0].elements[7].button.clicked)
        panel.sections[0].elements[5].button.texture.ID = panel.sections[0].elements[7].button.texture.ID;
    if(panel.sections[0].elements[8].button.clicked)
        panel.sections[0].elements[5].button.texture.ID = panel.sections[0].elements[8].button.texture.ID;
    if(panel.sections[0].elements[9].button.clicked)
        panel.sections[0].elements[5].button.texture.ID = panel.sections[0].elements[9].button.texture.ID;
    if(panel.sections[0].elements[10].button.clicked)
        panel.sections[0].elements[5].button.texture.ID = panel.sections[0].elements[10].button.texture.ID;

    if(panel.sections[0].elements[5].button.clicked){
        std::string test = showFileSystemObjectSelectionDialog("Select a texture file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_TEXTURE, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);

        if(test.size()){
            if( 
                panel.sections[0].elements[5].button.texture.ID != panel.sections[0].elements[7].button.texture.ID &&
                panel.sections[0].elements[5].button.texture.ID != panel.sections[0].elements[8].button.texture.ID &&
                panel.sections[0].elements[5].button.texture.ID != panel.sections[0].elements[9].button.texture.ID &&
                panel.sections[0].elements[5].button.texture.ID != panel.sections[0].elements[10].button.texture.ID
            )
                glDeleteTextures(1, &panel.sections[0].elements[5].button.texture.ID);
            
            Texture uploadedTexture;
            uploadedTexture.load(test.c_str());
            panel.sections[0].elements[5].button.texture = uploadedTexture;
        }
    }

    
    //Change the transform matrix of the skybox (rotate it using the rotation range bar element)
    skybox.transformMatrix = glm::mat4(1);
    skybox.transformMatrix = glm::rotate(skybox.transformMatrix,glm::radians(panel.sections[0].elements[1].rangeBar.value),glm::vec3(0,1,0));

    //Change the skybox lod using the blur range bar element
    skybox.lod = panel.sections[0].elements[2].rangeBar.value/25.f;
    
    //Change the skybox opacity using the opacity range bar element
    skybox.opacity = panel.sections[0].elements[3].rangeBar.value/100.f;
    
    //End the dialog
    if((panel.sections[0].elements[2].button.clicked) || getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!panel.hover && *Mouse::LClick())){
        if(!dialogControl.firstFrameActivated){
            panel.sections[0].elements[0].button.clickState1 = false;
            dialogControl.unActivate();
        }
    }

    dialogControl.updateEnd(timer,0.15f);
}