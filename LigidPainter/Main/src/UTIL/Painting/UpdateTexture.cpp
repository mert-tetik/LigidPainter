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
}


void Painter::updateTexture(std::vector<Texture> &textures, Model &model,int textureRes){
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

    //Render the model with it's UV using the 3D Model shader (same with the one used to render the 3D model)
    tdModelShader.use();
    
    //Since the UV is between 0 - 1
    glm::mat4 orthoProjection = glm::ortho(0.f,1.f,0.f,1.f);
    tdModelShader.setMat4("oneZeroProjection",orthoProjection);
    
    if(threeDimensionalMode){
        //Make sure to not use the transform uniforms (Use UV instead (in render 2D mode))
        tdModelShader.setInt("useTransformUniforms",0);

        //Don't use pbr function
        tdModelShader.setInt("returnSingleTxtr",1);
        
        //2D Mode
        tdModelShader.setInt("render2D",1);

        //Draw the UV of the selected model
        model.meshes[0].Draw(); //TODO SELECT THE MESH
        
        //Set to default
        tdModelShader.setInt("render2D",0);
        tdModelShader.setInt("returnSingleTxtr",0);
    }
    else{

        //Don't use pbr function
        tdModelShader.setInt("returnSingleTxtr",1);
        
        //Enable transform uniforms
        tdModelShader.setInt("useTransformUniforms",1);

        //Transform uniforms
        tdModelShader.setVec2("scale",scale2D);
        tdModelShader.setVec3("pos",pos2D);
        
        //2D Mode
        tdModelShader.setInt("render2D",1);
        
        //Render
        glDrawArrays(GL_TRIANGLES,0,6);
        
        //Set to default
        tdModelShader.setInt("render2D",0);
        tdModelShader.setInt("useTransformUniforms",0);
        tdModelShader.setInt("returnSingleTxtr",0);
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