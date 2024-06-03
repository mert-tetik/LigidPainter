
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 (c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
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
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

#include "tinyfiledialogs.h"

DisplayerDialog::DisplayerDialog(int){

    this->bg_txtr_selection_panel = Panel(
        {
            {
                Section(
                    Element(Button()),
                    {
                        Button(ELEMENT_STYLE_STYLIZED, glm::vec2(2.f, 2.f), "Add Custom", Texture(), 4.f, false)
                    }
                )
            }
        },
        glm::vec2(10.f, 26.f),
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

    std::vector<std::string> bg_txtr_paths = UTIL::get_files_in_folder("./LigidPainter/Resources/Images/Background");
    
    for(std::string bg_path : bg_txtr_paths){
        Texture txtr;
        txtr.load(bg_path.c_str());
        
        this->bg_txtr_selection_panel.sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f, 4.f), txtr.title, txtr, 0.f, false));
    }


    //Create the panel
    this->panel = Panel(
        
        {
            {
                Section(
                    Element(Button()),
                    {
                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,6), "", Texture(), 2.f, false)), // 0
                        Element(RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1), "Rotation", Texture(), 2.f,0.f,360.f,0.f)), // 1
                        Element(RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1), "Blur", Texture(), 2.f,0.f,100.f,0.f)), // 2
                        Element(RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1), "Opacity", Texture(), 2.f,0.f,100.f,0.f)), // 3
                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "Color", Texture(), 2.f, false)), // 4
                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4), "", Texture(), 2.f,false)), // 5
                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2), "Select Background Image", Texture(), 1.f,false)), // 6
                        Element(RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1), "Image Opacity", Texture(), 1.f,0.f,1.f,0.f)), // 7
                    }
                )
            }
        },
        glm::vec2(20.f, 26.f),
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
    
    this->quit_bg_txtr_selection_btn = Button(ELEMENT_STYLE_SOLID, glm::vec2(2,2), "", appTextures.X, 1.f,false);
    this->quit_bg_txtr_selection_btn.color = glm::vec4(0.f);
    this->quit_bg_txtr_selection_btn.color2 = glm::vec4(0.f);
    this->quit_bg_txtr_selection_btn.outlineExtra = false;

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

void DisplayerDialog::show(Timer& timer, Skybox &skybox){
    
    this->dialogControl.activate();
    
    while (!getContext()->window.shouldClose())
    {
        /* code */
        dialogControl.updateStart(true);

        //Render the panel
        panel.render(timer, !dialog_log.isHovered());
        
        //Update the texture of the skybox displayer button
        panel.sections[0].elements[0].button.texture = skybox.displayingTexture;
        panel.sections[0].elements[1].rangeBar.value = skybox.rotation;
        panel.sections[0].elements[2].rangeBar.value = skybox.lod * 25.f;
        panel.sections[0].elements[3].rangeBar.value = skybox.opacity * 100.f;
        panel.sections[0].elements[4].button.color = glm::vec4(skybox.bgColor, 1.f);
        panel.sections[0].elements[5].button.texture = skybox.bg_txtr;
        panel.sections[0].elements[7].rangeBar.value = skybox.bg_txtr_opacity;
        
        //Render the skybox buttons if the skybox displayer is pressed
        for (size_t i = 0; i < skyboxes.size(); i++)
        {
            //Positioning the buttons
            skyboxes[i].pos = panel.sections[0].elements[0].button.pos;
            skyboxes[i].pos.x -= panel.sections[0].elements[0].button.scale.x;
            skyboxes[i].pos.x += skyboxes[i].scale.x;
            skyboxes[i].pos.x += skyboxes[i].scale.x * 2 * i;
            skyboxes[i].pos.y -= panel.sections[0].elements[0].button.scale.y;
            skyboxes[i].pos.y -= skyboxes[i].scale.y;
            skyboxes[i].scale.x = panel.sections[0].elements[0].button.scale.x/skyboxes.size();

            //Render the button
            skyboxes[i].render(timer, !dialog_log.isHovered());
            
            //If pressed to the skybox load the pressed skybox
            if(skyboxes[i].button.clicked){
                skybox.load("./LigidPainter/Resources/Cubemap/Skybox/sky" + std::to_string(i+1));
                skybox.createPrefilterMap();
                skybox.createDisplayingTxtr();

                //Unpress the skybox displayer
                panel.sections[0].elements[0].button.clickState1 = false;
            }
        }
        
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

        timer.transition(this->bg_txtr_selection_mode, this->bg_txtr_selection_mode_mixVal, 0.3f);

        if(bg_txtr_selection_mode && !this->bg_txtr_selection_panel.hover && *Mouse::LClick()){
            bg_txtr_selection_mode = false;
        }

        if(!bg_txtr_selection_mode){
            this->bg_txtr_selection_panel.hover = false;
        }

        if(panel.sections[0].elements[6].button.clicked){
            bg_txtr_selection_mode = true;
        }

        if(this->bg_txtr_selection_mode_mixVal){
            ShaderSystem::buttonShader().setFloat("properties.groupOpacity", this->bg_txtr_selection_mode_mixVal);
            
            this->quit_bg_txtr_selection_btn.pos = glm::vec3(
                                                                this->bg_txtr_selection_panel.pos.x + this->bg_txtr_selection_panel.scale.x - 1.f, 
                                                                this->bg_txtr_selection_panel.pos.y - this->bg_txtr_selection_panel.scale.y + 1.f + 1.f, 
                                                                this->bg_txtr_selection_panel.pos.z
                                                            );

            this->quit_bg_txtr_selection_btn.render(timer, !dialog_log.isHovered());

            this->quit_bg_txtr_selection_btn.scale = glm::vec2(1.f) * (1.f + this->quit_bg_txtr_selection_btn.hoverMixVal / 5.f);

            if(this->quit_bg_txtr_selection_btn.clicked){
                this->bg_txtr_selection_mode = false;
            }

            this->bg_txtr_selection_panel.scale.x = this->bg_txtr_selection_mode_mixVal * 10.f;
            this->bg_txtr_selection_panel.pos = glm::vec3(this->panel.pos.x + this->panel.scale.x + this->bg_txtr_selection_panel.scale.x, this->panel.pos.y, this->panel.pos.z);
            this->bg_txtr_selection_panel.render(timer, !dialog_log.isHovered());

            for (size_t i = 1; i < this->bg_txtr_selection_panel.sections[0].elements.size(); i++)
            {
                if(this->bg_txtr_selection_panel.sections[0].elements[i].button.clicked){
                    panel.sections[0].elements[5].button.texture = this->bg_txtr_selection_panel.sections[0].elements[i].button.texture;
                    bg_txtr_selection_mode = false;
                }
            }
            

            if(this->bg_txtr_selection_panel.sections[0].elements[0].button.clicked){
                std::string test = showFileSystemObjectSelectionDialog("Select an image file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_TEXTURE, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);

                if(test.size()){
                    Texture uploadedTexture;
                    uploadedTexture.load(test.c_str());

                    this->bg_txtr_selection_panel.sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f, 4.f), uploadedTexture.title, uploadedTexture, 0.f, false));
                
                    UTIL::copyFileToFolder(test, "./LigidPainter/Resources/Images/Background", 1);
                }
            }

            ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);
        }
        

        skybox.displayingTexture = panel.sections[0].elements[0].button.texture;
        skybox.rotation = panel.sections[0].elements[1].rangeBar.value;
        skybox.lod = panel.sections[0].elements[2].rangeBar.value;
        skybox.opacity = panel.sections[0].elements[3].rangeBar.value;
        skybox.bgColor = panel.sections[0].elements[4].button.color;
        skybox.bg_txtr = panel.sections[0].elements[5].button.texture;
        skybox.bg_txtr_opacity = panel.sections[0].elements[7].rangeBar.value;
        skybox.update_transformMatrix();
        
        //End the dialog
        if((panel.sections[0].elements[2].button.clicked) || getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!panel.hover && !this->bg_txtr_selection_panel.hover && !dialog_log.isHovered() && *Mouse::LClick())){
            if(!dialogControl.firstFrameActivated){
                dialogControl.unActivate();
            }
        }

        dialogControl.updateEnd(timer,0.15f);

        if(dialogControl.mixVal == 0.f)
            break;
    }
    

}