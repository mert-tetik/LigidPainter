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
#include "ShaderSystem/Shader.hpp"
#include "SettingsSystem/Settings.hpp"

#include <string>
#include <iostream>
#include <vector>

void Material::updateMaterialDisplayingTexture(
                                                float textureRes,
                                                Box box,
                                                bool updateMaterial,
                                                Camera matCam,
                                                int displayingMode
                                            )
{ 

    //Move the camera to the side
    glm::mat4 view = glm::lookAt(matCam.cameraPos, 
                                 glm::vec3(0), 
                                 glm::vec3(0.0, 1.0, 0.0));
    
    //The perspective projection matrix    
    glm::mat4 projectionMatrix = glm::perspective(
                                                    glm::radians(35.f), //Fov  
                                                    -1.f,  //Ratio (is 1 since the width & the height is equal to displayRes)
                                                    100.f,  //Near (the material is pretty close to the camera actually  ) 
                                                    0.1f    //Far
                                                );
    
    //Generates the material channels from scratch if the updateMaterial boolean set to true 
    if(updateMaterial)
    {
        //For every modifier the material has (Output every modifier the material has)
        //TODO : Material - update material function
        for (int i = this->materialModifiers.size() - 1; i >= 0; --i)    
        {
            this->materialModifiers[i].updateMaterialChannels(*this, getSphereModel()->meshes[0], textureRes, i);
        }
    }
    
    //!Update the material displaying texture
    
    glBindFramebuffer(GL_FRAMEBUFFER, this->displayingFBO);
    
    //Resolution of the material displaying texture
    const int displayRes = 2048;

    //Set the OpenGL viewport to the resolution of the material displaying texture
    glViewport(0,0,displayRes,displayRes);

    //Clear the capture framebuffer (displaying texture) with alpha zero color
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Use the 3D model rendering shader
    ShaderSystem::tdModelShader().use();

    //Throw the camera data to the shader
    ShaderSystem::tdModelShader().setInt("displayingMode", displayingMode);
    ShaderSystem::tdModelShader().setVec3("viewPos",matCam.cameraPos);
    ShaderSystem::tdModelShader().setMat4("view",view);
    ShaderSystem::tdModelShader().setMat4("projection",projectionMatrix);
    
    //Bind the channels of the material
    if(!this->materialModifiers.size()){
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, Settings::appTextures().noMaterialModifierIsConnectedToMaterialWarningImage.ID);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, Settings::appTextures().noMaterialModifierIsConnectedToMaterialWarningImage.ID);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, Settings::appTextures().noMaterialModifierIsConnectedToMaterialWarningImage.ID);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, Settings::appTextures().noMaterialModifierIsConnectedToMaterialWarningImage.ID);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, Settings::appTextures().noMaterialModifierIsConnectedToMaterialWarningImage.ID);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, Settings::appTextures().noMaterialModifierIsConnectedToMaterialWarningImage.ID);
    }
    else{
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, getSphereModel()->meshes[0].albedo.ID);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, getSphereModel()->meshes[0].roughness.ID);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, getSphereModel()->meshes[0].metallic.ID);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, getSphereModel()->meshes[0].normalMap.ID);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, getSphereModel()->meshes[0].heightMap.ID);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, getSphereModel()->meshes[0].ambientOcclusion.ID);
    }
    
    //Draw the sphere
    getSphereModel()->Draw();
    
    ShaderSystem::tdModelShader().setInt("displayingMode", 0);
    
    //Just in case (Is not necessary (probably (I guess))) !!Actually I'm 100% sure that's not necessary but u know. Just in case. lol
    glGenerateMipmap(GL_TEXTURE_2D);
    
    //!Finish (prepare rendering the GUI)

    //Use the button shader (Is necessary since that process is done in the middle of GUI rendering) 
    ShaderSystem::buttonShader().use();

    //Bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    
    //Set the OpenGL viewport to default
    glViewport(0, 0, getContext()->windowScale.x, getContext()->windowScale.y);
}