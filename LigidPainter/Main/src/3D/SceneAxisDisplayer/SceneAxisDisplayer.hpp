/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef THREED_SCENE_AXIS_DISPLAYER_HPP
#define THREED_SCENE_AXIS_DISPLAYER_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <string>

class SceneAxisDisplayer
{
public:
    unsigned int VBO;
    unsigned int VAO;

    /// @brief Default constructor (does nothing) (use @ref init instead to init the OpenGL objects)
    SceneAxisDisplayer();

    /// @brief Initializes / Creates OpenGL vertex objects regarding 2D square rendering
    void init();

    /// @brief Binds the 2D square vertex objects
    void bindBuffers();
    
    /// @brief Unbinds the 2D square vertex objects
    void unbindBuffers();
    
    /// @brief Draw the square (is not used) 
    void draw();
};

#endif