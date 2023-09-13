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

#include <string>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::loadModel(std::string const &path,bool triangulate)
{
    title = UTIL::getLastWordBySeparatingWithChar(path,UTIL::folderDistinguisher());
    title = UTIL::removeExtension(title);

    this->newModelAdded = true;

    meshes.clear();
 
    //LGDLOG::start << "Loading : " << path;

    if(UTIL::toLowercase(UTIL::getFileExtension(path)) == "obj"){
        Model model = FileHandler::readOBJFile(path);
        this->meshes = model.meshes;
    }
    else if(UTIL::toLowercase(UTIL::getFileExtension(path)) == "fbx"){
        Model model = FileHandler::readFBXFile(path);
        this->meshes = model.meshes;
    }
    else{
        /* Print error message */
        LGDLOG::start<< 
            "ERROR : Can't read 3D model file. LigidPainter does not have a valid importer for the file format : " << 
            UTIL::toLowercase(UTIL::getFileExtension(path)) << 
        LGDLOG::end;

        return;
    }

    //Generating the displaying texture

    if(this->displayingTxtr == 0)
        glGenTextures(1, &this->displayingTxtr);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,this->displayingTxtr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    //Resolution of the material displaying texture
    const int displayRes = 512;
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, displayRes, displayRes, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);

    unsigned int FBO;

    //Capturing framebuffer
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    
    //Render buffer to render the sphere 3D model (for depth testing)
    unsigned int RBO;
	glGenRenderbuffers(1,&RBO);
	glBindRenderbuffer(GL_RENDERBUFFER,RBO);

    glDisable(GL_CULL_FACE);
	
    //Set the renderbuffer to store depth
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, displayRes, displayRes);
	
    //Give the renderbuffer to the framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);
    
    //Bind the displaying texture of the material to the capture framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->displayingTxtr, 0);

    glm::vec3 camPos = glm::vec3(7.f, 0.f, 0.f);

    //Move the camera to the side
    glm::mat4 view = glm::lookAt(camPos, 
                                 glm::vec3(0), 
                                 glm::vec3(0.0, 1.0, 0.0));
    
    //The perspective projection matrix    
    glm::mat4 projectionMatrix = glm::perspective(
                                                    glm::radians(35.f), //Fov  
                                                    -1.f,  //Ratio (is 1 since the width & the height is equal to displayRes)
                                                    100.f,  //Near (the material is pretty close to the camera actually  ) 
                                                    0.1f    //Far
                                                );
    
    //!Update the material displaying texture
    
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    

    //Set the OpenGL viewport to the resolution of the material displaying texture
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
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    
    //Set the OpenGL viewport to default
    glViewport(0, 0, getContext()->windowScale.x, getContext()->windowScale.y);
    glDeleteFramebuffers(1, &FBO);
    glDeleteRenderbuffers(1, &RBO);
}