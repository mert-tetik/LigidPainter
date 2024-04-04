/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <iostream>

#include "UTIL/Util.hpp"

#include "Toolkit/VectorScene/VectorScene.hpp"

#include "GUI/Panels.hpp"

#include "3D/ThreeD.hpp"
#include "3D/Camera/Camera.hpp"

bool Camera::isCamInverted(){
    return ((int)std::floor((std::abs(this->pitch) + 90.f) / 180.f) % 2 == 1);
}

void Camera::setCameraPosition(glm::vec3 pos){
    
    this->cameraPos = pos;

    this->radius = glm::distance(this->cameraPos, this->originPos);

    this->pitch = glm::degrees(asin((this->cameraPos.y - this->originPos.y) / -this->radius));
    this->yaw = glm::degrees(atan2((this->cameraPos.z - this->originPos.z) / (this->radius * cos(glm::radians(this->pitch))),
                                    (this->cameraPos.x - this->originPos.x) / (this->radius * cos(glm::radians(this->pitch)))));
}

void Camera::setCameraRadius(float radius){
    this->radius = radius;
    
    //The distance between the camera & center 
    float originCameraDistance = glm::distance(this->originPos,this->cameraPos)/10;

    //Zoom in-out
    this->cameraPos.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)) * this->radius + this->originPos.x;
    this->cameraPos.y = sin(glm::radians(this->pitch)) * -this->radius + this->originPos.y;
    this->cameraPos.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)) * this->radius + this->originPos.z;
}

void Camera::interaction(float scroll, glm::vec2 mouseOffset){
    const float sensitivity = 0.14f; //Mouse sensivity 

    bool moved = false;

    bool mouseR_CTRL = getContext()->window.isMouseButtonPressed(LIGIDGL_MOUSE_BUTTON_RIGHT) == LIGIDGL_PRESS && getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_PRESS; 
    bool mouseR = getContext()->window.isMouseButtonPressed(LIGIDGL_MOUSE_BUTTON_RIGHT) == LIGIDGL_PRESS; 
    bool mouseM = getContext()->window.isMouseButtonPressed(LIGIDGL_MOUSE_BUTTON_MIDDLE) == LIGIDGL_PRESS; 

    if(this->isCamInverted())
        mouseOffset.x *= -1;

    if((mouseR_CTRL || mouseM) && (mouseOffset.x || mouseOffset.y))
    {
        moved = true;

        // Straight Movement
        float straightMovSensivity = glm::distance(this->originPos, this->cameraPos)/10;

        // Calculate half the sensitivity for later use
        const float half_sensitivity = sensitivity / 2.0f;

        // Calculate sine and cosine of yaw and pitch angles in radians
        const float sin_yaw = sin(glm::radians(this->yaw));
        const float cos_yaw = cos(glm::radians(this->yaw));
        const float sin_pitch = sin(glm::radians(this->pitch));
        const float cos_pitch = cos(glm::radians(this->pitch));

        // Calculate the x and z offsets based on yaw angle, mouse movement, sensitivity, and half sensitivity
        float x_offset = sin_yaw * mouseOffset.x * sensitivity * straightMovSensivity * half_sensitivity;
        float z_offset = cos_yaw * mouseOffset.x * sensitivity * straightMovSensivity * half_sensitivity;

        // Check if pitch is greater than 60 degrees or less than -60 degrees
        if (this->pitch > 60.0f || this->pitch < -60.0f) {
            
            // Add additional x and z offsets based on yaw, pitch, mouse movement, sensitivity, and half sensitivity
            x_offset += cos_yaw * sin_pitch * mouseOffset.y * sensitivity * straightMovSensivity * half_sensitivity;
            z_offset -= sin_yaw * sin_pitch * mouseOffset.y * sensitivity * straightMovSensivity * half_sensitivity;
        }

        // Calculate the y offset based on pitch, mouse movement, sensitivity, and half sensitivity
        const float y_offset = cos_pitch * mouseOffset.y * sensitivity * straightMovSensivity * half_sensitivity;

        // Update camera's x position and origin position by subtracting x offset
        this->cameraPos.x -= x_offset;
        this->originPos.x -= x_offset;

        // Update camera's z position and origin position by adding z offset
        this->cameraPos.z += z_offset;
        this->originPos.z += z_offset;

        // Update camera's y position and origin position by adding y offset
        this->cameraPos.y += y_offset;
        this->originPos.y += y_offset;
    }
    else{
        if(mouseR && (mouseOffset.x || mouseOffset.y)){
            moved = true;
            
            this->yaw += mouseOffset.x * sensitivity;
            this->pitch -= mouseOffset.y * sensitivity;
        }
        
        if(scroll != 0.f)
        {
            moved = true;

            //The distance between the camera & center 
            float originCameraDistance = glm::distance(this->originPos, this->cameraPos) / 10;

            //Change the distance between camera & center (radius)
            if (scroll > 0) {
                this->radius -= originCameraDistance;
            }
            else if (scroll < 0) {
                this->radius += originCameraDistance;
            }
        }

        //Helical Movement
        //Rotates the Camera in 3 axis using yaw, pitch & radius values
        this->cameraPos.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)) * this->radius + this->originPos.x;
        this->cameraPos.y = sin(glm::radians(this->pitch)) * -this->radius + this->originPos.y;
        this->cameraPos.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)) * this->radius + this->originPos.z;
    }

    if(moved){
        if(glm::distance(this->cameraPos, glm::vec3(10.f, 0.f, 0.f)) < 1.f)
            this->originLocked = false;
        this->XPLocked = false;
        this->XNLocked = false;
        this->YPLocked = false;
        this->YNLocked = false;
        this->ZPLocked = false;
        this->ZNLocked = false;

        this->updateViewMatrix();
    }

}

void Camera::transition(glm::vec3 direction){
    glm::vec3 camPos;
    camPos = this->cameraPos;

    camPos.x += (direction.x - this->cameraPos.x) / 20.f;
    camPos.y += (direction.y - this->cameraPos.y) / 20.f;
    camPos.z += (direction.z - this->cameraPos.z) / 20.f;

    this->setCameraPosition(camPos);
}

void Camera::transition(glm::vec3 direction, glm::vec3 originPos){
    glm::vec3 camPos;
    camPos = this->cameraPos;
    
    this->originPos += (originPos - this->originPos) / 20.f; 
    
    camPos += (direction - this->cameraPos) / 20.f;
    this->setCameraPosition(camPos);
}

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

        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_1) || getContext()->window.isKeyPressed(LIGIDGL_KEY_NUMPAD_1) || (gizmo_scene.rHover && *Mouse::LClick())){
            this->XPLocked = true;
            this->XNLocked = false;
            this->YPLocked = false;
            this->YNLocked = false;
            this->ZPLocked = false;
            this->ZNLocked = false;
        }
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_2) || getContext()->window.isKeyPressed(LIGIDGL_KEY_NUMPAD_2) || (gizmo_scene.rHoverN && *Mouse::LClick())){
            this->XNLocked = true;
            this->XPLocked = false;
            this->YPLocked = false;
            this->YNLocked = false;
            this->ZPLocked = false;
            this->ZNLocked = false;
            
        }
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_3) || getContext()->window.isKeyPressed(LIGIDGL_KEY_NUMPAD_3) || (gizmo_scene.gHover && *Mouse::LClick())){
            this->YPLocked = true;
            this->XNLocked = false;
            this->XPLocked = false;
            this->YNLocked = false;
            this->ZPLocked = false;
            this->ZNLocked = false;
        }
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_4) || getContext()->window.isKeyPressed(LIGIDGL_KEY_NUMPAD_4) || (gizmo_scene.gHoverN && *Mouse::LClick())){
            this->YNLocked = true;
            this->XNLocked = false;
            this->YPLocked = false;
            this->XPLocked = false;
            this->ZPLocked = false;
            this->ZNLocked = false;
        }
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_5) || getContext()->window.isKeyPressed(LIGIDGL_KEY_NUMPAD_5) || (gizmo_scene.bHover && *Mouse::LClick())){
            this->ZPLocked = true;
            this->XNLocked = false;
            this->YPLocked = false;
            this->YNLocked = false;
            this->XPLocked = false;
            this->ZNLocked = false;
        }
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_6) || getContext()->window.isKeyPressed(LIGIDGL_KEY_NUMPAD_6) || (gizmo_scene.bHoverN && *Mouse::LClick())){
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
        this->updateViewMatrix();
    }
    if(this->XPLocked){        
        this->transition(glm::vec3(10.f, 0.f, 0.f) + this->originPos);
        this->updateViewMatrix();
    }
    if(this->XNLocked){        
        this->transition(glm::vec3(-10.f, 0.f, 0.f) + this->originPos);
        this->updateViewMatrix();
    }
    if(this->YPLocked){        
        this->transition(glm::vec3(0.f, 10.f, 0.f) + this->originPos);
        this->updateViewMatrix();
    }
    if(this->YNLocked){        
        this->transition(glm::vec3(0.f, -10.f, 0.f) + this->originPos);
        this->updateViewMatrix();
    }
    if(this->ZPLocked){        
        this->transition(glm::vec3(0.f, 0.f, 10.f) + this->originPos);
        this->updateViewMatrix();
    }
    if(this->ZNLocked){        
        this->transition(glm::vec3(0.f, 0.f, -10.f) + this->originPos);
        this->updateViewMatrix();
    }
}

glm::mat4 Camera::calculateViewMatrix(glm::vec3 camPos, glm::vec3 originPos){
    glm::mat4 viewMat;
    if(this->isCamInverted()){
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

void Camera::updateViewMatrix(glm::vec3 camPos, glm::vec3 originPos){
    this->viewMatrix = calculateViewMatrix(camPos, originPos);
}

void Camera::updateViewMatrix(){
    updateViewMatrix(this->cameraPos, this->originPos);
}