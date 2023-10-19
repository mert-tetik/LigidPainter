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

// OpenGL
#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

// GL Math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// LigidPainter
#include "UTIL/Util.hpp"
#include "SettingsSystem/Settings.hpp"
#include "MouseSystem/Mouse.hpp"

// System
#include <string>
#include <iostream>
#include <vector>

static bool isInArray(std::vector<int> vec, int num){
    for (size_t i = 0; i < vec.size(); i++)
    {
        if(vec[i] == num)
            return true;
    }
    
    return false;
}

void updatePrimitivesArrayTexture(Texture& primitivesArrayTexture, std::vector<int> primitivesArray, Mesh& selectedMesh, const int fragmentCount){

    unsigned char* pxs = new unsigned char[fragmentCount];

    for (size_t i = 0; i < fragmentCount; i++)
    {
        if(isInArray(primitivesArray, i))
            pxs[i] = 255;
        else
            pxs[i] = 0;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, primitivesArrayTexture.ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, fragmentCount, 1, 0, GL_RED, GL_UNSIGNED_BYTE, pxs);
    glGenerateMipmap(GL_TEXTURE_2D);

    delete[] pxs;
}

bool FaceSelection::interaction(Mesh& selectedMesh){

    // Scale of the window
    glm::vec2 windowSize = getContext()->windowScale;
    
    // Scale of the user's primary monitor
    glm::vec2 videoScale = *Settings::videoScale();
    
    // Position of the cursor
    glm::vec2 cursorPos = *Mouse::cursorPos();

    // Generating the model primitives texture
    if(!this->modelPrimitives.ID)
        glGenTextures(1, &this->modelPrimitives.ID);
    
    if(!this->selectedFaces.ID)
        glGenTextures(1, &this->selectedFaces.ID);
    
    // Updating the model primitives texture
    this->modelPrimitives.update(nullptr, windowSize.x, windowSize.y, GL_NEAREST, GL_RED, GL_R32F);

    // Generate & bind the framebuffer object to render the model primitives into the modelPrimitives texture
    Framebuffer FBO = Framebuffer(this->modelPrimitives, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, windowSize));
    FBO.bind();

    glClearColor(0.,0.,0.,1.);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, windowSize.x, windowSize.y);

    // Seting the primitive ID rendering shader 
    ShaderSystem::primitiveIDShader().use();
    ShaderSystem::primitiveIDShader().setMat4("view", getScene()->viewMatrix);
    ShaderSystem::primitiveIDShader().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::primitiveIDShader().setMat4("modelMatrix", getScene()->transformMatrix);
    
    glDepthFunc(GL_LESS);

    selectedMesh.Draw(false);

    float* pxs = new float[this->radius * this->radius]; 
    FBO.bind();
    glReadPixels(
                    cursorPos.x - this->radius/2, 
                    (windowSize.y - cursorPos.y) - this->radius/2, 
                    this->radius, 
                    this->radius,
                    GL_RED,
                    GL_FLOAT,
                    pxs
                );
                
    bool changesMade = false; 

    if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && *Mouse::LClick()){
        if(this->selectedPrimitiveIDs.size())
            changesMade = true;
        this->selectedPrimitiveIDs.clear();
    }
    
    for (size_t i = 0; i < this->radius * this->radius; i++)
    {
        if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL)){
            if(!isInArray(this->selectedPrimitiveIDs, pxs[i] - 1)){
                this->selectedPrimitiveIDs.push_back(pxs[i] - 1);
                changesMade = true;
            }
        }
        else{
            for (size_t ei = 0; ei < this->selectedPrimitiveIDs.size(); ei++)
            {
                if(this->selectedPrimitiveIDs[ei] == pxs[i] - 1){
                    this->selectedPrimitiveIDs.erase(this->selectedPrimitiveIDs.begin() + ei);
                    ei--;
                    changesMade = true;
                }
            }
        }

    }


    

    if(changesMade){
        const int fragmentCount = selectedMesh.indices.size() / 3; 

        updatePrimitivesArrayTexture(
                                        this->selectedFaces, 
                                        this->selectedPrimitiveIDs, 
                                        selectedMesh, 
                                        fragmentCount
                                    );
    }

    // Set back to default
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();

    FBO.deleteBuffers(false, true);
    delete[] pxs;
    return changesMade;
}