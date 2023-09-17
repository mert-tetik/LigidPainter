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

    this->srcCode = "";
    this->srcCode += "#version 400 core\n";
    
    std::string line;
    while (std::getline(rf, line))
    {
        this->srcCode += line + "\n";
    }
    
    this->shader.loadShaderPS("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", this->srcCode);

    this->generateDisplayingTexture();

    this->title = UTIL::removeExtension(UTIL::getLastWordBySeparatingWithChar(path, UTIL::folderDistinguisher()));

    return 1;
}

void Filter::generateDisplayingTexture(){
    glm::vec2 txtrRes = Settings::appTextures().filterDisplayerImage.getResolution();

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
    glBindTexture(GL_TEXTURE_2D, Settings::appTextures().filterDisplayerImage.ID);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //Finish
    Settings::defaultFramebuffer()->FBO.bind();
    ShaderSystem::buttonShader().use();

    glDeleteFramebuffers(1,&captureFBO);
}

void Filter::applyFilter(unsigned int txtr){

    Texture txtrObject = txtr;
    glm::vec2 txtrRes = txtrObject.getResolution();

    Texture duplicatedTxtr = txtrObject.duplicateTexture();  

    //Displaying resolution
    glm::vec2 displayRes = txtrRes;

    //Create the framebuffer
    unsigned int captureFBO;
    glGenFramebuffers(1,&captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER,captureFBO);
    
    //Bind the displaying texture to the capture framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, txtr, 0);

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
    glBindTexture(GL_TEXTURE_2D, duplicatedTxtr.ID);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //Finish
    Settings::defaultFramebuffer()->FBO.bind();
    ShaderSystem::buttonShader().use();

    glDeleteTextures(1, &duplicatedTxtr.ID);
    glDeleteFramebuffers(1,&captureFBO);
}

#define LGDFILTER_WRITEBITS(var, type, loc) if(!wf.write(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                    LGDLOG::start<< "ERROR : Writing texture data. Failed to write at : " << loc << LGDLOG::end;\
                                    return false; \
                                }

bool Filter::writeFilterData(std::ofstream& wf){
    int32_t srcCodeCharSize = this->srcCode.size();
    LGDFILTER_WRITEBITS(srcCodeCharSize, int32_t, "Filter source code character size");

    for (size_t i = 0; i < srcCodeCharSize; i++)
    {
        char c = this->srcCode[i];
        LGDFILTER_WRITEBITS(c, char, "Filter source code character");
    }

    return true;
}

#define LGDMATERIAL_READBITS(var, type, loc) if(!rf.read(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                LGDLOG::start<< "ERROR : Reading lgdmaterial file. Failed to read at : " << loc << LGDLOG::end;\
                                return false; \
                            }

bool Filter::readFilterData(std::ifstream& rf){
    int32_t srcCodeCharSize;
    LGDMATERIAL_READBITS(srcCodeCharSize, int32_t, "Filter source code character size");

    this->srcCode = "";
    for (size_t i = 0; i < srcCodeCharSize; i++)
    {
        char c;
        LGDMATERIAL_READBITS(c, char, "Filter source code character");
        this->srcCode.push_back(c);
    }

    this->shader.loadShaderPS("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", this->srcCode);

    this->generateDisplayingTexture();

    this->title = "ReadenFilter";

    return true;
}