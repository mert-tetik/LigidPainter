/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Updating the source texture after painting is released

*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "GUI/GUI.hpp"
#include "ShaderSystem/Shader.hpp"
#include "LibrarySystem/Library.hpp"

#include <string>
#include <iostream>
#include <vector>

// Defined in the RenderPanel.cpp
extern bool updateThePreRenderedPanels;

static Mesh customMatMesh;

static void captureTxtrToSourceTxtr(unsigned int &captureTexture, glm::ivec2 textureRes, unsigned int &selectedTextureID){
    //Bind the capture texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, captureTexture);
    
    //Get the pixels of the capture texture
    char* pixels = new char[textureRes.x * textureRes.y * 4];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_BYTE, pixels);
    
    Texture selectedTextureOBJ = Texture(selectedTextureID);
    selectedTextureOBJ.update(pixels, textureRes.x, textureRes.y);

    delete[] pixels; //Remove the capture texture's pixels out of the memory
    glDeleteTextures(1, &captureTexture);
}

void Painter::updateTheTexture(Texture txtr, Panel& twoDPaintingPanel, glm::mat4 windowOrtho, int paintingMode, Filter filterBtnFilter, Box twoDPaintingBox, glm::vec3 paintingColor, int channelI, float channelStr){
    glm::vec2 destScale = glm::vec2(txtr.getResolution());

    glActiveTexture(GL_TEXTURE0);

    Texture captureTexture = Texture(nullptr, destScale.x, destScale.y, GL_LINEAR);
    Framebuffer captureFBO = Framebuffer(captureTexture, GL_TEXTURE_2D);
    
    captureFBO.bind();

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Bind the selected texture (painted texture) to the albedo channel (to paint over that texture)
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, txtr.ID);

    //Set the viewport to the resolution of the texture
    glViewport(0,0,destScale.x,destScale.y);
    
    //Since the UV is between 0 - 1
    glm::mat4 orthoProjection = glm::ortho(0.f,1.f,0.f,1.f);

    ShaderSystem::buttonShader().use();
    
    ShaderSystem::buttonShader().setMat4("projection", glm::ortho(0.f, destScale.x, destScale.y, 0.f));
    ShaderSystem::buttonShader().setVec3("pos", glm::vec3(destScale.x  / 2.f, destScale.y / 2.f, 0.1));
    ShaderSystem::buttonShader().setVec2("scale", glm::vec2(destScale / 2.f));
    ShaderSystem::buttonShader().setFloat("properties.colorMixVal", 0.f);
    ShaderSystem::buttonShader().setInt("states.renderTexture",     1    );
    ShaderSystem::buttonShader().setVec2("properties.txtrScale", glm::vec2(1.f));
    ShaderSystem::buttonShader().setInt("properties.txtr",     0    );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, txtr.ID);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);

    ShaderSystem::buttonShader().setInt("states.renderTexture"  ,     0    );

    ShaderSystem::textureUpdatingShader().use();

    //*Fragment
    ShaderSystem::textureUpdatingShader().setInt("txtr", 5);
    ShaderSystem::textureUpdatingShader().setInt("paintingTexture", 6);
    ShaderSystem::textureUpdatingShader().setInt("brushModeState", paintingMode);
    ShaderSystem::textureUpdatingShader().setInt("usePaintingOver", this->usePaintingOver);
    ShaderSystem::textureUpdatingShader().setFloat("smearTransformStrength", this->smearTransformStrength);
    ShaderSystem::textureUpdatingShader().setFloat("smearBlurStrength", this->smearBlurStrength);
    ShaderSystem::textureUpdatingShader().setInt("multiChannelsPaintingMod", this->materialPainting);
    ShaderSystem::textureUpdatingShader().setInt("channelI", channelI);
    ShaderSystem::textureUpdatingShader().setFloat("channelStrength", channelStr);

    //* Bind the textures
    //painted texture
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, txtr.ID);
    
    ///@ref paintingTexture 
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, this->projectedPaintingTexture.ID);
    
    
    if(this->threeDimensionalMode){
        //Draw the UV of the selected model
        if(selectedMeshIndex < getModel()->meshes.size()){

            //*Vertex
            ShaderSystem::textureUpdatingShader().setMat4("orthoProjection", orthoProjection);
            ShaderSystem::textureUpdatingShader().setMat4("perspectiveProjection", getScene()->projectionMatrix);
            ShaderSystem::textureUpdatingShader().setMat4("view", getScene()->viewMatrix);

            getModel()->meshes[selectedMeshIndex].Draw(false);         
        }
    }
    else{
        //*Fragment
        ShaderSystem::projectingPaintedTextureShader().setInt("doDepthTest", 0);

        //*Vertex
        ShaderSystem::projectingPaintedTextureShader().setMat4("orthoProjection", glm::ortho(0.f,1.f,0.f,1.f));
        ShaderSystem::projectingPaintedTextureShader().setMat4("perspectiveProjection", windowOrtho);
        ShaderSystem::projectingPaintedTextureShader().setMat4("view", glm::mat4(1.));
        
        twoDPaintingBox.bindBuffers();
        glDrawArrays(GL_TRIANGLES, 0 ,6);
    }
    
    if(this->threeDimensionalMode){
        if(selectedMeshIndex < getModel()->meshes.size())
            captureTexture.removeSeams(getModel()->meshes[selectedMeshIndex], destScale);
    }

    //Delete the capture framebuffer
    captureFBO.deleteBuffers(false, false);

    //Copy capture texture into the source texture (painted texture)
    captureTxtrToSourceTxtr(captureTexture.ID, destScale, txtr.ID);


}

void Painter::updateTexture(Panel& twoDPaintingPanel, glm::mat4 windowOrtho, int paintingMode, Filter filterBtnFilter, Box twoDPaintingBox, Material& paintingCustomMat){
    
    if(!this->threeDimensionalMode && this->selectedDisplayingModeIndex != 2){
        LGDLOG::start << "ERROR : Painting : Invalid displaying mode for the 2D painting" << LGDLOG::end;
        return;
    }

    if(this->selectedMeshIndex >= getModel()->meshes.size()){
        LGDLOG::start << "ERROR : Painting : Invalid selected mesh" << LGDLOG::end;
        return;
    }
    
    if(this->selectedDisplayingModeIndex == 2 && (!this->selectedTexture.ID || glIsTexture(this->selectedTexture.ID) == GL_FALSE)){
        LGDLOG::start << "ERROR : Painting : Selected texture is invalid" << LGDLOG::end;
        return;
    }

    if(
            this->selectedDisplayingModeIndex == 1 &&
            (!getModel()->meshes[this->selectedMeshIndex].albedo.ID ||
            !getModel()->meshes[this->selectedMeshIndex].roughness.ID ||
            !getModel()->meshes[this->selectedMeshIndex].metallic.ID ||
            !getModel()->meshes[this->selectedMeshIndex].normalMap.ID ||
            !getModel()->meshes[this->selectedMeshIndex].heightMap.ID ||
            !getModel()->meshes[this->selectedMeshIndex].ambientOcclusion.ID)
        )
    {
        LGDLOG::start << "WARNING : Painting : Missing texture detected!" << LGDLOG::end;
    }

    int txtrI = this->getSelectedTextureIndexInLibrary();
    
    if(this->useCustomMaterial && this->selectedMeshIndex < getModel()->meshes.size()){
        glm::vec2 res = getModel()->meshes[this->selectedMeshIndex].albedo.getResolution();

        customMatMesh.EBO = getModel()->meshes[this->selectedMeshIndex].EBO;
        customMatMesh.VBO = getModel()->meshes[this->selectedMeshIndex].VBO;
        customMatMesh.VAO = getModel()->meshes[this->selectedMeshIndex].VAO;
        customMatMesh.indices = getModel()->meshes[this->selectedMeshIndex].indices;
        
        if(!customMatMesh.albedo.ID){
            customMatMesh.albedo = Texture(nullptr, res.x, res.y);
            customMatMesh.roughness = Texture(nullptr, res.x, res.y);
            customMatMesh.metallic = Texture(nullptr, res.x, res.y);
            customMatMesh.normalMap = Texture(nullptr, res.x, res.y);
            customMatMesh.heightMap = Texture(nullptr, res.x, res.y);
            customMatMesh.ambientOcclusion = Texture(nullptr, res.x, res.y);
        }
        else{
            customMatMesh.albedo.update(nullptr, res.x, res.y);
            customMatMesh.roughness.update(nullptr, res.x, res.y);
            customMatMesh.metallic.update(nullptr, res.x, res.y);
            customMatMesh.normalMap.update(nullptr, res.x, res.y);
            customMatMesh.heightMap.update(nullptr, res.x, res.y);
            customMatMesh.ambientOcclusion.update(nullptr, res.x, res.y);
        }

        for (int i = paintingCustomMat.materialModifiers.size() - 1; i >= 0; --i)    
        {
            paintingCustomMat.materialModifiers[i].updateMaterialChannels(paintingCustomMat, customMatMesh, res.x, i, Settings::appTextures().white, 0, false);
        }
    }

    if(this->materialPainting){
        registerPaintingAction(
                                    "Multi-channel painting", 
                                    Texture(), 
                                    getModel()->meshes[this->selectedMeshIndex].albedo, this->enableAlbedoChannel, 
                                    getModel()->meshes[this->selectedMeshIndex].roughness, this->enableRoughnessChannel,
                                    getModel()->meshes[this->selectedMeshIndex].metallic, this->enableMetallicChannel,
                                    getModel()->meshes[this->selectedMeshIndex].normalMap, this->enableNormalMapChannel,
                                    getModel()->meshes[this->selectedMeshIndex].heightMap, this->enableHeightMapChannel,
                                    getModel()->meshes[this->selectedMeshIndex].ambientOcclusion, this->enableAOChannel
                                );
    }
    else if(txtrI != -1){
        
        std::string actionTitle = "Unknown painting mode";
        
        if(paintingMode == 0)
            actionTitle = "Painting a texture";
        if(paintingMode == 1)
            actionTitle = "Softening a texture";
        if(paintingMode == 2)
            actionTitle = "Smearing a texture";
        if(paintingMode == 3)
            actionTitle = "Normal painting a texture";
        if(paintingMode == 4)
            actionTitle = "Filter painting a texture";

        registerPaintingAction(
                                    actionTitle, 
                                    Texture(), 
                                    this->selectedTexture, true, 
                                    Texture(), false,
                                    Texture(), false,
                                    Texture(), false,
                                    Texture(), false,
                                    Texture(), false
                                );
    }

    if(paintingMode == 4){
        filterBtnFilter.applyFilter(this->selectedTexture.ID, this->projectedPaintingTexture, 0);
    }
    else{
        if(this->materialPainting){
            for (size_t i = 0; i < 6; i++)
            {
                glm::vec3 clr;
                bool enableChannel;
                Texture txtr;
                Texture customMatTxtr;
                if(i == 0){
                    clr = this->getSelectedColor().getRGB_normalized();
                    enableChannel = this->enableAlbedoChannel;
                    txtr = getModel()->meshes[this->selectedMeshIndex].albedo;
                    customMatTxtr = customMatMesh.albedo;
                }
                if(i == 1){
                    clr = glm::vec3(this->roughnessVal);
                    enableChannel = this->enableRoughnessChannel;
                    txtr = getModel()->meshes[this->selectedMeshIndex].roughness;
                    customMatTxtr = customMatMesh.roughness;
                }
                if(i == 2){
                    clr = glm::vec3(this->metallicVal);
                    enableChannel = this->enableMetallicChannel;
                    txtr = getModel()->meshes[this->selectedMeshIndex].metallic;
                    customMatTxtr = customMatMesh.metallic;
                }
                if(i == 3){
                    clr = glm::vec3(this->normalMapStrengthVal);
                    enableChannel = this->enableNormalMapChannel;
                    txtr = getModel()->meshes[this->selectedMeshIndex].normalMap;
                    customMatTxtr = customMatMesh.normalMap;
                }
                if(i == 4){
                    clr = glm::vec3(this->heightMapVal);
                    enableChannel = this->enableHeightMapChannel;
                    txtr = getModel()->meshes[this->selectedMeshIndex].heightMap;
                    customMatTxtr = customMatMesh.heightMap;
                }
                if(i == 5){
                    clr = glm::vec3(this->ambientOcclusionVal);
                    enableChannel = this->enableAOChannel;
                    txtr = getModel()->meshes[this->selectedMeshIndex].ambientOcclusion;
                    customMatTxtr = customMatMesh.ambientOcclusion;
                }

                if(enableChannel){
                    if(!this->useCustomMaterial)
                        updateTheTexture(txtr, twoDPaintingPanel, windowOrtho, paintingMode, filterBtnFilter, twoDPaintingBox, clr, i, clr.r);
                    else{
                        txtr.mix(customMatTxtr, projectedPaintingTexture, true, false, false);
                        if(selectedMeshIndex < getModel()->meshes.size())
                            txtr.removeSeams(getModel()->meshes[selectedMeshIndex], txtr.getResolution());
                    }
                }
            }
        }
        else{
            updateTheTexture(this->selectedTexture, twoDPaintingPanel, windowOrtho, paintingMode, filterBtnFilter, twoDPaintingBox, this->getSelectedColor().getRGB_normalized(), 0, 1.f);
        }
    }

    if(txtrI != -1){
        Library::getTexture(txtrI)->copyDataToTheCopyContext();
        projectUpdatingThreadElements.updateTextures = true;
    }

    updateThePreRenderedPanels = true;
}

//Clear the painting texture
void Painter::refreshPainting(){
    glBindFramebuffer(GL_FRAMEBUFFER,this->paintingFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->paintingTexture16f, 0);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->paintingTexture8, 0);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->projectedPaintingTexture.ID, 0);
    //glClearColor(0,0,0,0);
    //glClear(GL_COLOR_BUFFER_BIT);

    Settings::defaultFramebuffer()->FBO.bind();
}