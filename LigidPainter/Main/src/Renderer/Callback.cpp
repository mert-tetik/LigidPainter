/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Callback.cpp - Contains the callback functions

*/

#include <iostream>
#include <filesystem>

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include "../../thirdparty/include/glm/glm.hpp"
#include "../../thirdparty/include/glm/gtc/matrix_transform.hpp"
#include "../../thirdparty/include/glm/gtc/type_ptr.hpp"
#include "../../thirdparty/include/glm/gtx/string_cast.hpp"

#include "GUI/Elements/Elements.hpp"
#include "GUI/GUI.hpp"
#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "Renderer.h"
#include "MouseSystem/Mouse.hpp"

void __getTextRendererDataToTheTextureSelection(TextRenderer& textRenderer);

void Renderer::mouseButtonCallback(
                                        LigidWindow window, 
                                        int button, 
                                        int action, 
                                        int mods
                                    )
{
    //Calculate double click
    if (button == LIGIDGL_MOUSE_BUTTON_LEFT && action == LIGIDGL_PRESS) {
        double currentTime = LigidGL::getTime();
        double timeSinceLastClick = currentTime - previousClickTime;

        //Double click done if clicked in 0.3s
        if (timeSinceLastClick < 0.3) {
            *Mouse::LDoubleClick() = true;
        }
    
        previousClickTime = currentTime;
    }
    
    //Left click
    if(button == 0){ 
        if(action == 1)
            *Mouse::LClick() = true;
    }  
    //Right click
    if(button == 1){
        if(action == 1)
            *Mouse::RClick() = true;
    }  
    //Mid click
    if(button == 2){
        if(action == 1)
            *Mouse::MClick() = true;
    }  

    //Left pressed
    if(button == 0){ 
        *Mouse::LPressed() = action;
    }  
    //Right pressed
    if(button == 1){
        *Mouse::RPressed() = action;
    }  
    //Mid pressed
    if(button == 2){
        *Mouse::MPressed() = action;
    }  
    
    //Mods
    *Mouse::mods() = mods;
    *Mouse::action() = action;
}

void Renderer::framebufferSizeCallback(
                                            LigidWindow window, 
                                            int width, 
                                            int height
                                        )
{
    //Update the window size from context
    this->context.windowScale.x = width;
    this->context.windowScale.y = height;

    //Update the perspective projection matrix to keep the ratio of the 3D model
    this->updateProjectionMatrix(); 

    //And ofc update the OpenGL viewport
    this->updateViewport(); 
}

void Renderer::scrollCallback(
                                LigidWindow window, 
                                double xoffset, 
                                double yoffset
                            )
{
    //Update the scroll value of the mouse class
    *Mouse::mouseScroll() = yoffset;
    
    if(!this->userInterface.anyDialogActive && !this->userInterface.anyPanelHover && this->painter.threeDimensionalMode)
    {
        //The distance between the camera & center 
        float originCameraDistance = glm::distance(this->scene.camera.originPos,this->scene.camera.cameraPos)/10;

        //Change the distance between camera & center (radius)
        if (yoffset > 0 && this->scene.camera.radius > 1) {
            this->scene.camera.radius -= originCameraDistance;
        }
        else if (yoffset < 0) {
            this->scene.camera.radius += originCameraDistance;
        }
        
        //Zoom in-out
        this->scene.camera.cameraPos.x = cos(glm::radians(this->scene.camera.yaw)) * cos(glm::radians(this->scene.camera.pitch)) * this->scene.camera.radius + this->scene.camera.originPos.x;
        this->scene.camera.cameraPos.y = sin(glm::radians(this->scene.camera.pitch)) * -this->scene.camera.radius + this->scene.camera.originPos.y;
        this->scene.camera.cameraPos.z = sin(glm::radians(this->scene.camera.yaw)) * cos(glm::radians(this->scene.camera.pitch)) * this->scene.camera.radius + this->scene.camera.originPos.z;

        //Update the view matrix after the camera position is changed
        this->updateViewMatrix();

        //Since the 3D model's position in the screen is changed update the painter's depth texture
        this->painter.updateTheDepthTexture = true;
    }
}

void Renderer::cursorPositionCallback(
                                        LigidWindow window,
                                        double xpos, //Mouse x position
                                        double ypos  //Mouse y position
                                    )
{
    //Update the mouse position of the mouse class
    Mouse::cursorPos()->x = xpos;
    Mouse::cursorPos()->y = ypos;
    
    //Get the mouse offset by subtracting current cursor position from last frame's cursor pos
    Mouse::mouseOffset()->x = Mouse::cursorPos()->x - lastMousePos.x;
    Mouse::mouseOffset()->y = Mouse::cursorPos()->y - lastMousePos.y;

    const float sensitivity = 0.2f; //Mouse sensivity (Increase the value to go brrrrrbrbrbrb) (effects the 3D model)
    
    Camera* cam = &this->scene.camera;

    if ((
            this->context.window.isMouseButtonPressed(LIGIDGL_MOUSE_BUTTON_RIGHT) == LIGIDGL_PRESS) && //If pressed to right mouse button
            window.isKeyPressed( LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_PRESS &&  //If pressed to CTRL button
            !this->userInterface.anyDialogActive && //If there is no active dialog (don't move the camera if a dialog is active)
            !this->userInterface.anyPanelHover        //Don't move the camera if cursor hover a panel
        ) 
    { 
        // Straight Movement
      
        // Calculate half the sensitivity for later use
        const float half_sensitivity = sensitivity / 2.0f;


        // Calculate sine and cosine of yaw and pitch angles in radians
        const float sin_yaw = sin(glm::radians(cam->yaw));
        const float cos_yaw = cos(glm::radians(cam->yaw));
        const float sin_pitch = sin(glm::radians(cam->pitch));
        const float cos_pitch = cos(glm::radians(cam->pitch));

        // Calculate the x and z offsets based on yaw angle, mouse movement, sensitivity, and half sensitivity
        float x_offset = sin_yaw * Mouse::mouseOffset()->x * sensitivity * half_sensitivity;
        float z_offset = cos_yaw * Mouse::mouseOffset()->x * sensitivity * half_sensitivity;

        // Check if pitch is greater than 60 degrees or less than -60 degrees
        if (cam->pitch > 60.0f || cam->pitch < -60.0f) {
            
            // Add additional x and z offsets based on yaw, pitch, mouse movement, sensitivity, and half sensitivity
            x_offset += cos_yaw * sin_pitch * Mouse::mouseOffset()->y * sensitivity * half_sensitivity;
            z_offset -= sin_yaw * sin_pitch * Mouse::mouseOffset()->y * sensitivity * half_sensitivity;
        }

        // Calculate the y offset based on pitch, mouse movement, sensitivity, and half sensitivity
        const float y_offset = cos_pitch * Mouse::mouseOffset()->y * sensitivity * half_sensitivity;

        // Update camera's x position and origin position by subtracting x offset
        cam->cameraPos.x -= x_offset;
        cam->originPos.x -= x_offset;

        // Update camera's z position and origin position by adding z offset
        cam->cameraPos.z += z_offset;
        cam->originPos.z += z_offset;

        // Update camera's y position and origin position by adding y offset
        cam->cameraPos.y += y_offset;
        cam->originPos.y += y_offset;

        //Since the 3D model's position in the screen is changed update the painter's depth texture
        this->painter.updateTheDepthTexture = true;
    }


    else if (
                this->context.window.isMouseButtonPressed(LIGIDGL_MOUSE_BUTTON_RIGHT) == LIGIDGL_PRESS && //If pressed to right mouse button
                (!this->userInterface.anyDialogActive || //If there is no active dialog (don't move the camera if a dialog is active)
                this->userInterface.materialEditorDialog.dialogControl.isActive()) &&
                !this->userInterface.anyPanelHover  //Don't move the camera if cursor hover a panel
            ) 
    { 
        if(this->userInterface.materialEditorDialog.dialogControl.isActive())
            cam = &this->userInterface.materialEditorDialog.displayerCamera;

        cam->yaw += Mouse::mouseOffset()->x * sensitivity;
        cam->pitch -= Mouse::mouseOffset()->y * sensitivity;

        //Disable 90+ degrees rotations in y axis
        if (cam->pitch > 89.0f)
            cam->pitch = 89.0f;
        if (cam->pitch < -89.0f)
            cam->pitch = -89.0f;

        //Helical Movement
        //Rotates the Camera in 3 axis using yaw, pitch & radius values
        cam->cameraPos.y = sin(glm::radians(cam->pitch)) * -cam->radius + cam->originPos.y;
        cam->cameraPos.x = cos(glm::radians(cam->yaw)) * cos(glm::radians(cam->pitch)) * cam->radius + cam->originPos.x;
        cam->cameraPos.z = sin(glm::radians(cam->yaw)) * cos(glm::radians(cam->pitch)) * cam->radius + cam->originPos.z;
        
        //Since the 3D model's position in the screen is changed update the painter's depth texture
        this->painter.updateTheDepthTexture = true;
    }


    //Update the view matrix after the camera position is changed
    this->updateViewMatrix();
    
    //This will be used as "last frame's cursor pos" for the cursor offset
    this->lastMousePos.x = Mouse::cursorPos()->x;
    this->lastMousePos.y = Mouse::cursorPos()->y;
}

void Renderer::keyCallback(
                                LigidWindow window, //Window
                                int key, //Pressed key
                                int scancode, //Scancode (related to the keyboard I guess)
                                int action, //LIGIDGL_PRESS , LIGIDGL_REPEAT , LIGIDGL_RELEASE
                                int mods //Pressed with CTRL, CTRL + ALT , ALT , etc.
                            )
{
    if(action == LIGIDGL_PRESS || action == LIGIDGL_REPEAT){ //1 or 2
        this->textRenderer.keyInput = true;
        this->textRenderer.key = key;
        this->textRenderer.mods = mods;
        this->textRenderer.action = action;
    }

    __getTextRendererDataToTheTextureSelection(textRenderer);
}