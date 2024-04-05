/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef THREED_CAMERA_HPP
#define THREED_CAMERA_HPP

#include <iostream>
#include <atomic>

#include "3D/ThreeD.hpp"

struct Camera{
    float yaw = -90.f;
    float pitch = 0.f;
    glm::vec3 cameraPos = glm::vec3(0,0,-10.f);
    glm::vec3 originPos = glm::vec3(0,0,0);
    float radius = 10.f; 

    glm::mat4 viewMatrix;

    bool isCamInverted();

    void setCameraPosition(glm::vec3 pos);
    void setCameraRadius(float radius);

    void interaction(float scroll, glm::vec2 mouseOffset);

    void transition(glm::vec3 direction);
    void transition(glm::vec3 direction, glm::vec3 originPos);

    glm::mat4 calculateViewMatrix(glm::vec3 camPos, glm::vec3 originPos);
    void updateViewMatrix(glm::vec3 camPos, glm::vec3 originPos);
    void updateViewMatrix();

    void posShortcutInteraction(bool active);

    bool XPLocked = false;
    bool XNLocked = false;
    bool YPLocked = false;
    bool YNLocked = false;
    bool ZPLocked = false;
    bool ZNLocked = false;
    bool originLocked = false;

    bool operator==(const Camera& otherCam) const;
    bool operator!=(const Camera& otherCam) const;
};

#endif