
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
        glm::vec2(24.f, 40.f),
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
   
    this->selectMeshButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(10.f, 15.f), "", Texture(), 6.f, false);
    this->selectMeshButton.meshSelection = true;
    this->selectMeshButton.textureSizeScale = 1.f;

    this->selectMeshText = Button(ELEMENT_STYLE_BASIC, glm::vec2(bgPanel.scale.x - selectMeshButton.scale.x - 1.f, 3.f), "Select The Mesh : ", Texture(), 6.f, false);
    this->selectMeshText.color = glm::vec4(0.);
    this->selectMeshText.outline = false;
    this->selectMeshText.outlineExtra = false;
    this->selectMeshText.textScale = 1.2f;
    
    this->bakingDialogText = Button(ELEMENT_STYLE_BASIC, glm::vec2(bgPanel.scale.x - selectMeshButton.scale.x - 1.f, 3.f), "Baking Dialog", Texture(), 6.f, false);
    this->bakingDialogText.color = glm::vec4(0.);
    this->bakingDialogText.outline = false;
    this->bakingDialogText.outlineExtra = false;
    this->bakingDialogText.textScale = 1.4f;
    
    this->bakingSettingsText = Button(ELEMENT_STYLE_BASIC, glm::vec2(bgPanel.scale.x - selectMeshButton.scale.x - 1.f, 3.f), "Baking Settings", Texture(), 6.f, false);
    this->bakingSettingsText.color = glm::vec4(0.);
    this->bakingSettingsText.outline = false;
    this->bakingSettingsText.outlineExtra = false;
    this->bakingSettingsText.textScale = 1.2f;
    
    this->selectedMeshTitleText = Button(ELEMENT_STYLE_BASIC, glm::vec2(bgPanel.scale.x - selectMeshButton.scale.x - 1.f, 2.f), "", Texture(), 6.f, false);
    this->selectedMeshTitleText.color = glm::vec4(0.);
    this->selectedMeshTitleText.outline = false;
    this->selectedMeshTitleText.outlineExtra = false;
    this->selectedMeshTitleText.textScale = 0.7f;
    
    pbrResultCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(5.f,2.f), "Pbr Result", 0.f);
    pbrUseLightingCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(5.f,2.f), "Pbr Use Lighting", 0.f);
    pbrRayTracingCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f,2.f), "Ray Tracing (Not Available)", 0.f);
    vertexPositionCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(5.f,2.f), "Vertex Position", 0.f);
    vertexNormalsCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(5.f,2.f), "Vertex Normals", 0.f);
    reflectanceCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(5.f,2.f), "Skybox Reflectance", 0.f);
    bakeButton = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(7.f,2), "Bake", Texture(), 1.f, false);
}

void BakingDialog::render(Timer timer, Skybox skybox){
    
    dialogControl.updateStart();

    if(dialogControl.firstFrameActivated){
        selectMeshButton.selectedMeshI = 0;
        if(selectMeshButton.selectedMeshI < getModel()->meshes.size()){
            selectMeshButton.texture = getModel()->meshes[selectMeshButton.selectedMeshI].displayingTxtr; 
            selectMeshButton.text = getModel()->meshes[selectMeshButton.selectedMeshI].materialName; 
        }
    }

    //Render the panel
    this->bgPanel.render(timer, true);
    
    this->bakingDialogText.pos = this->bgPanel.pos;
    this->bakingDialogText.pos.y -= this->bgPanel.scale.y - this->bakingDialogText.scale.y - 6.f;
    this->bakingDialogText.render(timer, false);
    
    this->selectMeshButton.pos = this->bgPanel.pos;
    this->selectMeshButton.pos.x += this->bgPanel.scale.x - this->selectMeshButton.scale.x - 2.f;
    this->selectMeshButton.pos.y -= 10.f;
    this->selectMeshButton.pos.z += 0.001f;
    
    this->selectMeshText.pos = selectMeshButton.pos;
    this->selectMeshText.pos.x -= this->selectMeshText.scale.x + this->selectMeshButton.scale.x ;
    this->selectMeshText.pos.y -= this->selectMeshButton.scale.y - selectMeshText.scale.y - 8.f;
    this->selectMeshText.render(timer, false);
    
    this->selectedMeshTitleText.pos = selectMeshText.pos;
    this->selectedMeshTitleText.pos.y += this->selectMeshText.scale.y + selectedMeshTitleText.scale.y;
    if(selectMeshButton.selectedMeshI < getModel()->meshes.size())
        this->selectedMeshTitleText.text = "Selected Mesh : " + getModel()->meshes[selectMeshButton.selectedMeshI].materialName; 
    this->selectedMeshTitleText.render(timer, false);
    
    this->selectMeshButton.render(timer, true);
    
    this->bakingSettingsText.pos = this->bgPanel.pos;
    this->bakingSettingsText.pos.y += 15.f;
    this->bakingSettingsText.render(timer, false);
    
    this->vertexPositionCheckbox.pos = this->bgPanel.pos;
    this->vertexPositionCheckbox.pos.x -= this->bgPanel.scale.x - this->vertexPositionCheckbox.scale.x - 4.f;
    this->vertexPositionCheckbox.pos.y += 21.f;
    this->vertexPositionCheckbox.render(timer, true);
    
    this->vertexNormalsCheckbox.render(timer, true);
    this->vertexNormalsCheckbox.pos = this->vertexPositionCheckbox.pos;
    this->vertexNormalsCheckbox.pos.x += this->vertexPositionCheckbox.scale.x + this->vertexNormalsCheckbox.scale.x;
    
    this->reflectanceCheckbox.render(timer, true);
    this->reflectanceCheckbox.pos = this->vertexNormalsCheckbox.pos;
    this->reflectanceCheckbox.pos.x += this->vertexNormalsCheckbox.scale.x + this->reflectanceCheckbox.scale.x;

    this->pbrResultCheckbox.render(timer, true);
    this->pbrResultCheckbox.pos = this->reflectanceCheckbox.pos;
    this->pbrResultCheckbox.pos.x += this->reflectanceCheckbox.scale.x + this->pbrResultCheckbox.scale.x;

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
        this->pbrUseLightingCheckbox.pos.y += this->pbrResultCheckbox.scale.y + this->pbrUseLightingCheckbox.scale.y;
        
        this->pbrRayTracingCheckbox.render(timer, false);
        this->pbrRayTracingCheckbox.pos = this->pbrUseLightingCheckbox.pos;
        this->pbrRayTracingCheckbox.pos.y += this->pbrUseLightingCheckbox.scale.y + this->pbrRayTracingCheckbox.scale.y;
    }
    
    this->bakeButton.pos = this->bgPanel.pos;
    this->bakeButton.pos.y += this->bgPanel.scale.y - this->bakeButton.scale.y - 6.f;
    this->bakeButton.render(timer, true);

    if(this->bakeButton.clicked && selectMeshButton.selectedMeshI < getModel()->meshes.size()){
        
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
        txtr.title = "baked_" + getModel()->meshes[selectMeshButton.selectedMeshI].materialName;

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
        glBindTexture(GL_TEXTURE_2D, getModel()->meshes[selectMeshButton.selectedMeshI].albedo.ID);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, getModel()->meshes[selectMeshButton.selectedMeshI].roughness.ID);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, getModel()->meshes[selectMeshButton.selectedMeshI].metallic.ID);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, getModel()->meshes[selectMeshButton.selectedMeshI].normalMap.ID);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, getModel()->meshes[selectMeshButton.selectedMeshI].heightMap.ID);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, getModel()->meshes[selectMeshButton.selectedMeshI].ambientOcclusion.ID);
        
        getModel()->meshes[selectMeshButton.selectedMeshI].Draw(false);

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