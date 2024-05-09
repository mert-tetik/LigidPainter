/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <iostream>
#include <atomic>

#include "UTIL/Util.hpp"
#include "UTIL/Video/Video.hpp"

#include "3D/ThreeD.hpp"
#include "3D/Scene/Scene.hpp"

#include "Toolkit/VectorScene/VectorScene.hpp"
#include "Toolkit/TextureFieldScene/TextureFieldScene.hpp"

struct Scene;

struct Context{
    LigidWindow window;
    glm::ivec2 windowScale;
    glm::mat4 ortho_projection;

    void updateGUIProjectionMatrix(float window_width, float window_height){
        //Update the UI projection using window size
        this->ortho_projection = glm::ortho(0.f, window_width, window_height, 0.f);
    }
};

class Gizmo;

Context* getContext();
Context* getCopyContext();
Context* getSecondContext();
Context* getLoadingContext();

Scene* getScene();
Model* getSphereModel();
Model* getPlaneModel();
Model* getMaterialDisplayerModel();
Model* getMaterialDisplayingModel();
Model* getTDBrushCursorModel();
Box* getBox();
VectorScene* getVectorScene();
TextureFieldScene* getTextureFieldScene();

struct AppTextures{ 
    //--Icons
    Texture TDModelIcon; 
    Texture softenIcon;  
    Texture smearIcon;
    Texture modifierIcon;
    Texture environmentIcon;
    Texture dropperIcon;
    Texture colorIcon;
    Texture brushIcon;
    Texture textureIcon;
    Texture arrowR;
    Texture arrowL;
    Texture arrowB;
    Texture arrowT;
    Texture threeDIcon;
    Texture twoDIcon;
    Texture exportIcon;
    Texture areaPaintingIcon;
    Texture gearIcon;
    Texture ligidPainterIcon;
    Texture nodeDisplayingMode;
    Texture paintingDisplayingMode;
    Texture solidPaintingDisplayingMode;
    Texture logButtonL;
    Texture logButtonR;
    Texture logButtonM;
    Texture colorGearIcon;
    Texture filterIcon;
    Texture resizingIcon;
    Texture blurIcon;
    Texture distortionIcon;
    Texture normalMapIcon;
    Texture packageIcon;
    Texture inkPenIcon;
    Texture bucketIcon;
    Texture bakingIcon;
    Texture materialIcon;
    Texture trashIcon;
    Texture eyeClosedIcon;
    Texture eyeOpenedIcon;
    Texture stripes1Icon;
    Texture stripes2Icon;
    Texture materialChannelsIcon;
    Texture paintingOverIcon;
    Texture mirrorIcon;
    Texture X;
    Texture flipH;
    Texture flipV;
    Texture plus;
    Texture minus;
    
    //--Textures
    Texture greetingDialogImage;  
    Texture newVersionBanner;  
    Texture filterDisplayerImage;  
    Texture noLayersWarningTexture;  
    Texture noMaterialModifierIsConnectedToMaterialWarningImage;
    Texture materialChannelMissingTexture;
    Texture baking_LigidRay;
    Texture baking_Default;
    
    //--SYSTEM
    Texture white;
    Texture black;
    Texture transparent;

    //--MASCOT CAT SPRITES
    Texture mascotCat_default;
    Texture mascotCat_smile;
    Texture mascotCat_rock;
    Texture mascotCat_relaxed;
    Texture mascotCat_dizzy;
    Texture mascotCat_thinking;
    Texture mascotCat_bread;
    Texture mascotCat_crying;
    Texture mascotCat_sleeping;
    Texture mascotCat_pawL;
    Texture mascotCat_pawR;
};

struct AppVideos{
    Video loading;
};

extern AppTextures appTextures;
extern AppVideos appVideos;

namespace Settings{

    struct DefaultFramebuffer{
    public:
        Framebuffer FBO;
        Texture bgTxtr;

        glm::ivec2 resolution;

        void init(glm::ivec2 resolution);

        void setResolution(glm::ivec2 resolution);

        void setViewport();

        void render();

        void update_bg_txtr();
    
    private:
        unsigned int orgID;
    };


    struct Properties{
        /*! @brief Set swapInterval(1) if true */
        bool VSync = false; 

        float framebufferResolutionDivier = 1.f;

        bool cat_allowComments = false;
        bool cat_verifyTheExit = true;
        bool cat_hide = false;
    };

    void loadAppTextures();
    void loadAppVideos();

    Properties* properties();

    glm::vec2* videoScale();

    DefaultFramebuffer* defaultFramebuffer();
};
#endif