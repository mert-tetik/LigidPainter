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

// Defined in Painter/faceSelection.cpp
void updatePrimitivesArrayTexture(Texture& primitivesArrayTexture, std::vector<bool> primitivesArray, Mesh& selectedMesh, std::vector<int>& changedIndices);

void Model::updateObjectIDsTexture(){
    glDepthFunc(GL_LESS);
    
    glm::ivec2 resolution = glm::ivec2(*Settings::videoScale());

    // Create the texture if it wasn't created
    if(!this->objectIDs.ID){
        glGenTextures(1, &this->objectIDs.ID);
    }

    // Remove if slow
    this->objectIDs.update(nullptr, resolution.x, resolution.y, GL_LINEAR, GL_RG, GL_RG32F);

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
            ShaderSystem::modelObjectID().setInt("meshI", i + 1);
            
            for (size_t objI = 0; objI < this->meshes[i].objects.size(); objI++)
            {
                ShaderSystem::modelObjectID().setVec2("objIndices[" + std::to_string(objI) + "]", this->meshes[i].objects[objI].vertIndices / glm::ivec2(3));
            }
            
            this->meshes[i].Draw(false);
        }
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
    
    Framebuffer FBO = Framebuffer(this->objectIDs, GL_TEXTURE_2D);
    FBO.bind();

    glm::vec2 pos = *Mouse::cursorPos();
    pos.y = getContext()->windowScale.y - pos.y;

    float* pxs = new float[2]; 
    
    glReadPixels(
                    pos.x, 
                    pos.y, 
                    1, 
                    1,
                    GL_RG,
                    GL_FLOAT,
                    pxs
                );

    //Set back to default shader
    ShaderSystem::buttonShader().use();

    //Delete the capture framebuffer
    FBO.deleteBuffers(false, true);

    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();

    int objI = pxs[0];
    int meshI = pxs[1] - 1;

    delete[] pxs;

    std::cout << "meshI" << meshI << std::endl;

    if(meshI < this->meshes.size() && meshI >= 0){

        std::vector<int> changedIndices;

        bool matched = false;
        for (size_t i = 0; i < this->meshes[meshI].selectedObjectIndices.size(); i++)
        {
            if(this->meshes[meshI].selectedObjectIndices[i] == objI)
                matched = true; 
        }
        
        if(!matched)
            this->meshes[meshI].selectedObjectIndices.push_back(objI);

        std::vector<bool> primitivesArray;
        primitivesArray.resize(this->meshes[meshI].indices.size() / 3);
        for (size_t oI = 0; oI < this->meshes[meshI].objects.size(); oI++)
        {
            bool contains = false;
            for (size_t i = 0; i < this->meshes[meshI].selectedObjectIndices.size(); i++)
            {
                if(this->meshes[meshI].selectedObjectIndices[i] == oI)
                    contains = true;
            }
            std::cout << "a" << std::endl;
            if(contains){
                for (size_t i = this->meshes[meshI].objects[oI].vertIndices.x / 3; i < this->meshes[meshI].objects[oI].vertIndices.y / 3; i++)
                {
                    if(i < primitivesArray.size()){
                        changedIndices.push_back(i);
                        primitivesArray[i] = true;
                    }
                }
            }
            std::cout << "b" << std::endl;
        }
    

        if(!this->meshes[meshI].selectedObjectPrimitivesTxtr.ID){
            glDeleteTextures(1, &this->meshes[meshI].selectedObjectPrimitivesTxtr.ID);
        }

        updatePrimitivesArrayTexture(this->meshes[meshI].selectedObjectPrimitivesTxtr, primitivesArray, this->meshes[meshI], changedIndices);
    }
}