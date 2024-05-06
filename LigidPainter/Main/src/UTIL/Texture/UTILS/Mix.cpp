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

void Texture::mix(Texture mix_txtr, Texture mask, bool maskAlpha, bool normalMapMode, bool invert){
    
    Shader already_bound_shader = ShaderUTIL::get_bound_shader();

    Texture duplicated_txtr = this->get_temp_copy_txtr("Texture::mix"); 

    // Get a FBO and bind it & update the viewport resolution
    Framebuffer FBO = FBOPOOL::requestFBO(*this, "Texture::mix");
    
    // Set shader
    ShaderSystem::grayScaleIDMaskingShader().use();
    ShaderSystem::grayScaleIDMaskingShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    ShaderSystem::grayScaleIDMaskingShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
    ShaderSystem::grayScaleIDMaskingShader().setVec2("scale", glm::vec2(0.5f, 0.5f));
    
    int mask_unit = GL::bindTexture_2D(mask.ID, "Texture::mix");
    int duplicated_txtr_unit = GL::bindTexture_2D(duplicated_txtr.ID, "Texture::mix");
    int mix_txtr_unit = GL::bindTexture_2D(mix_txtr.ID, "Texture::mix");
    ShaderSystem::grayScaleIDMaskingShader().setInt("maskTexture", mask_unit);
    ShaderSystem::grayScaleIDMaskingShader().setInt("texture_black", duplicated_txtr_unit);
    ShaderSystem::grayScaleIDMaskingShader().setInt("texture_white", mix_txtr_unit);
    
    ShaderSystem::grayScaleIDMaskingShader().setInt("maskAlpha", maskAlpha);
    ShaderSystem::grayScaleIDMaskingShader().setInt("normalMapMode", normalMapMode);
    ShaderSystem::grayScaleIDMaskingShader().setInt("invert", invert);
    ShaderSystem::grayScaleIDMaskingShader().setFloat("offset", 0.5f);
    
    /* Render the result */
    getBox()->draw("Texture::mix : Rendering result");
    
    //Finish
    GL::releaseTextureFromSlot(mask_unit, "Texture mix"); // Release the bound textures
    GL::releaseTextureFromSlot(duplicated_txtr_unit, "Texture mix"); // Release the bound textures
    GL::releaseTextureFromSlot(mix_txtr_unit, "Texture mix"); // Release the bound textures
    FBOPOOL::releaseFBO(FBO); // Release the FBO
    already_bound_shader.use();
}

void Texture::mix(Texture mix_txtr, Texture mask, bool mask_alpha){
    this->mix(mix_txtr, mask, mask_alpha, false, false);
}