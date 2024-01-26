/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Create the depth texture to depth test while painting (to prevent painting the backside of the model) (by rendering the 3D model with a special shader)

*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "ShaderSystem/Shader.hpp"
#include "SettingsSystem/Settings.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

void Painter::updatePosNormalTexture(){
    
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    
    const unsigned int resolution = 1024; 

    glm::ivec2 res = glm::ivec2(resolution);
    res.y /= Settings::videoScale()->x / Settings::videoScale()->y;

    //Create the capture framebuffer
    unsigned int captureFBO;
    glGenFramebuffers(1,&captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER,captureFBO);
    
    //Render buffer for depth testing (refresh the RBO if the videoscale value is changed)
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthRBO);

    if(!this->meshPosTxtr.ID)
        this->meshPosTxtr = Texture(nullptr, res.x, res.y, GL_LINEAR, GL_RGBA, GL_RGBA32F);
    if(!this->meshNormalTxtr.ID)
        this->meshNormalTxtr = Texture(nullptr, res.x, res.y, GL_LINEAR, GL_RGBA, GL_RGBA32F);

    for (size_t i = 0; i < 2; i++)
    {
        Texture txtr = this->meshPosTxtr;
        if(i == 1)
            txtr = this->meshNormalTxtr;

        //Bind the depth texture (Painter class public member variable)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, txtr.ID, 0);

        //Clear the depth texture
        glViewport(0, 0, getContext()->windowScale.x / (Settings::videoScale()->x / res.x), getContext()->windowScale.y / (Settings::videoScale()->y / res.y));
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Use the depth 3D shader
        ShaderSystem::renderModelData().use();
        ShaderSystem::renderModelData().setMat4("view", getScene()->viewMatrix);
        ShaderSystem::renderModelData().setMat4("projection", getScene()->projectionMatrix);
        ShaderSystem::renderModelData().setMat4("modelMatrix",getScene()->transformMatrix);
        ShaderSystem::renderModelData().setInt("state", i + 1);

        //Draw the selected mesh in 3D
        if(selectedMeshIndex < getModel()->meshes.size()){
            getModel()->meshes[selectedMeshIndex].Draw(false);
        }
    }
    
    //!Finished

    //Set back to default shader
    ShaderSystem::buttonShader().use();

    //Delete the capture framebuffer
    glDeleteFramebuffers(1,&captureFBO);

    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
}

void Painter::getPosNormalValOverPoint(glm::vec2 pointPos, float*& posData, float*& normalData){
        
    const unsigned int resolution = 1024; 
    glm::ivec2 res = glm::ivec2(resolution);
    res.y /= Settings::videoScale()->x / Settings::videoScale()->y;

    pointPos.x /= (Settings::videoScale()->x / res.x); 
    pointPos.y /= (Settings::videoScale()->y / res.y); 
    
    Framebuffer FBO = Framebuffer(this->meshPosTxtr, GL_TEXTURE_2D, "Mesh pos normal txtr check for cursor");
    FBO.bind();
    
    glReadPixels(
                    pointPos.x, 
                    pointPos.y, 
                    1, 
                    1,
                    GL_RGBA,
                    GL_FLOAT,
                    posData
                );

    FBO.setColorBuffer(this->meshNormalTxtr, GL_TEXTURE_2D);

    glReadPixels(
                    pointPos.x, 
                    pointPos.y, 
                    1, 
                    1,
                    GL_RGBA,
                    GL_FLOAT,
                    normalData
                );
    
    FBO.deleteBuffers(false, false);

    normalData[0] = normalData[0] * 2.f - 1.f;
    normalData[1] = normalData[1] * 2.f - 1.f;
    normalData[2] = normalData[2] * 2.f - 1.f;
    
    posData[0] = posData[0] * 2.f - 1.f;
    posData[1] = posData[1] * 2.f - 1.f;
    posData[2] = posData[2] * 2.f - 1.f;
}