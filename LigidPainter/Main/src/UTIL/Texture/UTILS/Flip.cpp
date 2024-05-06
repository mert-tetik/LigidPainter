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

void Texture::flipTexture(bool horizontal, bool vertical){
    
    // Get already bound shader program to set it back at the end
    Shader already_bound_shader = ShaderUTIL::get_bound_shader();

    // Get copy texture
    Texture copiedTxtr = this->get_temp_copy_txtr("Texture::flipTexture");
    
    // Get framebuffer
    Framebuffer FBO = FBOPOOL::requestFBO(*this, "Texture::flipTexture");
    
    // Clear the color data
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate the projection according to flip data 
    glm::mat4 projection = glm::ortho(0.f, 1.f, 1.f, 0.f);
    if(horizontal && vertical)
        projection = glm::ortho(1.f, 0.f, 0.f, 1.f);
    else if(horizontal)
        projection = glm::ortho(1.f, 0.f, 1.f, 0.f);
    else if(vertical)
        projection = glm::ortho(0.f, 1.f, 0.f, 1.f);

    // Set shader
    ShaderSystem::textureRenderingShader().use();
    ShaderSystem::textureRenderingShader().setMat4("projection", projection);
    ShaderSystem::textureRenderingShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
    ShaderSystem::textureRenderingShader().setVec2("scale", glm::vec2(0.5f, 0.5f));
    ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
    ShaderSystem::textureRenderingShader().setFloat("opacity", 1.f);
    int txtr_slot = GL::bindTexture_2D(copiedTxtr.ID, "Texture::flipTxtr");
    ShaderSystem::textureRenderingShader().setInt("txtr", txtr_slot);
    ShaderSystem::textureRenderingShader().setFloat("depthToleranceValue", 0);

    // Render result
    getBox()->draw("Texture::flipTexture : Rendering result");

    // Finish
    GL::releaseTextureFromSlot(txtr_slot, "Texture::flipTxtr");
    FBOPOOL::releaseFBO(FBO);
    already_bound_shader.use();
}