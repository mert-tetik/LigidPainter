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
#include <fstream>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"

Filter::Filter(){

}

int Filter::load(std::string path){
    std::ifstream rf(path);

    if (!rf.is_open()) {
        LGDLOG::start << "ERROR : Reading filter file : Can't open the file : " << path << LGDLOG::end;
        return 0;
    }

    std::string firstLine;
    if (!std::getline(rf, firstLine)) {
        LGDLOG::start << "ERROR : Reading filter file : Can't read the header : " << path << LGDLOG::end;
        return 0;
    }
    
    if(firstLine != "#ligidFilter"){
        LGDLOG::start << "ERROR : Reading filter file : Can't locate the header : " << path << LGDLOG::end;
        return 0;
    }

    std::string code;

    code += "#version 400 core\n";
    
    std::string line;
    while (std::getline(rf, line))
    {
        code += line + "\n";
    }
    
    this->shader.loadShaderPS("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", code);

    this->generateDisplayingTexture();

    return 1;
}

void Filter::generateDisplayingTexture(){
    glm::vec2 txtrRes = Settings::appTextures().greetingDialogImage.getResolution();

    glGenTextures(1, &this->displayingTxtr);
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, this->displayingTxtr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    //Displaying resolution
    glm::vec2 displayRes = glm::vec2(256);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, displayRes.x, displayRes.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);

    //Create the framebuffer
    unsigned int captureFBO;
    glGenFramebuffers(1,&captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER,captureFBO);
    
    //Bind the displaying texture to the capture framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->displayingTxtr, 0);

    //Clear the capture frame buffer(displaying texture) with color alpha zero
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, displayRes.x, displayRes.y);
    
    this->shader.use();

    glm::vec2 scale = displayRes / glm::vec2(2);
    glm::vec3 pos = glm::vec3(displayRes / glm::vec2(2),1.f);
    glm::mat4 projection = glm::ortho(0.f, displayRes.x, displayRes.y, 0.f);
    this->shader.setVec2("scale", scale); //Cover the screen
    this->shader.setVec3("pos", pos); //Cover the screen
    this->shader.setMat4("projection", projection); //Cover the screen
    
    this->shader.setInt("txtr", 0); //Cover the screen
    this->shader.setVec2("txtrResolution", txtrRes); //Cover the screen

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Settings::appTextures().greetingDialogImage.ID);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //Finish
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ShaderSystem::buttonShader().use();

    glDeleteFramebuffers(1,&captureFBO);
}