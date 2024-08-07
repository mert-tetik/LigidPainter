/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include "UTIL/Util.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/GL/GL.hpp"

#include "3D/ThreeD.hpp"

#include "GUI/GUI.hpp"
#include "GUI/Panels.hpp"

#include "Toolkit/VectorScene/VectorScene.hpp"

#include <vector>
#include <string>

AppTextures appTextures;
AppVideos appVideos;
Settings::Properties __settingsProperties;

Context __context; 
Context __copyContext; 
Context __secondContext; 
Context __loadingContext; 
Scene __scene; //3D Scene structure
VectorScene __vectorScene;
TextureFieldScene __textureFieldScene;
Model __sphereModel;
Model __planeModel;
Model __materialDisplayerModel;
Model __materialDisplayingModel;
Model __getTDBrushCursorModel;
Box __box;
Box __box_2;
Box __box_3;
Settings::DefaultFramebuffer __defaultFramebuffer;
Timer __timer;    

glm::vec2 __videoScale;

void Settings::initAppTextures(){
    glGenTextures(1, &appTextures.TDModelIcon.ID);
    glGenTextures(1, &appTextures.softenIcon.ID);
    glGenTextures(1, &appTextures.smearIcon.ID);
    glGenTextures(1, &appTextures.modifierIcon.ID);
    glGenTextures(1, &appTextures.environmentIcon.ID);
    glGenTextures(1, &appTextures.dropperIcon.ID);
    glGenTextures(1, &appTextures.colorIcon.ID);
    glGenTextures(1, &appTextures.brushIcon.ID);
    glGenTextures(1, &appTextures.textureIcon.ID);
    glGenTextures(1, &appTextures.arrowR.ID);
    glGenTextures(1, &appTextures.arrowL.ID);
    glGenTextures(1, &appTextures.arrowB.ID);
    glGenTextures(1, &appTextures.arrowT.ID);
    glGenTextures(1, &appTextures.threeDIcon.ID);
    glGenTextures(1, &appTextures.twoDIcon.ID);
    glGenTextures(1, &appTextures.exportIcon.ID);
    glGenTextures(1, &appTextures.areaPaintingIcon.ID);
    glGenTextures(1, &appTextures.gearIcon.ID);
    glGenTextures(1, &appTextures.nodeDisplayingMode.ID);
    glGenTextures(1, &appTextures.paintingDisplayingMode.ID);
    glGenTextures(1, &appTextures.solidPaintingDisplayingMode.ID);
    glGenTextures(1, &appTextures.ligidPainterIcon.ID);
    glGenTextures(1, &appTextures.logButtonL.ID);
    glGenTextures(1, &appTextures.logButtonR.ID);
    glGenTextures(1, &appTextures.logButtonM.ID);
    glGenTextures(1, &appTextures.colorGearIcon.ID);
    glGenTextures(1, &appTextures.filterIcon.ID);
    glGenTextures(1, &appTextures.resizingIcon.ID);
    glGenTextures(1, &appTextures.blurIcon.ID);
    glGenTextures(1, &appTextures.distortionIcon.ID);
    glGenTextures(1, &appTextures.normalMapIcon.ID);
    glGenTextures(1, &appTextures.packageIcon.ID);
    glGenTextures(1, &appTextures.inkPenIcon.ID);
    glGenTextures(1, &appTextures.bucketIcon.ID);
    glGenTextures(1, &appTextures.bakingIcon.ID);
    glGenTextures(1, &appTextures.materialIcon.ID);
    glGenTextures(1, &appTextures.trashIcon.ID);
    glGenTextures(1, &appTextures.materialChannelsIcon.ID);
    glGenTextures(1, &appTextures.paintingOverIcon.ID);
    glGenTextures(1, &appTextures.mirrorIcon.ID);
    glGenTextures(1, &appTextures.eyeClosedIcon.ID);
    glGenTextures(1, &appTextures.eyeOpenedIcon.ID);
    glGenTextures(1, &appTextures.stripes1Icon.ID);
    glGenTextures(1, &appTextures.stripes2Icon.ID);
    glGenTextures(1, &appTextures.X.ID);
    glGenTextures(1, &appTextures.flipH.ID);
    glGenTextures(1, &appTextures.flipV.ID);
    glGenTextures(1, &appTextures.plus.ID);
    glGenTextures(1, &appTextures.minus.ID);
    glGenTextures(1, &appTextures.greetingDialogImage.ID);
    glGenTextures(1, &appTextures.newVersionBanner.ID);
    glGenTextures(1, &appTextures.filterDisplayerImage.ID);
    glGenTextures(1, &appTextures.noLayersWarningTexture.ID);
    glGenTextures(1, &appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.ID);
    glGenTextures(1, &appTextures.materialChannelMissingTexture.ID);
    glGenTextures(1, &appTextures.baking_LigidRay.ID);
    glGenTextures(1, &appTextures.baking_Default.ID);
    glGenTextures(1, &appTextures.mascotCat_default.ID);
    glGenTextures(1, &appTextures.mascotCat_smile.ID);
    glGenTextures(1, &appTextures.mascotCat_rock.ID);
    glGenTextures(1, &appTextures.mascotCat_relaxed.ID);
    glGenTextures(1, &appTextures.mascotCat_dizzy.ID);
    glGenTextures(1, &appTextures.mascotCat_thinking.ID);
    glGenTextures(1, &appTextures.mascotCat_bread.ID);
    glGenTextures(1, &appTextures.mascotCat_crying.ID);
    glGenTextures(1, &appTextures.mascotCat_sleeping.ID);
    glGenTextures(1, &appTextures.mascotCat_pawL.ID);
    glGenTextures(1, &appTextures.mascotCat_pawR.ID);
    glGenTextures(1, &appTextures.mascotCat_speechBubble.ID);
    
    glGenTextures(1, &appTextures.loadingIcon.ID);
    glGenTextures(1, &appTextures.historyIcon.ID);
    glGenTextures(1, &appTextures.historyLibraryIcon.ID);
    glGenTextures(1, &appTextures.messageIcon.ID);

    char whitePxs[256 * 256 * 4];
    char blackPxs[256 * 256 * 4];

    for (int i = 0; i < 256 * 256 * 4; i += 4) {
        whitePxs[i] = 127;
        whitePxs[i + 1] = 127;
        whitePxs[i + 2] = 127;
        whitePxs[i + 3] = 127;

        blackPxs[i] = 0;
        blackPxs[i + 1] = 0;
        blackPxs[i + 2] = 0;
        blackPxs[i + 3] = 127;
    }

    appTextures.white = Texture(whitePxs, 256, 256, GL_NEAREST);
    appTextures.white.proceduralProps.proceduralID = 24;

    appTextures.black = Texture(blackPxs, 256, 256, GL_NEAREST);
    appTextures.black.proceduralProps.proceduralID = 24;
    appTextures.black.proceduralProps.proceduralnverted = true;
    
    appTextures.transparent = Texture((char*)nullptr, 1, 1, GL_NEAREST);
}

void Settings::loadAppTextures(){
    appTextures.TDModelIcon.load("./LigidPainter/Resources/Icons/TDModel.png");
    appTextures.softenIcon.load("./LigidPainter/Resources/Icons/Soften.png");
    appTextures.smearIcon.load("./LigidPainter/Resources/Icons/Smear.png");
    appTextures.modifierIcon.load("./LigidPainter/Resources/Icons/Modifier.png");
    appTextures.environmentIcon.load("./LigidPainter/Resources/Icons/Environment.png");
    appTextures.dropperIcon.load("./LigidPainter/Resources/Icons/Dropper.png");
    appTextures.colorIcon.load("./LigidPainter/Resources/Icons/Color.png");
    appTextures.brushIcon.load("./LigidPainter/Resources/Icons/Brush.png");
    appTextures.textureIcon.load("./LigidPainter/Resources/Icons/Texture.png");
    appTextures.arrowR.load("./LigidPainter/Resources/Icons/ArrowR.png");
    appTextures.arrowL.load("./LigidPainter/Resources/Icons/ArrowL.png");
    appTextures.arrowB.load("./LigidPainter/Resources/Icons/ArrowB.png");
    appTextures.arrowT.load("./LigidPainter/Resources/Icons/ArrowT.png");
    appTextures.threeDIcon.load("./LigidPainter/Resources/Icons/3D.png");
    appTextures.twoDIcon.load("./LigidPainter/Resources/Icons/2D.png");
    appTextures.exportIcon.load("./LigidPainter/Resources/Icons/Export.png");
    appTextures.areaPaintingIcon.load("./LigidPainter/Resources/Icons/Area.png");
    appTextures.gearIcon.load("./LigidPainter/Resources/Icons/Gear.png");
    appTextures.nodeDisplayingMode.load("./LigidPainter/Resources/Icons/NodeDisplayingMode.png");
    appTextures.paintingDisplayingMode.load("./LigidPainter/Resources/Icons/PaintingDisplayingMode.png");
    appTextures.solidPaintingDisplayingMode.load("./LigidPainter/Resources/Icons/SolidPaintingDisplayingMode.png");
    appTextures.ligidPainterIcon.load("./LigidPainter/Resources/Icons/LigidPainter.png");
    appTextures.logButtonL.load("./LigidPainter/Resources/Icons/LogButtonL.png");
    appTextures.logButtonR.load("./LigidPainter/Resources/Icons/LogButtonR.png");
    appTextures.logButtonM.load("./LigidPainter/Resources/Icons/LogButtonM.png");
    appTextures.colorGearIcon.load("./LigidPainter/Resources/Icons/ColorGear.png");
    appTextures.filterIcon.load("./LigidPainter/Resources/Icons/Filter.png");
    appTextures.resizingIcon.load("./LigidPainter/Resources/Icons/Resizing.png");
    appTextures.blurIcon.load("./LigidPainter/Resources/Icons/Blur.png");
    appTextures.distortionIcon.load("./LigidPainter/Resources/Icons/Distortion.png");
    appTextures.normalMapIcon.load("./LigidPainter/Resources/Icons/NormalMap.png");
    appTextures.packageIcon.load("./LigidPainter/Resources/Icons/Package.png");
    appTextures.inkPenIcon.load("./LigidPainter/Resources/Icons/InkPen.png");
    appTextures.bucketIcon.load("./LigidPainter/Resources/Icons/bucket.png");
    appTextures.bakingIcon.load("./LigidPainter/Resources/Icons/Baking.png");
    appTextures.materialIcon.load("./LigidPainter/Resources/Icons/Material.png");
    appTextures.trashIcon.load("./LigidPainter/Resources/Icons/Trash.png");
    appTextures.materialChannelsIcon.load("./LigidPainter/Resources/Icons/MaterialChannels.png");
    appTextures.paintingOverIcon.load("./LigidPainter/Resources/Icons/PaintingOver.png");
    appTextures.mirrorIcon.load("./LigidPainter/Resources/Icons/Mirror.png");
    appTextures.eyeClosedIcon.load("./LigidPainter/Resources/Icons/Eye_Closed.png");
    appTextures.eyeOpenedIcon.load("./LigidPainter/Resources/Icons/Eye_Open.png");
    appTextures.stripes1Icon.load("./LigidPainter/Resources/Icons/Stripes1.png");
    appTextures.stripes2Icon.load("./LigidPainter/Resources/Icons/Stripes2.png");
    appTextures.X.load("./LigidPainter/Resources/Icons/X.png");
    appTextures.flipH.load("./LigidPainter/Resources/Icons/FlipH.png");
    appTextures.flipV.load("./LigidPainter/Resources/Icons/FlipV.png");
    appTextures.plus.load("./LigidPainter/Resources/Icons/Plus.png");
    appTextures.minus.load("./LigidPainter/Resources/Icons/Minus.png");
    appTextures.greetingDialogImage.load("./LigidPainter/Resources/Images/greetingDialogImage.jpg");
    appTextures.newVersionBanner.load("./LigidPainter/Resources/Images/newVersionBanner.jpg");
    appTextures.filterDisplayerImage.load("./LigidPainter/Resources/Images/FilterDisplayerImage.jpg");
    appTextures.noLayersWarningTexture.load("./LigidPainter/Resources/Images/NoLayersWarning.jpg");
    appTextures.noMaterialModifierIsConnectedToMaterialWarningImage.load("./LigidPainter/Resources/Images/NoMaterialModifierIsConnectedToMaterial.jpg");
    appTextures.materialChannelMissingTexture.load("./LigidPainter/Resources/Images/MaterialChannelMissingTexture.jpg");
    appTextures.loadingIcon.load("./LigidPainter/Resources/Icons/Loading.png");
    appTextures.historyIcon.load("./LigidPainter/Resources/Icons/History.png");
    appTextures.historyLibraryIcon.load("./LigidPainter/Resources/Icons/HistoryLibrary.png");
    appTextures.messageIcon.load("./LigidPainter/Resources/Icons/Message.png");
    appTextures.baking_LigidRay.load("./LigidPainter/Resources/Images/Baking_LigidRay.jpg");
    appTextures.baking_Default.load("./LigidPainter/Resources/Images/Baking_Default.jpg");
    appTextures.mascotCat_default.load("./LigidPainter/Resources/Images/Mascot_Cat_Sprites/default.png");
    appTextures.mascotCat_smile.load("./LigidPainter/Resources/Images/Mascot_Cat_Sprites/smiling.png");
    appTextures.mascotCat_rock.load("./LigidPainter/Resources/Images/Mascot_Cat_Sprites/rock.png");
    appTextures.mascotCat_relaxed.load("./LigidPainter/Resources/Images/Mascot_Cat_Sprites/relaxed.png");
    appTextures.mascotCat_dizzy.load("./LigidPainter/Resources/Images/Mascot_Cat_Sprites/dizzy.png");
    appTextures.mascotCat_thinking.load("./LigidPainter/Resources/Images/Mascot_Cat_Sprites/thinking.png");
    appTextures.mascotCat_bread.load("./LigidPainter/Resources/Images/Mascot_Cat_Sprites/bread.png");
    appTextures.mascotCat_crying.load("./LigidPainter/Resources/Images/Mascot_Cat_Sprites/crying.png");
    appTextures.mascotCat_sleeping.load("./LigidPainter/Resources/Images/Mascot_Cat_Sprites/sleeping.png");
    appTextures.mascotCat_pawL.load("./LigidPainter/Resources/Images/Mascot_Cat_Sprites/pawL.png");
    appTextures.mascotCat_pawR.load("./LigidPainter/Resources/Images/Mascot_Cat_Sprites/pawR.png");
    appTextures.mascotCat_speechBubble.load("./LigidPainter/Resources/Images/Mascot_Cat_Sprites/SpeechBubble.png");

}

void Settings::loadAppVideos(){
    appVideos.loading.load_folder("LigidPainter/Resources/Videos/Loading Animation", 15);
}

Context* getContext(){
    return &__context;
}
Context* getCopyContext(){
    return &__copyContext;
}
Context* getSecondContext(){
    return &__secondContext;
}
Context* getLoadingContext(){
    return &__loadingContext;
}
Scene* getScene(){
    return &__scene;
}

VectorScene* getVectorScene(){
    return &__vectorScene;
} 
TextureFieldScene* getTextureFieldScene(){
    return &__textureFieldScene;
}
Model* getSphereModel(){
    return &__sphereModel;
}
Model* getPlaneModel(){
    return &__planeModel;
}
Model* getMaterialDisplayerModel(){
    return &__materialDisplayerModel;
}
Model* getMaterialDisplayingModel(){
    return &__materialDisplayingModel;
}
Model* getTDBrushCursorModel(){
    return &__getTDBrushCursorModel;
}
Box* getBox(){
    if(getContext()->window.isContextCurrent())
        return &__box;
    if(getSecondContext()->window.isContextCurrent())
        return &__box_2;
    if(getLoadingContext()->window.isContextCurrent())
        return &__box_3;

    return &__box;
}

Timer* getTimer(){
    return &__timer;
}

namespace Settings{
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
        glDisable(GL_DEPTH_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, getContext()->windowScale.x, getContext()->windowScale.y);

        ShaderSystem::defaultFramebufferShader().use();
        ShaderSystem::defaultFramebufferShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
        ShaderSystem::defaultFramebufferShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
        ShaderSystem::defaultFramebufferShader().setVec2("scale", glm::vec2(0.5f));
        
        ShaderSystem::defaultFramebufferShader().setVec2("resolution", this->resolution);
        ShaderSystem::defaultFramebufferShader().setInt("txtr", GL::bindTexture_2D(this->FBO.colorBuffer.ID, "DefaultFramebuffer::render"));
        
        getBox()->draw("DefaultFramebuffer::render");

        GL::releaseBoundTextures("DefaultFramebuffer::render");
        glBindFramebuffer(GL_FRAMEBUFFER, this->FBO.ID);
        
        glEnable(GL_DEPTH_TEST);
    }

    void DefaultFramebuffer::init(glm::ivec2 resolution){
        this->resolution = resolution;

        glActiveTexture(GL_TEXTURE0);

        //--------- init bgTxtr --------- 
        this->bgTxtr = Texture((char*)nullptr, resolution.x, resolution.y, GL_NEAREST);
        
        //--------- init FBO --------- 
        this->FBO = Framebuffer(Texture((char*)nullptr, resolution.x, resolution.y, GL_NEAREST), GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, resolution), "Default FBO");

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
        this->FBO.colorBuffer.update((char*)nullptr, resolution.x, resolution.y, GL_NEAREST);

        //--------- update bgTxtr --------- 
        bgTxtr.resize(glm::ivec2(resolution.x, resolution.y));

        //--------- update RBO --------- 
        this->FBO.renderBuffer.update(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, resolution);
    }

    void DefaultFramebuffer::setViewport(){
        if(this->FBO.ID != 0)
            glViewport(0, 0, this->resolution.x, this->resolution.y);
        else
            glViewport(0, 0, getContext()->windowScale.x, getContext()->windowScale.y);
    }

    void DefaultFramebuffer::update_bg_txtr(){
        Debugger::block("DefaultFramebuffer::update_bg_txtr"); // Start

        if(Settings::defaultFramebuffer()->FBO.ID != 0)
            Settings::defaultFramebuffer()->render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Create a framebuffer object (FBO)
        GLuint framebuffer;
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Settings::defaultFramebuffer()->bgTxtr.ID, 0);

        // Set up the blit
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); // Bind the default framebuffer as the source
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer); // Bind the FBO as the destination
        glBlitFramebuffer(0, 0, getContext()->windowScale.x, getContext()->windowScale.y, 0, 0, Settings::defaultFramebuffer()->bgTxtr.getResolution().x, Settings::defaultFramebuffer()->bgTxtr.getResolution().y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // Unbind the FBO
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Clean up resources (delete framebuffer and unneeded textures if necessary)
        glDeleteFramebuffers(1, &framebuffer);

        Debugger::block("DefaultFramebuffer::update_bg_txtr"); // End
    }
};