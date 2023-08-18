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

struct Camera{
    float yaw = -90.f;
    float pitch = 0.f;
    glm::vec3 cameraPos = glm::vec3(0,0,-10.f);
    glm::vec3 originPos = glm::vec3(0,0,0);
    float radius = 10.f; 
};

struct Scene{
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;

    const float fov = 45.f;
    const float aNear = 0.1f;
    const float aFar = 1000.0f;

    Camera camera;
};

struct Context{
    LigidWindow window;
    glm::ivec2 windowScale;
};

Context* getContext();
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
    };

    void loadAppTextures();

    AppTextures appTextures();

    Properties* properties();

    glm::vec2* videoScale();

};
#endif