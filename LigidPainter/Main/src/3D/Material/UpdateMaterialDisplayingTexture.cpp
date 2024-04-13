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
#include <glm/gtx/string_cast.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Settings/Settings.hpp"

#include <string>
#include <iostream>
#include <vector>

static Framebuffer material_displaying_FBO;
void Material::updateMaterialDisplayingTexture(
                                                float textureRes,
                                                bool updateMaterial,
                                                Camera matCam,
                                                int displayingMode,
                                                bool useCustomCam
                                            )
{
    // Init displaying capture framebuffer
    if(!material_displaying_FBO.ID)
        material_displaying_FBO = Framebuffer(this->displayingTexture, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(128)), "Material displaying fbo");

    updateMaterialDisplayingTexture(textureRes, updateMaterial, matCam, displayingMode, useCustomCam, material_displaying_FBO, *getMaterialDisplayerModel(), -1);
}

void Material::updateMaterialDisplayingTexture(
                                                float textureRes,
                                                bool updateMaterial,
                                                Camera matCam,
                                                int displayingMode,
                                                bool useCustomCam,
                                                Framebuffer customFBO,
                                                Model& displayModel,
                                                int specificUpdateI
                                            )
{ 

    //Resolution of the material displaying texture
    const int displayRes = customFBO.colorBuffer.getResolution().x;

    if(!useCustomCam){
        matCam.cameraPos = glm::vec3(0,0,3.5f);
        matCam.radius = -3.5f;
    }

    //Move the camera to the side
    glm::mat4 view = glm::lookAt(matCam.cameraPos, 
                                 glm::vec3(0), 
                                 glm::vec3(0.0, 1.0, 0.0));
    
    //The perspective projection matrix    
    glm::mat4 projectionMatrix = glm::perspective(
                                                    glm::radians(35.f), //Fov  
                                                    1.f,  //Ratio (is 1 since the width & the height is equal to displayRes)
                                                    0.1f,    //Near
                                                    100.f  //Far (the material is pretty close to the camera actually  ) 
                                                );
    
    //Generates the material channels from scratch if the updateMaterial boolean set to true 
    if(updateMaterial)
    {
        //For every modifier the material has (Output every modifier the material has)
        //TODO : Material - update material function
        for (size_t meshI = 0; meshI < displayModel.meshes.size(); meshI++)
        {
            glm::ivec2 albedoRes = displayModel.meshes[meshI].albedo.getResolution(); 
            displayModel.meshes[meshI].albedo.update(nullptr, albedoRes.x, albedoRes.y);

            glm::ivec2 roughnessRes = displayModel.meshes[meshI].roughness.getResolution(); 
            displayModel.meshes[meshI].roughness.update(nullptr, roughnessRes.x, roughnessRes.y);

            glm::ivec2 metallicRes = displayModel.meshes[meshI].metallic.getResolution(); 
            displayModel.meshes[meshI].metallic.update(nullptr, metallicRes.x, metallicRes.y);

            glm::ivec2 normalMapRes = displayModel.meshes[meshI].normalMap.getResolution(); 
            displayModel.meshes[meshI].normalMap.update(nullptr, normalMapRes.x, normalMapRes.y);

            glm::ivec2 heightMapRes = displayModel.meshes[meshI].heightMap.getResolution(); 
            displayModel.meshes[meshI].heightMap.update(nullptr, heightMapRes.x, heightMapRes.y);

            glm::ivec2 ambientOcclusionRes = displayModel.meshes[meshI].ambientOcclusion.getResolution(); 
            displayModel.meshes[meshI].ambientOcclusion.update(nullptr, ambientOcclusionRes.x, ambientOcclusionRes.y);

            this->apply_material(displayModel, displayModel.meshes[meshI], textureRes, specificUpdateI != -1);
        }
    }
    
    //!Update the material displaying texture
    customFBO.bind();

    //Set the OpenGL viewport to the resolution of the material displaying texture
    glViewport(0,0,displayRes,displayRes);

    //Clear the capture framebuffer (displaying texture) with alpha zero color
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Use the 3D model rendering shader
    ShaderSystem::PBRDisplayOnly().use();
    ShaderSystem::PBRDisplayOnly().setMat4("view",view);
    ShaderSystem::PBRDisplayOnly().setMat4("projection",projectionMatrix);
    ShaderSystem::PBRDisplayOnly().setMat4("modelMatrix", glm::mat4(1.f));
    
    ShaderSystem::PBRDisplayOnly().setVec3("viewPos",matCam.cameraPos);
    ShaderSystem::PBRDisplayOnly().setInt("skybox", 0); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, getScene()->skybox.ID);
    ShaderSystem::PBRDisplayOnly().setInt("prefilterMap", 1); glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, getScene()->skybox.IDPrefiltered);
    ShaderSystem::PBRDisplayOnly().setInt("albedoTxtr", 2);
    ShaderSystem::PBRDisplayOnly().setInt("roughnessTxtr", 3);
    ShaderSystem::PBRDisplayOnly().setInt("metallicTxtr", 4);
    ShaderSystem::PBRDisplayOnly().setInt("normalMapTxtr", 5);
    ShaderSystem::PBRDisplayOnly().setInt("heightMapTxtr", 6);
    ShaderSystem::PBRDisplayOnly().setInt("ambientOcclusionTxtr", 7);
    ShaderSystem::PBRDisplayOnly().setInt("displayingMode", displayingMode);
    
    for (size_t i = 0; i < displayModel.meshes.size(); i++)
    {
        //Bind the channels of the material
        if(!this->materialModifiers.size()){
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.ID);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.ID);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.ID);
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.ID);
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.ID);
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.ID);
        }
        else{
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, displayModel.meshes[i].albedo.ID);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, displayModel.meshes[i].roughness.ID);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, displayModel.meshes[i].metallic.ID);
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, displayModel.meshes[i].normalMap.ID);
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, displayModel.meshes[i].heightMap.ID);
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, displayModel.meshes[i].ambientOcclusion.ID);
        }
        
        //Draw the sphere
        displayModel.meshes[i].Draw();
    }
    
    
    ShaderSystem::tdModelShader().setInt("displayingMode", 0);
    
    //!Finish (prepare rendering the GUI)

    //Use the button shader (Is necessary since that process is done in the middle of GUI rendering) 
    ShaderSystem::buttonShader().use();

    //Bind the default framebuffer
    Settings::defaultFramebuffer()->FBO.bind();
    
    //Set the OpenGL viewport to default
    Settings::defaultFramebuffer()->setViewport();

    getBox()->bindBuffers();
}