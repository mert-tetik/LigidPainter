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

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "GUI/GUI.hpp"
#include "LibrarySystem/Library.hpp"
#include "SettingsSystem/Settings.hpp"

#include <vector>
#include <string>

Settings::AppTextures __appTextures;
Settings::Properties __settingsProperties;

Context __context; 
Context __copyContext; 
Scene __scene; //3D Scene structure
Model __model;
Model __sphereModel;
Model __materialDisplayerModel;
Box __box;
Settings::DefaultFramebuffer __defaultFramebuffer;

glm::vec2 __videoScale;

void Settings::loadAppTextures(){
    __appTextures.TDModelIcon.load("./LigidPainter/Resources/Icons/TDModel.png");
    __appTextures.softenIcon.load("./LigidPainter/Resources/Icons/Soften.png");
    __appTextures.smearIcon.load("./LigidPainter/Resources/Icons/Smear.png");
    __appTextures.modifierIcon.load("./LigidPainter/Resources/Icons/Modifier.png");
    __appTextures.environmentIcon.load("./LigidPainter/Resources/Icons/Environment.png");
    __appTextures.dropperIcon.load("./LigidPainter/Resources/Icons/Dropper.png");
    __appTextures.colorIcon.load("./LigidPainter/Resources/Icons/Color.png");
    __appTextures.brushIcon.load("./LigidPainter/Resources/Icons/Brush.png");
    __appTextures.arrowR.load("./LigidPainter/Resources/Icons/ArrowR.png");
    __appTextures.arrowL.load("./LigidPainter/Resources/Icons/ArrowL.png");
    __appTextures.arrowB.load("./LigidPainter/Resources/Icons/ArrowB.png");
    __appTextures.arrowT.load("./LigidPainter/Resources/Icons/ArrowT.png");
    __appTextures.threeDIcon.load("./LigidPainter/Resources/Icons/3D.png");
    __appTextures.twoDIcon.load("./LigidPainter/Resources/Icons/2D.png");
    __appTextures.exportIcon.load("./LigidPainter/Resources/Icons/Export.png");
    __appTextures.areaPaintingIcon.load("./LigidPainter/Resources/Icons/Area.png");
    __appTextures.gearIcon.load("./LigidPainter/Resources/Icons/Gear.png");
    __appTextures.nodeDisplayingMode.load("./LigidPainter/Resources/Icons/NodeDisplayingMode.png");
    __appTextures.paintingDisplayingMode.load("./LigidPainter/Resources/Icons/PaintingDisplayingMode.png");
    __appTextures.solidPaintingDisplayingMode.load("./LigidPainter/Resources/Icons/SolidPaintingDisplayingMode.png");
    __appTextures.ligidPainterIcon.load("./LigidPainter/Resources/Icons/LigidPainter.png");
    __appTextures.logButtonL.load("./LigidPainter/Resources/Icons/LogButtonL.png");
    __appTextures.logButtonR.load("./LigidPainter/Resources/Icons/LogButtonR.png");
    __appTextures.logButtonM.load("./LigidPainter/Resources/Icons/LogButtonM.png");
    __appTextures.colorGearIcon.load("./LigidPainter/Resources/Icons/ColorGear.png");
    __appTextures.filterIcon.load("./LigidPainter/Resources/Icons/Filter.png");
    __appTextures.resizingIcon.load("./LigidPainter/Resources/Icons/Resizing.png");
    __appTextures.blurIcon.load("./LigidPainter/Resources/Icons/Blur.png");
    __appTextures.distortionIcon.load("./LigidPainter/Resources/Icons/Distortion.png");
    __appTextures.normalMapIcon.load("./LigidPainter/Resources/Icons/NormalMap.png");
    __appTextures.packageIcon.load("./LigidPainter/Resources/Icons/Package.png");
    __appTextures.inkPenIcon.load("./LigidPainter/Resources/Icons/InkPen.png");
    __appTextures.bakingIcon.load("./LigidPainter/Resources/Icons/Baking.png");
    __appTextures.trashIcon.load("./LigidPainter/Resources/Icons/Trash.png");
    __appTextures.materialChannelsIcon.load("./LigidPainter/Resources/Icons/MaterialChannels.png");
    __appTextures.paintingOverIcon.load("./LigidPainter/Resources/Icons/PaintingOver.png");
    __appTextures.mirrorIcon.load("./LigidPainter/Resources/Icons/Mirror.png");
    __appTextures.greetingDialogImage.load("./LigidPainter/Resources/Images/greetingDialogImage.jpg");
    __appTextures.filterDisplayerImage.load("./LigidPainter/Resources/Images/FilterDisplayerImage.jpg");
    __appTextures.noMaterialConnectedToTheMeshWarningImage.load("./LigidPainter/Resources/Images/NoMaterialWarning.jpg");
    __appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.load("./LigidPainter/Resources/Images/NoMaterialModifierIsConnectedToMaterial.jpg");
    __appTextures.materialChannelMissingTexture.load("./LigidPainter/Resources/Images/MaterialChannelMissingTexture.jpg");
    
    char whitePxs[4] = {127,127,127,127};
    char blackPxs[4] = {0,0,0,127};
    __appTextures.white = Texture(whitePxs, 1, 1, GL_NEAREST);
    __appTextures.black = Texture(blackPxs, 1, 1, GL_NEAREST);
    __appTextures.transparent = Texture(nullptr, 1, 1, GL_NEAREST);
}

Context* getContext(){
    return &__context;
}
Context* getCopyContext(){
    return &__copyContext;
}
Scene* getScene(){
    return &__scene;
}
Model* getModel(){
    return &__model;
}
Model* getSphereModel(){
    return &__sphereModel;
}
Model* getMaterialDisplayerModel(){
    return &__materialDisplayerModel;
}
Box* getBox(){
    return &__box;
}

std::atomic<bool> mainThreadUsingCopyContext = false;

namespace Settings{
    AppTextures appTextures(){
        return __appTextures;
    }

    Properties* properties(){
        return &__settingsProperties;
    }
    
    glm::vec2* videoScale(){
        return &__videoScale;
    }

    DefaultFramebuffer* defaultFramebuffer(){
        return &__defaultFramebuffer;
    }

    void DefaultFramebuffer::render(){
        getBox()->bindBuffers();
        glDisable(GL_DEPTH_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, getContext()->windowScale.x, getContext()->windowScale.y);

        ShaderSystem::defaultFramebufferShader().use();
        ShaderSystem::defaultFramebufferShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
        ShaderSystem::defaultFramebufferShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
        ShaderSystem::defaultFramebufferShader().setVec2("scale", glm::vec2(0.5f));
        
        ShaderSystem::defaultFramebufferShader().setVec2("resolution", this->resolution);
        ShaderSystem::defaultFramebufferShader().setInt("txtr", 0);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->FBO.colorBuffer.ID);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glBindFramebuffer(GL_FRAMEBUFFER, this->FBO.ID);
        
        glEnable(GL_DEPTH_TEST);
    }

    void DefaultFramebuffer::init(glm::ivec2 resolution){
        this->resolution = resolution;

        glActiveTexture(GL_TEXTURE0);

        //--------- init bgTxtr --------- 
        this->bgTxtr = Texture(nullptr, resolution.x, resolution.y, GL_NEAREST);
        
        //--------- init FBO --------- 
        this->FBO = Framebuffer(Texture(nullptr, resolution.x, resolution.y, GL_NEAREST), GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT,resolution));

        orgID = this->FBO.ID;
        this->FBO.ID = 0;
    }

    void DefaultFramebuffer::setResolution(glm::ivec2 resolution){
        if(resolution.x == Settings::videoScale()->x)
            this->FBO.ID = 0; 
        else
            this->FBO.ID = orgID; 
        
        this->resolution = resolution;

        //--------- update colorBuffer --------- 
        this->FBO.colorBuffer.update(nullptr, resolution.x, resolution.y, GL_NEAREST);

        //--------- update bgTxtr --------- 
        bgTxtr.update(nullptr, resolution.x, resolution.y, GL_NEAREST);

        //--------- update RBO --------- 
        this->FBO.renderBuffer.update(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, resolution);
    }

    void DefaultFramebuffer::setViewport(){
        if(this->FBO.ID != 0)
            glViewport(0, 0, this->resolution.x, this->resolution.y);
        else
            glViewport(0, 0, getContext()->windowScale.x, getContext()->windowScale.y);
    }
};

void Camera::posShortcutInteraction(bool active){
    if(active){
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_0) || getContext()->window.isKeyPressed(LIGIDGL_KEY_NUMPAD_0)){
            this->originLocked = true;
            
            this->XPLocked = false;
            this->XNLocked = false;
            this->YPLocked = false;
            this->YNLocked = false;
            this->ZPLocked = false;
            this->ZNLocked = false;
        }

        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_1) || getContext()->window.isKeyPressed(LIGIDGL_KEY_NUMPAD_1)){
            this->XPLocked = true;
            this->XNLocked = false;
            this->YPLocked = false;
            this->YNLocked = false;
            this->ZPLocked = false;
            this->ZNLocked = false;
        }
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_2) || getContext()->window.isKeyPressed(LIGIDGL_KEY_NUMPAD_2)){
            this->XNLocked = true;
            this->XPLocked = false;
            this->YPLocked = false;
            this->YNLocked = false;
            this->ZPLocked = false;
            this->ZNLocked = false;
            
        }
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_3) || getContext()->window.isKeyPressed(LIGIDGL_KEY_NUMPAD_3)){
            this->YPLocked = true;
            this->XNLocked = false;
            this->XPLocked = false;
            this->YNLocked = false;
            this->ZPLocked = false;
            this->ZNLocked = false;
        }
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_4) || getContext()->window.isKeyPressed(LIGIDGL_KEY_NUMPAD_4)){
            this->YNLocked = true;
            this->XNLocked = false;
            this->YPLocked = false;
            this->XPLocked = false;
            this->ZPLocked = false;
            this->ZNLocked = false;
        }
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_5) || getContext()->window.isKeyPressed(LIGIDGL_KEY_NUMPAD_5)){
            this->ZPLocked = true;
            this->XNLocked = false;
            this->YPLocked = false;
            this->YNLocked = false;
            this->XPLocked = false;
            this->ZNLocked = false;
        }
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_6) || getContext()->window.isKeyPressed(LIGIDGL_KEY_NUMPAD_6)){
            this->ZNLocked = true;
            this->XNLocked = false;
            this->YPLocked = false;
            this->YNLocked = false;
            this->ZPLocked = false;
            this->XPLocked = false;
        }
    }


    if(this->originLocked){        
        this->transition(glm::vec3(10.f, 0.f, 0.f), glm::vec3(0.f));
        getScene()->updateViewMatrix();
        getScene()->updateProjectionMatrix();

    }
    if(this->XPLocked){        
        this->transition(glm::vec3(10.f, 0.f, 0.f) + this->originPos);
        getScene()->updateViewMatrix();
        getScene()->updateProjectionMatrix();

    }
    if(this->XNLocked){        
        this->transition(glm::vec3(-10.f, 0.f, 0.f) + this->originPos);
        getScene()->updateViewMatrix();
        getScene()->updateProjectionMatrix();

    }
    if(this->YPLocked){        
        this->transition(glm::vec3(0.f, 10.f, 0.f) + this->originPos);
        getScene()->updateViewMatrix();
        getScene()->updateProjectionMatrix();

    }
    if(this->YNLocked){        
        this->transition(glm::vec3(0.f, -10.f, 0.f) + this->originPos);
        getScene()->updateViewMatrix();
        getScene()->updateProjectionMatrix();

    }
    if(this->ZPLocked){        
        this->transition(glm::vec3(0.f, 0.f, 10.f) + this->originPos);
        getScene()->updateViewMatrix();
        getScene()->updateProjectionMatrix();

    }
    if(this->ZNLocked){        
        this->transition(glm::vec3(0.f, 0.f, -10.f) + this->originPos);
        getScene()->updateViewMatrix();
        getScene()->updateProjectionMatrix();

    }
}