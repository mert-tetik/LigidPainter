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
#include<GLFW/glfw3.h>

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

#include "tinyfiledialogs.h"

void Painter::updateDepthTexture(Model &model){
    //Create the capture framebuffer
    unsigned int captureFBO;
    glGenFramebuffers(1,&captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER,captureFBO);
    
    //Bind the depth texture (Painter class public member variable)
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthTexture, 0);

    //Clear the depth texture
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    //Use the depth 3D shader
    depth3DShader.use();
    //Since that shader is using the same vertex shader with 3d model shader
    depth3DShader.setInt("useTransformUniforms",0); //Disable transform uniforms

    //Set the model matrix
    glm::mat4 modelMatrix = glm::mat4(1);
    depth3DShader.setMat4("modelMatrix",modelMatrix);

    //Draw the selected mesh in 3D
    model.meshes[0].Draw();//TODO SELECT THE MESH

    //!Finished

    //Set back to default shader
    buttonShader.use();

    //Delete the capture framebuffer
    glDeleteFramebuffers(1,&captureFBO);
}