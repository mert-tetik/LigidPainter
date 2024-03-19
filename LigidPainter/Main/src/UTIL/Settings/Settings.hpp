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
#include "3D/ThreeD.hpp"
#include "Toolkit/VectorScene/VectorScene.hpp"
#include "Toolkit/TextureFieldScene/TextureFieldScene.hpp"

struct Context{
    LigidWindow window;
    glm::ivec2 windowScale;
};

class Gizmo;

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

    void setCameraRadius(float radius){
        this->radius = radius;
        
        //The distance between the camera & center 
        float originCameraDistance = glm::distance(this->originPos,this->cameraPos)/10;

        //Zoom in-out
        this->cameraPos.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)) * this->radius + this->originPos.x;
        this->cameraPos.y = sin(glm::radians(this->pitch)) * -this->radius + this->originPos.y;
        this->cameraPos.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)) * this->radius + this->originPos.z;
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

    /*! @brief UI Rendering projection
    L = 0, R = 1920, B = 1080, T = 0  */
    glm::mat4 gui_projection; 

    float fov = 45.f;
    float aNear = 0.1f;
    float aFar = 1000.0f;

    glm::vec3 transformLocation = glm::vec3(0);
    glm::vec3 transformRotation = glm::vec3(0);

    bool useOrtho = false;

    void updateGUIProjectionMatrix(float window_width, float window_height){
        //Update the UI projection using window size
        this->gui_projection = glm::ortho(0.f, window_width, window_height, 0.f);
    }

    void updateProjectionMatrix(float ratio){
        if(getContext()->windowScale.x){
            if(ratio == 0.f)
                ratio = (float)getContext()->windowScale.x / (float)getContext()->windowScale.y;
            if(!useOrtho){
                this->projectionMatrix = glm::perspective(glm::radians(this->fov), 
                                                    ratio, //Since the ratio is determined by the window scale, 3D Model won't be stretched by window resizing.
                                                    this->aNear, 
                                                    this->aFar);
            }
            else{
                this->projectionMatrix = glm::ortho((-this->fov * ratio)/10.f * this->camera.radius, (this->fov * ratio)/10.f * this->camera.radius, (-this->fov)/10.f * this->camera.radius, (this->fov)/10.f * this->camera.radius, (this->aNear), (this->aFar));
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

    bool backfaceCulling = false;
    bool useHeightMap = false;
    float heightMapStrength = 0.f;

    Camera camera;
};

Scene* getScene();
Model* getModel();
void setModel(Model* model);
Model* getSphereModel();
Model* getPlaneModel();
Model* getMaterialDisplayerModel();
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
    Texture noMaterialConnectedToTheMeshWarningImage;  
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

extern AppTextures appTextures;

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


    struct Properties{
        /*! @brief Set swapInterval(1) if true */
        bool VSync = false; 

        float framebufferResolutionDivier = 1.f;

        bool cat_allowComments = false;
        bool cat_verifyTheExit = true;
        bool cat_hide = false;
    };

    void loadAppTextures();

    Properties* properties();

    glm::vec2* videoScale();

    DefaultFramebuffer* defaultFramebuffer();
};
#endif