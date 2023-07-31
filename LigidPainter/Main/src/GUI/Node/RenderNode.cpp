/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "GUI/Elements/Elements.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <map>

static void initTexture(unsigned int &txtr, int textureRes){
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1,&txtr);
    glBindTexture(GL_TEXTURE_2D,txtr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureRes, textureRes, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
}

std::string cToS(glm::vec4 clr){
    if(clr == glm::vec4(1.,1.,1.,1.))
        return "white";
    if(clr == glm::vec4(0.,0.,0.,1.))
        return "black";
    if(clr == glm::vec4(1.,0.,0.,1.))
        return "red";
    if(clr == glm::vec4(1.,1.,0.,1.))
        return "yellow";
    if(clr == glm::vec4(1.,0.5,0.,1.))
        return "orange";
    if(clr == glm::vec4(0.,1.,0.,1.))
        return "green";
    if(clr == glm::vec4(0.,1.,1.,1.))
        return "cyan";
    if(clr == glm::vec4(0.,0.,1.,1.))
        return "blue";
    if(clr == glm::vec4(1.,0.,1.,1.))
        return "pink";

    return "other";
}

Mesh processNode(Node &node, std::vector<Node> &nodeScene, Library library, Mesh& mesh, Shader heightToNormalShader, Scene scene, int textureRes){
    
    std::cout << glGetError() << std::endl;
    
    std::cout << "AA" << std::endl;
    Mesh msh = mesh;
    initTexture(msh.albedo.ID, textureRes);
    initTexture(msh.roughness.ID, textureRes);
    initTexture(msh.metallic.ID, textureRes);
    initTexture(msh.normalMap.ID, textureRes);
    initTexture(msh.heightMap.ID, textureRes);
    initTexture(msh.ambientOcclusion.ID, textureRes);

    std::cout << "BB" << std::endl;
    if(node.nodeIndex == MATERIAL_ID_NODE){
        
        Box box;
        box.init();
        box.bindBuffers();
        
        // Load the id masking shader
        Shader idMaskingShader = Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/MaterialID.frag" ,nullptr, nullptr, nullptr);
        
        bool detectedWhite = false; 
        bool detectedRed = false; 
        bool detectedGreen = false; 
        bool detectedBlue = false; 
        bool detectedPink = false; 
        bool detectedYellow = false; 
        bool detectedOrange = false; 
        bool detectedCyan = false; 
        bool detectedBlack = false;
        
        std::cout << "CC" << std::endl;
        
        std::map<std::string, Mesh> retrievedMeshes;
        for (size_t i = 0; i < node.IOs.size(); i++)
        {
            // 0 : input
            // 1 : input without connections 
            // 2 : output
            // 3 : output without connections 
            if(node.IOs[i].state == 0){
                //An input can only have one connection unlike the outputs
                
                //TODO : If input has no connections
                retrievedMeshes[cToS(node.IOs[i].element.button.color)] = processNode(nodeScene[node.IOs[i].connections[0].nodeIndex], nodeScene, library, mesh, heightToNormalShader, scene, textureRes);
            }
        }
        std::cout << "DD" << std::endl;

        idMaskingShader.use();
        idMaskingShader.setMat4("projection", glm::ortho(0.f,1.f,0.f,1.f));
        idMaskingShader.setMat4("projectedPosProjection", glm::ortho(0.f,1.f,0.f,1.f));
        idMaskingShader.setVec3("pos", glm::vec3(0.5f));
        idMaskingShader.setVec2("scale", glm::vec2(0.5f));

        idMaskingShader.setInt("IDTexture", 0);
        idMaskingShader.setInt("texture_black", 1);
        idMaskingShader.setInt("texture_white", 2);
        idMaskingShader.setInt("texture_red", 3);
        idMaskingShader.setInt("texture_green", 4);
        idMaskingShader.setInt("texture_blue", 5);
        idMaskingShader.setInt("texture_yellow", 6);
        idMaskingShader.setInt("texture_orange", 7);
        idMaskingShader.setInt("texture_cyan", 8);
        idMaskingShader.setInt("texture_pink", 9);

        // Bind the id mask
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, node.IOs[0].element.button.texture.ID);
        
        std::cout << "EE" << std::endl;
        for (size_t channelI = 0; channelI < 6; channelI++)
        {
            
            /* Black */
            glActiveTexture(GL_TEXTURE1);
            if(channelI == 0)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 0.f , 1.f))].albedo.ID);
            if(channelI == 1)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 0.f , 1.f))].roughness.ID);
            if(channelI == 2)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 0.f , 1.f))].metallic.ID);
            if(channelI == 3)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 0.f , 1.f))].normalMap.ID);
            if(channelI == 4)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 0.f , 1.f))].heightMap.ID);
            if(channelI == 5)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 0.f , 1.f))].ambientOcclusion.ID);
            
            /* White */
            glActiveTexture(GL_TEXTURE2);
            if(channelI == 0)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 1.f , 1.f))].albedo.ID);
            if(channelI == 1)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 1.f , 1.f))].roughness.ID);
            if(channelI == 2)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 1.f , 1.f))].metallic.ID);
            if(channelI == 3)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 1.f , 1.f))].normalMap.ID);
            if(channelI == 4)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 1.f , 1.f))].heightMap.ID);
            if(channelI == 5)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 1.f , 1.f))].ambientOcclusion.ID);
            
            /* Red */
            glActiveTexture(GL_TEXTURE3);
            if(channelI == 0)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 0.f , 1.f))].albedo.ID);
            if(channelI == 1)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 0.f , 1.f))].roughness.ID);
            if(channelI == 2)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 0.f , 1.f))].metallic.ID);
            if(channelI == 3)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 0.f , 1.f))].normalMap.ID);
            if(channelI == 4)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 0.f , 1.f))].heightMap.ID);
            if(channelI == 5)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 0.f , 1.f))].ambientOcclusion.ID);
            
            /* Green */
            glActiveTexture(GL_TEXTURE4);
            if(channelI == 0)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 0.f , 1.f))].albedo.ID);
            if(channelI == 1)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 0.f , 1.f))].roughness.ID);
            if(channelI == 2)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 0.f , 1.f))].metallic.ID);
            if(channelI == 3)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 0.f , 1.f))].normalMap.ID);
            if(channelI == 4)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 0.f , 1.f))].heightMap.ID);
            if(channelI == 5)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 0.f , 1.f))].ambientOcclusion.ID);
            
            /* Blue */
            glActiveTexture(GL_TEXTURE5);
            if(channelI == 0)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 1.f , 1.f))].albedo.ID);
            if(channelI == 1)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 1.f , 1.f))].roughness.ID);
            if(channelI == 2)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 1.f , 1.f))].metallic.ID);
            if(channelI == 3)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 1.f , 1.f))].normalMap.ID);
            if(channelI == 4)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 1.f , 1.f))].heightMap.ID);
            if(channelI == 5)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 1.f , 1.f))].ambientOcclusion.ID);
            
            /* Yellow */
            glActiveTexture(GL_TEXTURE6);
            if(channelI == 0)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 0.f , 1.f))].albedo.ID);
            if(channelI == 1)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 0.f , 1.f))].roughness.ID);
            if(channelI == 2)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 0.f , 1.f))].metallic.ID);
            if(channelI == 3)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 0.f , 1.f))].normalMap.ID);
            if(channelI == 4)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 0.f , 1.f))].heightMap.ID);
            if(channelI == 5)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 0.f , 1.f))].ambientOcclusion.ID);
            
            /* Orange */
            glActiveTexture(GL_TEXTURE7);
            if(channelI == 0)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 0.5f, 0.f , 1.f))].albedo.ID);
            if(channelI == 1)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 0.5f, 0.f , 1.f))].roughness.ID);
            if(channelI == 2)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 0.5f, 0.f , 1.f))].metallic.ID);
            if(channelI == 3)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 0.5f, 0.f , 1.f))].normalMap.ID);
            if(channelI == 4)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 0.5f, 0.f , 1.f))].heightMap.ID);
            if(channelI == 5)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 0.5f, 0.f , 1.f))].ambientOcclusion.ID);
            
            /* Cyan */
            glActiveTexture(GL_TEXTURE8);
            if(channelI == 0)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 1.f , 1.f))].albedo.ID);
            if(channelI == 1)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 1.f , 1.f))].roughness.ID);
            if(channelI == 2)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 1.f , 1.f))].metallic.ID);
            if(channelI == 3)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 1.f , 1.f))].normalMap.ID);
            if(channelI == 4)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 1.f , 1.f))].heightMap.ID);
            if(channelI == 5)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 1.f , 1.f))].ambientOcclusion.ID);
            
            /* Pink */
            glActiveTexture(GL_TEXTURE9);
            if(channelI == 0)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 1.f , 1.f))].albedo.ID);
            if(channelI == 1)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 1.f , 1.f))].roughness.ID);
            if(channelI == 2)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 1.f , 1.f))].metallic.ID);
            if(channelI == 3)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 1.f , 1.f))].normalMap.ID);
            if(channelI == 4)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 1.f , 1.f))].heightMap.ID);
            if(channelI == 5)
                glBindTexture(GL_TEXTURE_2D, retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 1.f , 1.f))].ambientOcclusion.ID);
        
            unsigned int mshID;

            if(channelI == 0)
                mshID = msh.albedo.ID;
            if(channelI == 1)
                mshID = msh.roughness.ID;
            if(channelI == 2)
                mshID = msh.metallic.ID;
            if(channelI == 3)
                mshID = msh.normalMap.ID;
            if(channelI == 4)
                mshID = msh.heightMap.ID;
            if(channelI == 5)
                mshID = msh.ambientOcclusion.ID;
            
            std::cout << "FF : " << mshID << std::endl;
            
            unsigned int FBO;
            glGenFramebuffers(1,&FBO);
            glBindFramebuffer(GL_FRAMEBUFFER,FBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mshID, 0);
            glViewport(0, 0, textureRes, textureRes);
            
            glClearColor(0.5,1,0,1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glDrawArrays(GL_TRIANGLES, 0 , 6);
            
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(0, &FBO);
        }

        std::cout << "GG" << std::endl;


        // DELETE ALL THE MESH CHANNEL TEXTURES
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 0.f , 1.f))].albedo.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 0.f , 1.f))].roughness.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 0.f , 1.f))].metallic.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 0.f , 1.f))].normalMap.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 0.f , 1.f))].heightMap.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 0.f , 1.f))].ambientOcclusion.ID);
        
        /* White */
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 1.f , 1.f))].albedo.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 1.f , 1.f))].roughness.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 1.f , 1.f))].metallic.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 1.f , 1.f))].normalMap.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 1.f , 1.f))].heightMap.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 1.f , 1.f))].ambientOcclusion.ID);
        
        /* Red */
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 0.f , 1.f))].albedo.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 0.f , 1.f))].roughness.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 0.f , 1.f))].metallic.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 0.f , 1.f))].normalMap.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 0.f , 1.f))].heightMap.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 0.f , 1.f))].ambientOcclusion.ID);
        
        /* Green */
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 0.f , 1.f))].albedo.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 0.f , 1.f))].roughness.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 0.f , 1.f))].metallic.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 0.f , 1.f))].normalMap.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 0.f , 1.f))].heightMap.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 0.f , 1.f))].ambientOcclusion.ID);
        
        /* Blue */
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 1.f , 1.f))].albedo.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 1.f , 1.f))].roughness.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 1.f , 1.f))].metallic.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 1.f , 1.f))].normalMap.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 1.f , 1.f))].heightMap.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 1.f , 1.f))].ambientOcclusion.ID);
        
        /* Yellow */
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 0.f , 1.f))].albedo.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 0.f , 1.f))].roughness.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 0.f , 1.f))].metallic.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 0.f , 1.f))].normalMap.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 0.f , 1.f))].heightMap.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 0.f , 1.f))].ambientOcclusion.ID);
        
        /* Orange */
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 0.5f, 0.f , 1.f))].albedo.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 0.5f, 0.f , 1.f))].roughness.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 0.5f, 0.f , 1.f))].metallic.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 0.5f, 0.f , 1.f))].normalMap.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 0.5f, 0.f , 1.f))].heightMap.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 0.5f, 0.f , 1.f))].ambientOcclusion.ID);
        
        /* Cyan */
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 1.f , 1.f))].albedo.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 1.f , 1.f))].roughness.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 1.f , 1.f))].metallic.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 1.f , 1.f))].normalMap.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 1.f , 1.f))].heightMap.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 1.f , 1.f))].ambientOcclusion.ID);
        
        /* Pink */
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 1.f , 1.f))].albedo.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 1.f , 1.f))].roughness.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 1.f , 1.f))].metallic.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 1.f , 1.f))].normalMap.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 1.f , 1.f))].heightMap.ID);
        glDeleteTextures(1, &retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 1.f , 1.f))].ambientOcclusion.ID);
            
        std::cout << "HH" << std::endl;

        //Return mesh by masking the retrieved meshes
        return msh;
    }
    else if(node.nodeIndex == MATERIAL_NODE){
        std::cout << "JJ" << std::endl;
        
        Material material;
        for (size_t i = 0; i < library.materials.size(); i++)
        {
            if(node.materialID == library.materials[i].uniqueID)
                material = library.materials[i];
        }
        
        std::cout << "KK" << std::endl;
        for (int i = material.materialModifiers.size() - 1; i >= 0; --i)    
        {
            material.materialModifiers[i].updateMaterialChannels(material, msh, textureRes, i, scene.projectionMatrix, scene.viewMatrix, heightToNormalShader);
        }

        std::cout << "LL" << std::endl;
        
        //Return mesh by creating using material modifiers
        return msh;
    }

    return msh;
}

void Node::render(  glm::vec2 videoScale,
                    Mouse& mouse,
                    Timer &timer,
                    TextRenderer &textRenderer,
                    Panel nodeEditorPanel,
                    std::vector<Node> &meshNodeScene,
                    int currentNodeIndex,
                    NodePanel& nodePanelData,
                    Library &library,
                    Model &model,
                    int textureRes,
                    Scene scene
                )
{
    

    //Barriers (In order to prevent the overflow)
    this->cursorOnBarriers = renderBarriers(nodeEditorPanel,mouse);

    this->nodePanel.additionalPos.x = nodePanelData.position.x + nodePanelData.mixVal.x;
    this->nodePanel.additionalPos.y = nodePanelData.position.y + nodePanelData.mixVal.y - nodeEditorPanel.scale.y * 2;

    //Render the node panel which contains the input buttons and stuff
    nodePanel.render(videoScale,mouse,timer,textRenderer, this->nodeIndex == MATERIAL_ID_NODE || this->nodeIndex == MATERIAL_MASK_NODE);

    if(nodePanel.sections[0].elements[0].button.clicked){
        std::vector<NodeIO> inputs;
        std::vector<NodeIO> outputs;
        std::vector<glm::vec3> palette;
        palette = nodePanel.sections[0].elements[0].button.texture.getMaterialIDPalette();
        
        inputs.push_back(NodeIO("Texture", nodePanel.sections[0].elements[0].button, colorPalette.mainColor,colorPalette,buttonShader,videoScale,1));

        for (size_t i = 0; i < palette.size(); i++)
        {
            inputs.push_back(NodeIO("Input1",Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1), this->colorPalette, buttonShader,"Input1", Texture(), 2.f,false)),colorPalette.mainColor,colorPalette,buttonShader,videoScale,0));
            inputs[inputs.size() - 1].element.button.color = glm::vec4(palette[i], 1.f);
        }

        outputs.push_back(NodeIO("Output",Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1), this->colorPalette, buttonShader,"Output", Texture(), 2.f,false)),colorPalette.mainColor,colorPalette,buttonShader,videoScale,2));
        
        this->uploadNewIOs(inputs, outputs);
    }


    //Auto scaling the node panel compatible with it's elements
    if(nodePanel.sections.size()){
        
        nodePanel.scale.y = 0;
        for (size_t i = 0; i < nodePanel.sections[0].elements.size(); i++)
        {
            nodePanel.scale.y += nodePanel.sections[0].elements[i].scale.y;
            nodePanel.scale.y += nodePanel.sections[0].elements[i].panelOffset/2.f;
        }

        nodePanel.scale.y += 2;

    }
    
    //Render & process all the inputs & outputs
    for (size_t i = 0; i < IOs.size(); i++)
    {
        //IO state 0 = input
        //IO state 1 = input without connection
        //IO state 2 = output
        //IO state 3 = output without connection

        //Don't render the IO circle if the IO has no connection
        if(IOs[i].state != 1 && IOs[i].state != 3){

            //Move the IO circle to the node panel IO button             
            IOs[i].IOCircle.pos = nodePanel.sections[0].elements[i].pos;
            IOs[i].IOCircle.pos.z += 0.01f;        
        
            
            //Move the IO Circle to the left side of the the IO element if is input
            if(IOs[i].state == 0){//Is input
                IOs[i].IOCircle.pos.x -= nodePanel.sections[0].elements[i].scale.x;
            }

            //Move the IO Circle to the right side of the the IO element if is input
            else if(IOs[i].state == 2){//Is output
                IOs[i].IOCircle.pos.x += nodePanel.sections[0].elements[i].scale.x;
            }

            //If pressed to the circle
            if(IOs[i].IOCircle.clickState1){

                nodePanel.leftSide.pressed = false;
                nodePanel.rightSide.pressed = false;
                
                //If an input is pressed that already has a connection
                if(IOs[i].state == 0 && IOs[i].connections.size()){
                    int resNodeI = 1000;
                    int resIOI = 1000;
                    
                    getTheIOConnectedToTheInput(resNodeI,resIOI,currentNodeIndex,i,meshNodeScene);

                    if(resNodeI != 1000 && resIOI != 1000){
                        meshNodeScene[resNodeI].IOs[resIOI].IOCircle.clickState1 = true;
                        IOs[i].IOCircle.clickState1 = false;
                    }
                    
                    clearConnections(currentNodeIndex,i,meshNodeScene);

                }

                //Draw a line from the circle to the cursor
                drawLine(
                            glm::vec2(IOs[i].IOCircle.pos.x,IOs[i].IOCircle.pos.y), //Circle pos
                            mouse.cursorPos/videoScale * 100.f, //Cursor pos in the range of 0 - 100
                            videoScale,
                            nodeEditorPanel,
                            IOs[i].state == 2   
                        );

                //First frame line released
                if(!mouse.LPressed){
                    
                    //Those values will remain 1000 if no IO circle is hovered
                    int hoveredNodeI = 1000;
                    int hoveredIOI = 1000;

                    getHoveredInputs(hoveredNodeI,hoveredIOI,meshNodeScene);

                    //A IO circle is hovered
                    if(hoveredNodeI != 1000 && hoveredIOI != 1000){
                        
                        //Delete the previous connection if an input IO is connected
                        if(IOs[i].state == 0)
                            clearConnections(currentNodeIndex,i,meshNodeScene);

                        //If the circle hovered already has a connection & is an input
                        if(doHaveConnection(hoveredNodeI,hoveredNodeI,meshNodeScene) && getStateData(hoveredNodeI,hoveredNodeI,meshNodeScene) == 0){
                           //Than remove the connections of the circle hovered
                            clearConnections(hoveredNodeI,hoveredIOI,meshNodeScene);
                        }

                        
                        //Create a connection 
                        createConnection(hoveredNodeI,hoveredIOI,currentNodeIndex,i,meshNodeScene);

                        Material material;
                        for (size_t i = 0; i < library.materials.size(); i++)
                        {
                            if(meshNodeScene[hoveredNodeI].nodeIndex == MATERIAL_NODE){
                                if(meshNodeScene[hoveredNodeI].materialID == library.materials[i].uniqueID)
                                    material = library.materials[i];
                            }
                            else if(meshNodeScene[currentNodeIndex].nodeIndex == MATERIAL_NODE){
                                if(meshNodeScene[currentNodeIndex].materialID == library.materials[i].uniqueID)
                                    material = library.materials[i];
                            }
                        }

                        /*
                        int meshI = 0;
                        if(meshNodeScene[hoveredNodeI].nodeIndex == MATERIAL_NODE){
                            meshI = hoveredIOI;
                        }
                        else if(meshNodeScene[currentNodeIndex].nodeIndex == MATERIAL_NODE){
                            meshI = i;
                        }
                        */
                        
                        // TODO : Clear textures when node connection is disconnected 
                        if(meshNodeScene[hoveredNodeI].nodeIndex == MESH_NODE || meshNodeScene[currentNodeIndex].nodeIndex == MESH_NODE){
                            std::vector<Node> pipeline;
                            if(meshNodeScene[hoveredNodeI].nodeIndex == MESH_NODE){ //The hovered node is a mesh node (another node to the mesh node)
                                Mesh retMesh = processNode(meshNodeScene[currentNodeIndex], meshNodeScene, library, model.meshes[hoveredIOI], heightToNormalShader, scene, textureRes);
                                model.meshes[hoveredIOI].albedo.ID = retMesh.albedo.ID; 
                                model.meshes[hoveredIOI].roughness.ID = retMesh.roughness.ID; 
                                model.meshes[hoveredIOI].metallic.ID = retMesh.metallic.ID; 
                                model.meshes[hoveredIOI].normalMap.ID = retMesh.normalMap.ID; 
                                model.meshes[hoveredIOI].heightMap.ID = retMesh.heightMap.ID; 
                                model.meshes[hoveredIOI].ambientOcclusion.ID = retMesh.ambientOcclusion.ID; 

                                //TODO Delete old textures
                            }
                            else if(meshNodeScene[currentNodeIndex].nodeIndex == MESH_NODE){ //The node's itself is mesh node (mesh node to another) 
                                Mesh retMesh = processNode(meshNodeScene[hoveredNodeI], meshNodeScene, library, model.meshes[i], heightToNormalShader, scene, textureRes);
                                model.meshes[i].albedo.ID = retMesh.albedo.ID; 
                                model.meshes[i].roughness.ID = retMesh.roughness.ID; 
                                model.meshes[i].metallic.ID = retMesh.metallic.ID; 
                                model.meshes[i].normalMap.ID = retMesh.normalMap.ID; 
                                model.meshes[i].heightMap.ID = retMesh.heightMap.ID; 
                                model.meshes[i].ambientOcclusion.ID = retMesh.ambientOcclusion.ID; 
                                
                                //TODO Delete old textures
                            }

                            
                        }

                        /*
                        //For every modifier the material has (Output every modifier the material has)
                        for (int i = material.materialModifiers.size() - 1; i >= 0; --i)    
                        {
                            material.materialModifiers[i].updateMaterialChannels(material, model.meshes[meshI - 1], textureRes, i, scene.projectionMatrix, scene.viewMatrix, heightToNormalShader);
                        }
                        */
                    }
                }
            }

            //Render the connections of the IO if is an output
            if(IOs[i].state == 2){

                for (size_t conI = 0; conI < IOs[i].connections.size(); conI++)
                {

                    //Which node the connection is connected to
                    Node connectedNode = meshNodeScene[IOs[i].connections[conI].nodeIndex];

                    //Which IO circle the connection is connected to
                    NodeIO connectedIO = connectedNode.IOs[IOs[i].connections[conI].inputIndex]; 

                    drawLine(
                                glm::vec2(IOs[i].IOCircle.pos.x,IOs[i].IOCircle.pos.y), //Circle pos
                                glm::vec2(connectedIO.IOCircle.pos.x,connectedIO.IOCircle.pos.y), //Connected Circle pos
                                videoScale,
                                nodeEditorPanel,
                                1
                            );

                }
            }
            


            //Render the IO circle
            IOs[i].IOCircle.render(videoScale,mouse,timer,textRenderer,true);
        }
    }


    //Position the bar button        
    barButton.pos = nodePanel.pos;
    
    barButton.scale = nodePanel.scale;
    barButton.scale.y = 1.5f;
    
    barButton.pos.x += nodePanelData.position.x + nodePanelData.mixVal.x;
    barButton.pos.y = nodePanel.pos.y + nodePanelData.position.y + nodePanelData.mixVal.y - nodeEditorPanel.scale.y * 2 - nodePanel.scale.y; 
    barButton.pos.z += 0.02f;
    //Render the bar button
    barButton.render(videoScale,mouse,timer,textRenderer,!cursorOnBarriers);
    
    //Move the node panel if bar button is pressed
    if(barButton.clickState1 && !nodePanel.topSide.pressed){ //Pressed

        //Prevent moving multiple nodes
        for (size_t i = 0; i < meshNodeScene.size(); i++)
        {
            if(i != currentNodeIndex)
                meshNodeScene[i].barButton.clickState1 = false;
        }

        nodePanel.pos.x += mouse.mouseOffset.x/videoScale.x * 100.f;
        nodePanel.pos.y += mouse.mouseOffset.y/videoScale.y * 100.f;
    }
    
    glClear(GL_DEPTH_BUFFER_BIT);

    //buttonShader.setVec2("groupPos", glm::vec2(0));
    //buttonShader.setFloat("groupScale", 1);


}