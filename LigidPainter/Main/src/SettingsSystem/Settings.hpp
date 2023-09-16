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

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"

struct Context{
    LigidWindow window;
    glm::ivec2 windowScale;
};

Context* getContext();

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

    void updateViewMatrix(){
        if(this->camera.isCamInverted()){
            this->viewMatrix = glm::lookAt( 
                                            this->camera.cameraPos, 
                                            this->camera.originPos, 
                                            glm::vec3(0.0, -1.0, 0.0)
                                        );
        }
        else{
            this->viewMatrix = glm::lookAt( 
                                            this->camera.cameraPos, 
                                            this->camera.originPos, 
                                            glm::vec3(0.0, 1.0, 0.0)
                                        );
        }
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
        unsigned int FBO = 0;
        unsigned int RBO = 0;
        unsigned int colorBuffer = 0;

        glm::ivec2 resolution;

        void init(glm::ivec2 resolution){
            this->resolution = resolution;

            //--------- init colorBuffer --------- 
            glGenTextures(1, &this->colorBuffer);
            glBindTexture(GL_TEXTURE_2D, this->colorBuffer);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, resolution.x, resolution.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glGenerateMipmap(GL_TEXTURE_2D);

            //--------- init FBO --------- 
            glGenFramebuffers(1, &this->FBO);
            glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->colorBuffer, 0);

            //--------- init RBO --------- 
            glGenRenderbuffers(1,&RBO);
            glBindRenderbuffer(GL_RENDERBUFFER,RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resolution.x, resolution.y);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
        }

        void setResolution(glm::ivec2 resolution){
            this->resolution = resolution;

            //--------- update colorBuffer --------- 
            glBindTexture(GL_TEXTURE_2D, this->colorBuffer);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, resolution.x, resolution.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glGenerateMipmap(GL_TEXTURE_2D);

            //--------- update FBO --------- 
            glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->colorBuffer, 0);

            //--------- update RBO --------- 
            glBindRenderbuffer(GL_RENDERBUFFER,RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, resolution.x, resolution.y);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);
        }

        void setViewport(){
            glViewport(0, 0, this->resolution.x, this->resolution.y);
        }
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
        
        //--Textures
        Texture greetingDialogImage;  
        Texture filterDisplayerImage;  
        Texture noMaterialConnectedToTheMeshWarningImage;  
        Texture noMaterialModifierIsConnectedToMaterialWarningImage;
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
    };

    void loadAppTextures();

    AppTextures appTextures();

    Properties* properties();

    glm::vec2* videoScale();

    DefaultFramebuffer* defaultFramebuffer();
};
#endif