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
#include "UTIL/Texture/Texture.hpp"
#include "UTIL/Library/Library.hpp"

void apply_edge_wear(Mesh &mesh, Texture srcTxtr, Texture& destTxtr, int textureRes);

void Texture::generateProceduralTexture(Mesh &mesh, Texture& destTxtr, int textureRes){
    
    Shader already_bound_shader = ShaderUTIL::get_bound_shader();

    destTxtr.update((char*)nullptr, textureRes, textureRes);

    // ------- Edge Wear ------- 
    if(this->proceduralProps.proceduralID == 121){
        apply_edge_wear(mesh, *this, destTxtr, textureRes);
    }

    // ------- Regular Procedural Texture -------
    else{
        // Set shader
        ShaderSystem::to2DProcedural().use();
        ShaderSystem::to2DProcedural().setMat4("orthoProjection", glm::ortho(0.f,1.f,0.f,1.f));
        ShaderSystem::to2DProcedural().setMat4("perspectiveProjection", getScene()->projectionMatrix);
        ShaderSystem::to2DProcedural().setMat4("view", getScene()->camera.viewMatrix);
        
        ShaderSystem::to2DProcedural().setInt("proceduralID", (this->proceduralProps.proceduralID == 1000) ? -1 : this->proceduralProps.proceduralID);
        ShaderSystem::to2DProcedural().setFloat("proceduralScale", this->proceduralProps.proceduralScale);
        ShaderSystem::to2DProcedural().setFloat("proceduralStretch", this->proceduralProps.proceduralStretch);
        ShaderSystem::to2DProcedural().setFloat("proceduralScaleModelPos", this->proceduralProps.proceduralScaleModelPos);
        ShaderSystem::to2DProcedural().setInt("proceduralInverted", this->proceduralProps.proceduralnverted);
        ShaderSystem::to2DProcedural().setInt("proceduralGrayScale", this->proceduralProps.proceduralGrayScale);
        ShaderSystem::to2DProcedural().setFloat("proceduralBrightness", this->proceduralProps.proceduralBrightness);
        ShaderSystem::to2DProcedural().setInt("proceduralUseTexCoords", this->proceduralProps.proceduralUseTexCoords);
        ShaderSystem::to2DProcedural().setVec4("smartProperties", this->proceduralProps.smartProperties);
        ShaderSystem::to2DProcedural().setVec2("txtrRes", this->getResolution());

        ShaderSystem::to2DProcedural().setInt("proceduralTexture", GL::bindTexture_2D((this->proceduralProps.proceduralID == 1000) ? this->ID : this->proceduralProps.proceduralTextureID, "Texture::generateProceduralTexture"));
        
        unsigned int wrapParam = GL_REPEAT; 

        if(this->proceduralProps.proceduralMirroredRepeat)
            wrapParam = GL_MIRRORED_REPEAT; 

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapParam);

        // Get the FBO
        Framebuffer FBO = FBOPOOL::requestFBO(destTxtr.ID, "Texture::generateProceduralTexture");

        // Clear destTxtr
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render result
        mesh.Draw();

        GL::releaseBoundTextures("Mesh::generateProceduralTexture");
        FBOPOOL::releaseFBO(FBO);
    }

    // ------- Generating Normal Map -------
    if(this->proceduralProps.proceduralNormalMap){
        destTxtr.applyNormalMap(this->proceduralProps.proceduralNormalStrength, this->proceduralProps.proceduralNormalGrayScale, false); 
    }

    destTxtr.removeSeams(mesh);

    already_bound_shader.use();
}

static Texture procTxtr;

Texture Texture::generateProceduralTexture(Mesh &mesh, int textureRes){
    if(!procTxtr.ID){
        glGenTextures(1, &procTxtr.ID);
    }
    
    procTxtr.update((char*)nullptr, textureRes, textureRes);
    
    generateProceduralTexture(mesh, procTxtr, textureRes);
    
    return procTxtr;
}