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


void Painter::updateTexture(Panel& twoDPaintingPanel, glm::mat4 windowOrtho, float twoDSceneScroll, glm::vec2 twoDScenePos, int paintingMode){
    
    glm::vec2 textureRes = this->selectedTexture.getResolution();

    std::string actionTitle = "Unknown painting mode";
    
    if(this->selectedPaintingModeIndex == 0)
        actionTitle = "Painting a texture";
    if(this->selectedPaintingModeIndex == 1)
        actionTitle = "Softening a texture";
    if(this->selectedPaintingModeIndex == 2)
        actionTitle = "Smearing a texture";
    if(this->selectedPaintingModeIndex == 3)
        actionTitle = "Normal painting a texture";
    if(this->selectedPaintingModeIndex == 4)
        actionTitle = "Filter painting a texture";
        
    registerTextureAction(actionTitle, Texture(), selectedTexture);

    unsigned int captureFBO;
    unsigned int captureTexture;
    
    //Create the capture texture
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1,&captureTexture);
    glBindTexture(GL_TEXTURE_2D,captureTexture);

    //Params of the capture texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    //Allocate memory for capture texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureRes.x, textureRes.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);

    //Create the framebuffer
    glGenFramebuffers(1,&captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER,captureFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, captureTexture, 0);

    glClearColor(1,0,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Bind the selected texture (painted texture) to the albedo channel (to paint over that texture)
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, selectedTexture.ID);

    //Set the viewport to the resolution of the texture
    glViewport(0,0,textureRes.x,textureRes.y);
    
    //Since the UV is between 0 - 1
    glm::mat4 orthoProjection = glm::ortho(0.f,1.f,0.f,1.f);
    
    glm::vec2 destScale = glm::vec2(glm::vec2(this->selectedTexture.getResolution()));

    glm::mat4 twoDProjection = glm::ortho(
                                            twoDPaintingPanel.sections[0].elements[0].button.resultPos.x + twoDScenePos.x - destScale.x  * twoDSceneScroll,
                                            twoDPaintingPanel.sections[0].elements[0].button.resultPos.x + twoDScenePos.x + destScale.x  * twoDSceneScroll,
                                            twoDPaintingPanel.sections[0].elements[0].button.resultPos.y + twoDScenePos.y + destScale.y  * twoDSceneScroll,
                                            twoDPaintingPanel.sections[0].elements[0].button.resultPos.y + twoDScenePos.y - destScale.y  * twoDSceneScroll
                                        );
    
    if(this->threeDimensionalMode){

        ShaderSystem::buttonShader().use();
        
        ShaderSystem::buttonShader().setMat4("projection", glm::ortho(0.f, textureRes.x, textureRes.y, 0.f));
        ShaderSystem::buttonShader().setVec3("pos", glm::vec3(textureRes.x  / 2.f, textureRes.y / 2.f, 0.1));
        ShaderSystem::buttonShader().setVec2("scale", glm::vec2(textureRes / 2.f));
        ShaderSystem::buttonShader().setFloat("properties.colorMixVal", 0.f);
        ShaderSystem::buttonShader().setInt("states.renderTexture",     1    );
        ShaderSystem::buttonShader().setVec2("properties.txtrScale", glm::vec2(1.f));
        ShaderSystem::buttonShader().setInt("properties.txtr",     0    );

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->selectedTexture.ID);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);

        ShaderSystem::buttonShader().setInt("states.renderTexture"  ,     0    );

        ShaderSystem::textureUpdatingShader().use();

        //*Fragment
        ShaderSystem::textureUpdatingShader().setInt("doDepthTest", 1);
        ShaderSystem::textureUpdatingShader().setInt("txtr", 5);
        ShaderSystem::textureUpdatingShader().setInt("paintingTexture", 6);
        ShaderSystem::textureUpdatingShader().setInt("depthTexture", 7);
        ShaderSystem::textureUpdatingShader().setInt("paintingOverTexture", 10);
        ShaderSystem::textureUpdatingShader().setInt("brushModeState", paintingMode);
        ShaderSystem::textureUpdatingShader().setFloat("paintingOpacity", this->brushProperties.opacity / 100.f);
        ShaderSystem::textureUpdatingShader().setInt("usePaintingOver", this->usePaintingOver);
        ShaderSystem::textureUpdatingShader().setInt("paintingOverGrayScale", this->paintingOverGrayScale);
        ShaderSystem::textureUpdatingShader().setInt("paintingOverWraping", this->paintingOverWraping);
        ShaderSystem::textureUpdatingShader().setVec3("paintingColor", this->getSelectedColor().getRGB_normalized());

        //*Vertex
        ShaderSystem::textureUpdatingShader().setMat4("orthoProjection", orthoProjection);
        ShaderSystem::textureUpdatingShader().setMat4("perspectiveProjection", getScene()->projectionMatrix);
        ShaderSystem::textureUpdatingShader().setMat4("view", getScene()->viewMatrix);

        //* Bind the textures
        //painted texture
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, this->selectedTexture.ID);
        
        ///@ref paintingTexture 
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, this->paintingTexture);
        
        ///@ref depthTexture 
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, this->depthTexture);
        
        ///@ref paintingOverTexture 
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, this->paintingOverTexture);

        //Draw the UV of the selected model
        if(selectedMeshIndex < getModel()->meshes.size())
            getModel()->meshes[selectedMeshIndex].Draw();         
    }
    else{

        ShaderSystem::twoDPaintingModeAreaShader().use();

        //*Fragment
        ShaderSystem::twoDPaintingModeAreaShader().setInt("doDepthTest", 0);
        ShaderSystem::twoDPaintingModeAreaShader().setInt("txtr", 5);
        ShaderSystem::twoDPaintingModeAreaShader().setInt("paintingTexture", 6);
        ShaderSystem::twoDPaintingModeAreaShader().setInt("depthTexture", 7);
        ShaderSystem::twoDPaintingModeAreaShader().setInt("paintingOverTexture", 10);
        ShaderSystem::twoDPaintingModeAreaShader().setInt("brushModeState", paintingMode);
        ShaderSystem::twoDPaintingModeAreaShader().setFloat("paintingOpacity", this->brushProperties.opacity / 100.f);
        ShaderSystem::twoDPaintingModeAreaShader().setInt("usePaintingOver", this->usePaintingOver);
        ShaderSystem::twoDPaintingModeAreaShader().setInt("paintingOverGrayScale", this->paintingOverGrayScale);
        ShaderSystem::twoDPaintingModeAreaShader().setInt("paintingOverWraping", this->paintingOverWraping);
        ShaderSystem::twoDPaintingModeAreaShader().setVec3("paintingColor", this->getSelectedColor().getRGB_normalized());

        //*Vertex
        ShaderSystem::twoDPaintingModeAreaShader().setMat4("projectedPosProjection", windowOrtho);
        ShaderSystem::twoDPaintingModeAreaShader().setMat4("projection", twoDProjection);
        ShaderSystem::twoDPaintingModeAreaShader().setVec3("pos", twoDPaintingPanel.sections[0].elements[0].button.resultPos + glm::vec3(twoDScenePos, 0.f));
        ShaderSystem::twoDPaintingModeAreaShader().setVec2("scale", destScale * twoDSceneScroll);

        //* Bind the textures
        //painted texture
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, this->selectedTexture.ID);
        
        //paintingTexture 
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, this->paintingTexture);
        
        //depthTexture 
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, this->depthTexture);

        ///paintingOverTexture 
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, this->paintingOverTexture);

        //Render
        glDrawArrays(GL_TRIANGLES,0,6);
    
    }

    //Delete the capture framebuffer
    glDeleteFramebuffers(1,&captureFBO);
    
    //Copy capture texture into the source texture (painted texture)
    captureTxtrToSourceTxtr(captureTexture,textureRes,selectedTexture.ID);

    if(this->threeDimensionalMode)
        if(selectedMeshIndex < getModel()->meshes.size())
            this->selectedTexture.removeSeams(getModel()->meshes[selectedMeshIndex], textureRes);
}



//Clear the painting texture
void Painter::refreshPainting(){
    glBindFramebuffer(GL_FRAMEBUFFER,this->paintingFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->paintingTexture16f, 0);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    
    glBindFramebuffer(GL_FRAMEBUFFER,this->paintingFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->paintingTexture8, 0);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}