
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

#include "UTIL/Mouse/Mouse.hpp" 
#include "UTIL/Settings/Settings.hpp" 
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/Painting/Painter.hpp"

#include <string>
#include <iostream>
#include <vector>

// -------- 3D Rendering --------
static RangeBar* transform_x_rotation_rangebar;
static RangeBar* transform_y_rotation_rangebar;
static RangeBar* transform_z_rotation_rangebar;

static RangeBar* camera_fov_rangebar;
static RangeBar* camera_near_rangebar;
static RangeBar* camera_far_rangebar;
static CheckBox* camera_ortho_projection_checkbox;

static CheckBox* heightmap_apply_heightmap_checkbox;
static RangeBar* heightmap_heightmap_strength_rangebar;

static CheckBox* scene_backface_culling_checkbox;
static CheckBox* scene_render_grid_checkbox;
static CheckBox* scene_render_axis_displayer_checkbox;

// -------- System --------
static CheckBox* performance_vsync_checkbox;
static ComboBox* performance_framebuffer_resolution_divider_combobox;

static CheckBox* cat_enable_comments_checkbox;
static CheckBox* cat_verify_exit_checkbox;
static CheckBox* cat_hide_checkbox;

static Button* info_txt0_button;
static Button* info_txt1_button;
static Button* info_txt2_button;
static Button* info_txt3_button;
static Button* info_txt4_button;


SettingsDialog::SettingsDialog(int){
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
                                    Element(CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(2,2.f),"Backface Culling"  , 0.5f)),
                                    Element(CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(2,2.f),"Render scene tiles/grid"  , 2.f)),
                                    Element(CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(2,2.f),"Render scene axis displayer"  , 0.f))
                                }
                            )
                        };

    systemSettings = {
                                Section(
                                    SectionHolder(sectionBtnClr, 2.5f, "Performance"),
                                    {
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"VSync"  , 0.5f)),
                                        
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
                                    SectionHolder(sectionBtnClr, 0.5f, "Cat"),
                                    {
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f), "Allow Comments"  , 0.5f)),
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f), "Verify The Exit"  , 0.5f)),
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f), "Hide that annoying cat >:("  , 0.5f))
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
            };

    // -------- 3D Rendering --------
    transform_x_rotation_rangebar = &TDRendererSettings[0].elements[0].rangeBar;
    transform_y_rotation_rangebar = &TDRendererSettings[0].elements[1].rangeBar;
    transform_z_rotation_rangebar = &TDRendererSettings[0].elements[2].rangeBar;
        
    camera_fov_rangebar = &TDRendererSettings[1].elements[0].rangeBar;
    camera_near_rangebar = &TDRendererSettings[1].elements[1].rangeBar;
    camera_far_rangebar = &TDRendererSettings[1].elements[2].rangeBar;
    camera_ortho_projection_checkbox = &TDRendererSettings[1].elements[3].checkBox;
        
    heightmap_apply_heightmap_checkbox = &TDRendererSettings[2].elements[0].checkBox;
    heightmap_heightmap_strength_rangebar = &TDRendererSettings[2].elements[1].rangeBar;
        
    scene_backface_culling_checkbox = &TDRendererSettings[3].elements[0].checkBox;
    scene_render_grid_checkbox = &TDRendererSettings[3].elements[1].checkBox;
    scene_render_axis_displayer_checkbox = &TDRendererSettings[3].elements[2].checkBox;
        
    // -------- System --------
    performance_vsync_checkbox = &systemSettings[0].elements[0].checkBox;
    performance_framebuffer_resolution_divider_combobox = &systemSettings[0].elements[1].comboBox;
        
    cat_enable_comments_checkbox = &systemSettings[1].elements[0].checkBox;
    cat_verify_exit_checkbox = &systemSettings[1].elements[1].checkBox;
    cat_hide_checkbox = &systemSettings[1].elements[2].checkBox;
        
    info_txt0_button = &systemSettings[2].elements[0].button;
    info_txt1_button = &systemSettings[2].elements[1].button;
    info_txt2_button = &systemSettings[2].elements[2].button;
    info_txt3_button = &systemSettings[2].elements[3].button;
    info_txt4_button = &systemSettings[2].elements[4].button;
}



void SettingsDialog::show(Timer& timer){
    
    dialogControl.activate();

    while (!getContext()->window.shouldClose())
    {
        dialogControl.updateStart(true);   

        this->setPropertiesToDialog();

        this->renderPanel(timer);
        this->updateInfoTexts(timer);

        this->setDialogToProperties();

        getScene()->updateProjectionMatrix(0.f);
        getScene()->camera.updateViewMatrix();
        getScene()->updateTransformMatrix();

        //End the dialog
        if  (
                getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || //Escape key pressed 
                ((!bgPanel.hover && !dialog_log.isHovered() && *Mouse::LClick() && !dialog_log.isHovered())) && !dialogControl.firstFrameActivated //Mouse Lclick out of the panel
            )
        {
            dialogControl.unActivate();
            Settings::defaultFramebuffer()->setResolution(*Settings::videoScale() / Settings::properties()->framebufferResolutionDivier);
        }

        dialogControl.updateEnd(timer,0.15f);   
        if(dialogControl.mixVal == 0.f)
            break;
    }
}





void SettingsDialog::setPropertiesToDialog(){
    
    // -------- 3D Rendering --------
    transform_x_rotation_rangebar->value = getScene()->transformRotation.x;
    transform_y_rotation_rangebar->value = getScene()->transformRotation.y;
    transform_z_rotation_rangebar->value = getScene()->transformRotation.z;
    
    camera_fov_rangebar->value = getScene()->fov;
    camera_near_rangebar->value = getScene()->aNear;
    camera_far_rangebar->value = getScene()->aFar;
    camera_ortho_projection_checkbox->clickState1 = getScene()->useOrtho;
    
    heightmap_apply_heightmap_checkbox->clickState1 = getScene()->useHeightMap;
    heightmap_heightmap_strength_rangebar->value = getScene()->heightMapStrength;
    
    scene_backface_culling_checkbox->clickState1 = getScene()->backfaceCulling;
    scene_render_grid_checkbox->clickState1 = getScene()->renderTiles;
    scene_render_axis_displayer_checkbox->clickState1 = getScene()->renderAxisDisplayer;
    
    // -------- System --------
    performance_vsync_checkbox->clickState1 = Settings::properties()->VSync;
    performance_framebuffer_resolution_divider_combobox->selectedIndex = UTIL::findCorrespondingIndex(std::to_string(Settings::properties()->framebufferResolutionDivier), performance_framebuffer_resolution_divider_combobox->texts);
    
    cat_enable_comments_checkbox->clickState1 = Settings::properties()->cat_allowComments;
    cat_verify_exit_checkbox->clickState1 = Settings::properties()->cat_verifyTheExit;
    cat_hide_checkbox->clickState1 = Settings::properties()->cat_hide;
}

void SettingsDialog::setDialogToProperties(){
    
    // -------- 3D Rendering --------
    getScene()->transformRotation.x = transform_x_rotation_rangebar->value;
    getScene()->transformRotation.y = transform_y_rotation_rangebar->value;
    getScene()->transformRotation.z = transform_z_rotation_rangebar->value;
    
    getScene()->fov = camera_fov_rangebar->value;
    getScene()->aNear = camera_near_rangebar->value;
    getScene()->aFar = camera_far_rangebar->value;
    getScene()->useOrtho = camera_ortho_projection_checkbox->clickState1;
    
    getScene()->useHeightMap = heightmap_apply_heightmap_checkbox->clickState1;
    getScene()->heightMapStrength = heightmap_heightmap_strength_rangebar->value;
    
    getScene()->backfaceCulling = scene_backface_culling_checkbox->clickState1;
    getScene()->renderTiles = scene_render_grid_checkbox->clickState1;
    getScene()->renderAxisDisplayer = scene_render_axis_displayer_checkbox->clickState1;
    
    // -------- System --------
    Settings::properties()->VSync = performance_vsync_checkbox->clickState1;
    Settings::properties()->framebufferResolutionDivier = std::stof(performance_framebuffer_resolution_divider_combobox->texts[performance_framebuffer_resolution_divider_combobox->selectedIndex]);
    
    Settings::properties()->cat_allowComments = cat_enable_comments_checkbox->clickState1;
    Settings::properties()->cat_verifyTheExit = cat_verify_exit_checkbox->clickState1;
    Settings::properties()->cat_hide = cat_hide_checkbox->clickState1;
}

void SettingsDialog::renderPanel(Timer& timer){
    
    // ------- Render the panel -------    
    if(this->selectedSettingsMode == 0)
        panel.sections = systemSettings; 
    else if(this->selectedSettingsMode == 1)
        panel.sections = TDRendererSettings; 
    
    bgPanel.render(timer, !dialog_log.isHovered());
    panel.render(timer, !dialog_log.isHovered());
    
    if(this->selectedSettingsMode == 0)
        systemSettings = panel.sections; 
    else if(this->selectedSettingsMode == 1)
        TDRendererSettings = panel.sections; 
    
    // ------- Render the mode buttons -------    
    systemSettingsBtn.pos = bgPanel.pos;
    systemSettingsBtn.pos.z += 0.001f;
    systemSettingsBtn.pos.y -= bgPanel.scale.y - systemSettingsBtn.scale.y;
    systemSettingsBtn.pos.x -= systemSettingsBtn.scale.y * 2.f;
    this->systemSettingsBtn.render(timer, !dialog_log.isHovered());
    
    TDRendererSettingsBtn.pos = bgPanel.pos;
    TDRendererSettingsBtn.pos.z += 0.001f;
    TDRendererSettingsBtn.pos.y -= bgPanel.scale.y - systemSettingsBtn.scale.y;
    TDRendererSettingsBtn.pos.x += TDRendererSettingsBtn.scale.y * 2.f;
    this->TDRendererSettingsBtn.render(timer, !dialog_log.isHovered());

    // ------- Handle mode buttons interactions -------    
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

    if(heightmap_heightmap_strength_rangebar->pointerPressed && !*Mouse::LPressed() && heightmap_apply_heightmap_checkbox->clickState1){
        for (size_t i = 0; i < getScene()->model->meshes.size(); i++)
        {
            getScene()->model->meshes[i].processHeightMap();
        }
    }
}

void SettingsDialog::updateInfoTexts(Timer& timer){
    info_txt0_button->text = "Painting framebuffer resolution : " + std::to_string(painting_projected_painting_FBO.colorBuffer.getResolution().x) + "x" + std::to_string(painting_projected_painting_FBO.colorBuffer.getResolution().y);
    
    info_txt1_button->text = "Video scale : " + std::to_string(Settings::videoScale()->x) + "x" + std::to_string(Settings::videoScale()->y);
    
    info_txt2_button->text = "Framebuffer resolution : " + std::to_string(Settings::videoScale()->x / Settings::properties()->framebufferResolutionDivier) + "x" + std::to_string(Settings::videoScale()->y / Settings::properties()->framebufferResolutionDivier);
    
    if(Settings::properties()->framebufferResolutionDivier == 1.f)
        info_txt3_button->text = "Using default framebuffer (Multisampling disabled)";
    else
        info_txt3_button->text = "Using custom framebuffer";
    
    info_txt4_button->text = "Current frame rate : " + std::to_string(timer.FPS) + " & The greatest frame rate value : " + std::to_string(timer.mostHighFPSValue);
}