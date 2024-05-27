
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

All rights reserved.

Official GitHub Link : https:github.com/mert-tetik/LigidPainter
Official Web Page : https:ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/
#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"

#include "3D/ThreeD.hpp"

#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Threads/Threads.hpp"
#include "UTIL/GL/GL.hpp"

#include <string>
#include <iostream>
#include <vector>

DialogControl::DialogControl(){

}

DialogControl::DialogControl(bool active){
    this->active = active;
}

Texture blurred_bgTexture;
void action_BLUR(Texture* receivedTexture, Texture* destTxtr, int blurIndex, float directionalDirection, glm::vec2 radialPos, float strength);

static void update_blurred_bgTexture(float mixVal){
    if(!blurred_bgTexture.ID)
        blurred_bgTexture = Texture((char*)nullptr, 1024, 1024);

    action_BLUR(&Settings::defaultFramebuffer()->bgTxtr, &blurred_bgTexture, 1, 0.f, glm::vec2(0.f), 0.1f * mixVal);
}

static void drawBG()
{
    ShaderSystem::bluringShader();

    ShaderSystem::defaultFramebufferShader().use();
    ShaderSystem::defaultFramebufferShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    ShaderSystem::defaultFramebufferShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
    ShaderSystem::defaultFramebufferShader().setVec2("scale", glm::vec2(0.5f));
    
    ShaderSystem::defaultFramebufferShader().setVec2("resolution", Settings::defaultFramebuffer()->resolution);
    ShaderSystem::defaultFramebufferShader().setInt("txtr", GL::bindTexture_2D(blurred_bgTexture.ID, "DialogControl : drawBG"));

    getBox()->draw("DialogControl : drawBG");

    GL::releaseBoundTextures("DialogControl : drawBG");
}

void DialogControl::updateStart(bool loop_mode){
    this->loop_mode = loop_mode;
    
    if(loop_mode){
        if(this->mixVal != 1.f){
            update_blurred_bgTexture(this->mixVal);
        }

        getContext()->window.setCursorVisibility(true);

        getContext()->window.pollEvents();

        getContext()->updateGUIProjectionMatrix(getContext()->windowScale.x, getContext()->windowScale.y);
        getScene()->updateProjectionMatrix(0.f);
        
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    else{
        glClear(GL_DEPTH_BUFFER_BIT);
    }
    

    if(loop_mode){
        glDisable(GL_DEPTH_TEST);
        drawBG();
        glEnable(GL_DEPTH_TEST);
    }

    ShaderSystem::buttonShader().use();
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", mixVal);

    globalFirstFrameActivated = false;

    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
}

bool _dialog_ended = false;

void DialogControl::updateEnd(Timer &timer, float transitionDuration){
    
    if(loop_mode){
        material_thread.update_thread_result();

        // Update local timer data
        timer.tick = false;
        if(timer.runTimer(1.f)){
            timer.tick = true;
            std::cout << timer.FPS << std::endl;
        }
    }

    timer.transition(active,mixVal,transitionDuration);
    if(firstFrameActivated)
        globalFirstFrameActivated = true;
    firstFrameActivated = false;

    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.);

    if(loop_mode){
        if(!Settings::properties()->cat_hide){
            Debugger::block("GUI : Log Dialog"); // Start
            dialog_log.render(timer);
            Debugger::block("GUI : Log Dialog"); // End
        }

        //Set mouse states to default
        *Mouse::LClick() = false;
        *Mouse::RClick() = false;
        *Mouse::MClick() = false;
        *Mouse::LDoubleClick() = false;
        *Mouse::mouseOffset() = glm::vec2(0);
        *Mouse::mods() = 0;
        *Mouse::mouseScroll() = 0;
        *Mouse::action() = 0;
        Mouse::updateCursor();  

        //Set keyboard states to default
        textRenderer.keyInput = false;
        textRenderer.mods = 0;

        if(Settings::defaultFramebuffer()->FBO.ID != 0)
            Settings::defaultFramebuffer()->render();
        Settings::defaultFramebuffer()->setViewport();

        getContext()->window.swapBuffers();
    }

    glClear(GL_DEPTH_BUFFER_BIT);

    if(loop_mode && this->mixVal == 0.f){
        _dialog_ended = true;
    }
}

void DialogControl::activate(){
    active = true;
    firstFrameActivated = true;
}

void DialogControl::unActivate(){
    active = false;
}

bool DialogControl::isActive(){
    if(mixVal > 0.1f)
        return true;
    
    return active;
}

bool DialogControl::isComplete(){
    return mixVal > 0.9f;
}