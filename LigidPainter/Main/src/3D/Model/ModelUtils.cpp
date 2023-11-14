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

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "MouseSystem/Mouse.hpp"

void Model::updateObjectIDsTexture(){
    glDepthFunc(GL_LESS);
    
    glm::ivec2 resolution = glm::ivec2(*Settings::videoScale());

    // Create the texture if it wasn't created
    if(!this->objectIDs.ID){
        glGenTextures(1, &this->objectIDs.ID);
    }

    // Remove if slow
    this->objectIDs.update(nullptr, resolution.x, resolution.y, GL_LINEAR, GL_RED, GL_R32F);

    Framebuffer captureFBO = Framebuffer(this->objectIDs, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(resolution)));
    captureFBO.bind();

    {
        //Clear the object ids texture
        glViewport(0, 0, resolution.x, resolution.y);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Use the depth 3D shader
        ShaderSystem::modelObjectID().use();
        ShaderSystem::modelObjectID().setMat4("view", getScene()->viewMatrix);
        ShaderSystem::modelObjectID().setMat4("projection", getScene()->projectionMatrix);
        ShaderSystem::modelObjectID().setMat4("modelMatrix",getScene()->transformMatrix);

        for (size_t i = 0; i < this->meshes.size(); i++){
            
            ShaderSystem::modelObjectID().setInt("objIndicesArraySize", this->meshes[i].objects.size());
            
            for (size_t objI = 0; objI < this->meshes[i].objects.size(); objI++)
            {
                ShaderSystem::modelObjectID().setVec2("objIndices[" + std::to_string(objI* 2) + "]", this->meshes[i].objects[objI].vertIndices / glm::ivec2(3));
            }
            
            this->meshes[i].Draw(false);
        }

        std::cout << std::endl;
    }
    
    //!Finished

    //Set back to default shader
    ShaderSystem::buttonShader().use();

    //Delete the capture framebuffer
    captureFBO.deleteBuffers(false, true);

    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();

    glDepthFunc(GL_LEQUAL);
}

void Model::selectObject(){
    
    std::cout << objectIDs.ID << std::endl;
    Framebuffer FBO = Framebuffer(this->objectIDs, GL_TEXTURE_2D);
    FBO.bind();

    glm::vec2 pos = *Mouse::cursorPos();
    pos.y = getContext()->windowScale.y - pos.y;

    float* pxs = new float[1]; 
    
    glReadPixels(
                    pos.x, 
                    pos.y, 
                    1, 
                    1,
                    GL_RED,
                    GL_FLOAT,
                    pxs
                );

    std::cout << "WOW : " << pxs[0] << std::endl;
    
    delete[] pxs;

    //Set back to default shader
    ShaderSystem::buttonShader().use();

    //Delete the capture framebuffer
    FBO.deleteBuffers(false, true);

    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();

    /*
    for (size_t meshI = 0; meshI < this->meshes.size(); meshI++)
    {
        selectedObjectPrimitiveIDFlags.resize(this->meshes[meshI].indices.size() / 3);

        for (size_t objI = 0; objI < this->meshes[meshI].objects.size(); objI++)
        {
            for (size_t i = this->meshes[meshI].objects[objI].vertIndices.x / 3; i < this->meshes[meshI].objects[objI].vertIndices.y / 3; i++)
            {
                if(i < selectedObjectPrimitiveIDFlags.size())
                    selectedObjectPrimitiveIDFlags[i] = true;
            }
        }
    }
    */
}