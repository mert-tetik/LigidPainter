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

void Texture::render_mesh(Mesh& mesh, MaterialChannels channels, Camera cam){
    Shader already_bound_shader = ShaderUTIL::get_bound_shader();

    // Get framebuffer
    Framebuffer FBO = FBOPOOL::requestFBO_with_RBO(*this, this->getResolution(), "Texture::render_mesh");
    
    // Clear color buffer
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set shader
    ShaderSystem::PBRDisplayOnly().use();
    ShaderSystem::PBRDisplayOnly().setMat4("view", cam.viewMatrix);
    ShaderSystem::PBRDisplayOnly().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::PBRDisplayOnly().setMat4("modelMatrix", glm::mat4(1.f));
    
    ShaderSystem::PBRDisplayOnly().setVec3("viewPos", cam.cameraPos);
    ShaderSystem::PBRDisplayOnly().setInt("skybox", GL::bindTexture_CubeMap(getScene()->skybox.ID, "Texture::renderMesh"));
    ShaderSystem::PBRDisplayOnly().setInt("prefilterMap", GL::bindTexture_CubeMap(getScene()->skybox.IDPrefiltered, "Texture::renderMesh"));
    ShaderSystem::PBRDisplayOnly().setInt("albedoTxtr", GL::bindTexture_2D(channels.albedo.ID, "Texture::renderMesh"));
    ShaderSystem::PBRDisplayOnly().setInt("roughnessTxtr", GL::bindTexture_2D(channels.roughness.ID, "Texture::renderMesh"));
    ShaderSystem::PBRDisplayOnly().setInt("metallicTxtr", GL::bindTexture_2D(channels.metallic.ID, "Texture::renderMesh"));
    ShaderSystem::PBRDisplayOnly().setInt("normalMapTxtr", GL::bindTexture_2D(channels.normalMap.ID, "Texture::renderMesh"));
    ShaderSystem::PBRDisplayOnly().setInt("heightMapTxtr", GL::bindTexture_2D(channels.heightMap.ID, "Texture::renderMesh"));
    ShaderSystem::PBRDisplayOnly().setInt("ambientOcclusionTxtr", GL::bindTexture_2D(channels.ambientOcclusion.ID, "Texture::renderMesh"));
    ShaderSystem::PBRDisplayOnly().setInt("displayingMode", 0);

    // Render result
    mesh.Draw("Texture::render_mesh");

    // Finish 
    GL::releaseBoundTextures("Texture::render_mesh"); // Release bound textures
    FBOPOOL::releaseFBO(FBO); // Release the FBO
    ShaderUTIL::release_bound_shader();
    already_bound_shader.use(); // Use already bound shader program
}