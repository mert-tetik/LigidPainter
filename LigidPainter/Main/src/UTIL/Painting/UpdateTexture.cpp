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


static void captureTxtrToSourceTxtr(unsigned int &captureTexture, glm::ivec2 textureRes, unsigned int &selectedTextureID){
    //Bind the capture texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,captureTexture);
    
    //Get the pixels of the capture texture
    char* pixels = new char[textureRes.x * textureRes.y * 4];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_BYTE, pixels);
    
    //Bind the source texture (painted texture)
    glBindTexture(GL_TEXTURE_2D,selectedTextureID);
    
    //Texture params
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    //Insert capture texture's pixels to the source texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureRes.x, textureRes.y, 0, GL_RGBA, GL_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

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
    
    if(this->threeDimensionalMode)
        if(selectedMeshIndex < getModel()->meshes.size())
            this->selectedTexture.removeSeams(getModel()->meshes[selectedMeshIndex], destScale);

    //Delete the capture framebuffer
    captureFBO.deleteBuffers(false, false);

    //Copy capture texture into the source texture (painted texture)
    captureTxtrToSourceTxtr(captureTexture.ID, destScale, txtr.ID);


}

void Painter::updateTexture(Panel& twoDPaintingPanel, glm::mat4 windowOrtho, int paintingMode, Filter filterBtnFilter, Box twoDPaintingBox){
    
    glm::vec2 textureRes = this->selectedTexture.getResolution();

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
        
    int txtrI = this->getSelectedTextureIndexInLibrary();
    if(txtrI != -1){
        // TODO : Same for the texture editor dialog
        registerTextureAction(actionTitle, Texture(), selectedTexture);
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
                if(i == 0){
                    clr = this->getSelectedColor().getRGB_normalized();
                    enableChannel = this->enableAlbedoChannel;
                }
                if(i == 1){
                    clr = glm::vec3(this->roughnessVal);
                    enableChannel = this->enableRoughnessChannel;
                }
                if(i == 2){
                    clr = glm::vec3(this->metallicVal);
                    enableChannel = this->enableMetallicChannel;
                }
                if(i == 3){
                    clr = glm::vec3(this->normalMapStrengthVal);
                    enableChannel = this->enableNormalMapChannel;
                }
                if(i == 4){
                    clr = glm::vec3(this->heightMapVal);
                    enableChannel = this->enableHeightMapChannel;
                }
                if(i == 5){
                    clr = glm::vec3(this->ambientOcclusionVal);
                    enableChannel = this->enableAOChannel;
                }
                if(enableChannel)
                    updateTheTexture(getModel()->meshes[this->selectedMeshIndex].albedo, twoDPaintingPanel, windowOrtho, paintingMode, filterBtnFilter, twoDPaintingBox, clr, i, clr.r);
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