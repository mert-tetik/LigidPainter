
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
#include "MouseSystem/Mouse.hpp" 
#include "SettingsSystem/Settings.hpp" 

#include <string>
#include <iostream>
#include <vector>

SettingsDialog::SettingsDialog(){}

SettingsDialog::SettingsDialog(ColorPalette colorPalette){
    
    //Create the panel
    this->panel = Panel(colorPalette,{
        {
            Section(
                Element(Button()),
                {
                    Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(4,2),colorPalette, "Settings",Texture(),0.f,false)), 
                }
            ),
            Section(
                Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(4,2),colorPalette,"System",Texture(),4.f,true)),
                {

                    Element(ComboBox(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),colorPalette,
                    {
                        "256",
                        "512",
                        "1024",
                        "2048",
                        "4096"
                    }, "Texture Resolution", 2.f)),
                    
                    Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),colorPalette, "VSync"  , 2.f)),
                    
                    Element(ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(2,1.f), colorPalette,
                    {
                        "1024x768",
                        "1152x864",
                        "1280x720",
                        "1280x768",
                        "1280x800",
                        "1280x960",
                        "1280x1024",
                        "1360x768",
                        "1366x768",
                        "1400x1050",
                        "1440x900",
                        "1600x900",
                        "1680x1050",
                        "1920x1080",
                        "2560×1,440",
                        "3840×2,160",
                        "7680×4,320"

                    }, "Framebuffer Resolution",2.f)),
                }
            ),
            Section(
                Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(4,2),colorPalette,"Model Rendering",Texture(), 4.f,true)),
                {
                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),colorPalette, "X : Rotation",Texture(),0.f, 0.f, 360.f, 0.f)), 
                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),colorPalette, "Y : Rotation",Texture(),0.f, 0.f, 360.f, 0.f)), 
                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),colorPalette, "Z : Rotation",Texture(),0.f, 0.f, 360.f, 0.f)), 
                    
                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),colorPalette, "X : Transform",Texture(),2.f, -50.f, 50.f, 0.f)), 
                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),colorPalette, "Y : Transform",Texture(),0.f, -50.f, 50.f, 0.f)), 
                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),colorPalette, "Z : Transform",Texture(),0.f, -50.f, 50.f, 0.f)), 
                
                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),colorPalette, "Fov", Texture(),2.f, 1.f, 180.f, 40.f)), 
                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),colorPalette, "Near", Texture(),0.f, 0.f, 1.f, 0.1f)), 
                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),colorPalette, "Far", Texture(),0.f, 0.f, 1000.f, 1000.f)), 
                    Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),colorPalette, "Orthographic projection"  , 2.f)),

                    Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),colorPalette, "Backface Culling"  , 2.f)),
                    
                    Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),colorPalette, "Apply Height Map"  , 2.f)),
                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),colorPalette, "Height Map Strength", Texture(),0.f, 0.f, 1.f, 0.1f)), 
                }
            )
        }
    },glm::vec2(25.f, 40.f), glm::vec3(50.f,50.f,0.8f),colorPalette.mainColor,colorPalette.thirdColor,true,true,true,true,true,1.f,1.f,{},0.25f,false);
}

void SettingsDialog::render(ColorPalette colorPalette, Timer timer, TextRenderer &textRenderer){
    
    dialogControl.updateStart();   

    //Set the combo box selected index as the textureRes
    int txtrRes = 256;
    for (size_t i = 0; i < panel.sections[1].elements[0].comboBox.texts.size(); i++)
    {
        if(Settings::properties()->textureRes == txtrRes)
            panel.sections[1].elements[0].comboBox.selectedIndex = i;
        
        txtrRes*=2;
    }

    panel.sections[2].elements[0].rangeBar.value = getScene()->transformRotation.x;
    panel.sections[2].elements[1].rangeBar.value = getScene()->transformRotation.y;
    panel.sections[2].elements[2].rangeBar.value = getScene()->transformRotation.z;
    
    panel.sections[2].elements[3].rangeBar.value = getScene()->transformLocation.x;
    panel.sections[2].elements[4].rangeBar.value = getScene()->transformLocation.y;
    panel.sections[2].elements[5].rangeBar.value = getScene()->transformLocation.z;
    
    panel.sections[2].elements[6].rangeBar.value = getScene()->fov;
    panel.sections[2].elements[7].rangeBar.value = getScene()->aNear;
    panel.sections[2].elements[8].rangeBar.value = getScene()->aFar;
    panel.sections[2].elements[9].checkBox.clickState1 = getScene()->useOrtho;
    
    panel.sections[2].elements[11].checkBox.clickState1 = Settings::properties()->useHeightMap;
    panel.sections[2].elements[12].rangeBar.value = Settings::properties()->heightMapStrength;

    if(panel.sections[2].elements[12].rangeBar.pointerPressed && !*Mouse::LPressed() && panel.sections[2].elements[11].checkBox.clickState1){
        for (size_t i = 0; i < getModel()->meshes.size(); i++)
        {
            getModel()->meshes[i].processHeightMap();
        }
    }

    //Render the panel    
    panel.render(timer,textRenderer,true);

    getScene()->transformRotation.x = panel.sections[2].elements[0].rangeBar.value;
    getScene()->transformRotation.y = panel.sections[2].elements[1].rangeBar.value;
    getScene()->transformRotation.z = panel.sections[2].elements[2].rangeBar.value;
    
    getScene()->transformLocation.x = panel.sections[2].elements[3].rangeBar.value;
    getScene()->transformLocation.y = panel.sections[2].elements[4].rangeBar.value;
    getScene()->transformLocation.z = panel.sections[2].elements[5].rangeBar.value;

    getScene()->fov = panel.sections[2].elements[6].rangeBar.value;
    getScene()->aNear = panel.sections[2].elements[7].rangeBar.value;
    getScene()->aFar = panel.sections[2].elements[8].rangeBar.value;
    getScene()->useOrtho = panel.sections[2].elements[9].checkBox.clickState1;
    
    Settings::properties()->useHeightMap = panel.sections[2].elements[11].checkBox.clickState1;
    Settings::properties()->heightMapStrength = panel.sections[2].elements[12].rangeBar.value;
    
    //Set the vsync option as the vsync checkbox element
    Settings::properties()->VSync = panel.sections[1].elements[1].checkBox.clickState1;
    
    //Set the backface culling option as the backface culling checkbox element
    Settings::properties()->backfaceCulling = panel.sections[2].elements[10].checkBox.clickState1;
    
    //If pressed to any of the combo box element change the texture res
    Settings::properties()->textureRes = stoi(panel.sections[1].elements[0].comboBox.texts[panel.sections[1].elements[0].comboBox.selectedIndex]);
    
    if(panel.sections[2].elements[11].checkBox.hover && *Mouse::LClick()){
        for (size_t i = 0; i < getModel()->meshes.size(); i++)
        {
            getModel()->meshes[i].processHeightMap();
        }
    }
    
    getScene()->updateProjectionMatrix();
    getScene()->updateViewMatrix();
    getScene()->updateTransformMatrix();

    //End the dialog
    if  (
            getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || //Escape key pressed 
            ((!panel.hover && *Mouse::LClick())) && !dialogControl.firstFrameActivated || //Mouse Lclick out of the panel
            (panel.sections[0].elements[0].button.hover && *Mouse::LDoubleClick()) //If the menu button double clicked
        )
    {
        dialogControl.unActivate();
    }

    dialogControl.updateEnd(timer,0.15f);   
}