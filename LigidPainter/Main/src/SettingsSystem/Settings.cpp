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
    __appTextures.greetingDialogImage.load("./LigidPainter/Resources/Images/greetingDialogImage.jpg");
    __appTextures.filterDisplayerImage.load("./LigidPainter/Resources/Images/FilterDisplayerImage.jpg");
    __appTextures.noMaterialConnectedToTheMeshWarningImage.load("./LigidPainter/Resources/Images/NoMaterialWarning.jpg");
    __appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.load("./LigidPainter/Resources/Images/NoMaterialModifierIsConnectedToMaterial.jpg");
}

Context* getContext(){
    return &__context;
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
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, getContext()->windowScale.x, getContext()->windowScale.y);

        ShaderSystem::defaultFramebufferShader().use();
        ShaderSystem::defaultFramebufferShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
        ShaderSystem::defaultFramebufferShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
        ShaderSystem::defaultFramebufferShader().setVec2("scale", glm::vec2(0.5f));
        
        ShaderSystem::defaultFramebufferShader().setVec2("resolution", this->resolution);
        ShaderSystem::defaultFramebufferShader().setInt("txtr", 0);
        ShaderSystem::defaultFramebufferShader().setInt("txtr2", 1);
        ShaderSystem::defaultFramebufferShader().setInt("multisamples", 1);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->FBO.colorBuffer.ID);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glBindFramebuffer(GL_FRAMEBUFFER, this->FBO.ID);
    }

    void DefaultFramebuffer::init(glm::ivec2 resolution){
        this->resolution = resolution;

        glEnable(GL_MULTISAMPLE);

        glActiveTexture(GL_TEXTURE0);

        //--------- init colorBuffer --------- 
        unsigned int colorBuffer;
        glGenTextures(1, &colorBuffer);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorBuffer);

        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Settings::properties()->framebufferSamples, GL_RGBA8, resolution.x, resolution.y, GL_TRUE);

        //--------- init bgTxtr --------- 
        this->bgTxtr = Texture(nullptr, resolution.x, resolution.y, GL_NEAREST);
        
        //--------- init FBO --------- 
        this->FBO = Framebuffer(colorBuffer, GL_TEXTURE_2D_MULTISAMPLE, Renderbuffer(GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT,resolution, Settings::properties()->framebufferSamples));

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
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->FBO.colorBuffer.ID);

        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Settings::properties()->framebufferSamples, GL_RGBA8, resolution.x, resolution.y, GL_TRUE);

        //--------- update bgTxtr --------- 
        bgTxtr.update(nullptr, resolution.x, resolution.y, GL_NEAREST);

        //--------- update RBO --------- 
        this->FBO.renderBuffer.update(GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, resolution, Settings::properties()->framebufferSamples);
    }

    void DefaultFramebuffer::setViewport(){
        if(this->FBO.ID != 0)
            glViewport(0, 0, this->resolution.x, this->resolution.y);
        else
            glViewport(0, 0, getContext()->windowScale.x, getContext()->windowScale.y);
    }
};