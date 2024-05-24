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

#include "3D/ThreeD.hpp"
#include "3D/Camera/Camera.hpp"
#include "3D/Scene/Scene.hpp"

#include "Toolkit/VectorScene/VectorScene.hpp"

#include "GUI/Panels.hpp"

void Scene::render_scene(Timer& timer){
    if(!panels_any_hovered() && !*Mouse::LPressed())
        this->camera.interaction(*Mouse::mouseScroll(), *Mouse::mouseOffset(), false);

    Debugger::block("Skybox Rendering"); // Start
    this->render_skybox();
    Debugger::block("Skybox Rendering"); // End

    Debugger::block("Rendering scene decorations"); // Start

    if(this->renderTiles){
        ShaderSystem::sceneTilesShader().use();
        ShaderSystem::sceneTilesShader().setMat4("view", this->camera.viewMatrix);
        ShaderSystem::sceneTilesShader().setMat4("projection", this->projectionMatrix);
        ShaderSystem::sceneTilesShader().setMat4("modelMatrix",glm::mat4(1));
        ShaderSystem::sceneTilesShader().setVec3("camPos", this->camera.cameraPos);
        
        this->tiles.draw();
        glClear(GL_DEPTH_BUFFER_BIT);

        ShaderUTIL::release_bound_shader();
    }
    
    if(this->renderAxisDisplayer){
        ShaderSystem::sceneAxisDisplayerShader().use();
        ShaderSystem::sceneAxisDisplayerShader().setMat4("view", this->camera.viewMatrix);
        ShaderSystem::sceneAxisDisplayerShader().setMat4("projection", this->projectionMatrix);
        ShaderSystem::sceneAxisDisplayerShader().setMat4("modelMatrix",glm::mat4(1));
        this->axisDisplayer.draw();
        glClear(GL_DEPTH_BUFFER_BIT);

        ShaderUTIL::release_bound_shader();
    }

    Debugger::block("Rendering scene decorations"); // End

    Debugger::block("3D Model"); // Start    
    this->render_model(timer);
    Debugger::block("3D Model"); // End
}