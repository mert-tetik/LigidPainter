/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

SceneTiles.hpp : Is used to render a single 2D square.
          Mostly used by GUI elements. (GUI folder)
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/GL/GL.hpp"

#include "3D/SceneTiles/SceneTiles.hpp"
#include "3D/Mesh/Mesh.hpp"
    
SceneTiles::SceneTiles(){}

void SceneTiles::init(){
    const int tile_x_count = 200;
    const int tile_z_count = 200;

    const float tile_x_spacing = 1.f;
    const float tile_z_spacing = 1.f;

    const float single_line_length = 200.f;

    const float y_axis_location = 0.f; // At the center
    
    this->x_start_line.startPoint = glm::vec3(-(tile_x_count / 2) * tile_x_spacing, y_axis_location, -(single_line_length / 2.f));
    this->x_start_line.endPoint = glm::vec3(-(tile_x_count / 2) * tile_x_spacing, y_axis_location, +(single_line_length / 2.f));
    
    this->z_start_line.startPoint = glm::vec3(-(single_line_length / 2.f), y_axis_location, -(tile_z_count / 2) * tile_z_spacing);
    this->z_start_line.endPoint = glm::vec3(+(single_line_length / 2.f), y_axis_location, -(tile_z_count / 2) * tile_z_spacing);

    for (int x = 0; x < tile_x_count; x++)
    {
        this->x_offsets.push_back(glm::vec3(x * tile_x_spacing, 0.f, 0.f));
    }
    for (int z = 0; z < tile_z_count; z++)
    {
        this->z_offsets.push_back(glm::vec3(0.f, 0.f, z * tile_z_spacing));
    }
}

void SceneTiles::draw(){
    // Render the x axis
    render_line(this->x_start_line, this->x_offsets, ShaderSystem::sceneTilesShader());
    // Render the z axis
    render_line(this->z_start_line, this->z_offsets, ShaderSystem::sceneTilesShader());
}