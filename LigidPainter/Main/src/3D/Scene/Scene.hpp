/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef THREED_SCENE_HPP
#define THREED_SCENE_HPP

#include <iostream>
#include <atomic>

#include "3D/ThreeD.hpp"
#include "3D/Camera/Camera.hpp"

struct Scene{
    glm::mat4 projectionMatrix;
    glm::mat4 transformMatrix;

    Camera camera;    

    float fov = 45.f;
    float aNear = 0.1f;
    float aFar = 1000.0f;

    glm::vec3 transformLocation = glm::vec3(0);
    glm::vec3 transformRotation = glm::vec3(0);

    bool useOrtho = false;

    bool renderTiles = true;
    SceneTiles tiles;
    
    bool renderAxisDisplayer = false;
    SceneAxisDisplayer axisDisplayer;

    Skybox skybox;
    void render_skybox();
    
    bool backfaceCulling = false;
    bool useHeightMap = false;
    float heightMapStrength = 0.f;
    Model* model = new Model();
    Mesh* get_selected_mesh();
    void render_model();

    void render_scene();
    
    void updateProjectionMatrix(float ratio);
    void updateTransformMatrix();
};

#endif