
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
#include "NodeSystem/Node/Node.hpp"
#include "LibrarySystem/Library.hpp"

#include <string>
#include <iostream>
#include <vector>

#include "tinyfiledialogs.h"

BakingDialog::BakingDialog(){

    //Create the panel
    this->bgPanel = Panel(
        {
            {
                Section(
                    Element(Button()),
                    {
                    }
                )
            }
        },
        glm::vec2(20.f, 22.f),
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
   
    this->exportChannelsIntoLibraryPanel = Panel(
        {
            {
                Section(
                    Element(Button()),
                    {
                        CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(2,2), "Albedo Channel", 7.f),
                        CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(2,2), "Roughness Channel", 0.f),
                        CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(2,2), "Metallic Channel", 0.f),
                        CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(2,2), "Normal Map Channel", 0.f),
                        CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(2,2), "Height Map Channel", 0.f),
                        CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(2,2), "Ambient Occlusion Channel", 0.f),
                        Element(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(2,2), "Export Into The Library", Texture(), 3.f, false))
                    }
                )
            }
        },
        glm::vec2(8.f,22.f),
        glm::vec3(50.f,50.f,0.8f),
        ColorPalette::secondColor,
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

    for (size_t i = 0; i < 6; i++)
    {
        this->exportChannelsIntoLibraryPanel.sections[0].elements[i].checkBox.clickState1 = true;
    }
    
    pbrResultCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(5.f,2.f), "Pbr Result", 0.f);
    pbrUseLightingCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(5.f,2.f), "Pbr Use Lighting", 0.f);
    pbrRayTracingCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(5.f,2.f), "Pbr Ray Tracing", 0.f);
    vertexPositionCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(5.f,2.f), "Vertex Position", 0.f);
    vertexNormalsCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(5.f,2.f), "Vertex Normals", 0.f);
    reflectanceCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(5.f,2.f), "Skybox Reflectance", 0.f);
    bakeButton = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(7.f,2), "Bake", Texture(), 1.f, false);
}

void BakingDialog::render(Timer timer){
    
    dialogControl.updateStart();

    //Render the panel
    this->bgPanel.render(timer, true);

    this->exportChannelsIntoLibraryPanel.pos = this->bgPanel.pos;
    this->exportChannelsIntoLibraryPanel.pos.x -= this->bgPanel.scale.x - this->exportChannelsIntoLibraryPanel.scale.x;
    this->exportChannelsIntoLibraryPanel.pos.z += 0.001f;
    this->exportChannelsIntoLibraryPanel.render(timer, true);

    this->vertexPositionCheckbox.render(timer, true);
    this->vertexPositionCheckbox.pos = this->exportChannelsIntoLibraryPanel.pos;
    this->vertexPositionCheckbox.pos.y -= this->exportChannelsIntoLibraryPanel.scale.x;
    this->vertexPositionCheckbox.pos.x += this->exportChannelsIntoLibraryPanel.scale.x + this->vertexPositionCheckbox.scale.x;
    
    this->vertexNormalsCheckbox.render(timer, true);
    this->vertexNormalsCheckbox.pos = this->vertexPositionCheckbox.pos;
    this->vertexNormalsCheckbox.pos.y += this->vertexPositionCheckbox.scale.y + this->vertexNormalsCheckbox.scale.y;
    
    this->reflectanceCheckbox.render(timer, true);
    this->reflectanceCheckbox.pos = this->vertexNormalsCheckbox.pos;
    this->reflectanceCheckbox.pos.y += this->vertexNormalsCheckbox.scale.y + this->reflectanceCheckbox.scale.y;

    this->pbrResultCheckbox.render(timer, true);
    this->pbrResultCheckbox.pos = this->reflectanceCheckbox.pos;
    this->pbrResultCheckbox.pos.y += this->reflectanceCheckbox.scale.y + this->pbrResultCheckbox.scale.y;
    
    std::vector<CheckBox*> checkBoxes; 
    checkBoxes.push_back(&vertexPositionCheckbox);
    checkBoxes.push_back(&vertexNormalsCheckbox);
    checkBoxes.push_back(&reflectanceCheckbox);
    checkBoxes.push_back(&pbrResultCheckbox);

    for (size_t i = 0; i < checkBoxes.size(); i++){
        if(checkBoxes[i]->clickState1 && i != this->selectedBakeMode){
            this->selectedBakeMode = i;
            for (size_t j = 0; j < checkBoxes.size(); j++)
            {
                checkBoxes[j]->clickState1 = false;
            }
        }
    }

    for (size_t i = 0; i < checkBoxes.size(); i++)
    {
        if(checkBoxes[i]->clickState1 == false && i == this->selectedBakeMode)
            checkBoxes[i]->clickState1 = true;
    }
    

    if(this->pbrResultCheckbox.clickState1){
        this->pbrUseLightingCheckbox.render(timer, true);
        this->pbrUseLightingCheckbox.pos = this->pbrResultCheckbox.pos;
        this->pbrUseLightingCheckbox.pos.x += this->pbrResultCheckbox.scale.x + this->pbrUseLightingCheckbox.scale.x;
        
        this->pbrRayTracingCheckbox.render(timer, true);
        this->pbrRayTracingCheckbox.pos = this->pbrUseLightingCheckbox.pos;
        this->pbrRayTracingCheckbox.pos.y += this->pbrUseLightingCheckbox.scale.y + this->pbrRayTracingCheckbox.scale.y;
    }
    
    this->bakeButton.pos = this->exportChannelsIntoLibraryPanel.pos;
    this->bakeButton.pos.x = this->bgPanel.pos.x + this->exportChannelsIntoLibraryPanel.scale.x;
    this->bakeButton.pos.y += this->exportChannelsIntoLibraryPanel.scale.y - this->bakeButton.scale.y * 4.;
    this->bakeButton.render(timer, true);
    

    if(exportChannelsIntoLibraryPanel.sections[0].elements[exportChannelsIntoLibraryPanel.sections[0].elements.size()-1].button.clicked){
        NodeScene::updateNodeResults(Settings::properties()->textureRes, -1);

        //Update all the materials connected to the mesh output & export it's textures
        for (size_t i = 0; i < getModel()->meshes.size(); i++)
        {
            //For all the channels
            for (size_t channelI = 0; channelI < 6; channelI++)
            {
                Texture channelTxtr;
                
                if(channelI == 0){
                    channelTxtr = getModel()->meshes[i].albedo;
                    channelTxtr.title = "albedo";
                }
                if(channelI == 1){
                    channelTxtr = getModel()->meshes[i].roughness;
                    channelTxtr.title = "roughness";
                }
                if(channelI == 2){
                    channelTxtr = getModel()->meshes[i].metallic;
                    channelTxtr.title = "metallic";
                }
                if(channelI == 3){
                    channelTxtr = getModel()->meshes[i].normalMap;
                    channelTxtr.title = "normalMap";
                }
                if(channelI == 4){
                    channelTxtr = getModel()->meshes[i].heightMap;
                    channelTxtr.title = "heightMap";
                }
                if(channelI == 5){
                    channelTxtr = getModel()->meshes[i].ambientOcclusion;
                    channelTxtr.title = "ambientOcclusion";
                }

                if(this->exportChannelsIntoLibraryPanel.sections[0].elements[channelI].checkBox.clickState1)
                    Library::addTexture(channelTxtr.duplicateTexture());
            }
        }
    }
    
    //End the dialog
    if(getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!bgPanel.hover && *Mouse::LClick())){
        if(!dialogControl.firstFrameActivated){
            dialogControl.unActivate();
        }
    }

    dialogControl.updateEnd(timer,0.15f);
}