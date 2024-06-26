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
    
    Shader already_bound_shader = ShaderUTIL::get_bound_shader();

    const int displayRes = 512;

    //Generating the displaying texture
    if(displayingTxtr.ID == 0)
        displayingTxtr = Texture((char*)nullptr, displayRes, displayRes, GL_LINEAR);
    else
        displayingTxtr.update((char*)nullptr, displayRes, displayRes, GL_LINEAR);


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
    
    Framebuffer FBO = FBOPOOL::requestFBO_with_RBO(displayingTxtr, displayingTxtr.getResolution(), "Generating model displaying texture");

    //Set the OpenGL viewport to the resolution of the model displaying texture
    glViewport(0,0,displayRes,displayRes);

    //Clear the capture framebuffer (displaying texture) with alpha zero color
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Use the 3D model rendering shader
    ShaderSystem::solidShadingShader().use();

    ShaderSystem::solidShadingShader().setInt("use_txtr", false);

    //Throw the camera data to the shader
    ShaderSystem::solidShadingShader().setMat4("projection", projectionMatrix);
    ShaderSystem::solidShadingShader().setMat4("view", view);
    ShaderSystem::solidShadingShader().setMat4("modelMatrix", glm::mat4(1));
    
    ShaderSystem::solidShadingShader().setVec3("viewPos", camPos);
    
    // Render result (the model)
    this->Draw();
    
    ShaderUTIL::release_bound_shader();

    /* Delete the framebuffer & the renderbuffer*/
    FBOPOOL::releaseFBO(FBO);
}

static float getBig(glm::vec3 vec){
    float minVal = std::min(vec.x, std::min(vec.y, vec.z));
    float maxVal = std::max(vec.x, std::max(vec.y, vec.z));
    
    if (vec.x != minVal && vec.x != maxVal) {
        return vec.x;
    } 
    else if (vec.y != minVal && vec.y != maxVal) {
        return vec.y;
    } 
    else {
        return vec.z;
    }
}

void Model::resize_meshes(){
    
    glm::vec3 big = glm::vec3(0.f);
    glm::vec3 center = glm::vec3(0.f);
    int counter = 0;
    for (size_t i = 0; i < meshes.size(); i++)
    {
        for (size_t vi = 0; vi < meshes[i].vertices.size(); vi++)
        {
            if(big.x < abs(meshes[i].vertices[vi].Position.x))
                big.x = abs(meshes[i].vertices[vi].Position.x);
            
            if(big.y < abs(meshes[i].vertices[vi].Position.y))
                big.y = abs(meshes[i].vertices[vi].Position.y);
            
            if(big.z < abs(meshes[i].vertices[vi].Position.z))
                big.z = abs(meshes[i].vertices[vi].Position.z);
        
            center += meshes[i].vertices[vi].Position;

            counter++;
        }
    }

    if(big.x == 0 || big.y == 0 || big.z == 0){
        return;
    }

    center /= counter;
    center /= big;
    
    for (size_t i = 0; i < meshes.size(); i++){
        for (size_t vi = 0; vi < meshes[i].vertices.size(); vi++)
        {
            meshes[i].vertices[vi].Position.x = meshes[i].vertices[vi].Position.x / getBig(big);
            meshes[i].vertices[vi].Position.y = meshes[i].vertices[vi].Position.y / getBig(big);
            meshes[i].vertices[vi].Position.z = meshes[i].vertices[vi].Position.z / getBig(big);

            meshes[i].vertices[vi].Position -= center;
        }
    }
    
    big = glm::vec3(0.f);

    for (size_t i = 0; i < meshes.size(); i++)
    {
        for (size_t vi = 0; vi < meshes[i].vertices.size(); vi++)
        {
            if(big.x < abs(meshes[i].vertices[vi].Position.x))
                big.x = abs(meshes[i].vertices[vi].Position.x);
            
            if(big.y < abs(meshes[i].vertices[vi].Position.y))
                big.y = abs(meshes[i].vertices[vi].Position.y);
            
            if(big.z < abs(meshes[i].vertices[vi].Position.z))
                big.z = abs(meshes[i].vertices[vi].Position.z);
        }
    }

    for (size_t i = 0; i < meshes.size(); i++){
        for (size_t vi = 0; vi < meshes[i].vertices.size(); vi++)
        {
            meshes[i].vertices[vi].Position.x = meshes[i].vertices[vi].Position.x / getBig(big);
            meshes[i].vertices[vi].Position.y = meshes[i].vertices[vi].Position.y / getBig(big);
            meshes[i].vertices[vi].Position.z = meshes[i].vertices[vi].Position.z / getBig(big);
        }
    }

    for (size_t i = 0; i < meshes.size(); i++){
        meshes[i].update_vertex_buffers();
    }
}