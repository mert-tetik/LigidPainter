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

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <iostream>
#include <atomic>

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"

struct Context{
    LigidWindow window;
    glm::ivec2 windowScale;
};

Context* getContext();
Context* getCopyContext();
extern std::atomic<bool> mainThreadUsingCopyContext;

struct Camera{
    float yaw = -90.f;
    float pitch = 0.f;
    glm::vec3 cameraPos = glm::vec3(0,0,-10.f);
    glm::vec3 originPos = glm::vec3(0,0,0);
    float radius = 10.f; 

    bool isCamInverted(){
        return ((int)std::floor((std::abs(this->pitch) + 90.f) / 180.f) % 2 == 1);
    }

    void setCameraPosition(glm::vec3 pos){
        
        this->cameraPos = pos;

        this->radius = glm::distance(this->cameraPos, this->originPos);

        this->pitch = glm::degrees(asin((this->cameraPos.y - this->originPos.y) / -this->radius));
        this->yaw = glm::degrees(atan2((this->cameraPos.z - this->originPos.z) / (this->radius * cos(glm::radians(this->pitch))),
                                       (this->cameraPos.x - this->originPos.x) / (this->radius * cos(glm::radians(this->pitch)))));
    }

    void transition(glm::vec3 direction){
        glm::vec3 camPos;
        camPos = this->cameraPos;

        camPos.x += (direction.x - this->cameraPos.x) / 20.f;
        camPos.y += (direction.y - this->cameraPos.y) / 20.f;
        camPos.z += (direction.z - this->cameraPos.z) / 20.f;
    
        this->setCameraPosition(camPos);
    }
    
    void transition(glm::vec3 direction, glm::vec3 originPos){
        glm::vec3 camPos;
        camPos = this->cameraPos;
        
        this->originPos += (originPos - this->originPos) / 20.f; 
        
        camPos += (direction - this->cameraPos) / 20.f;
        this->setCameraPosition(camPos);
    }

    void posShortcutInteraction(bool active);

    bool XPLocked = false;
    bool XNLocked = false;
    bool YPLocked = false;
    bool YNLocked = false;
    bool ZPLocked = false;
    bool ZNLocked = false;
    bool originLocked = false;
};

struct Scene{
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 transformMatrix;

    float fov = 45.f;
    float aNear = 0.1f;
    float aFar = 1000.0f;

    glm::vec3 transformLocation = glm::vec3(0);
    glm::vec3 transformRotation = glm::vec3(0);

    bool useOrtho = false;

    void updateProjectionMatrix(){
        if(getContext()->windowScale.x){
            float videoRatio = (float)getContext()->windowScale.x / (float)getContext()->windowScale.y;
            if(!useOrtho){
                this->projectionMatrix = glm::perspective(glm::radians(this->fov), 
                                                    videoRatio, //Since the ratio is determined by the window scale, 3D Model won't be stretched by window resizing.
                                                    this->aNear, 
                                                    this->aFar);
            }
            else{
                this->projectionMatrix = glm::ortho((-this->fov * videoRatio)/10.f * this->camera.radius, (this->fov * videoRatio)/10.f * this->camera.radius, (-this->fov)/10.f * this->camera.radius, (this->fov)/10.f * this->camera.radius, (this->aNear), (this->aFar));
            }
        }
    }

    glm::mat4 calculateViewMatrix(glm::vec3 camPos, glm::vec3 originPos){
        glm::mat4 viewMat;
        if(this->camera.isCamInverted()){
            viewMat = glm::lookAt( 
                                            camPos, 
                                            originPos, 
                                            glm::vec3(0.0, -1.0, 0.0)
                                        );
        }
        else{
            viewMat = glm::lookAt( 
                                            camPos, 
                                            originPos, 
                                            glm::vec3(0.0, 1.0, 0.0)
                                        );
        }

        return viewMat;
    }

    void updateViewMatrix(glm::vec3 camPos, glm::vec3 originPos){
        this->viewMatrix = calculateViewMatrix(camPos, originPos);
    }

    void updateViewMatrix(){
        updateViewMatrix(this->camera.cameraPos, this->camera.originPos);
    }

    void updateTransformMatrix(){
        this->transformMatrix = glm::mat4(1.f);
        this->transformMatrix = glm::translate(this->transformMatrix, transformLocation);
        this->transformMatrix = glm::rotate(this->transformMatrix, glm::radians(transformRotation.x), glm::vec3(0.f, 1.f, 0.f));
        this->transformMatrix = glm::rotate(this->transformMatrix, glm::radians(transformRotation.y), glm::vec3(1.f, 0.f, 0.f));
        this->transformMatrix = glm::rotate(this->transformMatrix, glm::radians(transformRotation.z), glm::vec3(0.f, 0.f, 1.f));
    }

    bool renderTiles = true;
    SceneTiles tiles;
    
    bool renderAxisDisplayer = false;
    SceneAxisDisplayer axisDisplayer;

    Camera camera;
};

Scene* getScene();
Model* getModel();
Model* getSphereModel();
Model* getMaterialDisplayerModel();
Box* getBox();

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
    
    private:
        unsigned int orgID;
    };

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
        Texture bakingIcon;
        Texture trashIcon;
        Texture materialChannelsIcon;
        Texture paintingOverIcon;
        Texture mirrorIcon;
        
        //--Textures
        Texture greetingDialogImage;  
        Texture filterDisplayerImage;  
        Texture noMaterialConnectedToTheMeshWarningImage;  
        Texture noMaterialModifierIsConnectedToMaterialWarningImage;
        Texture materialChannelMissingTexture;
        
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

    struct Properties{
        /*! @brief Set swapInterval(1) if true */
        bool VSync = false; 

        bool backfaceCulling = false;

        float framebufferResolutionDivier = 1.f;
        float paintingResolutionDivier = 1.f;
        float paintingDepthTextureResolutionDivier = 1.f;

        /*! @brief Textures will be generated with that resolution value */
        int textureRes = 1024; 

        int useHeightMap;
        float heightMapStrength;

        bool cat_allowComments = false;
        bool cat_verifyTheExit = true;
        bool cat_hide = false;
    };

    void loadAppTextures();

    AppTextures appTextures();

    Properties* properties();

    glm::vec2* videoScale();

    DefaultFramebuffer* defaultFramebuffer();
};
#endif