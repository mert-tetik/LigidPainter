/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Mouse/Mouse.hpp"

#include "GUI/GUI.hpp"

#include "3D/Model/Model.hpp"

// System
#include <string>
#include <iostream>
#include <vector>
#include <unordered_set>

static bool boxSelectionInteraction(Timer &timer, Model* model, int meshI);

void updatePrimitivesArrayTexture(Texture& primitivesArrayTexture, std::vector<byte> primitivesArray, std::vector<byte>& prevPrimArray, Mesh& selectedMesh, std::vector<int>& changedIndices, bool updateAll){

    glm::ivec2 prevRes = primitivesArrayTexture.getResolution();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, primitivesArrayTexture.ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    if((primitivesArray.size() != prevPrimArray.size()) || updateAll){
        while (glGetError() != GL_NO_ERROR)
        {
        }

        unsigned char* pxs = new unsigned char[std::ceil(sqrt(primitivesArray.size())) * std::ceil(sqrt(primitivesArray.size()))];
        
        for (size_t i = 0; i < std::ceil(sqrt(primitivesArray.size())) * std::ceil(sqrt(primitivesArray.size())); i++)
        {
            if(i + 1 < primitivesArray.size())
                pxs[i] = primitivesArray[i + 1] * 126;
            else
                pxs[i] = primitivesArray[primitivesArray.size()-1] * 126;
        }

        if(prevRes.x == std::ceil(sqrt(primitivesArray.size())) && prevRes.y == std::ceil(sqrt(primitivesArray.size())))
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, std::ceil(sqrt(primitivesArray.size())), std::ceil(sqrt(primitivesArray.size())), GL_RED, GL_UNSIGNED_BYTE, pxs);
        else 
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, std::ceil(sqrt(primitivesArray.size())), std::ceil(sqrt(primitivesArray.size())), 0, GL_RED, GL_UNSIGNED_BYTE, pxs);
        
        glGenerateMipmap(GL_TEXTURE_2D);

        if(glGetError() == 1281){
            LGDLOG::start << "ERROR : Face selection : This mesh's vertex data is too high to apply a face selection." << LGDLOG::end;
        }

        delete[] pxs;
    }
    else{
        for (size_t i = 0; i < changedIndices.size(); i++)
        {
            if(changedIndices[i] < primitivesArray.size()){
                if(primitivesArray[changedIndices[i]] != prevPrimArray[changedIndices[i]]){
                    unsigned char pxs[1];
                    pxs[0] = (unsigned char)(primitivesArray[changedIndices[i]] * 126);

                    glm::ivec2 offset = glm::ivec2(changedIndices[i] % (int)std::ceil(sqrt(primitivesArray.size())), changedIndices[i] / (int)std::ceil(sqrt(primitivesArray.size())));
                    glTexSubImage2D(GL_TEXTURE_2D, 0, offset.x - 1, offset.y, 1, 1, GL_RED, GL_UNSIGNED_BYTE, pxs);
                }
            }
        }
        
        changedIndices.clear();
    }
    
    prevPrimArray = primitivesArray;
}

static Framebuffer capture_FBO;
static glm::vec2 lastMousePos;

bool face_selection_interaction(Timer& timer, Model* model, int meshI, Camera cam, glm::vec2 cursorPos, bool renderAllModel, bool registerHistory){

    // Scale of the window
    glm::vec2 windowSize = getContext()->windowScale;
    
    // Scale of the user's primary monitor
    glm::vec2 videoScale = *Settings::videoScale();
    
    if(model->meshes[meshI].face_selection_data.selectedPrimitiveIDs.size() != model->meshes[meshI].indices.size() / 3){
        model->meshes[meshI].face_selection_data.selectedPrimitiveIDs.resize(model->meshes[meshI].indices.size() / 3);
    }

    // Generating the model primitives texture
    if(!model->meshes[meshI].face_selection_data.modelPrimitives.ID)
        glGenTextures(1, &model->meshes[meshI].face_selection_data.modelPrimitives.ID);
    
    if(!model->meshes[meshI].face_selection_data.selectedFaces.ID)
        glGenTextures(1, &model->meshes[meshI].face_selection_data.selectedFaces.ID);
    
    // Updating the model primitives texture
    model->meshes[meshI].face_selection_data.modelPrimitives.update(nullptr, windowSize.x, windowSize.y, GL_NEAREST, GL_RED, GL_R32F);
    
    if(*Mouse::LClick()){
        lastMousePos = cursorPos;
    }

    bool changesMade = false;
    
    if(shortcuts_CTRL_A()){
        if(registerHistory)
            registerFaceSelectionAction("Select all the faces - Painting", model->meshes[meshI].face_selection_data.selectedPrimitiveIDs, model->meshes[meshI].face_selection_data.prevPrimArray, meshI);

        std::fill(model->meshes[meshI].face_selection_data.selectedPrimitiveIDs.begin(), model->meshes[meshI].face_selection_data.selectedPrimitiveIDs.end(), 2);
        
        model->meshes[meshI].face_selection_data.changedIndices.clear();
        for (size_t i = 0; i < model->meshes[meshI].face_selection_data.selectedPrimitiveIDs.size(); i++)
        {
            model->meshes[meshI].face_selection_data.changedIndices.push_back(i);
        }
        
        changesMade = true;
    }

    else if(*Mouse::LPressed()){
        if(*Mouse::LClick() && registerHistory)
            registerFaceSelectionAction("Face selection - Painting", model->meshes[meshI].face_selection_data.selectedPrimitiveIDs, model->meshes[meshI].face_selection_data.prevPrimArray, meshI);
       
        // Generate & bind the framebuffer object to render the model primitives into the modelPrimitives texture
        if(!capture_FBO.ID)
            capture_FBO = Framebuffer(model->meshes[meshI].face_selection_data.modelPrimitives, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, *Settings::videoScale()), "Face selection");
        else
            capture_FBO.setColorBuffer(model->meshes[meshI].face_selection_data.modelPrimitives, GL_TEXTURE_2D);

        capture_FBO.bind();

        glClearColor(0.,0.,0.,0.);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, windowSize.x, windowSize.y);

        // Seting the primitive ID rendering shader 
        ShaderSystem::primitiveIDShader().use();
        ShaderSystem::primitiveIDShader().setMat4("view", cam.viewMatrix);
        ShaderSystem::primitiveIDShader().setMat4("projection", getScene()->projectionMatrix);
        ShaderSystem::primitiveIDShader().setMat4("modelMatrix", getScene()->transformMatrix);
        
        glDepthFunc(GL_LESS);

        if(!renderAllModel){
            ShaderSystem::primitiveIDShader().setInt("emptyFlag", false);
            model->meshes[meshI].Draw(false);
        }
        else{
            for (size_t i = 0; i < getScene()->model->meshes.size(); i++)
            {
                ShaderSystem::primitiveIDShader().setInt("emptyFlag", getScene()->model->meshes[i].materialName != model->meshes[meshI].materialName);
                getScene()->model->meshes[i].Draw(false);
            }
        }

        const int mouseFillQuality = 5;

        for (size_t distI = 0; distI < mouseFillQuality; distI++)
        {
            glm::vec2 scale;
            glm::vec2 pos;

            if(model->meshes[meshI].face_selection_data.selectionModeIndex == 0){
                pos.x = (lastMousePos.x + (cursorPos.x - lastMousePos.x) / mouseFillQuality * distI) - model->meshes[meshI].face_selection_data.radius/2;
                pos.y = (windowSize.y - (lastMousePos.y + (cursorPos.y - lastMousePos.y) / mouseFillQuality * distI)) - model->meshes[meshI].face_selection_data.radius/2;
                scale = glm::vec2(model->meshes[meshI].face_selection_data.radius);
            }
            
            else if(model->meshes[meshI].face_selection_data.selectionModeIndex == 1){
                
                glm::vec2 boxSelectionStartSc = model->meshes[meshI].face_selection_data.boxSelectionStart / 100.f * *Settings::videoScale();
                glm::vec2 boxSelectionEndSc = model->meshes[meshI].face_selection_data.boxSelectionEnd / 100.f * *Settings::videoScale();
        
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
            capture_FBO.bind();
            
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

                    
            if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && (*Mouse::LClick() || model->meshes[meshI].face_selection_data.selectionModeIndex == 1)){
                changesMade = true;
                
                std::fill(model->meshes[meshI].face_selection_data.selectedPrimitiveIDs.begin(), model->meshes[meshI].face_selection_data.selectedPrimitiveIDs.end(), false);
                
                model->meshes[meshI].face_selection_data.changedIndices.clear();
                for (size_t i = 0; i < model->meshes[meshI].face_selection_data.selectedPrimitiveIDs.size(); i++)
                {
                    model->meshes[meshI].face_selection_data.changedIndices.push_back(i);
                }
            }
            
            for (size_t i = 0; i < scale.x * scale.y; i++)
            {
                if(
                        true
                    )
                {
                    if(pxs[i] < model->meshes[meshI].face_selection_data.selectedPrimitiveIDs.size() && pxs[i] >= 0){
                        if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL)){
                            if(model->meshes[meshI].face_selection_data.selectedPrimitiveIDs[pxs[i]] == 0){
                                model->meshes[meshI].face_selection_data.selectedPrimitiveIDs[pxs[i]] = 2;
                                model->meshes[meshI].face_selection_data.changedIndices.push_back(pxs[i]);
                                changesMade = true;
                            }
                        }
                        else{
                            if(model->meshes[meshI].face_selection_data.selectedPrimitiveIDs[pxs[i]] == 2){
                                model->meshes[meshI].face_selection_data.selectedPrimitiveIDs[pxs[i]] = 0;
                                model->meshes[meshI].face_selection_data.changedIndices.push_back(pxs[i]);
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
        const int fragmentCount = model->meshes[meshI].indices.size() / 3; 

        updatePrimitivesArrayTexture(
                                        model->meshes[meshI].face_selection_data.selectedFaces, 
                                        model->meshes[meshI].face_selection_data.selectedPrimitiveIDs, 
                                        model->meshes[meshI].face_selection_data.prevPrimArray, 
                                        model->meshes[meshI], 
                                        model->meshes[meshI].face_selection_data.changedIndices,
                                        true
                                    );
    }

    boxSelectionInteraction(timer, model, meshI);

    // Set back to default
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();

    ShaderSystem::buttonShader().use();

    getBox()->bindBuffers();

    glDepthFunc(GL_LEQUAL);

    lastMousePos = cursorPos;

    return changesMade;
}

static bool __lastMousePressState = false;

bool boxSelectionInteraction(Timer &timer, Model* model, int meshI){
    
    glClear(GL_DEPTH_BUFFER_BIT);

    if(*Mouse::LClick()){
        model->meshes[meshI].face_selection_data.boxSelectionStart = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
        lastMousePos = *Mouse::cursorPos();
    }

    model->meshes[meshI].face_selection_data.boxSelectionEnd = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
    
    glm::vec2 btnScale = (model->meshes[meshI].face_selection_data.boxSelectionEnd - model->meshes[meshI].face_selection_data.boxSelectionStart) / 2.f;

    if(model->meshes[meshI].face_selection_data.boxSelectionStart.x > model->meshes[meshI].face_selection_data.boxSelectionEnd.x){
        btnScale.x = (model->meshes[meshI].face_selection_data.boxSelectionStart.x - model->meshes[meshI].face_selection_data.boxSelectionEnd.x) / 2.f;
    } 
    if(model->meshes[meshI].face_selection_data.boxSelectionStart.y > model->meshes[meshI].face_selection_data.boxSelectionEnd.y){
        btnScale.y = (model->meshes[meshI].face_selection_data.boxSelectionStart.y - model->meshes[meshI].face_selection_data.boxSelectionEnd.y) / 2.f;
    } 

    Button btn = Button(ELEMENT_STYLE_SOLID, btnScale, "", Texture(), 0.f, false);
    btn.pos = glm::vec3(model->meshes[meshI].face_selection_data.boxSelectionStart + btn.scale, 0.9f); 
    
    if(model->meshes[meshI].face_selection_data.boxSelectionStart.x > model->meshes[meshI].face_selection_data.boxSelectionEnd.x)
        btn.pos.x = model->meshes[meshI].face_selection_data.boxSelectionEnd.x + btn.scale.x;
    if(model->meshes[meshI].face_selection_data.boxSelectionStart.y > model->meshes[meshI].face_selection_data.boxSelectionEnd.y)
        btn.pos.y = model->meshes[meshI].face_selection_data.boxSelectionEnd.y + btn.scale.y;
    
    if(*Mouse::LPressed())
        btn.render(timer, false); 

    glClear(GL_DEPTH_BUFFER_BIT);
    
    bool applyBox = __lastMousePressState && !*Mouse::LPressed();
    __lastMousePressState = *Mouse::LPressed();
    
    return applyBox;
}