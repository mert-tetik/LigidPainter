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

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "Toolkit/Toolkits.hpp"
#include "GUI/Panels.hpp"

#include <string>
#include <iostream>
#include <vector>

void render_dots(glm::vec2 pos, glm::vec2 scale, glm::vec2 dotPos, float dotSize){
    ShaderSystem::dotsShader().use();

    ShaderSystem::dotsShader().setMat4("projection", getContext()->ortho_projection);
    ShaderSystem::dotsShader().setVec3("pos", glm::vec3(pos, 0.5f));
    ShaderSystem::dotsShader().setVec2("scale", scale);

    ShaderSystem::dotsShader().setVec2("dotPos", dotPos);
    ShaderSystem::dotsShader().setFloat("scroll", dotSize);

    /* Render the dots */
    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Rendering dots to window");

    /* Clear the depth buffer of the current framebuffers*/
    glClear(GL_DEPTH_BUFFER_BIT);

    ShaderSystem::buttonShader().use();
}