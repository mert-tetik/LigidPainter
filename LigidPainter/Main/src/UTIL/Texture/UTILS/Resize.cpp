/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/GL/GL.hpp"
#include "UTIL/Framebuffers/Framebuffer_Pool.hpp"

void Texture::resize(const glm::ivec2 newResolution){
    // Get already bound shader program to bind it back at the end
    Shader already_bound_shader = ShaderUTIL::get_bound_shader();
    
    //Get copy of the texture
    Texture copiedTxtr = this->get_temp_copy_txtr("Texture::resize");

    // Update the resolution of the texture
    this->update((char*)nullptr, newResolution.x, newResolution.y);

    // Get a FBO
    Framebuffer FBO = FBOPOOL::requestFBO(*this, "Texture::resize");    
    
    // Clear the texture
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set shader
    ShaderSystem::textureRenderingShader().use();
    ShaderSystem::textureRenderingShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    ShaderSystem::textureRenderingShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
    ShaderSystem::textureRenderingShader().setVec2("scale", glm::vec2(0.5f, 0.5f));
    ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
    ShaderSystem::textureRenderingShader().setFloat("opacity", 1.f);
    ShaderSystem::textureRenderingShader().setFloat("depthToleranceValue", 0);
    int txtr_unit = GL::bindTexture_2D(copiedTxtr.ID, "Texture::resize");
    ShaderSystem::textureRenderingShader().setInt("txtr", txtr_unit);

    // Render result
    getBox()->draw("Texture::resize : Rendering result");

    // Finish
    GL::releaseTextureFromSlot(txtr_unit, "Texture::resize");
    FBOPOOL::releaseFBO(FBO);
    already_bound_shader.use();
}