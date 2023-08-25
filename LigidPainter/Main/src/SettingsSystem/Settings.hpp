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

namespace Settings{
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
        
        //--Textures
        Texture greetingDialogImage;  
        Texture noMaterialConnectedToTheMeshWarningImage;  
    };

    struct Properties{
        /*! @brief Set swapInterval(1) if true */
        bool VSync = false; 

        bool backfaceCulling = false;

        /*! @brief Textures will be generated with that resolution value */
        int textureRes = 1024; 

        int useHeightMap;
        float heightMapStrength;
    };

    void loadAppTextures();

    AppTextures appTextures();

    Properties* properties();

    glm::vec2* videoScale();

};
#endif