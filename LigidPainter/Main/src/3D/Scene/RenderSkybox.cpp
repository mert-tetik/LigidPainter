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
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Shader/Shader.hpp"

#include "3D/ThreeD.hpp"
#include "3D/Camera/Camera.hpp"
#include "3D/Scene/Scene.hpp"

#include "GUI/Panels.hpp"

#include "Toolkit/VectorScene/VectorScene.hpp"

void Scene::render_skybox(Painter& painter){
    //Skybox shader
    ShaderSystem::skyboxShader().use();
    ShaderSystem::skyboxShader().setMat4("view", this->camera.viewMatrix);
    ShaderSystem::skyboxShader().setMat4("projection", this->projectionMatrix);
    ShaderSystem::skyboxShader().setMat4("transformMatrix", skybox.transformMatrix);
    ShaderSystem::skyboxShader().setFloat("lod", skybox.lod);
    ShaderSystem::skyboxShader().setVec3("bgColor", skybox.bgColor);
    ShaderSystem::skyboxShader().setFloat("opacity", skybox.opacity);
    ShaderSystem::skyboxShader().setInt("skybox",0);
    
    ShaderSystem::skyboxShader().setInt("gradient", 0);
    ShaderSystem::skyboxShader().setFloat("gradientOffset", 0.f);
    
    //Render the skybox
    skybox.draw(true);

    // Render the background image
    getBox()->bindBuffers();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dialog_displayer.panel.sections[0].elements[5].button.texture.ID);
    ShaderSystem::buttonShader().use();
    ShaderSystem::buttonShader().setVec3("pos", glm::vec3(getContext()->windowScale / glm::ivec2(2), 0.1));
    ShaderSystem::buttonShader().setVec2("scale", getContext()->windowScale / glm::ivec2(2));
    ShaderSystem::buttonShader().setFloat("properties.colorMixVal", 0.f);
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", dialog_displayer.panel.sections[0].elements[6].rangeBar.value);
    ShaderSystem::buttonShader().setInt("states.renderTexture",     1    );
    ShaderSystem::buttonShader().setInt("properties.txtr",     0    );
    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Renderer::renderSkybox");
    glBindTexture(GL_TEXTURE_2D, 0);
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);
    ShaderSystem::buttonShader().setInt("states.renderTexture"  ,     0    );
    glClear(GL_DEPTH_BUFFER_BIT);
    getBox()->unbindBuffers();
}