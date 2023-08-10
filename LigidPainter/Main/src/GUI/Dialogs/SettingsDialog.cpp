
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 Copyright (c) 2022-2023, Mert Tetik

 All rights reserved.

 Official GitHub Link : https:github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------
 */

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "GUI/GUI.hpp"

#include <string>
#include <iostream>
#include <vector>

SettingsDialog::SettingsDialog(){}

SettingsDialog::SettingsDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,AppTextures appTextures){
    this->context = context;
    
    //Create the panel
    this->panel = Panel(colorPalette,{
        {
            Section(
                Element(Button()),
                {
                    Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(4,2),colorPalette,"Settings",Texture(),0.f,false)), 

                    Element(ComboBox(ELEMENT_STYLE_BASIC,glm::vec2(4,2),colorPalette,
                    {
                        "256",
                        "512",
                        "1024",
                        "2048",
                        "4096"
                    },"Texture Resolution",4.f),context.window),
                    
                    Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "VSync"  , 2.f)),
                    
                    Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette, "Backface Culling"  , 2.f)),
                }
            )
        }
    },glm::vec2(15.f),glm::vec3(50.f,50.f,0.8f),colorPalette.mainColor,colorPalette.thirdColor,true,true,true,true,true,1.f,1.f,{},0.25f,false);
}

void SettingsDialog::render(LigidWindow originalWindow, ColorPalette colorPalette, Mouse& mouse, Timer timer, TextRenderer &textRenderer,
                            Library &library, glm::vec2 videoScale, AppSettings& settings){
    
    dialogControl.updateStart();   

    //Set the combo box selected index as the textureRes
    int txtrRes = 256;
    for (size_t i = 0; i < panel.sections[0].elements[1].comboBox.texts.size(); i++)
    {
        if(settings.textureRes == txtrRes)
            panel.sections[0].elements[1].comboBox.selectedIndex = i;
        
        txtrRes*=2;
    }

    //Set the vsync option as the vsync checkbox element
    settings.VSync = panel.sections[0].elements[2].checkBox.clickState1;
    
    //Set the backface culling option as the backface culling checkbox element
    settings.backfaceCulling = panel.sections[0].elements[3].checkBox.clickState1;

    //Render the panel    
    panel.render(videoScale,mouse,timer,textRenderer,true);

    //If pressed to any of the combo box element change the texture res
    for (size_t i = 0; i < panel.sections[0].elements[1].comboBox.hover.size(); i++)
    {
        if(panel.sections[0].elements[1].comboBox.hover[i] && mouse.LClick){
            settings.textureRes = stoi(panel.sections[0].elements[1].comboBox.texts[panel.sections[0].elements[1].comboBox.selectedIndex]);
        }
    }
    
    //End the dialog
    if  (
            context.window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || //Escape key pressed 
            ((!panel.hover && mouse.LClick)) && !dialogControl.firstFrameActivated || //Mouse Lclick out of the panel
            (panel.sections[0].elements[0].button.hover && mouse.LDoubleClick) //If the menu button double clicked
        )
    {
        dialogControl.unActivate();
    }

    dialogControl.updateEnd(timer,0.15f);   
}