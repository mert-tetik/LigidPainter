/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

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
                customModel.meshes.push_back(Mesh(getModel()->meshes[i].vertices, getModel()->meshes[i].indices, ""));
            }
        }

        return &customModel;
    }

    return getMaterialDisplayerModel();
}