/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Create the depth texture to depth test while painting (to prevent painting the backside of the model) (by rendering the 3D model with a special shader)

*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "ShaderSystem/Shader.hpp"
#include "SettingsSystem/Settings.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

void Painter::updateDepthTexture(){
    std::vector<MirrorSide> depthTextures;
    
    if(this->oSide.active)
        depthTextures.push_back(oSide);
    if(this->oXSide.active)
        depthTextures.push_back(oXSide);
    if(this->oYSide.active)
        depthTextures.push_back(oYSide);
    if(this->oXYSide.active)
        depthTextures.push_back(oXYSide);
    if(this->oZSide.active)
        depthTextures.push_back(oZSide);
    if(this->oXZSide.active)
        depthTextures.push_back(oXZSide);
    if(this->oYZSide.active)
        depthTextures.push_back(oYZSide);
    if(this->oXYZSide.active)
        depthTextures.push_back(oXYZSide);        
    
    glDepthFunc(GL_LESS);
    
    glm::ivec2 depthRes = glm::ivec2(*Settings::videoScale() / Settings::properties()->paintingDepthTextureResolutionDivier);

    //Create the capture framebuffer
    unsigned int captureFBO;
    glGenFramebuffers(1,&captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER,captureFBO);
    
    //Render buffer for depth testing (refresh the RBO if the videoscale value is changed)
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthRBO);

    for (size_t i = 0; i < depthTextures.size(); i++)
    {
        //Bind the depth texture (Painter class public member variable)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthTextures[i].depthTexture.ID, 0);

        //Clear the depth texture
        glViewport(0, 0, depthRes.x, depthRes.y);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 view = depthTextures[i].getViewMat(glm::vec3(this->mirrorXOffset,this->mirrorYOffset,this->mirrorZOffset));

        //Use the depth 3D shader
        ShaderSystem::depth3D().use();
        ShaderSystem::depth3D().setMat4("view", view);
        ShaderSystem::depth3D().setMat4("projection", getScene()->projectionMatrix);
        ShaderSystem::depth3D().setMat4("modelMatrix",getScene()->transformMatrix);
        ShaderSystem::depth3D().setInt("usingMeshSelection",this->faceSelection.activated);
        ShaderSystem::depth3D().setInt("hideUnselected",this->faceSelection.hideUnselected);
        ShaderSystem::depth3D().setInt("selectedPrimitiveIDS", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->faceSelection.selectedFaces.ID);

        //Draw the selected mesh in 3D
        if(selectedMeshIndex < getModel()->meshes.size())
            getModel()->meshes[selectedMeshIndex].Draw(false);
    }
    


    //!Finished

    //Set back to default shader
    ShaderSystem::buttonShader().use();

    //Delete the capture framebuffer
    glDeleteFramebuffers(1,&captureFBO);

    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();

    glDepthFunc(GL_LEQUAL);

}