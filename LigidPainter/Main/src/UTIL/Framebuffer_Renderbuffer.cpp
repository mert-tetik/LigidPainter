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
    
Renderbuffer::Renderbuffer(){
    
}

Renderbuffer::Renderbuffer(unsigned int internalformat, glm::ivec2 resolution){
    glGenRenderbuffers(1, &this->ID);
    glBindRenderbuffer(GL_RENDERBUFFER, this->ID);
    glRenderbufferStorage(GL_RENDERBUFFER, internalformat, resolution.x, resolution.y);
}

Renderbuffer::Renderbuffer(unsigned int internalformat, glm::ivec2 resolution, int samples){
    glGenRenderbuffers(1, &this->ID);
    glBindRenderbuffer(GL_RENDERBUFFER, this->ID);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalformat, resolution.x, resolution.y);
}

void Renderbuffer::update(unsigned int internalformat, glm::ivec2 resolution){
    glBindRenderbuffer(GL_RENDERBUFFER, this->ID);
    glRenderbufferStorage(GL_RENDERBUFFER, internalformat, resolution.x, resolution.y);
}

void Renderbuffer::update(unsigned int internalformat, glm::ivec2 resolution, int samples){
    glBindRenderbuffer(GL_RENDERBUFFER, this->ID);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalformat, resolution.x, resolution.y);
}


Framebuffer::Framebuffer(){

}
Framebuffer::Framebuffer(Texture colorBuffer, unsigned int textureTarget){
    this->colorBuffer = colorBuffer;
    
    glGenFramebuffers(1, &this->ID);
    glBindFramebuffer(GL_FRAMEBUFFER, this->ID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, colorBuffer.ID, 0);
}
Framebuffer::Framebuffer(Texture colorBuffer, unsigned int textureTarget, Renderbuffer renderbuffer){
    this->colorBuffer = colorBuffer;
    this->renderBuffer = renderbuffer;
    
    glGenFramebuffers(1, &this->ID);
    glBindFramebuffer(GL_FRAMEBUFFER, this->ID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, colorBuffer.ID, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer.ID);
}
void Framebuffer::generate(){
    glGenFramebuffers(1, &this->ID);
}
void Framebuffer::bind(){
    glBindFramebuffer(GL_FRAMEBUFFER, this->ID);
}
void Framebuffer::setColorBuffer(Texture colorBuffer, unsigned int textureTarget){
    this->colorBuffer = colorBuffer;
    
    glBindFramebuffer(GL_FRAMEBUFFER, this->ID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, colorBuffer.ID, 0);
}