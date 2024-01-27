
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 (c) 2024 Mert Tetik. All rights reserved.

 Official GitHub Link : https://github.com/mert-tetik/LigidPainter
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
    this->systemSettingsBtn = Button(ELEMENT_STYLE_BASIC, glm::vec2(4,2), "System", appTextures.gearIcon, 0.f, true); 
    this->TDRendererSettingsBtn = Button(ELEMENT_STYLE_BASIC, glm::vec2(4,2), "3D Renderer", appTextures.TDModelIcon, 0.f, true); 
    
    //Create the background panel
    this->bgPanel = Panel({
        {
            Section()
        }
    },glm::vec2(20.5f, 22.5f), glm::vec3(50.f,50.f,0.8f),ColorPalette::secondColor, ColorPalette::thirdColor,true,true,true,true,true,1.f,1.f,{},0.25f,false);
    
    //Create the panel
    this->panel = Panel({
        {
            Section()
        }
    },glm::vec2(20.f, 20.f), glm::vec3(50.f,52.f,0.8f), ColorPalette::mainColor, ColorPalette::secondColor, true,true,true,true,true,1.f,1.f,{},0.25f,false);

    glm::vec4 sectionBtnClr = glm::vec4(ColorPalette::secondColor.r, ColorPalette::secondColor.g, ColorPalette::secondColor.b, 1.f);

    TDRendererSettings = {
                            Section(
                                SectionHolder(sectionBtnClr, 1.f, "Transform"),
                                {
                                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),"X : Rotation",Texture(),0.5f, 0.f, 360.f, 0.f)), 
                                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),"Y : Rotation",Texture(),2.f, 0.f, 360.f, 0.f)), 
                                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),"Z : Rotation",Texture(),2.f, 0.f, 360.f, 0.f)), 
                                    
                                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),"X : Translate",Texture(),3.f, -50.f, 50.f, 0.f)), 
                                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),"Y : Translate",Texture(),2.f, -50.f, 50.f, 0.f)), 
                                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),"Z : Translate",Texture(),2.f, -50.f, 50.f, 0.f)), 
                                }
                            ),
                            Section(
                                SectionHolder(sectionBtnClr, 0.5f, "Camera"),
                                {       
                                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),"Fov", Texture(),0.5f, 1.f, 180.f, 40.f)), 
                                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),"Near", Texture(),2.f, 0.f, 1.f, 0.1f)), 
                                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),"Far", Texture(),2.f, 0.f, 1000.f, 1000.f)), 
                                    Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Orthographic projection"  , 0.f)),
                                }
                            ),
                            Section(
                                SectionHolder(sectionBtnClr, 1.5f, "Height Map"),
                                {       
                                    Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Apply Height Map"  , 0.5f)),
                                    Element(RangeBar(ELEMENT_STYLE_BASIC,glm::vec2(2,1.f),"Height Map Strength", Texture(),0.f, 0.f, 1.f, 0.1f)), 
                                }
                            ),
                            Section(
                                SectionHolder(sectionBtnClr, 0.5f, "Scene"),
                                {       
                                    Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Backface Culling"  , 0.5f)),
                                    Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Render scene tiles/grid"  , 2.f)),
                                    Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Render scene axis displayer"  , 0.f))
                                }
                            )
                        };

    systemSettings = {
                                Section(
                                    Element(),
                                    {
                                        Element(ComboBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),
                                        {
                                            "256",
                                            "512",
                                            "1024",
                                            "2048",
                                            "4096"
                                        }, "Texture Resolution", 1.f))
                                    }
                                ),

                                Section(
                                    SectionHolder(sectionBtnClr, 2.5f, "Performance"),
                                    {
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"VSync"  , 0.5f)),
                                        
                                        Element(ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(2,1.5f), 
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
                                        
                                        Element(ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(2,1.5f), 
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
                                        
                                        Element(ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(2,1.5f), 
                                        {
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
                                            "3.000000"

                                        }, "Framebuffer Resolution Divider",2.f))
                                    }
                                ),
                                Section(
                                    SectionHolder(sectionBtnClr, 1.5f, "Info"),
                                    {
                                        Button(ELEMENT_STYLE_SOLID, glm::vec2(1.f), "", Texture(), 0.5f, false), // Painting resolution
                                        Button(ELEMENT_STYLE_SOLID, glm::vec2(1.f), "", Texture(), 0.f, false), // Painting Depth Texture Resolution Divider
                                        Button(ELEMENT_STYLE_SOLID, glm::vec2(1.f), "", Texture(), 0.f, false), // Framebuffer resolution 
                                        Button(ELEMENT_STYLE_SOLID, glm::vec2(1.f), "", Texture(), 0.f, false), // Framebuffer state 
                                        Button(ELEMENT_STYLE_SOLID, glm::vec2(1.f), "", Texture(), 0.f, false)  // Framerate data 
                                    }
                                ),
                                Section(
                                    SectionHolder(sectionBtnClr, 0.5f, "Cat"),
                                    {
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f), "Allow Comments"  , 0.5f)),
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f), "Verify The Exit"  , 0.5f)),
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f), "Hide that annoying cat >:("  , 0.5f))
                                    }
                                ),
            };
}

void SettingsDialog::render(Timer timer, Painter &painter, LogDialog& logDialog){
    
    dialogControl.updateStart();   

    //Set the combo box selected index as the textureRes
    int txtrRes = 256;
    for (size_t i = 0; i < systemSettings[0].elements[0].comboBox.texts.size(); i++)
    {
        if(Settings::properties()->textureRes == txtrRes)
            systemSettings[0].elements[0].comboBox.selectedIndex = i;
        
        txtrRes*=2;
    }


    TDRendererSettings[0].elements[0].rangeBar.value = getScene()->transformRotation.x;
    TDRendererSettings[0].elements[1].rangeBar.value = getScene()->transformRotation.y;
    TDRendererSettings[0].elements[2].rangeBar.value = getScene()->transformRotation.z;
    TDRendererSettings[0].elements[3].rangeBar.value = getScene()->transformLocation.x;
    TDRendererSettings[0].elements[4].rangeBar.value = getScene()->transformLocation.y;
    TDRendererSettings[0].elements[5].rangeBar.value = getScene()->transformLocation.z;
    
    TDRendererSettings[1].elements[0].rangeBar.value = getScene()->fov;
    TDRendererSettings[1].elements[1].rangeBar.value = getScene()->aNear;
    TDRendererSettings[1].elements[2].rangeBar.value = getScene()->aFar;
    TDRendererSettings[1].elements[3].checkBox.clickState1 = getScene()->useOrtho;
    
    TDRendererSettings[2].elements[0].checkBox.clickState1 = Settings::properties()->useHeightMap;
    TDRendererSettings[2].elements[1].rangeBar.value = Settings::properties()->heightMapStrength;
    
    TDRendererSettings[3].elements[0].checkBox.clickState1 = Settings::properties()->backfaceCulling;
    TDRendererSettings[3].elements[1].checkBox.clickState1 = getScene()->renderTiles;
    TDRendererSettings[3].elements[2].checkBox.clickState1 = getScene()->renderAxisDisplayer;
    
    systemSettings[0].elements[0].comboBox.selectedIndex = UTIL::findCorrespondingIndex(std::to_string(Settings::properties()->textureRes), systemSettings[0].elements[0].comboBox.texts);
    
    systemSettings[1].elements[0].checkBox.clickState1 = Settings::properties()->VSync;
    systemSettings[1].elements[1].comboBox.selectedIndex = UTIL::findCorrespondingIndex(std::to_string(Settings::properties()->paintingResolutionDivier), systemSettings[1].elements[1].comboBox.texts);
    systemSettings[1].elements[2].comboBox.selectedIndex = UTIL::findCorrespondingIndex(std::to_string(Settings::properties()->paintingDepthTextureResolutionDivier), systemSettings[1].elements[2].comboBox.texts);
    systemSettings[1].elements[3].comboBox.selectedIndex = UTIL::findCorrespondingIndex(std::to_string(Settings::properties()->framebufferResolutionDivier), systemSettings[1].elements[3].comboBox.texts);
    
    systemSettings[3].elements[0].checkBox.clickState1 = Settings::properties()->cat_allowComments;
    systemSettings[3].elements[1].checkBox.clickState1 = Settings::properties()->cat_verifyTheExit;
    systemSettings[3].elements[2].checkBox.clickState1 = Settings::properties()->cat_hide;

    glm::ivec2 paintingRes = glm::ivec2(painter.getBufferResolutions(0));
    glm::ivec2 depthRes = glm::ivec2(painter.getBufferResolutions(1));
    glm::ivec2 framebufferRes = glm::ivec2(*Settings::videoScale() / Settings::properties()->framebufferResolutionDivier);

    systemSettings[2].elements[0].button.text = "Painting resolution : " + std::to_string(paintingRes.x) + "x" + std::to_string(paintingRes.y);
    systemSettings[2].elements[1].button.text = "Painting depth texture resolution : " + std::to_string(depthRes.x) + "x" + std::to_string(depthRes.y);
    systemSettings[2].elements[2].button.text = "Framebuffer resolution : " + std::to_string(framebufferRes.x) + "x" + std::to_string(framebufferRes.y);
    if(Settings::properties()->framebufferResolutionDivier == 1.f)
        systemSettings[2].elements[3].button.text = "Using default framebuffer (Multisampling disabled)";
    else
        systemSettings[2].elements[3].button.text = "Using custom framebuffer";
    systemSettings[2].elements[4].button.text = "Current frame rate : " + std::to_string(timer.FPS) + " & The greatest frame rate value : " + std::to_string(timer.mostHighFPSValue);

    if(TDRendererSettings[2].elements[1].rangeBar.pointerPressed && !*Mouse::LPressed() && TDRendererSettings[2].elements[0].checkBox.clickState1){
        for (size_t i = 0; i < getModel()->meshes.size(); i++)
        {
            getModel()->meshes[i].processHeightMap();
        }
    }

    if(this->selectedSettingsMode == 0)
        panel.sections = systemSettings; 
    else if(this->selectedSettingsMode == 1)
        panel.sections = TDRendererSettings; 
    
    //Render the panel    
    bgPanel.render(timer,true);
    panel.render(timer,true);
    
    if(this->selectedSettingsMode == 0)
        systemSettings = panel.sections; 
    else if(this->selectedSettingsMode == 1)
        TDRendererSettings = panel.sections; 
    
    systemSettingsBtn.pos = bgPanel.pos;
    systemSettingsBtn.pos.z += 0.001f;
    systemSettingsBtn.pos.y -= bgPanel.scale.y - systemSettingsBtn.scale.y;
    systemSettingsBtn.pos.x -= systemSettingsBtn.scale.y * 2.f;
    this->systemSettingsBtn.render(timer, true);
    TDRendererSettingsBtn.pos = bgPanel.pos;
    TDRendererSettingsBtn.pos.z += 0.001f;
    TDRendererSettingsBtn.pos.y -= bgPanel.scale.y - systemSettingsBtn.scale.y;
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

    getScene()->transformRotation.x = TDRendererSettings[0].elements[0].rangeBar.value;
    getScene()->transformRotation.y = TDRendererSettings[0].elements[1].rangeBar.value;
    getScene()->transformRotation.z = TDRendererSettings[0].elements[2].rangeBar.value;
    getScene()->transformLocation.x = TDRendererSettings[0].elements[3].rangeBar.value;
    getScene()->transformLocation.y = TDRendererSettings[0].elements[4].rangeBar.value;
    getScene()->transformLocation.z = TDRendererSettings[0].elements[5].rangeBar.value;
    
    getScene()->fov = TDRendererSettings[1].elements[0].rangeBar.value;
    getScene()->aNear = TDRendererSettings[1].elements[1].rangeBar.value;
    getScene()->aFar = TDRendererSettings[1].elements[2].rangeBar.value;
    getScene()->useOrtho = TDRendererSettings[1].elements[3].checkBox.clickState1;
    
    Settings::properties()->useHeightMap = TDRendererSettings[2].elements[0].checkBox.clickState1;
    Settings::properties()->heightMapStrength = TDRendererSettings[2].elements[1].rangeBar.value;
    
    Settings::properties()->backfaceCulling = TDRendererSettings[3].elements[0].checkBox.clickState1;
    getScene()->renderTiles = TDRendererSettings[3].elements[1].checkBox.clickState1;
    getScene()->renderAxisDisplayer = TDRendererSettings[3].elements[2].checkBox.clickState1;
    
    Settings::properties()->textureRes = std::stoi(systemSettings[0].elements[0].comboBox.texts[systemSettings[0].elements[0].comboBox.selectedIndex]);
    
    Settings::properties()->VSync = systemSettings[1].elements[0].checkBox.clickState1;
    Settings::properties()->paintingResolutionDivier = std::stof(systemSettings[1].elements[1].comboBox.texts[systemSettings[1].elements[1].comboBox.selectedIndex]);
    Settings::properties()->paintingDepthTextureResolutionDivier = std::stof(systemSettings[1].elements[2].comboBox.texts[systemSettings[1].elements[2].comboBox.selectedIndex]);
    Settings::properties()->framebufferResolutionDivier = std::stof(systemSettings[1].elements[3].comboBox.texts[systemSettings[1].elements[3].comboBox.selectedIndex]);
    
    Settings::properties()->cat_allowComments = systemSettings[3].elements[0].checkBox.clickState1;
    Settings::properties()->cat_verifyTheExit = systemSettings[3].elements[1].checkBox.clickState1;
    Settings::properties()->cat_hide = systemSettings[3].elements[2].checkBox.clickState1;

    if(TDRendererSettings[2].elements[0].checkBox.hover && *Mouse::LClick()){
        for (size_t i = 0; i < getModel()->meshes.size(); i++)
        {
            getModel()->meshes[i].processHeightMap();
        }
    }
    
    getScene()->updateProjectionMatrix(0.f);
    getScene()->updateViewMatrix();
    getScene()->updateTransformMatrix();

    //End the dialog
    if  (
            getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || //Escape key pressed 
            ((!bgPanel.hover && *Mouse::LClick() && !logDialog.isHovered())) && !dialogControl.firstFrameActivated //Mouse Lclick out of the panel
        )
    {
        dialogControl.unActivate();
        Settings::defaultFramebuffer()->setResolution(*Settings::videoScale() / Settings::properties()->framebufferResolutionDivier);
    }

    dialogControl.updateEnd(timer,0.15f);   
}