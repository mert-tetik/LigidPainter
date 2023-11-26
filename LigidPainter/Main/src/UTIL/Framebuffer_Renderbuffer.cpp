/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

Box.hpp : Is used to render a single 2D square.
          Mostly used by GUI elements.
*/

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "ShaderSystem/Shader.hpp"
#include "GUI/GUI.hpp"
    
// --------- Renderbuffer -----------

Renderbuffer::Renderbuffer(){
    
}

Renderbuffer::Renderbuffer(unsigned int internalformat, unsigned int attachment, glm::ivec2 resolution){
    this->internalformat = internalformat;
    this->attachment = attachment;

    glGenRenderbuffers(1, &this->ID);
    glBindRenderbuffer(GL_RENDERBUFFER, this->ID);
    glRenderbufferStorage(GL_RENDERBUFFER, internalformat, resolution.x, resolution.y);
}

Renderbuffer::Renderbuffer(unsigned int internalformat, unsigned int attachment, glm::ivec2 resolution, int samples){
    this->internalformat = internalformat;
    this->attachment = attachment;

    glGenRenderbuffers(1, &this->ID);
    glBindRenderbuffer(GL_RENDERBUFFER, this->ID);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalformat, resolution.x, resolution.y);
}

void Renderbuffer::update(unsigned int internalformat, unsigned int attachment, glm::ivec2 resolution){
    this->internalformat = internalformat;
    this->attachment = attachment;

    glBindRenderbuffer(GL_RENDERBUFFER, this->ID);
    glRenderbufferStorage(GL_RENDERBUFFER, internalformat, resolution.x, resolution.y);
}

void Renderbuffer::update(unsigned int internalformat, unsigned int attachment, glm::ivec2 resolution, int samples){
    this->internalformat = internalformat;
    this->attachment = attachment;

    glBindRenderbuffer(GL_RENDERBUFFER, this->ID);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalformat, resolution.x, resolution.y);
}



// ------------ Framebuffer -------------

Framebuffer::Framebuffer(){

}

#define TEST_FRAMEBUFFER \
GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); \
if(status != GL_FRAMEBUFFER_COMPLETE){\
    std::cout << "ERROR : Framebuffer is not valid. Error code: " << status << std::endl; \
}


Framebuffer::Framebuffer(Texture colorBuffer, unsigned int textureTarget){
    this->colorBuffer = colorBuffer;
    
    glGenFramebuffers(1, &this->ID);
    glBindFramebuffer(GL_FRAMEBUFFER, this->ID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, colorBuffer.ID, 0);

    TEST_FRAMEBUFFER
}

Framebuffer::Framebuffer(Texture colorBuffer, unsigned int textureTarget, Renderbuffer renderbuffer){
    this->colorBuffer = colorBuffer;
    this->renderBuffer = renderbuffer;
    
    glGenFramebuffers(1, &this->ID);
    glBindFramebuffer(GL_FRAMEBUFFER, this->ID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, colorBuffer.ID, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, renderbuffer.attachment, GL_RENDERBUFFER, renderbuffer.ID);

    TEST_FRAMEBUFFER
}

void Framebuffer::update(Texture colorBuffer, unsigned int textureTarget, Renderbuffer renderbuffer){
    this->colorBuffer = colorBuffer;
    this->renderBuffer = renderbuffer;
    
    glBindFramebuffer(GL_FRAMEBUFFER, this->ID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, colorBuffer.ID, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, renderbuffer.attachment, GL_RENDERBUFFER, renderbuffer.ID);

    TEST_FRAMEBUFFER
}

void Framebuffer::generate(){
    glGenFramebuffers(1, &this->ID);
}

void Framebuffer::bind(){
    glBindFramebuffer(GL_FRAMEBUFFER, this->ID);
    if(this->renderBuffer.ID)
        glBindRenderbuffer(GL_RENDERBUFFER, this->renderBuffer.ID);
}

void Framebuffer::setColorBuffer(Texture colorBuffer, unsigned int textureTarget){
    this->colorBuffer = colorBuffer;
    
    glBindFramebuffer(GL_FRAMEBUFFER, this->ID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, colorBuffer.ID, 0);
}

void Framebuffer::deleteBuffers(bool delColorBuffer, bool delRenderBuffer){
    glDeleteRenderbuffers(1, &this->ID);
    
    if(delColorBuffer)
        glDeleteTextures(1, &this->colorBuffer.ID);

    if(delRenderBuffer)
        glDeleteRenderbuffers(1, &this->renderBuffer.ID);
}