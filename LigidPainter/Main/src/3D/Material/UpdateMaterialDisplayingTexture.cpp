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

void Material::updateMaterialDisplayingTexture(
                                                float textureRes,
                                                bool updateMaterial,
                                                Camera matCam,
                                                int displayingMode,
                                                bool useCustomCam,
                                                Mesh& displayMesh,
                                                MaterialChannels material_channels
                                            )
{
    updateMaterialDisplayingTexture(textureRes, updateMaterial, matCam, displayingMode, useCustomCam, displayMesh, material_channels, this->displayingTexture, -1);
}

void Material::updateMaterialDisplayingTexture(
                                                float textureRes,
                                                bool updateMaterial,
                                                Camera matCam,
                                                int displayingMode,
                                                bool useCustomCam,
                                                Mesh& displayMesh,
                                                MaterialChannels material_channels,
                                                Texture custom_display_txtr,
                                                int specificUpdateI
                                            )
{ 
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
    
    
    //!Update the material displaying texture
    Framebuffer FBO = FBOPOOL::requestFBO_with_RBO(custom_display_txtr, custom_display_txtr.getResolution(), "Material::updateMaterialDisplayingTexture");

    //Clear the capture framebuffer (displaying texture) with alpha zero color
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Use the 3D model rendering shader
    ShaderSystem::PBRDisplayOnly().use();
    ShaderSystem::PBRDisplayOnly().setMat4("view",view);
    ShaderSystem::PBRDisplayOnly().setMat4("projection",projectionMatrix);
    ShaderSystem::PBRDisplayOnly().setMat4("modelMatrix", glm::mat4(1.f));
    ShaderSystem::PBRDisplayOnly().setVec3("viewPos",matCam.cameraPos);
    
    {
        ShaderSystem::PBRDisplayOnly().setInt("skybox", GL::bindTexture_CubeMap(getScene()->skybox.ID, "Material::updateMaterialDisplayingTexture")); 
        ShaderSystem::PBRDisplayOnly().setInt("prefilterMap", GL::bindTexture_CubeMap(getScene()->skybox.IDPrefiltered, "Material::updateMaterialDisplayingTexture")); 
        ShaderSystem::PBRDisplayOnly().setInt("albedoTxtr", GL::bindTexture_2D((!this->materialModifiers.size()) ? appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.ID : material_channels.albedo.ID, "Material::updateMaterialDisplayingTexture"));
        ShaderSystem::PBRDisplayOnly().setInt("roughnessTxtr", GL::bindTexture_2D((!this->materialModifiers.size()) ? appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.ID : material_channels.roughness.ID, "Material::updateMaterialDisplayingTexture"));
        ShaderSystem::PBRDisplayOnly().setInt("metallicTxtr", GL::bindTexture_2D((!this->materialModifiers.size()) ? appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.ID : material_channels.metallic.ID, "Material::updateMaterialDisplayingTexture"));
        ShaderSystem::PBRDisplayOnly().setInt("normalMapTxtr", GL::bindTexture_2D((!this->materialModifiers.size()) ? appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.ID : material_channels.normalMap.ID, "Material::updateMaterialDisplayingTexture"));
        ShaderSystem::PBRDisplayOnly().setInt("heightMapTxtr", GL::bindTexture_2D((!this->materialModifiers.size()) ? appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.ID : material_channels.heightMap.ID, "Material::updateMaterialDisplayingTexture"));
        ShaderSystem::PBRDisplayOnly().setInt("ambientOcclusionTxtr", GL::bindTexture_2D((!this->materialModifiers.size()) ? appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.ID : material_channels.ambientOcclusion.ID, "Material::updateMaterialDisplayingTexture"));
        ShaderSystem::PBRDisplayOnly().setInt("displayingMode", displayingMode);
        
        //Draw the sphere
        displayMesh.Draw("Material::updateMaterialDisplayingTexture");
        
        GL::releaseBoundTextures("Material::updateMaterialDisplayingTexture");
    }
    
    ShaderSystem::PBRDisplayOnly().setInt("displayingMode", 0);
    ShaderUTIL::release_bound_shader();
    FBOPOOL::releaseFBO(FBO);
}