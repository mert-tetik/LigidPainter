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

void Texture::generateNormalMap(Texture& normalMap, float proceduralNormalStrength, bool proceduralNormalGrayScale, bool alphaMode){

    Texture height_map = *this;

    if(normalMap.ID == this->ID)
        height_map = this->get_temp_copy_txtr("Texture::generateNormalMap");
        

    // Get the already bound shader to set back at the end
    Shader already_bound_shader = ShaderUTIL::get_bound_shader();

    // Get the FBO
    Framebuffer FBO = FBOPOOL::requestFBO(normalMap, "Texture::generateNormalMap");

    // Clear the color buffer before rendering
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Set shader
    ShaderSystem::heightToNormalMap().use();
    
    ShaderSystem::heightToNormalMap().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    ShaderSystem::heightToNormalMap().setMat4("projectedPosProjection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    ShaderSystem::heightToNormalMap().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
    ShaderSystem::heightToNormalMap().setVec2("scale", glm::vec2(0.5f));

    int txtr_slot = GL::bindTexture_2D(height_map.ID, "Texture::generateNormalMap");
    ShaderSystem::heightToNormalMap().setInt("heightMap", txtr_slot);
    ShaderSystem::heightToNormalMap().setInt("alphaMode", alphaMode);
    ShaderSystem::heightToNormalMap().setInt("txtrRes", normalMap.getResolution().x);
    ShaderSystem::heightToNormalMap().setFloat("strength", proceduralNormalStrength);
    ShaderSystem::heightToNormalMap().setInt("grayScale", proceduralNormalGrayScale);

    // Render result
    getBox()->draw("Texture::generateNormalMap : Render result");

    // Finish
    GL::releaseTextureFromSlot(txtr_slot ,"Texture::generateNormalMap"); // Release bound textures
    FBOPOOL::releaseFBO(FBO); // Release the FBO
    already_bound_shader.use(); // Use the default shader program back
}

void Texture::applyNormalMap(float proceduralNormalStrength, bool proceduralNormalGrayScale, bool alphaMode){
    this->generateNormalMap(*this, proceduralNormalStrength, proceduralNormalGrayScale, alphaMode);
}