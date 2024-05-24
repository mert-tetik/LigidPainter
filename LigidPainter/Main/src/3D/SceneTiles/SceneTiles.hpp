/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef THREED_SCENE_TILES_HPP
#define THREED_SCENE_TILES_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "3D/Line/Line.hpp"

#include <string>

class SceneTiles
{
public:
    /// @brief Default constructor (does nothing) (use @ref init instead to init the OpenGL objects)
    SceneTiles();

    Line3D x_start_line;
    std::vector<glm::vec3> x_offsets;
    Line3D z_start_line;
    std::vector<glm::vec3> z_offsets;

    /// @brief Initializes / Creates OpenGL vertex objects regarding 2D square rendering
    void init();
    
    /// @brief Draw the square (is not used) 
    void draw();
};

#endif