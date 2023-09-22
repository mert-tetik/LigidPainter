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
    
    //Resolution of the model displaying texture
    const int displayRes = 512;

    //Generating the displaying texture
    if(this->displayingTxtr.ID == 0)
        this->displayingTxtr = Texture(nullptr, displayRes, displayRes, GL_LINEAR);
    else
        this->displayingTxtr.update(nullptr, displayRes, displayRes, GL_LINEAR);

    Framebuffer FBO = Framebuffer(this->displayingTxtr, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(displayRes)));

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