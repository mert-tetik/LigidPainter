/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/Elements.hpp"

#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/GL/GL.hpp"

#include <string>
#include <iostream>
#include <vector>

void render_sprite_image(
                            Timer &timer, //Timer that handles the animations
                            bool doMouseTracking, //If there is need to check if mouse hover
                            glm::vec3 pos,
                            glm::vec2 scale,
                            Texture image
                        )
{
    // pos value % of the video scale
    glm::vec3 resultPos = glm::vec3( 
                          UTIL::getPercent(*Settings::videoScale(), glm::vec2(pos.x,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value

    // scale value % of the video scale
    glm::vec2 resultScale = UTIL::getPercent(*Settings::videoScale(), scale);

    ShaderSystem::spriteRenderingShader().use();
    ShaderSystem::spriteRenderingShader().setMat4("projection", getContext()->ortho_projection);
    ShaderSystem::spriteRenderingShader().setInt("txtr", GL::bindTexture_2D(image.ID, "render_sprite_image"));

    glm::vec2 side_scale = glm::vec2(UTIL::getPercent(Settings::videoScale()->x, 0.5f) ,resultScale.y);
    glm::vec2 mid_scale = glm::vec2(resultScale.x - side_scale.x * 2.f, resultScale.y);

    ShaderSystem::spriteRenderingShader().setVec2("scale", side_scale);
    ShaderSystem::spriteRenderingShader().setVec3("pos", glm::vec3(resultPos.x - side_scale.x - mid_scale.x , resultPos.y, resultPos.z));
    ShaderSystem::spriteRenderingShader().setInt("spriteI", 0);
    getBox()->draw("render_sprite_image 1");
    
    ShaderSystem::spriteRenderingShader().setVec2("scale", mid_scale);
    ShaderSystem::spriteRenderingShader().setVec3("pos", glm::vec3(resultPos.x, resultPos.y, resultPos.z));
    ShaderSystem::spriteRenderingShader().setInt("spriteI", 1);
    getBox()->draw("render_sprite_image 2");
    
    ShaderSystem::spriteRenderingShader().setVec2("scale", side_scale);
    ShaderSystem::spriteRenderingShader().setVec3("pos", glm::vec3(resultPos.x + side_scale.x + mid_scale.x, resultPos.y, resultPos.z));
    ShaderSystem::spriteRenderingShader().setInt("spriteI", 2);
    getBox()->draw("render_sprite_image 3");
}