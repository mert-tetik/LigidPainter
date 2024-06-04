/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/GL/GL.hpp"
#include "UTIL/Framebuffers/Framebuffer_Pool.hpp"
#include "UTIL/Texture/Texture.hpp"
#include "UTIL/Library/Library.hpp"

void Texture::generateProceduralDisplayingTexture(int displayingTextureRes, int displayMode, float radius, float rotation){
    Shader already_bound_shader = ShaderUTIL::get_bound_shader();
    
    // If texture is not generated 
    if(!this->ID){
        this->ID = Texture((char*)nullptr, displayingTextureRes, displayingTextureRes).ID;
    }

    // Update 
    this->update((char*)nullptr, displayingTextureRes, displayingTextureRes);
    
    // Generate the texture pack texture 
    if(this->proceduralProps.textureSelectionDialog_selectedMode == 3){
        if(this->proceduralProps.textureSelectionDialog_selectedTextureIndex < Library::getTexturePackArraySize()){
            Library::getTexturePack(this->proceduralProps.textureSelectionDialog_selectedTextureIndex)->apply(
                                                                                                                *this,
                                                                                                                this->proceduralProps.txtrPackScale,
                                                                                                                this->proceduralProps.txtrPackCount,
                                                                                                                this->proceduralProps.txtrPackRotation_Jitter,
                                                                                                                this->proceduralProps.txtrPackSize_Jitter,
                                                                                                                this->proceduralProps.txtrPackOpacity_Jitter,
                                                                                                                this->proceduralProps.txtrPackScatter
                                                                                                            );
        }
        
        already_bound_shader.use();

        return;
    }
    
    // Set the rendering properties
    glDepthFunc(GL_LEQUAL);

    // Generate the smart texture displaying texture
    if(displayMode == 1 || displayMode == 2){
        Camera matCam;
        matCam.cameraPos = glm::vec3(radius * cos(glm::radians(rotation)), 0.0f, radius * sin(glm::radians(rotation)));;
        matCam.radius = radius;

        //Move the camera to the side
        glm::mat4 view = glm::lookAt(matCam.cameraPos,
                                     glm::vec3(0),
                                     glm::vec3(0.0, 1.0, 0.0));

        //The perspective projection matrix    
        glm::mat4 projectionMatrix = glm::perspective(
                                                        glm::radians(35.f), //Fov  
                                                        -1.f,  //Ratio (is 1 since the width & the height is equal to displayRes)
                                                        0.1f,    //Near
                                                        100.f  //Far (the material is pretty close to the camera actually  ) 
                                                    );

        //Use the 3D model rendering shader
        ShaderSystem::PBRDisplayOnly().use();
        ShaderSystem::PBRDisplayOnly().setMat4("view", view);
        ShaderSystem::PBRDisplayOnly().setMat4("projection", projectionMatrix);
        ShaderSystem::PBRDisplayOnly().setMat4("modelMatrix", glm::mat4(1.f));
        
        ShaderSystem::PBRDisplayOnly().setVec3("viewPos",matCam.cameraPos);
        ShaderSystem::PBRDisplayOnly().setInt("displayingMode", 0); 

        Model model;
        if(displayMode == 1)
            model = *getMaterialDisplayerModel();
        else if(displayMode == 2)
            model = *getScene()->model;

        for (size_t i = 0; i < model.meshes.size(); i++)
        {
            Texture proc = this->generateProceduralTexture(model.meshes[i], 416);
            
            ShaderSystem::PBRDisplayOnly().use();
            ShaderSystem::PBRDisplayOnly().setInt("skybox", GL::bindTexture_CubeMap(getScene()->skybox.ID, "Texture::generateNormalMap"));
            ShaderSystem::PBRDisplayOnly().setInt("prefilterMap", GL::bindTexture_CubeMap(getScene()->skybox.IDPrefiltered, "Texture::generateNormalMap"));
            ShaderSystem::PBRDisplayOnly().setInt("albedoTxtr", GL::bindTexture_2D(proc.ID, "Texture::generateProceduralDisplayingTexture")); 
            ShaderSystem::PBRDisplayOnly().setInt("roughnessTxtr", GL::bindTexture_2D(appTextures.white.ID, "Texture::generateProceduralDisplayingTexture")); 
            ShaderSystem::PBRDisplayOnly().setInt("metallicTxtr", GL::bindTexture_2D(appTextures.black.ID, "Texture::generateProceduralDisplayingTexture")); 
            ShaderSystem::PBRDisplayOnly().setInt("normalMapTxtr", GL::bindTexture_2D(proc.ID, "Texture::generateProceduralDisplayingTexture")); 
            ShaderSystem::PBRDisplayOnly().setInt("heightMapTxtr", GL::bindTexture_2D(proc.ID, "Texture::generateProceduralDisplayingTexture")); 
            ShaderSystem::PBRDisplayOnly().setInt("ambientOcclusionTxtr", GL::bindTexture_2D(appTextures.white.ID, "Texture::generateProceduralDisplayingTexture")); 
            
            // Get the capture framebuffer
            Framebuffer FBO = FBOPOOL::requestFBO_with_RBO(*this, this->getResolution(), "Texture::generateProceduralDisplayingTexture");

            // Clear the color buffer
            glClearColor(0,0,0,0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            model.meshes[i].Draw("Texture::generateProceduralDisplayingTexture");
            
            GL::releaseBoundTextures("Texture::generateProceduralDisplayingTexture");
            FBOPOOL::releaseFBO(FBO);
            ShaderUTIL::release_bound_shader();
        }
    }

    // Generate regular procedural textures
    if(displayMode == 0){
        // Set shader
        ShaderSystem::proceduralDisplayerShader().use();
        ShaderSystem::proceduralDisplayerShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
        ShaderSystem::proceduralDisplayerShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
        ShaderSystem::proceduralDisplayerShader().setVec2("scale", glm::vec2(0.5f));
        
        ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", this->proceduralProps.proceduralID);                
        ShaderSystem::proceduralDisplayerShader().setFloat("proceduralScale", this->proceduralProps.proceduralScale);
        ShaderSystem::proceduralDisplayerShader().setFloat("proceduralStretch", this->proceduralProps.proceduralStretch);
        ShaderSystem::proceduralDisplayerShader().setFloat("proceduralScaleModelPos", this->proceduralProps.proceduralScaleModelPos);
        ShaderSystem::proceduralDisplayerShader().setInt("proceduralInverted", this->proceduralProps.proceduralnverted);
        ShaderSystem::proceduralDisplayerShader().setInt("proceduralGrayScale", this->proceduralProps.proceduralGrayScale);
        ShaderSystem::proceduralDisplayerShader().setFloat("proceduralBrightness", this->proceduralProps.proceduralBrightness);
        ShaderSystem::proceduralDisplayerShader().setInt("proceduralGrayScale", this->proceduralProps.proceduralGrayScale);
        ShaderSystem::proceduralDisplayerShader().setFloat("displayOpacity", 1.f);
        ShaderSystem::proceduralDisplayerShader().setInt("proceduralTexture", GL::bindTexture_2D(this->proceduralProps.proceduralTextureID, "Texture::generateProceduralDisplayingTexture"));

        unsigned int wrapParam = GL_REPEAT; 
        if(this->proceduralProps.proceduralMirroredRepeat)
            wrapParam = GL_MIRRORED_REPEAT; 

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapParam);
        
        // Get the capture framebuffer
        Framebuffer FBO = FBOPOOL::requestFBO_with_RBO(*this, this->getResolution(), "Texture::generateProceduralDisplayingTexture");

        // Clear the color buffer
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render result
        getBox()->draw("Texture::generateProceduralDisplayingTexture : Rendering procedural texture");

        // Release bound textures
        GL::releaseBoundTextures("Texture::generateProceduralDisplayingTexture");
        FBOPOOL::releaseFBO(FBO);
        ShaderUTIL::release_bound_shader();
    }

    // Generate normal map
    if(this->proceduralProps.proceduralNormalMap && displayMode == 0){
        this->applyNormalMap(this->proceduralProps.proceduralNormalStrength, this->proceduralProps.proceduralGrayScale, false); 
    }

    // Finish
    already_bound_shader.use();
}

void Texture::generateProceduralDisplayingTexture(int displayingTextureRes, int displayMode){
    this->generateProceduralDisplayingTexture(displayingTextureRes, displayMode, 4.f, 272.f);
}