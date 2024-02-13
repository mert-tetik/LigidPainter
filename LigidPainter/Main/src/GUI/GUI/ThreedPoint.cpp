/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
    Timer takes care of time related things
    Like getting fps or frame rendering speed
    And helps you manage the time
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "MouseSystem/Mouse.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

static Texture threeDPointsStencilTexture;
static Framebuffer threeDPointsStencilFBO;

bool ThreeDPoint::render(Timer &timer, bool doMouseTracking, Painter& painter, bool stencilTest, float radius, bool canMove){

    Framebuffer bindedFBO;
    bindedFBO.makeCurrentlyBindedFBO();

    glm::mat4 transMat = glm::mat4(1.f);
    transMat = glm::translate(transMat, this->pos);
    
    if(stencilTest)
        transMat = glm::scale(transMat, glm::vec3(radius * 3.5f));
    else
        transMat = glm::scale(transMat, glm::vec3(radius));

    ShaderSystem::color3d().use();
    ShaderSystem::color3d().setMat4("view", getScene()->viewMatrix);
    ShaderSystem::color3d().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::color3d().setMat4("modelMatrix", transMat);
    ShaderSystem::color3d().setFloat("depthToleranceValue", 0);
 
    if(stencilTest){
        ShaderSystem::color3d().setVec4("color", glm::vec4(1.f));
    }
    else{
        if(!this->active)
            ShaderSystem::color3d().setVec4("color", this->color);
        else
            ShaderSystem::color3d().setVec4("color", this->colorActive);
    }

    getSphereModel()->Draw();    

    bool clicked = false;
    if(*Mouse::LClick() && doMouseTracking && !stencilTest){
        
        if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL))
            this->active = false;

        const unsigned int resolution = 512;
        
        glm::vec2 pointPos = glm::vec2(Mouse::cursorPos()->x, (float)getContext()->windowScale.y - Mouse::cursorPos()->y);        
        pointPos.x /= (Settings::videoScale()->x / resolution); 
        pointPos.y /= (Settings::videoScale()->y / resolution); 

        if(!threeDPointsStencilTexture.ID){
            threeDPointsStencilTexture = Texture(nullptr, resolution, resolution);
            threeDPointsStencilFBO = Framebuffer(threeDPointsStencilTexture, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(resolution)), "threeDPointsStencilFBO");
        }

        threeDPointsStencilFBO.bind();
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, (float)getContext()->windowScale.x / ((float)Settings::videoScale()->x / (float)resolution), (float)getContext()->windowScale.y / ((float)Settings::videoScale()->y / (float)resolution));

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

        this->render(timer, false, painter, true, radius, false);

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

        if(stencilData[0] > 100){
            this->active = !this->active;
            this->clickState1 = true;
            clicked = true;
        }

        delete[] stencilData;

        bindedFBO.bind();
        Settings::defaultFramebuffer()->setViewport();
    }

    if(!*Mouse::LPressed())
        this->clickState1 = false;

    this->moving = false;
    if(
            this->areMovingConditionsSet(canMove)
        )
    {
        this->moving = true;

        glm::mat4 transMat = glm::mat4(1.f);
        transMat = glm::translate(transMat, this->pos);

        glm::vec4 screenPos = getScene()->projectionMatrix * getScene()->viewMatrix * transMat * glm::vec4(glm::vec3(0.f), 1.f);
        screenPos /= screenPos.z;
        screenPos /= screenPos.w;
        
        screenPos.x = (screenPos.x + 1.f) / 2.f;
        screenPos.y = 1.f - (screenPos.y + 1.f) / 2.f;
        
        screenPos.x *= getContext()->windowScale.x;
        screenPos.y *= getContext()->windowScale.y;

        float* posData = new float[4];
        float* normalData = new float[4];

        painter.getPosNormalValOverPoint(glm::vec2(screenPos.x + Mouse::mouseOffset()->x, getContext()->windowScale.y - screenPos.y - Mouse::mouseOffset()->y), posData, normalData, true);
        bindedFBO.bind();

        if(posData[3] != 0.f){
            this->pos.x = posData[0];
            this->pos.y = posData[1];
            this->pos.z = posData[2]; 
            
            this->normal.x = normalData[0];
            this->normal.y = normalData[1];
            this->normal.z = normalData[2]; 
        }

        delete[] posData;
        delete[] normalData;
    }

    return clicked;
}

bool ThreeDPoint::areMovingConditionsSet(bool canMove){
    return  (getContext()->window.isKeyPressed(LIGIDGL_KEY_G) || this->clickState1) && 
            this->active && 
            !*Mouse::RPressed() && 
            !*Mouse::MPressed() && 
            !*Mouse::mouseScroll() &&
            canMove;
}