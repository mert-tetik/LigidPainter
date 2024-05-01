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
#include "UTIL/Threads/Threads.hpp"
#include "UTIL/GL/GL.hpp"

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

    material_displaying_FBO.setColorBuffer(this->displayingTexture, GL_TEXTURE_2D);

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
    Shader initial_bound_shader = ShaderUTIL::get_bound_shader();

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
            //material_thread.apply_material(this, &displayModel, meshI);
            //this->apply_material(displayModel, displayModel.meshes[meshI], textureRes, specificUpdateI != -1);
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
    
    for (size_t i = 0; i < displayModel.meshes.size(); i++)
    {
        ShaderSystem::PBRDisplayOnly().setInt("skybox", GL::bindTexture_CubeMap(getScene()->skybox.ID, "Material::updateMaterialDisplayingTexture")); 
        ShaderSystem::PBRDisplayOnly().setInt("prefilterMap", GL::bindTexture_CubeMap(getScene()->skybox.IDPrefiltered, "Material::updateMaterialDisplayingTexture")); 
        ShaderSystem::PBRDisplayOnly().setInt("albedoTxtr", GL::bindTexture_2D((!this->materialModifiers.size()) ? appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.ID : displayModel.meshes[i].albedo.ID, "Material::updateMaterialDisplayingTexture"));
        ShaderSystem::PBRDisplayOnly().setInt("roughnessTxtr", GL::bindTexture_2D((!this->materialModifiers.size()) ? appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.ID : displayModel.meshes[i].roughness.ID, "Material::updateMaterialDisplayingTexture"));
        ShaderSystem::PBRDisplayOnly().setInt("metallicTxtr", GL::bindTexture_2D((!this->materialModifiers.size()) ? appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.ID : displayModel.meshes[i].metallic.ID, "Material::updateMaterialDisplayingTexture"));
        ShaderSystem::PBRDisplayOnly().setInt("normalMapTxtr", GL::bindTexture_2D((!this->materialModifiers.size()) ? appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.ID : displayModel.meshes[i].normalMap.ID, "Material::updateMaterialDisplayingTexture"));
        ShaderSystem::PBRDisplayOnly().setInt("heightMapTxtr", GL::bindTexture_2D((!this->materialModifiers.size()) ? appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.ID : displayModel.meshes[i].heightMap.ID, "Material::updateMaterialDisplayingTexture"));
        ShaderSystem::PBRDisplayOnly().setInt("ambientOcclusionTxtr", GL::bindTexture_2D((!this->materialModifiers.size()) ? appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.ID : displayModel.meshes[i].ambientOcclusion.ID, "Material::updateMaterialDisplayingTexture"));
        ShaderSystem::PBRDisplayOnly().setInt("displayingMode", displayingMode);
        
        //Draw the sphere
        displayModel.meshes[i].Draw();
        
        GL::releaseBoundTextures("Material::updateMaterialDisplayingTexture");
    }
    
    ShaderSystem::PBRDisplayOnly().setInt("displayingMode", 0);
    initial_bound_shader.use();
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
    getBox()->bindBuffers();
}