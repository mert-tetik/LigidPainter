
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

#include <string>
#include <iostream>
#include <vector>

DialogControl::DialogControl(){

}

DialogControl::DialogControl(bool active){
    this->active = active;
}

static void drawBG()
{
    ShaderSystem::defaultFramebufferShader().use();
    ShaderSystem::defaultFramebufferShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    ShaderSystem::defaultFramebufferShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
    ShaderSystem::defaultFramebufferShader().setVec2("scale", glm::vec2(0.5f));
    
    ShaderSystem::defaultFramebufferShader().setVec2("resolution", Settings::defaultFramebuffer()->resolution);
    ShaderSystem::defaultFramebufferShader().setInt("txtr", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Settings::defaultFramebuffer()->bgTxtr.ID);

    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "DialogControl : DrawBG");
    
    ShaderSystem::buttonShader().use();
}

void DialogControl::updateStart(){
    getContext()->window.pollEvents();

    getScene()->updateGUIProjectionMatrix(getContext()->windowScale.x, getContext()->windowScale.y);
    
    // Prevent rendering the application if the window is minimized
    while (getContext()->window.isMinimized()){
        getContext()->window.pollEvents();
    }
    
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawBG();
    glClear(GL_DEPTH_BUFFER_BIT);

    ShaderSystem::buttonShader().use();
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", mixVal);
    ShaderSystem::buttonShader().setMat4("projection", getScene()->gui_projection);

    globalFirstFrameActivated = false;

    getBox()->bindBuffers();
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
}

void DialogControl::updateEnd(Timer &timer, float transitionDuration){
    
    // Update local timer data
    timer.tick = false;
    if(timer.runTimer(1.f)){
        timer.tick = true;
        std::cout << timer.FPS << std::endl;
    }

    timer.transition(active,mixVal,transitionDuration);
    if(firstFrameActivated)
        globalFirstFrameActivated = true;
    firstFrameActivated = false;

    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.);

    getContext()->window.swapBuffers();

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

    glClear(GL_DEPTH_BUFFER_BIT);
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