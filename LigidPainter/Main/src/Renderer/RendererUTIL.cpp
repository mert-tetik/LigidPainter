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
#include <filesystem>

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "Renderer.h"

#include "GUI/Elements.hpp"
#include "GUI/GUI.hpp"
#include "GUI/Panels.hpp"

#include "3D/ThreeD.hpp"

#include "Toolkit/Toolkits.hpp"

#include "UTIL/Util.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"

bool _ligid_renderer_render_first_frame = true;

// 3D Point
extern bool aPointWasAlreadyActivated;

void Renderer::start_render(){
    glm::ivec2 maxWindowSize;
    getContext()->window.getMaximizedScreenSize(maxWindowSize.x, maxWindowSize.y);
    *Settings::videoScale() = maxWindowSize; 

    //Handle user input and interact with the windowing system
    getContext()->window.pollEvents();

    // Prevent rendering the application if the window is minimized
    while (getContext()->window.isMinimized()){
        getContext()->window.pollEvents();
    }
    
    // Update local timer data
    timer.tick = false;
    if(timer.runTimer(1.f)){
        timer.tick = true;
        std::cout << timer.FPS << std::endl;
    }
    
    if(_ligid_renderer_render_first_frame)
        getContext()->window.setWindowSize(Settings::videoScale()->x, Settings::videoScale()->y);

    //Update OpenGL viewport every frame
    Settings::defaultFramebuffer()->setViewport();

    //VSync
    if(Settings::properties()->VSync)
        LigidGL::setSwapInterval(1); //Enable VSync
    else
        LigidGL::setSwapInterval(0); //Disable VSync

    Settings::defaultFramebuffer()->FBO.bind();

    //Default blending settings
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glDepthFunc(GL_LEQUAL);

    //Refresh the default framebuffer    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    getContext()->updateGUIProjectionMatrix(getContext()->windowScale.x, getContext()->windowScale.y);
    getScene()->updateProjectionMatrix(0.f);

    // Process the shortcut inputs & move the camera gradually if necessary
    getScene()->camera.posShortcutInteraction(true);
}

void Renderer::end_render(){
        //Set mouse states to default
    *Mouse::LClick() = false;
    *Mouse::RClick() = false;
    *Mouse::MClick() = false;
    
    if(!getContext()->window.isMouseButtonPressed(LIGIDGL_MOUSE_BUTTON_LEFT))
        *Mouse::LPressed() = false;
    if(!getContext()->window.isMouseButtonPressed(LIGIDGL_MOUSE_BUTTON_RIGHT))
        *Mouse::RPressed() = false;
    if(!getContext()->window.isMouseButtonPressed(LIGIDGL_MOUSE_BUTTON_MIDDLE))
        *Mouse::MPressed() = false;

    *Mouse::LDoubleClick() = false;
    *Mouse::mouseOffset() = glm::vec2(0);
    *Mouse::mods() = 0;
    *Mouse::mouseScroll() = 0;

    aPointWasAlreadyActivated = false;

    //Set keyboard states to default
    textRenderer.keyInput = false;
    textRenderer.mods = 0;
    textRenderer.action = 0;

    //Let the getScene()->model->newModelAdded be true for an another cycle
    if(previousModelNewModelAdded == true)
        getScene()->model->newModelAdded = false;

    previousModelNewModelAdded = getScene()->model->newModelAdded; 

    //Cursor is changing there
    //Sets the active cursor (mouse.activeCursor) as the cursor
    //Than changes the active cursor as default cursor
    Mouse::updateCursor();  

    // ------- Rendering the framebuffer result ------- 
    if(true){

        Debugger::block("Low resolution framebuffer result"); // Start

        if(Settings::defaultFramebuffer()->FBO.ID != 0)
            Settings::defaultFramebuffer()->render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Create a framebuffer object (FBO)
        GLuint framebuffer;
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Settings::defaultFramebuffer()->bgTxtr.ID, 0);

        // Set up the blit
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); // Bind the default framebuffer as the source
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer); // Bind the FBO as the destination
        glBlitFramebuffer(0, 0, getContext()->windowScale.x, getContext()->windowScale.y, 0, 0, Settings::defaultFramebuffer()->resolution.x, Settings::defaultFramebuffer()->resolution.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // Unbind the FBO
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Clean up resources (delete framebuffer and unneeded textures if necessary)
        glDeleteFramebuffers(1, &framebuffer);

        Debugger::block("Low resolution framebuffer result"); // End
    }


    //Swap the front and back buffers of the window
    getContext()->window.swapBuffers();

    getBox()->unbindBuffers(); //Finish rendering the UI

    if(timer.seconds == 1)
        dialog_greeting.show(timer);

    _ligid_renderer_render_first_frame = false;
}