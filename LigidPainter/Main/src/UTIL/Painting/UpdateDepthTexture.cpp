/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

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

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

void Painter::updateDepthTexture(Model &model, glm::vec2 windowScale){
    glDepthFunc(GL_LESS);
    
    //Create the capture framebuffer
    unsigned int captureFBO;
    glGenFramebuffers(1,&captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER,captureFBO);
    
    //Render buffer for depth testing (refresh the RBO if the videoscale value is changed)
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthRBO);

    //Bind the depth texture (Painter class public member variable)
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthTexture, 0);

    glViewport(0,0,videoScale.x,videoScale.y);

    //Clear the depth texture
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Use the depth 3D shader
    depth3DShader.use();
    //Since that shader is using the same vertex shader with 3d model shader

    //Set the model matrix
    glm::mat4 modelMatrix = glm::mat4(1);
    depth3DShader.setMat4("modelMatrix",modelMatrix);

    //Draw the selected mesh in 3D
    if(selectedMeshIndex < model.meshes.size())
        model.meshes[selectedMeshIndex].Draw();

    //!Finished

    //Set back to default shader
    buttonShader.use();

    //Delete the capture framebuffer
    glDeleteFramebuffers(1,&captureFBO);

    glViewport(0,0,windowScale.x,windowScale.y);

    glDepthFunc(GL_LEQUAL);

}