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

void Texture::removeSeams(Mesh& mesh){
    
    // Get already bound shader to set it back at the end
    Shader already_bound_shader = ShaderUTIL::get_bound_shader();
    
    // Get the copy of the texture
    Texture copy_txtr = this->get_temp_copy_txtr("Texture::removeSeams");

    // Get the FBO
    Framebuffer FBO = FBOPOOL::requestFBO(*this, "Texture::removeSeams");

    // Clear color buffer of the FBO
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Set shader 
    ShaderSystem::boundaryExpandingShader().use();
    ShaderSystem::boundaryExpandingShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    ShaderSystem::boundaryExpandingShader().setMat4("projectedPosProjection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    ShaderSystem::boundaryExpandingShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
    ShaderSystem::boundaryExpandingShader().setVec2("scale", glm::vec2(0.5f, 0.5f));
    
    ShaderSystem::boundaryExpandingShader().setInt("resolution", copy_txtr.getResolution().x);

    int uvMask_unit = GL::bindTexture_2D(mesh.uvMask.ID, "Texture::removeSeams");
    int copy_txtr_unit = GL::bindTexture_2D(copy_txtr.ID, "Texture::removeSeams");

    ShaderSystem::boundaryExpandingShader().setInt("whiteUVTexture", uvMask_unit);
    ShaderSystem::boundaryExpandingShader().setInt("originalTexture", copy_txtr_unit);
    
    // Render the result
    getBox()->draw("Texture::removeSeams : Rendering result");

    // Finish
    GL::releaseTextureFromSlot(uvMask_unit, "Texture::removeSeams");
    GL::releaseTextureFromSlot(copy_txtr_unit, "Texture::removeSeams");
    ShaderUTIL::release_bound_shader();
    FBOPOOL::releaseFBO(FBO);
    already_bound_shader.use();
}

void Texture::removeUnselectedFaces(Mesh& mesh){

    // Get already bound shader to set it back at the end
    Shader already_bound_shader = ShaderUTIL::get_bound_shader();
    
    // Get the copy of the texture
    Texture copy_txtr = this->get_temp_copy_txtr("Texture::removeUnselectedFaces");
    
    // Get the FBO
    Framebuffer FBO = FBOPOOL::requestFBO(*this, "Texture::removeSeams");

    // Clear the color buffer
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the shader
    ShaderSystem::removeUnselectedFacesShader().use();
    ShaderSystem::removeUnselectedFacesShader().setMat4("orthoProjection", glm::ortho(0.f, 1.f, 0.f, 1.f));
    ShaderSystem::removeUnselectedFacesShader().setMat4("perspectiveProjection", glm::mat4(0.f));
    ShaderSystem::removeUnselectedFacesShader().setMat4("view", glm::mat4(0.f));
    ShaderUTIL::set_shader_struct_face_selection_data(ShaderSystem::removeUnselectedFacesShader(), mesh);
    
    int txtr_unit = GL::bindTexture_2D(copy_txtr.ID, "Texture : removeUnselectedFaces");
    ShaderSystem::removeUnselectedFacesShader().setInt("txtr", txtr_unit);

    mesh.Draw("Texture::removeUnselectedFaces");

    // Finish
    GL::releaseTextureFromSlot(txtr_unit, "Texture::removeUnselectedFaces");
    ShaderUTIL::release_bound_shader();
    FBOPOOL::releaseFBO(FBO);
    already_bound_shader.use();
}