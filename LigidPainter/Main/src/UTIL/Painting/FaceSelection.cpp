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
#include <glm/gtx/string_cast.hpp>

// LigidPainter
#include "UTIL/Util.hpp"
#include "SettingsSystem/Settings.hpp"
#include "MouseSystem/Mouse.hpp"
#include "GUI/GUI.hpp"

// System
#include <string>
#include <iostream>
#include <vector>
#include <unordered_set>

static std::vector<bool> prevPrimArray;

void updatePrimitivesArrayTexture(Texture& primitivesArrayTexture, std::vector<bool> primitivesArray, Mesh& selectedMesh, std::vector<int>& changedIndices, bool updateAll){

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, primitivesArrayTexture.ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    if((primitivesArray.size() != prevPrimArray.size() || changedIndices.size() > 100) || updateAll){
        while (glGetError() != GL_NO_ERROR)
        {
        }

        unsigned char* pxs = new unsigned char[primitivesArray.size()];
        
        for (size_t i = 0; i < primitivesArray.size(); i++)
        {
            pxs[i] = primitivesArray[i] * 255;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, (int)sqrt(primitivesArray.size()), (int)sqrt(primitivesArray.size()), 0, GL_RED, GL_UNSIGNED_BYTE, pxs);
        glGenerateMipmap(GL_TEXTURE_2D);

        if(glGetError() == 1281){
            LGDLOG::start << "ERROR : Face selection : This mesh's vertex data is too high to apply a face selection." << LGDLOG::end;
        }
    }
    else{
        for (size_t i = 0; i < changedIndices.size(); i++)
        {
            if(changedIndices[i] < primitivesArray.size()){
                if(primitivesArray[changedIndices[i]] != prevPrimArray[changedIndices[i]]){
                    unsigned char pxs[1];
                    pxs[0] = (unsigned char)(primitivesArray[changedIndices[i]] * 255);

                    glm::ivec2 offset = glm::ivec2(changedIndices[i] % int(sqrt(primitivesArray.size())), changedIndices[i] / int(sqrt(primitivesArray.size())));
                    glTexSubImage2D(GL_TEXTURE_2D, 0, offset.x - 1, offset.y, 1, 1, GL_RED, GL_UNSIGNED_BYTE, pxs);
                }
            }
        }
        changedIndices.clear();
    }
    
    prevPrimArray = primitivesArray;
}
static glm::vec2 lastMousePos;
bool FaceSelection::interaction(Mesh& selectedMesh, bool mouseInteraction){

    // Scale of the window
    glm::vec2 windowSize = getContext()->windowScale;
    
    // Scale of the user's primary monitor
    glm::vec2 videoScale = *Settings::videoScale();
    
    // Position of the cursor
    glm::vec2 cursorPos = *Mouse::cursorPos();

    if(this->selectedPrimitiveIDs.size() != selectedMesh.indices.size() / 3){
        this->selectedPrimitiveIDs.resize(selectedMesh.indices.size() / 3);
    }

    // Generating the model primitives texture
    if(!this->modelPrimitives.ID)
        glGenTextures(1, &this->modelPrimitives.ID);
    
    if(!this->selectedFaces.ID)
        glGenTextures(1, &this->selectedFaces.ID);
    
    // Updating the model primitives texture
    this->modelPrimitives.update(nullptr, windowSize.x, windowSize.y, GL_NEAREST, GL_RED, GL_R32F);
    
    if(*Mouse::LClick()){
        lastMousePos = *Mouse::cursorPos();
    }

    bool changesMade = false;
    if(mouseInteraction){

        // Generate & bind the framebuffer object to render the model primitives into the modelPrimitives texture
        if(!this->FBO.ID)
            this->FBO = Framebuffer(this->modelPrimitives, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, windowSize));
            
        this->FBO.bind();

        glClearColor(0.,0.,0.,0.);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, windowSize.x, windowSize.y);

        // Seting the primitive ID rendering shader 
        ShaderSystem::primitiveIDShader().use();
        ShaderSystem::primitiveIDShader().setMat4("view", getScene()->viewMatrix);
        ShaderSystem::primitiveIDShader().setMat4("projection", getScene()->projectionMatrix);
        ShaderSystem::primitiveIDShader().setMat4("modelMatrix", getScene()->transformMatrix);
        
        glDepthFunc(GL_LESS);

        selectedMesh.Draw(false);

        const int mouseFillQuality = 5;

        for (size_t distI = 0; distI < mouseFillQuality; distI++)
        {
            glm::vec2 scale;
            glm::vec2 pos;

            if(this->selectionModeIndex == 0){
                pos.x = (lastMousePos.x + (cursorPos.x - lastMousePos.x) / mouseFillQuality * distI) - this->radius/2;
                pos.y = (windowSize.y - (lastMousePos.y + (cursorPos.y - lastMousePos.y) / mouseFillQuality * distI)) - this->radius/2;
                scale = glm::vec2(radius);
            }
            
            else if(this->selectionModeIndex == 1){
                
                glm::vec2 boxSelectionStartSc = this->boxSelectionStart / 100.f * *Settings::videoScale();
                glm::vec2 boxSelectionEndSc = this->boxSelectionEnd / 100.f * *Settings::videoScale();
        
                boxSelectionStartSc.y = (windowSize.y - boxSelectionStartSc.y);
                boxSelectionEndSc.y = (windowSize.y - boxSelectionEndSc.y);
                
                scale = (boxSelectionEndSc - boxSelectionStartSc);
                pos = boxSelectionStartSc;

                if(boxSelectionStartSc.x > boxSelectionEndSc.x){
                    scale.x = (boxSelectionStartSc.x - boxSelectionEndSc.x);
                    pos.x = boxSelectionEndSc.x;
                } 
                if(boxSelectionStartSc.y > boxSelectionEndSc.y){
                    scale.y = (boxSelectionStartSc.y - boxSelectionEndSc.y);
                    pos.y = boxSelectionEndSc.y;
                } 
            }
            
            float* pxs = new float[(int)scale.x * (int)scale.y]; 
            this->FBO.bind();
            
            if(pos.x + scale.x < windowSize.x && pos.y + scale.y < windowSize.y){
                glReadPixels(
                                pos.x, 
                                pos.y, 
                                scale.x, 
                                scale.y,
                                GL_RED,
                                GL_FLOAT,
                                pxs
                            );
            }

                    
            if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && (*Mouse::LClick() || this->selectionModeIndex == 1)){
                changesMade = true;
                
                std::fill(this->selectedPrimitiveIDs.begin(), this->selectedPrimitiveIDs.end(), false);
                
                changedIndices.clear();
                for (size_t i = 0; i < this->selectedPrimitiveIDs.size(); i++)
                {
                    changedIndices.push_back(i);
                }
            }
            
            for (size_t i = 0; i < scale.x * scale.y; i++)
            {
                
                if(
                        true
                    )
                {
                    if(pxs[i] < this->selectedPrimitiveIDs.size() && pxs[i] >= 0){
                        if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL)){
                            if(this->selectedPrimitiveIDs[pxs[i]] == false){
                                this->selectedPrimitiveIDs[pxs[i]] = true;
                                changedIndices.push_back(pxs[i]);
                                changesMade = true;
                            }
                        }
                        else{
                            if(this->selectedPrimitiveIDs[pxs[i]] == true){
                                this->selectedPrimitiveIDs[pxs[i]] = false;
                                changedIndices.push_back(pxs[i]);
                                changesMade = true;
                            }
                        }
                    }
                }
            }

            delete[] pxs;
        }

    }
    else{
        changesMade = true;
    }
    
    if(changesMade){
        const int fragmentCount = selectedMesh.indices.size() / 3; 

        updatePrimitivesArrayTexture(
                                        this->selectedFaces, 
                                        this->selectedPrimitiveIDs, 
                                        selectedMesh, 
                                        changedIndices,
                                        false
                                    );
    }


    // Set back to default
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();

    ShaderSystem::buttonShader().use();

    getBox()->bindBuffers();

    glDepthFunc(GL_LEQUAL);

    lastMousePos = *Mouse::cursorPos();

    return changesMade;
}

static bool __lastMousePressState = false;

bool FaceSelection::boxSelectionInteraction(Timer &timer){
    
    glClear(GL_DEPTH_BUFFER_BIT);

    if(*Mouse::LClick()){
        this->boxSelectionStart = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
        lastMousePos = *Mouse::cursorPos();
    }

    this->boxSelectionEnd = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
    
    glm::vec2 btnScale = (this->boxSelectionEnd - this->boxSelectionStart) / 2.f;

    if(this->boxSelectionStart.x > this->boxSelectionEnd.x){
        btnScale.x = (this->boxSelectionStart.x - this->boxSelectionEnd.x) / 2.f;
    } 
    if(this->boxSelectionStart.y > this->boxSelectionEnd.y){
        btnScale.y = (this->boxSelectionStart.y - this->boxSelectionEnd.y) / 2.f;
    } 

    Button btn = Button(ELEMENT_STYLE_SOLID, btnScale, "", Texture(), 0.f, false);
    btn.pos = glm::vec3(this->boxSelectionStart + btn.scale, 0.9f); 
    
    if(this->boxSelectionStart.x > this->boxSelectionEnd.x)
        btn.pos.x = this->boxSelectionEnd.x + btn.scale.x;
    if(this->boxSelectionStart.y > this->boxSelectionEnd.y)
        btn.pos.y = this->boxSelectionEnd.y + btn.scale.y;
    
    if(*Mouse::LPressed())
        btn.render(timer, false); 

    glClear(GL_DEPTH_BUFFER_BIT);
    
    bool applyBox = __lastMousePressState && !*Mouse::LPressed();
    __lastMousePressState = *Mouse::LPressed();
    
    return applyBox;
}