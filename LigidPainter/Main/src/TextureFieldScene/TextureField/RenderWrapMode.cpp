/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/


#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"
#include "MouseSystem/Mouse.hpp"
#include "TextureFieldScene/TextureFieldScene.hpp"

#include <string>
#include <iostream>
#include <vector>

/* Defined in the TextureField.cpp */
extern bool textureField_alreadyInteracted;

void TextureField::renderWrappedTextureField(                            
                                                Timer& timer, 
                                                Painter& painter, 
                                                std::vector<TextureField>& srcVector, 
                                                int& srcVectorI, 
                                                bool editMode, 
                                                bool generatingTextureMode, 
                                                bool doMouseTracking
                                            )
{
    // Get the currently binded framebuffer
    Framebuffer bindedFBO;
    bindedFBO.makeCurrentlyBindedFBO();

    // Unactive the texture field if its not the editMode or r mouse button or middle mosue button button is pressed  
    if((*Mouse::RPressed() || *Mouse::MPressed()) || !editMode)
        this->active = false; 
    
    if(this->threeDPointTopLeft.pos == glm::vec3(0.f) && this->threeDPointBottomRight.pos == glm::vec3(0.f) && *Mouse::LClick() && editMode){
        // If no point is selected
        this->placeFirstPoint(painter, bindedFBO);
    }
    else if(this->threeDPointBottomRight.pos == glm::vec3(0.f) && *Mouse::LClick() && editMode){
        // If the start point is selected
        this->placeSecondPoint(painter, bindedFBO);
    }
    else{
        // Render the model first with alpha zero to depth test
        ShaderSystem::alphaZero3D().use();
        ShaderSystem::alphaZero3D().setMat4("view", getScene()->viewMatrix);
        ShaderSystem::alphaZero3D().setMat4("projection", getScene()->projectionMatrix);
        ShaderSystem::alphaZero3D().setMat4("modelMatrix", getScene()->transformMatrix);

        ShaderSystem::alphaZero3D().setInt("usingMeshSelection", painter.faceSelection.activated);
        ShaderSystem::alphaZero3D().setInt("hideUnselected", painter.faceSelection.hideUnselected);
        ShaderSystem::alphaZero3D().setInt("selectedPrimitiveIDS", 0);
        ShaderSystem::alphaZero3D().setInt("meshMask", 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, painter.faceSelection.selectedFaces.ID);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, painter.faceSelection.meshMask.ID);
        
        if(painter.selectedMeshIndex < getModel()->meshes.size()){
            ShaderSystem::alphaZero3D().setInt("primitiveCount", getModel()->meshes[painter.selectedMeshIndex].indices.size() / 3);
            getModel()->meshes[painter.selectedMeshIndex].Draw(false);
        }

        // Render the points
        if(!generatingTextureMode && editMode){
            this->renderPoints(timer, painter, doMouseTracking);
            
            if(getContext()->window.isKeyClicked(LIGIDGL_KEY_G) && this->isAnyWrapPointActive() && !textureField_alreadyInteracted)
                registerTextureFieldAction("Wrapped texture field - Point moved", srcVector);
            
            // Update the wrap box if any of the points moved & register move action
            if(this->didAnyWrapPointMove())
            {
                this->updateWrapBox(painter);

                textureField_alreadyInteracted = true;
            }
        }
        
        glClear(GL_DEPTH_BUFFER_BIT);

        // Render the model first with alpha zero to depth test
        ShaderSystem::alphaZero3D().use();
        ShaderSystem::alphaZero3D().setMat4("view", getScene()->viewMatrix);
        ShaderSystem::alphaZero3D().setMat4("projection", getScene()->projectionMatrix);
        ShaderSystem::alphaZero3D().setMat4("modelMatrix", getScene()->transformMatrix);

        ShaderSystem::alphaZero3D().setInt("usingMeshSelection", painter.faceSelection.activated);
        ShaderSystem::alphaZero3D().setInt("hideUnselected", painter.faceSelection.hideUnselected);
        ShaderSystem::alphaZero3D().setInt("selectedPrimitiveIDS", 0);
        ShaderSystem::alphaZero3D().setInt("meshMask", 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, painter.faceSelection.selectedFaces.ID);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, painter.faceSelection.meshMask.ID);

        if(painter.selectedMeshIndex < getModel()->meshes.size()){
            ShaderSystem::alphaZero3D().setInt("primitiveCount", getModel()->meshes[painter.selectedMeshIndex].indices.size() / 3);
            getModel()->meshes[painter.selectedMeshIndex].Draw(false);
        }

        // Render the wrapped texture
        if(this->threeDPointTopLeft.pos != glm::vec3(0.f) && this->threeDPointBottomRight.pos != glm::vec3(0.f))
            this->renderWrappedTextureBox(generatingTextureMode);

        // Unactivate the texture field if clicked outside of it
        if(
            *Mouse::LClick() && 
            doMouseTracking && 
            !wrap_deleteButton.hover && !wrap_flipHorizontalButton.hover && !wrap_flipVerticalButton.hover && !wrap_unwrapModeButton.hover && !wrap_detailModeButton.hover 
        )
            this->active = false;


        if(
                *Mouse::LClick() && 
                doMouseTracking && 
                !wrap_deleteButton.hover && !wrap_flipHorizontalButton.hover && !wrap_flipVerticalButton.hover && !wrap_unwrapModeButton.hover && !wrap_detailModeButton.hover && 
                editMode &&
                !textureField_alreadyInteracted
            )
        {
            this->checkIfWrappedTextureClicked(bindedFBO, painter, !this->isAnyWrapPointActive() && doMouseTracking);
        }
        
        if(this->active){
            this->renderWrappedModifyElements(timer, doMouseTracking);
        
            if(wrap_deleteButton.clicked){
                registerTextureFieldAction("Texture field deleted", srcVector);
                srcVector.erase(srcVector.begin() + srcVectorI);
                srcVectorI--;
            }
            else if(wrap_flipHorizontalButton.clicked){
                registerTextureFieldAction("Wrapped texture field - flip in x axis", srcVector);
                this->threeDWrapBox.flipX = !this->threeDWrapBox.flipX;

                this->updateWrapBox(painter);
            }
            else if(wrap_flipVerticalButton.clicked){
                registerTextureFieldAction("Wrapped texture field - flip in y axis", srcVector);
                this->threeDWrapBox.flipY = !this->threeDWrapBox.flipY;

                this->updateWrapBox(painter);
            }
            else if(wrap_unwrapModeButton.clicked){
                registerTextureFieldAction("Texture field - Unwrapped", srcVector);
                this->wrapMode = false;

                this->unplaceWrapPoints();
                
            }
            else if(wrap_detailModeButton.hover && *Mouse::LClick()){
                registerTextureFieldAction("Wrapped Texture field - Detail mode activated", srcVector);
                this->setDetailedWrapPoints(painter);
            }
        }
    }

    if((this->threeDPointTopLeft.pos == glm::vec3(0.f) || this->threeDPointBottomRight.pos == glm::vec3(0.f)) && !generatingTextureMode && editMode){
        renderWrapPointDecidingScene(timer, bindedFBO);
    }

    getBox()->bindBuffers();
    ShaderSystem::buttonShader().use();
    glClear(GL_DEPTH_BUFFER_BIT);
}




































void TextureField::setDetailedWrapPoints(Painter& painter){
    this->threeDWrapBox.getDetailedVertices(
                                                &detailed_threeDPoint_r1_c2.pos, &detailed_threeDPoint_r1_c2.detailI,
                                                &detailed_threeDPoint_r1_c3.pos, &detailed_threeDPoint_r1_c3.detailI,
                                                &detailed_threeDPoint_r1_c4.pos, &detailed_threeDPoint_r1_c4.detailI,

                                                &detailed_threeDPoint_r2_c1.pos, &detailed_threeDPoint_r2_c1.detailI,
                                                &detailed_threeDPoint_r2_c2.pos, &detailed_threeDPoint_r2_c2.detailI,
                                                &detailed_threeDPoint_r2_c3.pos, &detailed_threeDPoint_r2_c3.detailI,
                                                &detailed_threeDPoint_r2_c4.pos, &detailed_threeDPoint_r2_c4.detailI,
                                                &detailed_threeDPoint_r2_c5.pos, &detailed_threeDPoint_r2_c5.detailI,

                                                &detailed_threeDPoint_r3_c1.pos, &detailed_threeDPoint_r3_c1.detailI,
                                                &detailed_threeDPoint_r3_c2.pos, &detailed_threeDPoint_r3_c2.detailI,
                                                &detailed_threeDPoint_r3_c3.pos, &detailed_threeDPoint_r3_c3.detailI,
                                                &detailed_threeDPoint_r3_c4.pos, &detailed_threeDPoint_r3_c4.detailI,
                                                &detailed_threeDPoint_r3_c5.pos, &detailed_threeDPoint_r3_c5.detailI,
                                                
                                                &detailed_threeDPoint_r4_c1.pos, &detailed_threeDPoint_r4_c1.detailI,
                                                &detailed_threeDPoint_r4_c2.pos, &detailed_threeDPoint_r4_c2.detailI,
                                                &detailed_threeDPoint_r4_c3.pos, &detailed_threeDPoint_r4_c3.detailI,
                                                &detailed_threeDPoint_r4_c4.pos, &detailed_threeDPoint_r4_c4.detailI,
                                                &detailed_threeDPoint_r4_c5.pos, &detailed_threeDPoint_r4_c5.detailI,

                                                &detailed_threeDPoint_r5_c2.pos, &detailed_threeDPoint_r5_c2.detailI,
                                                &detailed_threeDPoint_r5_c3.pos, &detailed_threeDPoint_r5_c3.detailI,
                                                &detailed_threeDPoint_r5_c4.pos, &detailed_threeDPoint_r5_c4.detailI,
                                                painter
                                            );

    this->updateWrapBox(painter);
}

void TextureField::updateWrapBox(Painter& painter){
    
    if(!this->threeDWrapBox.VBO)
        return;

    if(this->wrap_detailModeButton.clickState1){
        this->threeDWrapBox.updateDetailed(
                                                threeDPointTopLeft.pos, threeDPointTopRight.pos, threeDPointBottomLeft.pos, threeDPointBottomRight.pos, 
                                                (threeDPointTopLeft.normal +threeDPointTopRight.normal + threeDPointBottomLeft.normal + threeDPointBottomRight.normal) / 4.f,

                                                detailed_threeDPoint_r1_c2.pos, detailed_threeDPoint_r1_c2.detailI,
                                                detailed_threeDPoint_r1_c3.pos, detailed_threeDPoint_r1_c3.detailI,
                                                detailed_threeDPoint_r1_c4.pos, detailed_threeDPoint_r1_c4.detailI,

                                                detailed_threeDPoint_r2_c1.pos, detailed_threeDPoint_r2_c1.detailI,
                                                detailed_threeDPoint_r2_c2.pos, detailed_threeDPoint_r2_c2.detailI,
                                                detailed_threeDPoint_r2_c3.pos, detailed_threeDPoint_r2_c3.detailI,
                                                detailed_threeDPoint_r2_c4.pos, detailed_threeDPoint_r2_c4.detailI,
                                                detailed_threeDPoint_r2_c5.pos, detailed_threeDPoint_r2_c5.detailI,

                                                detailed_threeDPoint_r3_c1.pos, detailed_threeDPoint_r3_c1.detailI,
                                                detailed_threeDPoint_r3_c2.pos, detailed_threeDPoint_r3_c2.detailI,
                                                detailed_threeDPoint_r3_c3.pos, detailed_threeDPoint_r3_c3.detailI,
                                                detailed_threeDPoint_r3_c4.pos, detailed_threeDPoint_r3_c4.detailI,
                                                detailed_threeDPoint_r3_c5.pos, detailed_threeDPoint_r3_c5.detailI,
                                                
                                                detailed_threeDPoint_r4_c1.pos, detailed_threeDPoint_r4_c1.detailI,
                                                detailed_threeDPoint_r4_c2.pos, detailed_threeDPoint_r4_c2.detailI,
                                                detailed_threeDPoint_r4_c3.pos, detailed_threeDPoint_r4_c3.detailI,
                                                detailed_threeDPoint_r4_c4.pos, detailed_threeDPoint_r4_c4.detailI,
                                                detailed_threeDPoint_r4_c5.pos, detailed_threeDPoint_r4_c5.detailI,

                                                detailed_threeDPoint_r5_c2.pos, detailed_threeDPoint_r5_c2.detailI,
                                                detailed_threeDPoint_r5_c3.pos, detailed_threeDPoint_r5_c3.detailI,
                                                detailed_threeDPoint_r5_c4.pos, detailed_threeDPoint_r5_c4.detailI,
                                                painter
                                            );
    }
    else{
        this->threeDWrapBox.update(
                                threeDPointTopLeft.pos, threeDPointTopRight.pos, threeDPointBottomLeft.pos, threeDPointBottomRight.pos, 
                                (threeDPointTopLeft.normal +threeDPointTopRight.normal + threeDPointBottomLeft.normal + threeDPointBottomRight.normal) / 4.f,
                                painter
                            );        
    }
}

void TextureField::unplaceWrapPoints(){
    // Unplace 4 main points
    threeDPointTopLeft.pos = glm::vec3(0.f);
    threeDPointTopRight.pos = glm::vec3(0.f);
    threeDPointBottomLeft.pos = glm::vec3(0.f);
    threeDPointBottomRight.pos = glm::vec3(0.f);

    // Unplace detailed points
    detailed_threeDPoint_r1_c2.pos = glm::vec3(0.f);
    detailed_threeDPoint_r1_c3.pos = glm::vec3(0.f);
    detailed_threeDPoint_r1_c4.pos = glm::vec3(0.f);
    detailed_threeDPoint_r2_c1.pos = glm::vec3(0.f);
    detailed_threeDPoint_r2_c2.pos = glm::vec3(0.f);
    detailed_threeDPoint_r2_c3.pos = glm::vec3(0.f);
    detailed_threeDPoint_r2_c4.pos = glm::vec3(0.f);
    detailed_threeDPoint_r2_c5.pos = glm::vec3(0.f);
    detailed_threeDPoint_r3_c1.pos = glm::vec3(0.f);
    detailed_threeDPoint_r3_c2.pos = glm::vec3(0.f);
    detailed_threeDPoint_r3_c3.pos = glm::vec3(0.f);
    detailed_threeDPoint_r3_c4.pos = glm::vec3(0.f);
    detailed_threeDPoint_r3_c5.pos = glm::vec3(0.f);
    detailed_threeDPoint_r4_c1.pos = glm::vec3(0.f);
    detailed_threeDPoint_r4_c2.pos = glm::vec3(0.f);
    detailed_threeDPoint_r4_c3.pos = glm::vec3(0.f);
    detailed_threeDPoint_r4_c4.pos = glm::vec3(0.f);
    detailed_threeDPoint_r4_c5.pos = glm::vec3(0.f);
    detailed_threeDPoint_r5_c2.pos = glm::vec3(0.f);
    detailed_threeDPoint_r5_c3.pos = glm::vec3(0.f);
    detailed_threeDPoint_r5_c4.pos = glm::vec3(0.f);

    // Automatically disable detail mode
    this->wrap_detailModeButton.clickState1 = false;
}


glm::vec2 threeDPointTopLeftCursorPos;

void TextureField::placeFirstPoint(Painter& painter, Framebuffer bindedFBO){
    // Retrieve the position value of the 3D model here
    float* posData = new float[4]; 
    float* normalData = new float[4]; 

    // Get the position value of the 3D model wherever it got clicked
    glm::vec2 currentCursorPos = glm::vec2(Mouse::cursorPos()->x, getContext()->windowScale.y - Mouse::cursorPos()->y);
    painter.getPosNormalValOverPoint(currentCursorPos, posData, normalData, true);
    
    // Store the cursor position for the top left point (this data will be used to generate a square for the placeSecondPoint function) 
    threeDPointTopLeftCursorPos = currentCursorPos; 

    // If clicked to the model and not into the space assign the position
    if(posData[3] == 1.f){
        this->threeDPointTopLeft.pos = glm::vec3(posData[0], posData[1], posData[2]);
        this->threeDPointTopLeft.normal = glm::vec3(normalData[0], normalData[1], normalData[2]);
    }

    //Finish
    bindedFBO.bind();
    delete[] posData;
    delete[] normalData;
}

void TextureField::placeSecondPoint(Painter& painter, Framebuffer bindedFBO){
    // Retrieve the position values of the 3D model here
    float* posData = new float[4]; 
    float* normalData = new float[4]; 

    // Get the position value of the 3D model wherever it got clicked (for the bottom right point)
    glm::vec2 currentCursorPos = glm::vec2(Mouse::cursorPos()->x, getContext()->windowScale.y - Mouse::cursorPos()->y);
    painter.getPosNormalValOverPoint(currentCursorPos, posData, normalData, true);
    
    // If clicked to the model and not into the space assign the position (for the bottom right point)
    if(posData[3] == 1.f){
        // Assign the position to the bottom right point
        this->threeDPointBottomRight.pos = glm::vec3(posData[0], posData[1], posData[2]);
        this->threeDPointBottomRight.normal = glm::vec3(normalData[0], normalData[1], normalData[2]);
    
        // We will be trying to place the top right point here until we find a correct place for it
        // Top right point will get close to the bottom right point as we can't place it
        // If we try more than 50 times abort the point selection procecss
        // (Same goes for both top right point and bottom left point)

        //---------------------- GENERATE TOP RIGHT POINT ----------------------

        // Generate a cursor position for the top right point using the cursor position when the top left point is placed
        glm::vec2 topRightCursorPos = glm::vec2(currentCursorPos.x, threeDPointTopLeftCursorPos.y);
        
        int tryCounter = 0;
        while ((posData[3] != 1.f || tryCounter == 0)) // Until we find a correct place
        {
            // Write the position data at the generated cursor position to the posData
            painter.getPosNormalValOverPoint(topRightCursorPos, posData, normalData, true);
            this->threeDPointTopRight.pos = glm::vec3(posData[0], posData[1], posData[2]);
            this->threeDPointTopRight.normal = glm::vec3(normalData[0], normalData[1], normalData[2]);

            // Abort the point placing process if tried more than 50 times and print an error
            if(tryCounter > 50){
                this->unplaceWrapPoints();

                LGDLOG::start << "ERROR : Invalid point for the wrapped texture field!" << LGDLOG::end;

                break;
            }
            
            // Get the generated cursor pos close to the bottom right cursor
            topRightCursorPos = glm::mix(topRightCursorPos, currentCursorPos, 0.25f);  
            
            // Increase the try counter after a try
            tryCounter++;
        }
        
        
        //---------------------- GENERATE BOTTOM LEFT POINT ----------------------

        // Generate a cursor position for the bottom left point using the cursor position when the top left point is placed
        glm::vec2 bottomLeftCursorPos = glm::vec2(threeDPointTopLeftCursorPos.x, currentCursorPos.y);
        
        tryCounter = 0;
        while ((posData[3] != 1.f || tryCounter == 0) && tryCounter < 50) // Until we find a correct place
        { 
            // Write the position data at the generated cursor position to the posData
            painter.getPosNormalValOverPoint(bottomLeftCursorPos, posData, normalData, true);
            this->threeDPointBottomLeft.pos = glm::vec3(posData[0], posData[1], posData[2]);
            this->threeDPointBottomLeft.normal = glm::vec3(normalData[0], normalData[1], normalData[2]);
        
            // Abort the point placing process if tried more than 50 times and print an error
            if(tryCounter > 50){
                this->unplaceWrapPoints();

                LGDLOG::start << "ERROR : Invalid point for the wrapped texture field!" << LGDLOG::end;

                break;
            }
            
            // Get the generated cursor pos close to the bottom right cursor
            bottomLeftCursorPos = glm::mix(bottomLeftCursorPos, currentCursorPos, 0.25f);  
            
            // Increase the try counter after a try
            tryCounter++;
        }
        
        // Generate or update the threeDWrapBox after placing 4 main points 
        if(!this->threeDWrapBox.VBO){
            this->threeDWrapBox.init(
                                    threeDPointTopLeft.pos, threeDPointTopRight.pos, threeDPointBottomLeft.pos, threeDPointBottomRight.pos, 
                                    (threeDPointTopLeft.normal + threeDPointTopRight.normal + threeDPointBottomLeft.normal + threeDPointBottomRight.normal) / 4.f,
                                    painter
                                );
        }
        else{
            this->updateWrapBox(painter);
        }

    }

    // Finish
    bindedFBO.bind();
    delete[] posData;
    delete[] normalData;
}

void TextureField::renderPoints(Timer& timer, Painter& painter, bool doMouseTracking){
    // Render 4 main points
    this->threeDPointTopLeft.render(timer, doMouseTracking, painter, false, 0.015f, !textureField_alreadyInteracted);
    this->threeDPointTopRight.render(timer, doMouseTracking, painter, false, 0.015f, !textureField_alreadyInteracted);
    this->threeDPointBottomLeft.render(timer, doMouseTracking, painter, false, 0.015f, !textureField_alreadyInteracted);
    this->threeDPointBottomRight.render(timer, doMouseTracking, painter, false, 0.015f, !textureField_alreadyInteracted);

    if(wrap_detailModeButton.clickState1 && (threeDPointBottomLeft.pos != glm::vec3(0.f) || threeDPointBottomRight.pos != glm::vec3(0.f))){
        // Render detailed points
        this->detailed_threeDPoint_r1_c2.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
        this->detailed_threeDPoint_r1_c3.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
        this->detailed_threeDPoint_r1_c4.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
        this->detailed_threeDPoint_r2_c1.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
        this->detailed_threeDPoint_r2_c2.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
        this->detailed_threeDPoint_r2_c3.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
        this->detailed_threeDPoint_r2_c4.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
        this->detailed_threeDPoint_r2_c5.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
        this->detailed_threeDPoint_r3_c1.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
        this->detailed_threeDPoint_r3_c2.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
        this->detailed_threeDPoint_r3_c3.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
        this->detailed_threeDPoint_r3_c4.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
        this->detailed_threeDPoint_r3_c5.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
        this->detailed_threeDPoint_r4_c1.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
        this->detailed_threeDPoint_r4_c2.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
        this->detailed_threeDPoint_r4_c3.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
        this->detailed_threeDPoint_r4_c4.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
        this->detailed_threeDPoint_r4_c5.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
        this->detailed_threeDPoint_r5_c2.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
        this->detailed_threeDPoint_r5_c3.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
        this->detailed_threeDPoint_r5_c4.render(timer, doMouseTracking, painter, false, 0.005f, !textureField_alreadyInteracted);
    }
}

static float calculateDepthToleranceValue(glm::vec3 origin){
    float distance = glm::distance(getScene()->camera.cameraPos, origin);

    if(distance < 2.f)
        return 0.01f;
    else if(distance < 5.f)
        return 0.001f;
    else if(distance < 10.f)
        return 0.0001f;

    return 0.00001f;
}

void TextureField::renderWrappedTextureBox(bool generatingTextureMode){
    // --------------- Use the 3D texture rendering shader program ---------------
    ShaderSystem::threeDTextureRenderingShader().use();
    
    // --------------- Set vertex shader data ---------------
    ShaderSystem::threeDTextureRenderingShader().setMat4("view", getScene()->viewMatrix);
    ShaderSystem::threeDTextureRenderingShader().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::threeDTextureRenderingShader().setMat4("modelMatrix", glm::mat4(1.f));
    
    // --------------- Set fragment shader data ---------------
    ShaderSystem::threeDTextureRenderingShader().setFloat("depthToleranceValue", calculateDepthToleranceValue((threeDPointTopLeft.pos + threeDPointTopRight.pos + threeDPointBottomLeft.pos + threeDPointBottomRight.pos) / 4.f));
    ShaderSystem::threeDTextureRenderingShader().setInt("txtr", 0); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, this->texture.ID);
    
    if(generatingTextureMode)
        ShaderSystem::threeDTextureRenderingShader().setFloat("opacity", 1.0f);
    else
        ShaderSystem::threeDTextureRenderingShader().setFloat("opacity", 0.5f);
    
    if(this->active && !generatingTextureMode)
        ShaderSystem::threeDTextureRenderingShader().setVec3("mixClr", ColorPalette::themeColor);
    else
        ShaderSystem::threeDTextureRenderingShader().setVec3("mixClr", glm::vec3(0.f));

    // --------------- Render the wrapped texture ---------------
    this->threeDWrapBox.draw();

    // --------------- Finish ---------------
    ShaderSystem::threeDTextureRenderingShader().use();
    ShaderSystem::threeDTextureRenderingShader().setFloat("depthToleranceValue", 0);
}

bool TextureField::didAnyWrapPointMove(){
    return threeDPointTopLeft.moving || threeDPointTopRight.moving || threeDPointBottomLeft.moving || threeDPointBottomRight.moving ||
            ((detailed_threeDPoint_r1_c2.moving ||
            detailed_threeDPoint_r1_c3.moving ||
            detailed_threeDPoint_r1_c4.moving ||
            detailed_threeDPoint_r2_c1.moving ||
            detailed_threeDPoint_r2_c2.moving ||
            detailed_threeDPoint_r2_c3.moving ||
            detailed_threeDPoint_r2_c4.moving ||
            detailed_threeDPoint_r2_c5.moving ||
            detailed_threeDPoint_r3_c1.moving ||
            detailed_threeDPoint_r3_c2.moving ||
            detailed_threeDPoint_r3_c3.moving ||
            detailed_threeDPoint_r3_c4.moving ||
            detailed_threeDPoint_r3_c5.moving ||
            detailed_threeDPoint_r4_c1.moving ||
            detailed_threeDPoint_r4_c2.moving ||
            detailed_threeDPoint_r4_c3.moving ||
            detailed_threeDPoint_r4_c4.moving ||
            detailed_threeDPoint_r4_c5.moving ||
            detailed_threeDPoint_r5_c2.moving ||
            detailed_threeDPoint_r5_c3.moving ||
            detailed_threeDPoint_r5_c4.moving) && wrap_detailModeButton.clickState1);
}

void TextureField::renderWrappedModifyElements(Timer& timer, bool doMouseTracking){
    // Prep rendering gui elements
    glClear(GL_DEPTH_BUFFER_BIT);
    ShaderSystem::buttonShader().use();
    getBox()->bindBuffers();

    // Positioning the elements
    this->wrap_flipHorizontalButton.pos = this->wrap_deleteButton.pos;
    this->wrap_flipHorizontalButton.pos.x += this->wrap_flipHorizontalButton.scale.x + this->wrap_deleteButton.scale.x;
    
    this->wrap_flipVerticalButton.pos = this->wrap_flipHorizontalButton.pos;
    this->wrap_flipVerticalButton.pos.x += this->wrap_flipVerticalButton.scale.x + this->wrap_flipHorizontalButton.scale.x;
    
    this->wrap_unwrapModeButton.pos = this->wrap_flipVerticalButton.pos;
    this->wrap_unwrapModeButton.pos.x += this->wrap_unwrapModeButton.scale.x + this->wrap_flipVerticalButton.scale.x;
    
    this->wrap_detailModeButton.pos = this->wrap_unwrapModeButton.pos;
    this->wrap_detailModeButton.pos.x += this->wrap_detailModeButton.scale.x + this->wrap_unwrapModeButton.scale.x;

    // Rendering the elements
    this->wrap_deleteButton.render(timer, doMouseTracking);
    this->wrap_flipHorizontalButton.render(timer, doMouseTracking);
    this->wrap_flipVerticalButton.render(timer, doMouseTracking);
    this->wrap_unwrapModeButton.render(timer, doMouseTracking);
    this->wrap_detailModeButton.render(timer, doMouseTracking);
}

bool textureFields_decidingWrapPointsMode = false;
static Button cancelBtn(ELEMENT_STYLE_STYLIZED, glm::vec2(2.f,1.5f), "Cancel", Texture(), 0.f, false);

void TextureField::renderWrapPointDecidingScene(Timer& timer, Framebuffer bindedFBO){
    // Set the decidingWrapPointsMode flag to true (this flag prevents camera movements in the callback.cpp)
    textureFields_decidingWrapPointsMode = true;

    // Get ready to render gui elements
    bindedFBO.bind();
    getBox()->bindBuffers();
    ShaderSystem::buttonShader().use();
    glClear(GL_DEPTH_BUFFER_BIT);
    Settings::defaultFramebuffer()->setViewport();
    
    // Info texts
    Button text1 = Button(ELEMENT_STYLE_BASIC, glm::vec2(5.f,1.f), "Wrap Point Selection", Texture(), 0.f, false);
    Button text2 = Button(ELEMENT_STYLE_BASIC, glm::vec2(20.f,1.f), "Please select 2 points from the top left to the bottom right in a way that forms a square by left-clicking on your 3D model", Texture(), 0.f, false);

    // Positioning info texts
    text1.pos = glm::vec3(50.f, 10.f, 0.7f);
    text2.pos = text1.pos;
    text2.pos.y += text1.scale.y + text2.scale.y;
    
    // Positioning cancel button
    cancelBtn.pos = text2.pos;
    cancelBtn.pos.y += text2.scale.y + cancelBtn.scale.y;

    // Rendering info texts
    text1.render(timer, false);
    text2.render(timer, false);
    
    // Rendering cancel button
    cancelBtn.render(timer, true);

    // Cancel initial point selection mode if the cancel button is clicked
    if(cancelBtn.clicked){
        this->wrapMode = false;

        textureFields_decidingWrapPointsMode = false;

        this->unplaceWrapPoints();
    }
}

bool TextureField::isAnyWrapPointActive(){
    return threeDPointTopLeft.active || threeDPointTopRight.active || threeDPointBottomLeft.active || threeDPointBottomRight.active ||
            ((detailed_threeDPoint_r1_c2.active ||
            detailed_threeDPoint_r1_c3.active ||
            detailed_threeDPoint_r1_c4.active ||
            detailed_threeDPoint_r2_c1.active ||
            detailed_threeDPoint_r2_c2.active ||
            detailed_threeDPoint_r2_c3.active ||
            detailed_threeDPoint_r2_c4.active ||
            detailed_threeDPoint_r2_c5.active ||
            detailed_threeDPoint_r3_c1.active ||
            detailed_threeDPoint_r3_c2.active ||
            detailed_threeDPoint_r3_c3.active ||
            detailed_threeDPoint_r3_c4.active ||
            detailed_threeDPoint_r3_c5.active ||
            detailed_threeDPoint_r4_c1.active ||
            detailed_threeDPoint_r4_c2.active ||
            detailed_threeDPoint_r4_c3.active ||
            detailed_threeDPoint_r4_c4.active ||
            detailed_threeDPoint_r4_c5.active ||
            detailed_threeDPoint_r5_c2.active ||
            detailed_threeDPoint_r5_c3.active ||
            detailed_threeDPoint_r5_c4.active) && wrap_detailModeButton.clickState1);
}

static Texture threeDPointsStencilTexture;
static Framebuffer threeDPointsStencilFBO;

void TextureField::checkIfWrappedTextureClicked(Framebuffer bindedFBO, Painter& painter, bool doMouseTracking){
    // Positioning the first modifying button (delete button) (other modifying buttons labeled with wrap_ will be positioned according to this)
    wrap_deleteButton.pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f; 
    wrap_deleteButton.pos.x -= (wrap_deleteButton.scale.x + wrap_flipHorizontalButton.scale.x + wrap_flipVerticalButton.scale.x + wrap_unwrapModeButton.scale.x + wrap_detailModeButton.scale.x) / 1.5f; 
    wrap_deleteButton.pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f; 
    wrap_deleteButton.pos.y -= 2.f; 
    wrap_deleteButton.pos.z = 0.7f; 

    // Checking buffers resolution
    const unsigned int resolution = 512;

    // Get the position (scaled to resolution)
    glm::vec2 pointPos = glm::vec2(Mouse::cursorPos()->x, (float)getContext()->windowScale.y - Mouse::cursorPos()->y);        
    pointPos.x /= (Settings::videoScale()->x / resolution); 
    pointPos.y /= (Settings::videoScale()->y / resolution); 

    // Generate the checking buffers
    if(!threeDPointsStencilTexture.ID){
        threeDPointsStencilTexture = Texture(nullptr, resolution, resolution);
        threeDPointsStencilFBO = Framebuffer(threeDPointsStencilTexture, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(resolution)), "threeDPointsStencilFBO");
    }

    // Bind the checking fbo
    threeDPointsStencilFBO.bind();

    // Clear the texture buffer
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the viewport
    glViewport(0, 0, (float)getContext()->windowScale.x / ((float)Settings::videoScale()->x / (float)resolution), (float)getContext()->windowScale.y / ((float)Settings::videoScale()->y / (float)resolution));

    // Render the model first with alpha zero to depth test
    ShaderSystem::alphaZero3D().use();
    ShaderSystem::alphaZero3D().setMat4("view", getScene()->viewMatrix);
    ShaderSystem::alphaZero3D().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::alphaZero3D().setMat4("modelMatrix", getScene()->transformMatrix);

    ShaderSystem::alphaZero3D().setInt("usingMeshSelection", painter.faceSelection.activated);
    ShaderSystem::alphaZero3D().setInt("hideUnselected", painter.faceSelection.hideUnselected);
    ShaderSystem::alphaZero3D().setInt("selectedPrimitiveIDS", 0);
    ShaderSystem::alphaZero3D().setInt("meshMask", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, painter.faceSelection.selectedFaces.ID);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, painter.faceSelection.meshMask.ID);

    if(painter.selectedMeshIndex < getModel()->meshes.size()){
        ShaderSystem::alphaZero3D().setInt("primitiveCount", getModel()->meshes[painter.selectedMeshIndex].indices.size() / 3);
        getModel()->meshes[painter.selectedMeshIndex].Draw(false);
    }
    
    // Then render the wrapped image
    ShaderSystem::color3d().use();
    ShaderSystem::color3d().setMat4("view", getScene()->viewMatrix);
    ShaderSystem::color3d().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::color3d().setMat4("modelMatrix", getScene()->transformMatrix);
    ShaderSystem::color3d().setVec4("color", glm::vec4(1.f, 1.f, 1.f, 1.f));
    ShaderSystem::color3d().setFloat("depthToleranceValue", calculateDepthToleranceValue((threeDPointTopLeft.pos + threeDPointTopRight.pos + threeDPointBottomLeft.pos + threeDPointBottomRight.pos) / 4.f));
    this->threeDWrapBox.draw();
    
    // Get the pixel value on top of the cursor
    unsigned char* stencilData = new unsigned char[4];
    glReadPixels(
                    pointPos.x, 
                    pointPos.y, 
                    1, 
                    1,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    stencilData
                );

    // If the pixel on top of the cursor is opaque 
    if(stencilData[0] > 100){
        textureField_alreadyInteracted = true;
        
        if(doMouseTracking)
            this->active = true; // Activate the texture field
    }

    // Finish
    delete[] stencilData;
    bindedFBO.bind();
    Settings::defaultFramebuffer()->setViewport();
}