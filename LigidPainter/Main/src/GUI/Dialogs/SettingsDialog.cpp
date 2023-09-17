
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
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

SettingsDialog::SettingsDialog(){
    this->systemSettingsBtn = Button(ELEMENT_STYLE_BASIC, glm::vec2(4,2), "System", Settings::appTextures().gearIcon, 0.f, true); 
    this->TDRendererSettingsBtn = Button(ELEMENT_STYLE_BASIC, glm::vec2(4,2), "3D Renderer", Settings::appTextures().TDModelIcon, 0.f, true); 
    
    //Create the panel
    this->panel = Panel({
        {
            Section(
                Element(Button()),
                {
                    Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(4,2),"Settings",Texture(),0.f,false)), 
                }
            ),
            Section()
        }
    },glm::vec2(20.f, 37.f), glm::vec3(50.f,50.f,0.8f),ColorPalette::mainColor,ColorPalette::thirdColor,true,true,true,true,true,1.f,1.f,{},0.25f,false);

    TDRendererSettings = Section(
                                Element(),
                                {
                                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),"X : Rotation",Texture(),15.f, 0.f, 360.f, 0.f)), 
                                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),"Y : Rotation",Texture(),0.f, 0.f, 360.f, 0.f)), 
                                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),"Z : Rotation",Texture(),0.f, 0.f, 360.f, 0.f)), 
                                    
                                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),"X : Transform",Texture(),2.f, -50.f, 50.f, 0.f)), 
                                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),"Y : Transform",Texture(),0.f, -50.f, 50.f, 0.f)), 
                                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),"Z : Transform",Texture(),0.f, -50.f, 50.f, 0.f)), 
                                
                                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),"Fov", Texture(),2.f, 1.f, 180.f, 40.f)), 
                                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),"Near", Texture(),0.f, 0.f, 1.f, 0.1f)), 
                                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),"Far", Texture(),0.f, 0.f, 1000.f, 1000.f)), 
                                    Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Orthographic projection"  , 0.f)),

                                    Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Backface Culling"  , 2.f)),
                                    
                                    Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Apply Height Map"  , 2.f)),
                                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),"Height Map Strength", Texture(),0.f, 0.f, 1.f, 0.1f)), 
                                    
                                    Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Render scene tiles/grid"  , 2.f)),
                                    Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Render scene axis displayer"  , 0.f))
                                }
                            );

    systemSettings = Section(
                                    Element(),
                                    {

                                        Element(ComboBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),
                                        {
                                            "256",
                                            "512",
                                            "1024",
                                            "2048",
                                            "4096"
                                        }, "Texture Resolution", 15.f)),
                                        
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"VSync"  , 2.f)),
                                        
                                        
                                        Element(ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(2,2.f), 
                                        {
                                            "1.000000",
                                            "1.200000",
                                            "1.500000",
                                            "1.700000",
                                            "2.000000",
                                            "2.500000",
                                            "3.000000",
                                            "4.000000",
                                            "8.000000"

                                        }, "Painting Resolution Divider",2.f)),
                                        
                                        Element(ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(2,2.f), 
                                        {
                                            "1.000000",
                                            "1.200000",
                                            "1.500000",
                                            "1.700000",
                                            "2.000000",
                                            "2.500000",
                                            "3.000000",
                                            "4.000000",
                                            "8.000000"

                                        }, "Painting Depth Texture Resolution Divider", 2.f)),
                                        
                                        Element(ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(2,2.f), 
                                        {
                                            "0.900000",
                                            "1.000000",
                                            "1.050000",
                                            "1.100000",
                                            "1.150000",
                                            "1.200000",
                                            "1.300000",
                                            "1.500000",
                                            "1.700000",
                                            "2.000000",
                                            "2.500000",
                                            "3.000000",
                                            "4.000000",
                                            "8.000000"

                                        }, "Framebuffer Resolution Divider",2.f)),

                                        RangeBar(ELEMENT_STYLE_STYLIZED, glm::vec2(2.f), "Framebuffer Samples", Texture(), 2.f, 1, 16, 4),

                                        // --- Info ---
                                        Button(ELEMENT_STYLE_SOLID, glm::vec2(1.f), "", Texture(), 0.f, false), // Painting resolution
                                        Button(ELEMENT_STYLE_SOLID, glm::vec2(1.f), "", Texture(), 0.f, false), // Painting Depth Texture Resolution Divider
                                        Button(ELEMENT_STYLE_SOLID, glm::vec2(1.f), "", Texture(), 0.f, false), // Framebuffer resolution 
                                        Button(ELEMENT_STYLE_SOLID, glm::vec2(1.f), "", Texture(), 0.f, false), // Framebuffer state 
                                    }
                                );
}

void SettingsDialog::render(Timer timer, Painter &painter){
    
    dialogControl.updateStart();   

    //Set the combo box selected index as the textureRes
    int txtrRes = 256;
    for (size_t i = 0; i < systemSettings.elements[0].comboBox.texts.size(); i++)
    {
        if(Settings::properties()->textureRes == txtrRes)
            systemSettings.elements[0].comboBox.selectedIndex = i;
        
        txtrRes*=2;
    }

    TDRendererSettings.elements[0].rangeBar.value = getScene()->transformRotation.x;
    TDRendererSettings.elements[1].rangeBar.value = getScene()->transformRotation.y;
    TDRendererSettings.elements[2].rangeBar.value = getScene()->transformRotation.z;
    TDRendererSettings.elements[3].rangeBar.value = getScene()->transformLocation.x;
    TDRendererSettings.elements[4].rangeBar.value = getScene()->transformLocation.y;
    TDRendererSettings.elements[5].rangeBar.value = getScene()->transformLocation.z;
    TDRendererSettings.elements[6].rangeBar.value = getScene()->fov;
    TDRendererSettings.elements[7].rangeBar.value = getScene()->aNear;
    TDRendererSettings.elements[8].rangeBar.value = getScene()->aFar;
    TDRendererSettings.elements[9].checkBox.clickState1 = getScene()->useOrtho;
    TDRendererSettings.elements[11].checkBox.clickState1 = Settings::properties()->useHeightMap;
    TDRendererSettings.elements[12].rangeBar.value = Settings::properties()->heightMapStrength;
    TDRendererSettings.elements[13].checkBox.clickState1 = getScene()->renderTiles;
    TDRendererSettings.elements[14].checkBox.clickState1 = getScene()->renderAxisDisplayer;
    systemSettings.elements[0].comboBox.selectedIndex = UTIL::findCorrespondingIndex(std::to_string(Settings::properties()->textureRes), systemSettings.elements[0].comboBox.texts);
    systemSettings.elements[1].checkBox.clickState1 = Settings::properties()->VSync;
    systemSettings.elements[2].comboBox.selectedIndex = UTIL::findCorrespondingIndex(std::to_string(Settings::properties()->paintingResolutionDivier), systemSettings.elements[2].comboBox.texts);
    systemSettings.elements[3].comboBox.selectedIndex = UTIL::findCorrespondingIndex(std::to_string(Settings::properties()->paintingDepthTextureResolutionDivier), systemSettings.elements[3].comboBox.texts);
    systemSettings.elements[4].comboBox.selectedIndex = UTIL::findCorrespondingIndex(std::to_string(Settings::properties()->framebufferResolutionDivier), systemSettings.elements[4].comboBox.texts);
    systemSettings.elements[5].rangeBar.value = Settings::properties()->framebufferSamples;

    glm::ivec2 paintingRes = glm::ivec2(*Settings::videoScale() / Settings::properties()->paintingResolutionDivier);
    glm::ivec2 depthRes = glm::ivec2(*Settings::videoScale() / Settings::properties()->paintingDepthTextureResolutionDivier);
    glm::ivec2 framebufferRes = glm::ivec2(*Settings::videoScale() / Settings::properties()->framebufferResolutionDivier);

    systemSettings.elements[6].button.text = "Painting resolution : " + std::to_string(paintingRes.x) + "x" + std::to_string(paintingRes.y);
    systemSettings.elements[7].button.text = "Painting depth texture resolution : " + std::to_string(depthRes.x) + "x" + std::to_string(depthRes.y);
    systemSettings.elements[8].button.text = "Framebuffer resolution : " + std::to_string(framebufferRes.x) + "x" + std::to_string(framebufferRes.y);
    
    if(Settings::properties()->framebufferResolutionDivier == 1.f)
        systemSettings.elements[9].button.text = "Using default framebuffer (Multisampling disabled)";
    else
        systemSettings.elements[9].button.text = "Using custom framebuffer";


    if(TDRendererSettings.elements[12].rangeBar.pointerPressed && !*Mouse::LPressed() && TDRendererSettings.elements[11].checkBox.clickState1){
        for (size_t i = 0; i < getModel()->meshes.size(); i++)
        {
            getModel()->meshes[i].processHeightMap();
        }
    }

    if(this->selectedSettingsMode == 0)
        panel.sections[1] = systemSettings; 
    else if(this->selectedSettingsMode == 1)
        panel.sections[1] = TDRendererSettings; 
    
    //Render the panel    
    panel.render(timer,true);
    
    if(this->selectedSettingsMode == 0)
        systemSettings = panel.sections[1]; 
    else if(this->selectedSettingsMode == 1)
        TDRendererSettings = panel.sections[1]; 
    
    systemSettingsBtn.pos = panel.pos;
    systemSettingsBtn.pos.z += 0.001f;
    systemSettingsBtn.pos.y -= panel.scale.y - systemSettingsBtn.scale.y - panel.sections[0].elements[0].scale.y * 4.f;
    systemSettingsBtn.pos.x -= systemSettingsBtn.scale.y * 2.f;
    this->systemSettingsBtn.render(timer, true);
    TDRendererSettingsBtn.pos = panel.pos;
    TDRendererSettingsBtn.pos.z += 0.001f;
    TDRendererSettingsBtn.pos.y -= panel.scale.y - systemSettingsBtn.scale.y - panel.sections[0].elements[0].scale.y * 4.f;
    TDRendererSettingsBtn.pos.x += TDRendererSettingsBtn.scale.y * 2.f;
    this->TDRendererSettingsBtn.render(timer, true);

    std::vector<Button*> btns;
    btns.push_back(&this->systemSettingsBtn);
    btns.push_back(&this->TDRendererSettingsBtn);

    for (size_t i = 0; i < btns.size(); i++){
        if(btns[i]->clickState1 && i != this->selectedSettingsMode){
            this->selectedSettingsMode = i;
            for (size_t j = 0; j < btns.size(); j++)
            {
                btns[j]->clickState1 = false;
            }
        }
    }

    for (size_t i = 0; i < btns.size(); i++)
    {
        if(btns[i]->clickState1 == false && i == this->selectedSettingsMode)
            btns[i]->clickState1 = true;
    }

    getScene()->transformRotation.x = TDRendererSettings.elements[0].rangeBar.value;
    getScene()->transformRotation.y = TDRendererSettings.elements[1].rangeBar.value;
    getScene()->transformRotation.z = TDRendererSettings.elements[2].rangeBar.value;
    getScene()->transformLocation.x = TDRendererSettings.elements[3].rangeBar.value;
    getScene()->transformLocation.y = TDRendererSettings.elements[4].rangeBar.value;
    getScene()->transformLocation.z = TDRendererSettings.elements[5].rangeBar.value;
    getScene()->fov = TDRendererSettings.elements[6].rangeBar.value;
    getScene()->aNear = TDRendererSettings.elements[7].rangeBar.value;
    getScene()->aFar = TDRendererSettings.elements[8].rangeBar.value;
    getScene()->useOrtho = TDRendererSettings.elements[9].checkBox.clickState1;
    Settings::properties()->backfaceCulling = TDRendererSettings.elements[10].checkBox.clickState1;
    Settings::properties()->useHeightMap = TDRendererSettings.elements[11].checkBox.clickState1;
    Settings::properties()->heightMapStrength = TDRendererSettings.elements[12].rangeBar.value;

    getScene()->renderTiles = TDRendererSettings.elements[13].checkBox.clickState1;
    getScene()->renderAxisDisplayer = TDRendererSettings.elements[14].checkBox.clickState1;
    
    Settings::properties()->textureRes = std::stoi(systemSettings.elements[0].comboBox.texts[systemSettings.elements[0].comboBox.selectedIndex]);
    Settings::properties()->VSync = systemSettings.elements[1].checkBox.clickState1;
    Settings::properties()->paintingResolutionDivier = std::stof(systemSettings.elements[2].comboBox.texts[systemSettings.elements[2].comboBox.selectedIndex]);
    Settings::properties()->paintingDepthTextureResolutionDivier = std::stof(systemSettings.elements[3].comboBox.texts[systemSettings.elements[3].comboBox.selectedIndex]);
    Settings::properties()->framebufferResolutionDivier = std::stof(systemSettings.elements[4].comboBox.texts[systemSettings.elements[4].comboBox.selectedIndex]);
    Settings::properties()->framebufferSamples = systemSettings.elements[5].rangeBar.value;
    
    if(TDRendererSettings.elements[11].checkBox.hover && *Mouse::LClick()){
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
        painter.refreshBuffers();
        dialogControl.unActivate();
        Settings::defaultFramebuffer()->setResolution(*Settings::videoScale() / Settings::properties()->framebufferResolutionDivier);
    }

    dialogControl.updateEnd(timer,0.15f);   
}