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

static Texture normalMapTxtr;
static Texture normalMapTxtrBlurred;
static Texture noiseTxtr;
static Texture destTxtrCopy;

void apply_edge_wear(Mesh &mesh, Texture srcTxtr, Texture& destTxtr, int textureRes){
    if(!normalMapTxtr.ID){
        normalMapTxtr = Texture((char*)nullptr, 1024, 1024);
        normalMapTxtrBlurred = Texture((char*)nullptr, 1024, 1024);
        noiseTxtr = Texture((char*)nullptr, 1024, 1024);
        destTxtrCopy = Texture((char*)nullptr, textureRes, textureRes);
    }

    destTxtrCopy.update((char*)nullptr, textureRes, textureRes);
    
    // noiseTxtr
    {
        // Generate the noise texture
        noiseTxtr.proceduralProps.proceduralID = 74;
        noiseTxtr.generateProceduralTexture(mesh, noiseTxtr, 1024);
    }

    // normalMapTxtr
    {
        // Get framebuffer
        Framebuffer FBO = FBOPOOL::requestFBO(normalMapTxtr, "apply_edge_wear");
        
        // Clear the normalMapTxtr
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set shader
        ShaderSystem::normalVectorShader().use();
        ShaderSystem::normalVectorShader().setMat4("orthoProjection", glm::ortho(0.f,1.f,0.f,1.f));
        ShaderSystem::normalVectorShader().setMat4("perspectiveProjection", getScene()->projectionMatrix);
        ShaderSystem::normalVectorShader().setMat4("view", getScene()->camera.viewMatrix);

        // Render the result
        mesh.Draw("Texture::apply_edge_wear");

        FBOPOOL::releaseFBO(FBO);
    }
    
    // normalMapTxtrBlurred
    {
        // Get framebuffer
        Framebuffer FBO = FBOPOOL::requestFBO(normalMapTxtrBlurred, "apply_edge_wear");
        
        // Clear the normalMapTxtrBlurred
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set shader
        ShaderSystem::bluringShader().use();
        ShaderSystem::bluringShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
        ShaderSystem::bluringShader().setMat4("projectedPosProjection", glm::ortho(0.f, 1.f, 1.f, 0.f));
        ShaderSystem::bluringShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
        ShaderSystem::bluringShader().setVec2("scale", glm::vec2(0.5f));
        
        ShaderSystem::bluringShader().setInt("txtr", GL::bindTexture_2D(normalMapTxtr.ID, "Texture::generateProceduralTexture"));
        ShaderSystem::bluringShader().setInt("uvMask", GL::bindTexture_2D(mesh.uvMask.ID, "Texture::generateProceduralTexture"));
        ShaderSystem::bluringShader().setVec2("txtrRes", glm::vec2(textureRes));
        ShaderSystem::bluringShader().setFloat("blurVal", srcTxtr.proceduralProps.smartProperties.x * 35.f);
        
        // Render result
        getBox()->draw("Texture::generateProceduralTexture : Edge wear : Blurred normal vec");

        GL::releaseBoundTextures("Texture::generateProceduralTexture");
        FBOPOOL::releaseFBO(FBO);
    }

    // destTxtr
    {
        // Get framebuffer
        Framebuffer FBO = FBOPOOL::requestFBO(destTxtr, "apply_edge_wear");
        
        // Clear the destTxtr
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        ShaderSystem::edgeWearShader().use();
        ShaderSystem::edgeWearShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
        ShaderSystem::edgeWearShader().setMat4("projectedPosProjection", glm::ortho(0.f, 1.f, 1.f, 0.f));
        ShaderSystem::edgeWearShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
        ShaderSystem::edgeWearShader().setVec2("scale", glm::vec2(0.5f));
        
        ShaderSystem::edgeWearShader().setInt("normalVectorTxtr", GL::bindTexture_2D(normalMapTxtr.ID, "Texture::generateProceduralTexture"));
        ShaderSystem::edgeWearShader().setInt("normalVectorTxtrBlurred", GL::bindTexture_2D(normalMapTxtrBlurred.ID, "Texture::generateProceduralTexture"));
        ShaderSystem::edgeWearShader().setInt("noiseTexture", GL::bindTexture_2D(noiseTxtr.ID, "Texture::generateProceduralTexture"));
        ShaderSystem::edgeWearShader().setInt("invert", srcTxtr.proceduralProps.proceduralnverted);
        ShaderSystem::edgeWearShader().setFloat("brightness", srcTxtr.proceduralProps.proceduralBrightness);
        ShaderSystem::edgeWearShader().setFloat("softness", srcTxtr.proceduralProps.smartProperties.y);
        ShaderSystem::edgeWearShader().setFloat("noiseStrength", srcTxtr.proceduralProps.smartProperties.w);
        
        getBox()->draw("Texture::generateProceduralTexture : Edge wear : Result");
        
        GL::releaseBoundTextures("Texture::generateProceduralTexture");
        FBOPOOL::releaseFBO(FBO);
    }

    // Blur the result
    {
        destTxtr.duplicateTexture(destTxtrCopy, "apply_edge_wear");
        
        Framebuffer FBO = FBOPOOL::requestFBO(destTxtr, "apply_edge_wear");
        
        // Clear the destTxtr
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Bluring the result        
        ShaderSystem::bluringShader().use();
        ShaderSystem::bluringShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
        ShaderSystem::bluringShader().setMat4("projectedPosProjection", glm::ortho(0.f, 1.f, 1.f, 0.f));
        ShaderSystem::bluringShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
        ShaderSystem::bluringShader().setVec2("scale", glm::vec2(0.5f));
        
        ShaderSystem::bluringShader().setInt("txtr", GL::bindTexture_2D(destTxtrCopy.ID, "Texture::generateProceduralTexture"));
        ShaderSystem::bluringShader().setInt("uvMask", GL::bindTexture_2D(mesh.uvMask.ID, "Texture::generateProceduralTexture"));
        ShaderSystem::bluringShader().setVec2("txtrRes", glm::vec2(textureRes));
        ShaderSystem::bluringShader().setFloat("blurVal", srcTxtr.proceduralProps.smartProperties.z * 35.f);
        
        getBox()->draw("Texture::generateProceduralTexture : Edge wear : Blurring result");

        GL::releaseBoundTextures("Texture::generateProceduralTexture");
        FBOPOOL::releaseFBO(FBO);
    }
}