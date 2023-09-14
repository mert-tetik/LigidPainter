
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
        glm::vec2(28.f, 22.f),
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
    this->selectMeshPanel = Panel(
        {
            Section(
                Element(Button()),
                {
                    Button(ELEMENT_STYLE_SOLID, glm::vec2(15.f), "", Texture(), 6.f, false),
                    Button(ELEMENT_STYLE_SOLID, glm::vec2(4.f), "Mataryal_3", Texture(), 0.f, false)
                }
            )
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
    pbrRayTracingCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f,2.f), "Ray Tracing (Not Available)", 0.f);
    vertexPositionCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(5.f,2.f), "Vertex Position", 0.f);
    vertexNormalsCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(5.f,2.f), "Vertex Normals", 0.f);
    reflectanceCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(5.f,2.f), "Skybox Reflectance", 0.f);
    bakeButton = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(7.f,2), "Bake", Texture(), 1.f, false);

    meshSelectionText = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(7.f,2), "Mesh Selection", Texture(), 0.f, false);
    exportMaterialChannelsText = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(7.f,2), "Export Material Channels", Texture(), 0.f, false);
    bakingMaterialChannelsText = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(7.f,2), "Bake Material Channels", Texture(), 0.f, false);

    meshSelectionText.color = glm::vec4(0.f);
    exportMaterialChannelsText.color = glm::vec4(0.f);
    bakingMaterialChannelsText.color = glm::vec4(0.f);
    
    meshSelectionText.textScale = 0.65f;
    exportMaterialChannelsText.textScale = 0.65f;
    bakingMaterialChannelsText.textScale = 0.65f;

    selectMeshPanel.sections[0].elements[0].button.meshSelection = true;
}

void BakingDialog::render(Timer timer, Skybox skybox){
    
    dialogControl.updateStart();

    if(dialogControl.firstFrameActivated){
        selectMeshPanel.sections[0].elements[0].button.selectedMeshI = 0;
        if(selectMeshPanel.sections[0].elements[0].button.selectedMeshI < getModel()->meshes.size()){
            selectMeshPanel.sections[0].elements[0].button.texture = getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].displayingTxtr; 
            selectMeshPanel.sections[0].elements[0].button.text = getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].materialName; 
        }
    }

    selectMeshPanel.sections[0].elements[1].button.text = selectMeshPanel.sections[0].elements[0].button.text;

    //Render the panel
    this->bgPanel.render(timer, true);

    this->selectMeshPanel.pos = this->bgPanel.pos;
    this->selectMeshPanel.pos.x -= this->bgPanel.scale.x - this->exportChannelsIntoLibraryPanel.scale.x;
    this->selectMeshPanel.pos.z += 0.001f;
    this->selectMeshPanel.render(timer, true);
    
    this->exportChannelsIntoLibraryPanel.pos = this->selectMeshPanel.pos;
    this->exportChannelsIntoLibraryPanel.pos.x += this->selectMeshPanel.scale.x + this->exportChannelsIntoLibraryPanel.scale.x;
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

    this->meshSelectionText.pos = exportChannelsIntoLibraryPanel.pos;
    this->meshSelectionText.pos.y -= exportChannelsIntoLibraryPanel.scale.y - this->meshSelectionText.scale.y * 2.f;
    this->meshSelectionText.pos.x = this->selectMeshPanel.pos.x;
    this->meshSelectionText.render(timer, false);
    
    this->exportMaterialChannelsText.pos = exportChannelsIntoLibraryPanel.pos;
    this->exportMaterialChannelsText.pos.y -= exportChannelsIntoLibraryPanel.scale.y - this->exportMaterialChannelsText.scale.y * 2.f;
    this->exportMaterialChannelsText.render(timer, false);
    
    this->bakingMaterialChannelsText.pos = exportChannelsIntoLibraryPanel.pos;
    this->bakingMaterialChannelsText.pos.y -= exportChannelsIntoLibraryPanel.scale.y - this->bakingMaterialChannelsText.scale.y * 2.f;
    this->bakingMaterialChannelsText.pos.x = bakeButton.pos.x;
    this->bakingMaterialChannelsText.render(timer, false);
    
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
        
        this->pbrRayTracingCheckbox.render(timer, false);
        this->pbrRayTracingCheckbox.pos = this->pbrUseLightingCheckbox.pos;
        this->pbrRayTracingCheckbox.pos.y += this->pbrUseLightingCheckbox.scale.y + this->pbrRayTracingCheckbox.scale.y;
    }
    
    this->bakeButton.pos = this->exportChannelsIntoLibraryPanel.pos;
    this->bakeButton.pos.x = this->bgPanel.pos.x + this->exportChannelsIntoLibraryPanel.scale.x + this->selectMeshPanel.scale.x;
    this->bakeButton.pos.y += this->exportChannelsIntoLibraryPanel.scale.y - this->bakeButton.scale.y * 4.;
    this->bakeButton.render(timer, true);
    

    if(exportChannelsIntoLibraryPanel.sections[0].elements[exportChannelsIntoLibraryPanel.sections[0].elements.size()-1].button.clicked && selectMeshPanel.sections[0].elements[0].button.selectedMeshI < getModel()->meshes.size()){
        NodeScene::updateNodeResults(Settings::properties()->textureRes, -1);

        //Update all the materials connected to the mesh output & export it's textures
            //For all the channels
        for (size_t channelI = 0; channelI < 6; channelI++)
        {
            Texture channelTxtr;
            
            if(channelI == 0){
                channelTxtr = getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].albedo;
                channelTxtr.title = "albedo_" + getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].materialName;
            }
            if(channelI == 1){
                channelTxtr = getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].roughness;
                channelTxtr.title = "roughness_" + getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].materialName;
            }
            if(channelI == 2){
                channelTxtr = getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].metallic;
                channelTxtr.title = "metallic_" + getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].materialName;
            }
            if(channelI == 3){
                channelTxtr = getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].normalMap;
                channelTxtr.title = "normalMap_" + getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].materialName;
            }
            if(channelI == 4){
                channelTxtr = getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].heightMap;
                channelTxtr.title = "heightMap_" + getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].materialName;
            }
            if(channelI == 5){
                channelTxtr = getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].ambientOcclusion;
                channelTxtr.title = "ambientOcclusion_" + getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].materialName;
            }

            if(this->exportChannelsIntoLibraryPanel.sections[0].elements[channelI].checkBox.clickState1)
                Library::addTexture(channelTxtr.duplicateTexture());
        }
    }

    if(this->bakeButton.clicked && selectMeshPanel.sections[0].elements[0].button.selectedMeshI < getModel()->meshes.size()){
        
        Camera cam;

        //Move the camera to the side
        glm::mat4 view = glm::lookAt(cam.cameraPos, 
                                     glm::vec3(0), 
                                     glm::vec3(0.0, 1.0, 0.0));

        //The perspective projection matrix    
        glm::mat4 projectionMatrix = glm::perspective(
                                                        glm::radians(35.f), //Fov  
                                                        -1.f,  //Ratio (is 1 since the width & the height is equal to displayRes)
                                                        100.f,  //Near (the material is pretty close to the camera actually  ) 
                                                        0.1f    //Far
                                                    );

        ShaderSystem::bakingShader().use();
        ShaderSystem::bakingShader().setVec3("viewPos", cam.cameraPos); 
        ShaderSystem::bakingShader().setInt("skybox", 0); 
        ShaderSystem::bakingShader().setInt("prefilterMap", 1); 
        ShaderSystem::bakingShader().setInt("albedoTxtr", 2); 
        ShaderSystem::bakingShader().setInt("roughnessTxtr", 3); 
        ShaderSystem::bakingShader().setInt("metallicTxtr", 4); 
        ShaderSystem::bakingShader().setInt("normalMapTxtr", 5); 
        ShaderSystem::bakingShader().setInt("heightMapTxtr", 6); 
        ShaderSystem::bakingShader().setInt("ambientOcclusionTxtr", 7); 
        ShaderSystem::bakingShader().setInt("modeIndex", this->selectedBakeMode); 
        ShaderSystem::bakingShader().setInt("useLights", this->pbrUseLightingCheckbox.clickState1); 

        ShaderSystem::bakingShader().setMat4("orthoProjection", glm::ortho(0.f, 1.f, 0.f, 1.f)); 
        ShaderSystem::bakingShader().setMat4("perspectiveProjection", projectionMatrix); 
        ShaderSystem::bakingShader().setMat4("view", view); 


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.ID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.IDPrefiltered);

        Texture txtr = Texture(nullptr, Settings::properties()->textureRes, Settings::properties()->textureRes);
        txtr.title = "baked_" + getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].materialName;

        /* Capturing FBO */
        unsigned int FBO; 
        glGenFramebuffers(1,&FBO);
        glBindFramebuffer(GL_FRAMEBUFFER,FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, txtr.ID, 0);
        glClearColor(0,0,0,0);

        unsigned int RBO;
        glGenRenderbuffers(1,&RBO);
        glBindRenderbuffer(GL_RENDERBUFFER,RBO);
        
        //Set the renderbuffer to store depth
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, Settings::properties()->textureRes, Settings::properties()->textureRes);
        
        //Give the renderbuffer to the framebuffer
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glDepthFunc(GL_LEQUAL);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].albedo.ID);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].roughness.ID);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].metallic.ID);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].normalMap.ID);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].heightMap.ID);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].ambientOcclusion.ID);
        
        getModel()->meshes[selectMeshPanel.sections[0].elements[0].button.selectedMeshI].Draw();

        glDeleteFramebuffers(1, &FBO);
        glDeleteRenderbuffers(1, &RBO);

        Library::addTexture(txtr);
    }
    
    //End the dialog
    if(getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!bgPanel.hover && *Mouse::LClick())){
        if(!dialogControl.firstFrameActivated){
            dialogControl.unActivate();
        }
    }

    dialogControl.updateEnd(timer,0.15f);
}