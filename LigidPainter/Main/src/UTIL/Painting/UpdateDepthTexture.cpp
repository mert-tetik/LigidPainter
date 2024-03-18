/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Settings/Settings.hpp"

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
    glDisable(GL_BLEND);
    
    glm::ivec2 depthRes = glm::vec2(getBufferResolutions(1));

    //Bind the capture framebuffer
    paintingFBO.setRenderbuffer(depthRBO1024);
    paintingFBO.bind();

    for (size_t i = 0; i < depthTextures.size(); i++)
    {
        //Bind the depth texture (Painter class public member variable)
        paintingFBO.setColorBuffer(depthTextures[i].depthTexture, GL_TEXTURE_2D);

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
        ShaderSystem::depth3D().setInt("meshMask", 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->faceSelection.selectedFaces.ID);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->faceSelection.meshMask.ID);

        //Draw the selected mesh in 3D
        if(selectedMeshIndex < getModel()->meshes.size()){
            ShaderSystem::depth3D().setInt("primitiveCount", getModel()->meshes[selectedMeshIndex].indices.size() / 3);
            getModel()->meshes[selectedMeshIndex].Draw(false);
        }
    }

    //!Finished

    //Set back to default shader
    ShaderSystem::buttonShader().use();

    this->paintingFBO.removeRenderbuffer();

    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
} 