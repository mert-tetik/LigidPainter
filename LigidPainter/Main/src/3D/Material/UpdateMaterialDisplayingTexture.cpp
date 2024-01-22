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
#include "ShaderSystem/Shader.hpp"
#include "SettingsSystem/Settings.hpp"

#include <string>
#include <iostream>
#include <vector>

void Material::updateMaterialDisplayingTexture(
                                                float textureRes,
                                                bool updateMaterial,
                                                Camera matCam,
                                                int displayingMode,
                                                bool useCustomCam
                                            )
{
    updateMaterialDisplayingTexture(textureRes, updateMaterial, matCam, displayingMode, useCustomCam, this->displayingFBO, *getMaterialDisplayerModel(), -1);
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
    Debugger::block("updateMaterialDisplayingTexture : 1474897489"); // End

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
    Debugger::block("updateMaterialDisplayingTexture : 1474897489"); // End
    
    Debugger::block("updateMaterialDisplayingTexture : 1238899889"); // End
    //Generates the material channels from scratch if the updateMaterial boolean set to true 
    if(updateMaterial)
    {
        Debugger::block("updateMaterialDisplayingTexture : 8989894512"); // End
        //For every modifier the material has (Output every modifier the material has)
        //TODO : Material - update material function
        for (size_t meshI = 0; meshI < displayModel.meshes.size(); meshI++)
        {
            Debugger::block("updateMaterialDisplayingTexture : 654684321312"); // End
            glm::ivec2 albedoRes = displayModel.meshes[meshI].albedo.getResolution(); 
            Debugger::block("updateMaterialDisplayingTexture : 654684321312"); // End
            
            Debugger::block("updateMaterialDisplayingTexture : 9846415311"); // End
            displayModel.meshes[meshI].albedo.update(nullptr, albedoRes.x, albedoRes.y);
            displayModel.meshes[meshI].roughness.update(nullptr, albedoRes.x, albedoRes.y);
            displayModel.meshes[meshI].metallic.update(nullptr, albedoRes.x, albedoRes.y);
            displayModel.meshes[meshI].normalMap.update(nullptr, albedoRes.x, albedoRes.y);
            displayModel.meshes[meshI].heightMap.update(nullptr, albedoRes.x, albedoRes.y);
            displayModel.meshes[meshI].ambientOcclusion.update(nullptr, albedoRes.x, albedoRes.y);
            Debugger::block("updateMaterialDisplayingTexture : 9846415311"); // End

            Debugger::block("updateMaterialDisplayingTexture : 888444222254984"); // End
            for (int i = this->materialModifiers.size() - 1; i >= 0; --i)    
            {
                if(i == specificUpdateI || specificUpdateI == -1)
                    this->materialModifiers[i].updateMaterialChannels(*this, displayModel.meshes[meshI], textureRes, i, appTextures.white, 0, specificUpdateI != -1, displayModel);
            }
            Debugger::block("updateMaterialDisplayingTexture : 888444222254984"); // End
        }
        Debugger::block("updateMaterialDisplayingTexture : 8989894512"); // End
    }
    Debugger::block("updateMaterialDisplayingTexture : 1238899889"); // End
    
    Debugger::block("updateMaterialDisplayingTexture : 8979846516231"); // End
    //!Update the material displaying texture
    customFBO.bind();

    //Set the OpenGL viewport to the resolution of the material displaying texture
    glViewport(0,0,displayRes,displayRes);

    //Clear the capture framebuffer (displaying texture) with alpha zero color
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Use the 3D model rendering shader
    ShaderSystem::tdModelShader().use();
    Debugger::block("updateMaterialDisplayingTexture : 8979846516231"); // End

    Debugger::block("updateMaterialDisplayingTexture : 9989898989"); // End
    //Throw the camera data to the shader
    ShaderSystem::tdModelShader().setInt("displayingMode", displayingMode);
    ShaderSystem::tdModelShader().setVec3("viewPos",matCam.cameraPos);
    ShaderSystem::tdModelShader().setMat4("view",view);
    ShaderSystem::tdModelShader().setMat4("projection",projectionMatrix);
    ShaderSystem::tdModelShader().setInt("usingMeshSelection", false);
    ShaderSystem::tdModelShader().setInt("meshSelectionEditing", false);
    ShaderSystem::tdModelShader().setInt("hideUnselected", false);
    ShaderSystem::tdModelShader().setInt("paintingOverWrap", false);
    
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
        displayModel.meshes[i].Draw(false);
    }
    Debugger::block("updateMaterialDisplayingTexture : 9989898989"); // End
    

    Debugger::block("updateMaterialDisplayingTexture : 79841313184794"); // End
    
    ShaderSystem::tdModelShader().setInt("displayingMode", 0);
    
    //!Finish (prepare rendering the GUI)

    //Use the button shader (Is necessary since that process is done in the middle of GUI rendering) 
    ShaderSystem::buttonShader().use();

    //Bind the default framebuffer
    Settings::defaultFramebuffer()->FBO.bind();
    
    //Set the OpenGL viewport to default
    Settings::defaultFramebuffer()->setViewport();

    getBox()->bindBuffers();
    Debugger::block("updateMaterialDisplayingTexture : 79841313184794"); // End
}