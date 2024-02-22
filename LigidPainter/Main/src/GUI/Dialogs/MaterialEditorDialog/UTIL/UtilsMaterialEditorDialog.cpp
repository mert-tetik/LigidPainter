/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "MouseSystem/Mouse.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

void MaterialEditorDialog::activate(){
    selectedMaterialModifierIndex = 0;
    dialogControl.activate();
}

void MaterialEditorDialog::deactivate(TextureSelectionDialog &textureSelectionDialog){
    selectedMaterialModifierIndex = 0;
    dialogControl.unActivate();
    textureSelectionDialog.dialogControl.unActivate();
}

void MaterialEditorDialog::updateSkyboxTxtr(){
    //Move the camera to the side
    glm::mat4 view = glm::lookAt(this->displayerCamera.cameraPos, 
                                 glm::vec3(0), 
                                 glm::vec3(0.0, 1.0, 0.0));
    
    //The perspective projection matrix    
    glm::mat4 projectionMatrix = glm::perspective(
                                                    glm::radians(35.f), //Fov  
                                                    1.f,  //Ratio (is 1 since the width & the height is equal to displayRes)
                                                    0.1f,    //Near
                                                    100.f  //Far (the material is pretty close to the camera actually  ) 
                                                );

    glm::mat4 modelMatrix = glm::mat4(1.f);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(10.f));


    //Skybox shader
    ShaderSystem::skyboxModelShader().use();
    ShaderSystem::skyboxModelShader().setMat4("view", view);
    ShaderSystem::skyboxModelShader().setMat4("projection", projectionMatrix);
    ShaderSystem::skyboxModelShader().setMat4("modelMatrix", modelMatrix);
    ShaderSystem::skyboxModelShader().setFloat("lod", 0);
    ShaderSystem::skyboxModelShader().setInt("gradient", 1);
    ShaderSystem::skyboxModelShader().setFloat("gradientOffset", 0.8f);

    glm::ivec2 res = this->skyboxFBO.colorBuffer.getResolution();

    this->skyboxFBO.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,res.x,res.y);

    //Render the skybox
    getSphereModel()->Draw();

    ShaderSystem::buttonShader().use();
    getBox()->bindBuffers();
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
}

static Model customModel;

Model* MaterialEditorDialog::getDisplayModel(){
    if(this->selectedModelModeIndex == 0)
        return getPlaneModel();
    if(this->selectedModelModeIndex == 1)
        return getMaterialDisplayerModel();
    if(this->selectedModelModeIndex == 2){
        if(getModel()->meshes.size() != customModel.meshes.size()){
            customModel.meshes.clear();

            for (size_t i = 0; i < getModel()->meshes.size(); i++){
                customModel.meshes.push_back(Mesh({}, {1,2,3}, "", true));
            }
        }

        for (size_t i = 0; i < customModel.meshes.size(); i++)
        {
            customModel.meshes[i].indices = getModel()->meshes[i].indices;
            customModel.meshes[i].VBO = getModel()->meshes[i].VBO;
            customModel.meshes[i].EBO = getModel()->meshes[i].EBO;
            customModel.meshes[i].VAO = getModel()->meshes[i].VAO;
            customModel.meshes[i].objects = getModel()->meshes[i].objects;
            customModel.meshes[i].uvMask = getModel()->meshes[i].uvMask;
        }
        return &customModel;
    }

    return getMaterialDisplayerModel();
}

void MaterialEditorDialog::positioningPanels(){
    navPanel.scale.x = bgPanel.scale.x - shortcutPanel.scale.x - layerPanel.scale.x - modifiersPanel.scale.x;
    navPanel.pos = shortcutPanel.pos;
    navPanel.pos.x += shortcutPanel.scale.x + navPanel.scale.x;
    navPanel.pos.y = bgPanel.pos.y - bgPanel.scale.y + navPanel.scale.y;

    materialDisplayer.pos = navPanel.pos;
    materialDisplayer.scale.y = bgPanel.scale.y - navPanel.scale.y;
    materialDisplayer.scale.x = navPanel.scale.x;
    materialDisplayer.pos.y += navPanel.scale.y + materialDisplayer.scale.y;

    barButton.pos.x = bgPanel.pos.x;
    barButton.pos.y = bgPanel.pos.y - bgPanel.scale.y - barButton.scale.y;

    displayModeComboBox.scale.y = navPanel.scale.y - 0.1;
    displayTxtrResComboBox.scale.y = navPanel.scale.y - 0.1;
    matDisplayerBallModeBtn.scale.y = navPanel.scale.y - 0.1;
    twoDModelModeBtn.scale.y = navPanel.scale.y - 0.1;
    customModelModeBtn.scale.y = navPanel.scale.y - 0.1;
    selectedModifierResultMode.scale.y = navPanel.scale.y - 0.1;
    overallResultMode.scale.y = navPanel.scale.y - 0.1;

    matDisplayerBallModeBtn.pos = navPanel.pos;
    matDisplayerBallModeBtn.pos.x += displayModeComboBox.scale.x - selectedModifierResultMode.scale.x * 3.f;
    twoDModelModeBtn.pos = matDisplayerBallModeBtn.pos; 
    twoDModelModeBtn.pos.x -= matDisplayerBallModeBtn.scale.x + twoDModelModeBtn.scale.x; 
    customModelModeBtn.pos = matDisplayerBallModeBtn.pos; 
    customModelModeBtn.pos.x += matDisplayerBallModeBtn.scale.x + customModelModeBtn.scale.x; 

    selectedModifierResultMode.pos = navPanel.pos;
    selectedModifierResultMode.pos.x += navPanel.scale.x - selectedModifierResultMode.scale.x * 4.f;
    overallResultMode.pos = selectedModifierResultMode.pos; 
    overallResultMode.pos.x -= selectedModifierResultMode.scale.x + overallResultMode.scale.x; 

    layerPanel.pos.x = modifiersPanel.pos.x - modifiersPanel.scale.x - layerPanel.scale.x; 

    displayModeComboBox.scale.x = navPanel.scale.x / 5.f;
    displayModeComboBox.pos = navPanel.pos;
    displayModeComboBox.pos.x -= navPanel.scale.x - displayModeComboBox.scale.x * 1.5f;
    displayTxtrResComboBox.scale.x = displayModeComboBox.scale.x;
    displayTxtrResComboBox.pos = displayModeComboBox.pos;
    displayTxtrResComboBox.pos.x += displayModeComboBox.scale.x + displayTxtrResComboBox.scale.x;
}

void MaterialEditorDialog::renderSkyboxTxtr(glm::mat4 projection){
    ShaderSystem::textureRenderingShader().use();
    ShaderSystem::textureRenderingShader().setMat4("projection", projection);
    ShaderSystem::textureRenderingShader().setVec2("scale", this->materialDisplayer.resultScale);
    ShaderSystem::textureRenderingShader().setVec3("pos", this->materialDisplayer.resultPos);
    ShaderSystem::textureRenderingShader().setInt("txtr", 0);
    ShaderSystem::textureRenderingShader().setFloat("opacity", this->dialogControl.mixVal);
    ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
    ShaderSystem::textureRenderingShader().setFloat("depthToleranceValue", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->skyboxFBO.colorBuffer.ID);

    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Material editor dialog : Render skybox");

    ShaderSystem::buttonShader().use();

    if(*Mouse::RPressed() || dialogControl.firstFrameActivated){
        this->updateSkyboxTxtr();
    }
}

void MaterialEditorDialog::renderZoomIndicator(Timer& timer, bool mouseTrackingFlag){
    if(this->displayerCamera.radius > 3.5f)
        this->displayerCamera.setCameraRadius(3.5f);
    if(this->displayerCamera.radius < 0.f)
        this->displayerCamera.setCameraRadius(0.f);

    bool displayZoomPanel = this->displayerCamera.radius != 3.5f;
    timer.transition(displayZoomPanel, this->zoomPanelMixVal, 0.2f);

    if(this->zoomPanelMixVal){
        zoomPanel.sections[0].elements[0].button.text = std::to_string(this->displayerCamera.radius);
        if(zoomPanel.sections[0].elements[0].button.text.size() > 3)
            zoomPanel.sections[0].elements[0].button.text.erase(zoomPanel.sections[0].elements[0].button.text.begin() + 3, zoomPanel.sections[0].elements[0].button.text.end());
        
        glClear(GL_DEPTH_BUFFER_BIT);

        zoomPanel.sections[0].elements[0].button.text += " z";
        zoomPanel.pos = shortcutPanel.pos;
        zoomPanel.pos.y = navPanel.pos.y;
        
        zoomPanel.scale.x = this->zoomPanelMixVal * 6.f;
        zoomPanel.sections[0].elements[0].scale.x = this->zoomPanelMixVal * 2.f;
        zoomPanel.sections[0].elements[1].scale.x = this->zoomPanelMixVal * 4.f;
        
        zoomPanel.pos.y += navPanel.scale.y + zoomPanel.scale.y;
        zoomPanel.pos.x += shortcutPanel.scale.x + zoomPanel.scale.x;
        zoomPanel.render(timer, mouseTrackingFlag);
    
        if(zoomPanel.sections[0].elements[1].button.clicked)
            this->displayerCamera.setCameraRadius(3.5f);
    }
}

void MaterialEditorDialog::renderNavPanel(Timer& timer, bool mouseTrackingFlag){
    
    // Render the elements
    this->twoDModelModeBtn.render(timer, mouseTrackingFlag);
    this->matDisplayerBallModeBtn.render(timer, mouseTrackingFlag);
    this->customModelModeBtn.render(timer, mouseTrackingFlag);
    this->overallResultMode.render(timer, mouseTrackingFlag);
    this->selectedModifierResultMode.render(timer, mouseTrackingFlag);
    this->displayModeComboBox.render(timer, true);
    this->displayTxtrResComboBox.render(timer, true);

    // Interactions
    if(twoDModelModeBtn.clicked){
        this->selectedModelModeIndex = 0;
        this->updateTheMaterial = true;
    }
    if(this->selectedModelModeIndex == 0)
        twoDModelModeBtn.color = ColorPalette::thirdColor;
    else
        twoDModelModeBtn.color = ColorPalette::secondColor;

    if(matDisplayerBallModeBtn.clicked){
        this->selectedModelModeIndex = 1;
        this->updateTheMaterial = true;
    }
    if(this->selectedModelModeIndex == 1)
        matDisplayerBallModeBtn.color = ColorPalette::thirdColor;
    else
        matDisplayerBallModeBtn.color = ColorPalette::secondColor;

    if(customModelModeBtn.clicked){
        this->selectedModelModeIndex = 2;
        this->updateTheMaterial = true;
    }
    if(this->selectedModelModeIndex == 2)
        customModelModeBtn.color = ColorPalette::thirdColor;
    else
        customModelModeBtn.color = ColorPalette::secondColor;

    if(overallResultMode.clicked){
        this->selectedResultModeIndex = 0;
        this->updateTheMaterial = true;
    }
    if(this->selectedResultModeIndex == 0)
        overallResultMode.color = ColorPalette::thirdColor;
    else
        overallResultMode.color = ColorPalette::secondColor;

    if(selectedModifierResultMode.clicked){
        this->selectedResultModeIndex = 1;
        this->updateTheMaterial = true;
    }
    if(this->selectedResultModeIndex == 1)
        selectedModifierResultMode.color = ColorPalette::thirdColor;
    else
        selectedModifierResultMode.color = ColorPalette::secondColor;


    if(dialogControl.firstFrameActivated){
        this->selectedResultModeIndex = 0;
    }
}