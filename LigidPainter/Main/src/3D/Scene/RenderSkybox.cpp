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
#include "UTIL/GL/GL.hpp"

#include "3D/ThreeD.hpp"
#include "3D/Camera/Camera.hpp"
#include "3D/Scene/Scene.hpp"

#include "GUI/Panels.hpp"

#include "Toolkit/VectorScene/VectorScene.hpp"

void Scene::render_skybox(){
    //Skybox shader
    ShaderSystem::skyboxShader().use();
    ShaderSystem::skyboxShader().setMat4("view", this->camera.viewMatrix);
    ShaderSystem::skyboxShader().setMat4("projection", this->projectionMatrix);
    ShaderSystem::skyboxShader().setMat4("transformMatrix", skybox.transformMatrix);
    ShaderSystem::skyboxShader().setFloat("lod", skybox.lod);
    ShaderSystem::skyboxShader().setVec3("bgColor", skybox.bgColor);
    ShaderSystem::skyboxShader().setFloat("opacity", skybox.opacity);
    ShaderSystem::skyboxShader().setInt("skybox", GL::bindTexture_CubeMap(skybox.IDPrefiltered, "Scene::render_skybox : Render skybox"));
    ShaderSystem::skyboxShader().setInt("gradient", 0);
    ShaderSystem::skyboxShader().setFloat("gradientOffset", 0.f);
    
    //Render the skybox
    skybox.draw();
    
    GL::releaseBoundTextures("Scene::render_skybox : Render skybox");

    // Render the background image
    getBox()->bindBuffers();

    ShaderSystem::textureRenderingShader().use();
    ShaderSystem::textureRenderingShader().setMat4("projection", getContext()->ortho_projection);
    ShaderSystem::textureRenderingShader().setVec2("scale", *Settings::videoScale());
    ShaderSystem::textureRenderingShader().setVec3("pos", glm::vec3(*Settings::videoScale() / glm::vec2(2), 0.1));
    ShaderSystem::textureRenderingShader().setInt("txtr", GL::bindTexture_2D(dialog_displayer.panel.sections[0].elements[5].button.texture.ID, "Scene::render_skybox : Render bg texture"));
    ShaderSystem::textureRenderingShader().setFloat("opacity", dialog_displayer.panel.sections[0].elements[6].rangeBar.value);
    ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
    ShaderSystem::textureRenderingShader().setFloat("depthToleranceValue", 0);

    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Scene::render_skybox : Render bg texture");
    
    GL::releaseBoundTextures("Scene::render_skybox : Render bg texture");

    glClear(GL_DEPTH_BUFFER_BIT);
    getBox()->unbindBuffers();
}