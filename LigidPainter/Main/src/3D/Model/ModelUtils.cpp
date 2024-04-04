/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

#include "UTIL/Util.hpp"
#include "UTIL/Mouse/Mouse.hpp"

#include "GUI/GUI.hpp"
#include "GUI/Panels.hpp"

#include "3D/Model/Model.hpp"

void Model::generateDisplayingTexture(){
    
    const int displayRes = 512;

    //Generating the displaying texture
    if(displayingTxtr.ID == 0)
        displayingTxtr = Texture(nullptr, displayRes, displayRes, GL_LINEAR);
    else
        displayingTxtr.update(nullptr, displayRes, displayRes, GL_LINEAR);

    Framebuffer FBO = Framebuffer(displayingTxtr, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(displayRes)), "Generating model displaying texture");

    glDisable(GL_CULL_FACE);

    glm::vec3 camPos = glm::vec3(3.5f, 0.f, 0.f);

    //Move the camera to the side
    glm::mat4 view = glm::lookAt(camPos, 
                                 glm::vec3(0), 
                                 glm::vec3(0.0, 1.0, 0.0));
    
    //The perspective projection matrix    
    glm::mat4 projectionMatrix = glm::perspective(
                                                    glm::radians(35.f), //Fov  
                                                    -1.f,  //Ratio (is 1 since the width & the height is equal to displayRes)
                                                    100.f,  //Near (the model is pretty close to the camera actually  ) 
                                                    0.1f    //Far
                                                );
    
    //!Update the model displaying texture
    
    FBO.bind();

    //Set the OpenGL viewport to the resolution of the model displaying texture
    glViewport(0,0,displayRes,displayRes);

    //Clear the capture framebuffer (displaying texture) with alpha zero color
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Use the 3D model rendering shader
    ShaderSystem::solidShadingShader().use();

    //Throw the camera data to the shader
    ShaderSystem::solidShadingShader().setMat4("projection", projectionMatrix);
    ShaderSystem::solidShadingShader().setMat4("view", view);
    ShaderSystem::solidShadingShader().setMat4("modelMatrix", glm::mat4(1));
    
    ShaderSystem::solidShadingShader().setVec3("viewPos", camPos);
    
    //Draw the sphere
    this->Draw();
    
    //!Finish (prepare rendering the GUI)

    //Use the button shader (Is necessary since that process is done in the middle of GUI rendering) 
    ShaderSystem::buttonShader().use();

    //Bind the default framebuffer
    Settings::defaultFramebuffer()->FBO.bind();
    
    //Set the OpenGL viewport to default
    Settings::defaultFramebuffer()->setViewport();    
    
    /* Delete the framebuffer & the renderbuffer*/
    FBO.deleteBuffers(false, true);
}