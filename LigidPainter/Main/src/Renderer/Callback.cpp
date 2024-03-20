/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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

#include "GUI/Elements.hpp"
#include "GUI/GUI.hpp"
#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "Renderer.h"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "GUI/Dialogs.hpp"

extern bool textureFields_decidingWrapPointsMode;

#define CAM_MOVE_CONDITION  !panels_any_hovered() && /*Don't move the camera if cursor hover a panel */\
                            !*Mouse::LPressed() &&\
                            !textureFields_decidingWrapPointsMode
                            

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
    getContext()->windowScale.x = width;
    getContext()->windowScale.y = height;

    //And ofc update the OpenGL viewport
    Settings::defaultFramebuffer()->setViewport();
}

void Renderer::scrollCallback(
                                LigidWindow window,
                                double xoffset, 
                                double yoffset
                            )
{
    // Update the scroll value of the mouse class
    *Mouse::mouseScroll() = yoffset;
}

glm::vec2 lastMousePos = glm::vec2(0.f);//This will be used as "last frame's cursor pos" for the cursor offset 
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

    //This will be used as "last frame's cursor pos" for the cursor offset
    lastMousePos.x = Mouse::cursorPos()->x;
    lastMousePos.y = Mouse::cursorPos()->y;
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
        textRenderer.keyInput = true;
        textRenderer.key = key;
        textRenderer.mods = mods;
        textRenderer.action = action;
    }
}