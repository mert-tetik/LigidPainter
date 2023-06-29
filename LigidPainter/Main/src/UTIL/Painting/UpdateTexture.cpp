/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Updating the source texture after painting is released

*/

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"

#include <string>
#include <iostream>
#include <vector>


static void captureTxtrToSourceTxtr(unsigned int &captureTexture, int textureRes, unsigned int &selectedTextureID){
    //Bind the capture texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,captureTexture);
    
    //Get the pixels of the capture texture
    char* pixels = new char[textureRes * textureRes * 4];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_BYTE, pixels);
    
    //Bind the source texture (painted texture)
    glBindTexture(GL_TEXTURE_2D,selectedTextureID);
    
    //Texture params
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    //Insert capture texture's pixels to the source texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureRes, textureRes, 0, GL_RGBA, GL_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    delete[] pixels; //Remove the capture texture's pixels out of the memory
    glDeleteTextures(1, &captureTexture);
}


void Painter::updateTexture(std::vector<Texture> &textures, Model &model,int textureRes, Scene scene){
    
    //Write the tmp file of the selected texture before updating the texture (for undo)
    selectedTexture.writeTMP();
    
    unsigned int captureFBO;
    unsigned int captureTexture;
    
    //Create the capture texture
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1,&captureTexture);
    glBindTexture(GL_TEXTURE_2D,captureTexture);

    //Params of the capture texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    //Allocate memory for capture texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureRes, textureRes, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);

    //Create the framebuffer
    glGenFramebuffers(1,&captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER,captureFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, captureTexture, 0);
    
    //Bind the selected texture (painted texture) to the albedo channel (to paint over that texture)
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, selectedTexture.ID);

    //Set the viewport to the resolution of the texture
    glViewport(0,0,textureRes,textureRes);

    
    //Since the UV is between 0 - 1
    glm::mat4 orthoProjection = glm::ortho(0.f,1.f,0.f,1.f);
    
    if(threeDimensionalMode){
        
        textureUpdatingShader.use();

        //*Fragment
        textureUpdatingShader.setInt("txtr", 5);
        textureUpdatingShader.setInt("paintingTexture", 6);
        textureUpdatingShader.setInt("depthTexture", 7);
        textureUpdatingShader.setInt("brushModeState", this->selectedPaintingModeIndex);
        textureUpdatingShader.setFloat("paintingOpacity", this->brushProperties.opacity / 100.f);
        textureUpdatingShader.setVec3("paintingColor", this->getSelectedColor().RGB / glm::vec3(255.f));

        //Vertex
        textureUpdatingShader.setMat4("orthoProjection", orthoProjection);
        textureUpdatingShader.setMat4("perspectiveProjection", scene.projectionMatrix);
        textureUpdatingShader.setMat4("view", scene.viewMatrix);

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

        //Draw the UV of the selected model
        if(selectedMeshIndex < model.meshes.size())
            model.meshes[selectedMeshIndex].Draw(); 
    }
    else{

        //Render
        glDrawArrays(GL_TRIANGLES,0,6);
    }

    //Delete the capture framebuffer
    glDeleteFramebuffers(1,&captureFBO);
    
    //Copy capture texture into the source texture (painted texture)
    captureTxtrToSourceTxtr(captureTexture,textureRes,selectedTexture.ID);
}



//Clear the painting texture
void Painter::refreshPainting(){
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}