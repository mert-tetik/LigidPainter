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

void Scene::render_scene(){

    Debugger::block("Skybox Rendering"); // Start
    this->render_skybox();
    Debugger::block("Skybox Rendering"); // End

    Debugger::block("Rendering scene decorations"); // Start

    if(this->renderTiles){
        ShaderSystem::sceneTilesShader().use();
        this->tiles.draw();
        glClear(GL_DEPTH_BUFFER_BIT);
    }
    
    if(this->renderAxisDisplayer){
        ShaderSystem::sceneAxisDisplayerShader().use();
        this->axisDisplayer.draw();
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    Debugger::block("Rendering scene decorations"); // End

    Debugger::block("3D Model"); // Start    
    this->render_model();
    Debugger::block("3D Model"); // End
}