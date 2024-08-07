/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Framebuffers/Framebuffer_Pool.hpp"
#include "UTIL/Texture/Texture.hpp"
#include "GUI/GUI.hpp"
    
// --------- Renderbuffer -----------

Renderbuffer::Renderbuffer(){
    
}

Renderbuffer::Renderbuffer(unsigned int ID, unsigned int internalformat, unsigned int attachment){
    this->ID = ID;
    this->internalformat = internalformat;
    this->attachment = attachment;
}

Renderbuffer::Renderbuffer(unsigned int internalformat, unsigned int attachment, glm::ivec2 resolution){

    LigidGL::cleanGLErrors();

    Debugger::block("Init RenderBuffer start"); // Start
    Debugger::block("Init RenderBuffer start"); // End

    Debugger::block("Init RenderBuffer"); // End
    this->internalformat = internalformat;
    this->attachment = attachment;

    glGenRenderbuffers(1, &this->ID);
    LigidGL::testGLError("Init RenderBuffer : generating renderbuffer");
    
    glBindRenderbuffer(GL_RENDERBUFFER, this->ID);
    LigidGL::testGLError("Init RenderBuffer : binding renderbuffer");
    
    glRenderbufferStorage(GL_RENDERBUFFER, internalformat, resolution.x, resolution.y);
    LigidGL::testGLError("Init RenderBuffer : allocating memeory");
    
    Debugger::block("Init RenderBuffer"); // End
}

Renderbuffer::Renderbuffer(unsigned int internalformat, unsigned int attachment, glm::ivec2 resolution, int samples){
    
    LigidGL::cleanGLErrors();

    Debugger::block("Init RenderBuffer start"); // Start
    Debugger::block("Init RenderBuffer start"); // End

    Debugger::block("Init RenderBuffer"); // End
    this->internalformat = internalformat;
    this->attachment = attachment;

    glGenRenderbuffers(1, &this->ID);
    LigidGL::testGLError("Init RenderBuffer : generating renderbuffer");

    glBindRenderbuffer(GL_RENDERBUFFER, this->ID);
    LigidGL::testGLError("Init RenderBuffer : binding renderbuffer");

    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalformat, resolution.x, resolution.y);
    LigidGL::testGLError("Init RenderBuffer : allocating memeory");

    Debugger::block("Init RenderBuffer"); // End

}

void Renderbuffer::update(unsigned int internalformat, unsigned int attachment, glm::ivec2 resolution){
    
    LigidGL::cleanGLErrors();

    Debugger::block("Update RenderBuffer start"); // Start
    Debugger::block("Update RenderBuffer start"); // End

    Debugger::block("Update RenderBuffer"); // End
    this->internalformat = internalformat;
    this->attachment = attachment;

    glBindRenderbuffer(GL_RENDERBUFFER, this->ID);
    LigidGL::testGLError("Update RenderBuffer : binding renderbuffer");

    // Check if the renderbuffer size matches the provided size
    GLint currentWidth, currentHeight;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &currentWidth);
    LigidGL::testGLError("Update RenderBuffer : Getting width data of the rbo");
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &currentHeight);
    LigidGL::testGLError("Update RenderBuffer : Getting height data of the rbo");

    if (currentWidth == resolution.x && currentHeight == resolution.y) {
        // Size matches, no need to reallocate
    }
    else{
        glRenderbufferStorage(GL_RENDERBUFFER, internalformat, resolution.x, resolution.y);
        LigidGL::testGLError("Update RenderBuffer : allocating memory");
    }

    Debugger::block("Update RenderBuffer"); // End
}

void Renderbuffer::update(unsigned int internalformat, unsigned int attachment, glm::ivec2 resolution, int samples){
    LigidGL::cleanGLErrors();

    Debugger::block("Update RenderBuffer start"); // Start
    Debugger::block("Update RenderBuffer start"); // End

    Debugger::block("Update RenderBuffer"); // End
    this->internalformat = internalformat;
    this->attachment = attachment;

    glBindRenderbuffer(GL_RENDERBUFFER, this->ID);
    LigidGL::testGLError("Update RenderBuffer : binding renderbuffer");

    // Check if the renderbuffer size matches the provided size
    GLint currentWidth, currentHeight;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &currentWidth);
    LigidGL::testGLError("Update RenderBuffer : Getting width data of the rbo");
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &currentHeight);
    LigidGL::testGLError("Update RenderBuffer : Getting height data of the rbo");

    if (currentWidth == resolution.x && currentHeight == resolution.y) {
        // Size matches, no need to reallocate
    }
    else{
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalformat, resolution.x, resolution.y);
        LigidGL::testGLError("Update RenderBuffer : allocating memory");
    }

    Debugger::block("Update RenderBuffer"); // End
}

glm::ivec2 Renderbuffer::getResolution(){
    glm::ivec2 resolution;

    GLint currentRenderbufferId;
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &currentRenderbufferId);

    // Bind the renderbuffer
    glBindRenderbuffer(GL_RENDERBUFFER, this->ID);

    // Get the width
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &resolution.x);

    // Get the height
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &resolution.y);

    // Unbind the renderbuffer
    glBindRenderbuffer(GL_RENDERBUFFER, currentRenderbufferId);

    return resolution;
}


// ------------ Framebuffer -------------

Framebuffer::Framebuffer(){

}

#define TEST_FRAMEBUFFER(location) \
GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); \
if(status != GL_FRAMEBUFFER_COMPLETE){\
    std::cout << "ERROR : " + (std::string)location + " : Framebuffer is not valid. Error code: " << status << " / " << purpose << std::endl; \
}


Framebuffer::Framebuffer(Texture colorBuffer, unsigned int textureTarget, std::string purpose){
    LigidGL::cleanGLErrors();

    Debugger::block("Init FBO start" + std::string(" : ") + this->purpose); // Start
    Debugger::block("Init FBO start" + std::string(" : ") + this->purpose); // End

    Debugger::block("Init FBO" + std::string(" : ") + this->purpose); // Start
    
    this->colorBuffer = colorBuffer;
    this->purpose = purpose;
    
    glGenFramebuffers(1, &this->ID);
    LigidGL::testGLError("Init FBO : generating FBO" + std::string(" : ") + this->purpose);
    glBindFramebuffer(GL_FRAMEBUFFER, this->ID);
    LigidGL::testGLError("Init FBO : binding FBO" + std::string(" : ") + this->purpose);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, colorBuffer.ID, 0);
    LigidGL::testGLError("Init FBO : attaching colorBuffer to FBO" + std::string(" : ") + this->purpose);
    
    Debugger::block("Init FBO" + std::string(" : ") + this->purpose); // End
    
    TEST_FRAMEBUFFER("Framebuffer::Framebuffer : " + purpose)
}

Framebuffer::Framebuffer(Texture colorBuffer, unsigned int textureTarget, Renderbuffer renderbuffer, std::string purpose){
    LigidGL::cleanGLErrors();

    Debugger::block("Init FBO start" + std::string(" : ") + this->purpose); // Start
    Debugger::block("Init FBO start" + std::string(" : ") + this->purpose); // End

    Debugger::block("Init FBO" + std::string(" : ") + this->purpose); // Start
    
    this->colorBuffer = colorBuffer;
    this->renderBuffer = renderbuffer;
    this->purpose = purpose;
    
    glGenFramebuffers(1, &this->ID);
    LigidGL::testGLError("Init FBO : generating FBO" + std::string(" : ") + this->purpose);
    glBindFramebuffer(GL_FRAMEBUFFER, this->ID);
    LigidGL::testGLError("Init FBO : binding FBO" + std::string(" : ") + this->purpose);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, colorBuffer.ID, 0);
    LigidGL::testGLError("Init FBO : attaching colorBuffer to FBO" + std::string(" : ") + this->purpose);     
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, renderbuffer.attachment, GL_RENDERBUFFER, renderbuffer.ID);
    LigidGL::testGLError("Init FBO : attaching renderBuffer to FBO" + std::string(" : ") + this->purpose);     
    
    Debugger::block("Init FBO" + std::string(" : ") + this->purpose); // End

    TEST_FRAMEBUFFER("Framebuffer::Framebuffer : " + purpose)
}

void Framebuffer::update(Texture colorBuffer, unsigned int textureTarget, Renderbuffer renderbuffer){
    LigidGL::cleanGLErrors();

    Debugger::block("Update FBO start" + std::string(" : ") + this->purpose); // Start
    Debugger::block("Update FBO start" + std::string(" : ") + this->purpose); // End

    Debugger::block("Update FBO" + std::string(" : ") + this->purpose); // Start
    
    this->colorBuffer = colorBuffer;
    this->renderBuffer = renderbuffer;
    
    glBindFramebuffer(GL_FRAMEBUFFER, this->ID);
    LigidGL::testGLError("Update FBO : binding FBO" + std::string(" : ") + this->purpose);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, colorBuffer.ID, 0);
    LigidGL::testGLError("Update FBO : attaching colorBuffer to FBO" + std::string(" : ") + this->purpose);     
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, renderbuffer.attachment, GL_RENDERBUFFER, renderbuffer.ID);
    LigidGL::testGLError("Update FBO : attaching renderBuffer to FBO" + std::string(" : ") + this->purpose);     

    Debugger::block("Update FBO" + std::string(" : ") + this->purpose); // End

    TEST_FRAMEBUFFER("Framebuffer::update : " + purpose)
}

void Framebuffer::generate(){
    LigidGL::cleanGLErrors();
    
    glGenFramebuffers(1, &this->ID);
    
    LigidGL::testGLError("Framebuffer::generate : Generating FBO" + std::string(" : ") + this->purpose);
}

void Framebuffer::bind(){
    LigidGL::cleanGLErrors();
    
    glBindFramebuffer(GL_FRAMEBUFFER, this->ID);
    LigidGL::testGLError("Framebuffer::bind : Binding FBO" + std::string(" : ") + this->purpose);
    
    if(this->renderBuffer.ID){
        glBindRenderbuffer(GL_RENDERBUFFER, this->renderBuffer.ID);
        LigidGL::testGLError("Framebuffer::bind : Binding RBO" + std::string(" : ") + this->purpose);
    }

    TEST_FRAMEBUFFER("Framebuffer::bind");
}

void Framebuffer::setColorBuffer(Texture colorBuffer, unsigned int textureTarget){
    LigidGL::cleanGLErrors();
    
    this->colorBuffer = colorBuffer;
    
    glBindFramebuffer(GL_FRAMEBUFFER, this->ID);
    bool success = LigidGL::testGLError("Framebuffer::setColorBuffer : Binding FBO" + std::string(" : ") + this->purpose);

    // Release all the color attachments
    {
        // Maximum number of color attachments, typically 8 but can vary
        GLint maxColorAttachments;
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
        LigidGL::testGLError("Framebuffer::setColorBuffer : glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments)" + std::string(" : ") + this->purpose);

        // Detach all color attachments
        for (GLint i = 0; i < maxColorAttachments; ++i) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, textureTarget, 0, 0);
            LigidGL::testGLError("Framebuffer::setColorBuffer : glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0)" + std::string(" : ") + this->purpose);
        }
    }
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, colorBuffer.ID, 0);
    LigidGL::testGLError("Framebuffer::setColorBuffer : attaching colorBuffer to FBO" + std::string(" : ") + this->purpose);

    GLuint attachments[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, attachments);
    LigidGL::testGLError("Framebuffer::setColorBuffer : glDrawBuffers" + std::string(" : ") + this->purpose);
    
    if(colorBuffer.ID){
        TEST_FRAMEBUFFER("Framebuffer::bind single color attachments");
    }
}

void Framebuffer::setColorBuffer(std::vector<Texture> colorBuffers, unsigned int textureTarget){
    LigidGL::cleanGLErrors();
    
    this->colorBuffer = colorBuffer;
    
    glBindFramebuffer(GL_FRAMEBUFFER, this->ID);
    LigidGL::testGLError("Framebuffer::setColorBuffer : Binding FBO" + std::string(" : ") + this->purpose);
    
    // Release all the color attachments
    {
        // Maximum number of color attachments, typically 8 but can vary
        GLint maxColorAttachments;
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
        LigidGL::testGLError("Framebuffer::setColorBuffer : glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments)" + std::string(" : ") + this->purpose);

        // Detach all color attachments
        for (GLint i = 0; i < maxColorAttachments; ++i) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
            LigidGL::testGLError("Framebuffer::setColorBuffer : glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0)" + std::string(" : ") + this->purpose);
        }
    }

    GLuint* attachments = new GLuint[colorBuffers.size()];

    for (size_t i = 0; i < colorBuffers.size(); i++)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, textureTarget, colorBuffers[i].ID, 0);
        LigidGL::testGLError("Framebuffer::setColorBuffer : attaching colorBuffer to FBO" + std::string(" : ") + this->purpose);
    
        attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    }
    
    glDrawBuffers(colorBuffers.size(), attachments);
    LigidGL::testGLError("Framebuffer::setColorBuffer : glDrawBuffers" + std::string(" : ") + this->purpose);

    delete[] attachments;
    
    TEST_FRAMEBUFFER("Framebuffer::bind multiple color attachments");
}

void Framebuffer::setRenderbuffer(Renderbuffer rbo){
    LigidGL::cleanGLErrors();
    
    this->renderBuffer = rbo;
    
    glBindFramebuffer(GL_FRAMEBUFFER, this->ID);
    LigidGL::testGLError("Framebuffer::setRenderbuffer : Binding FBO" + std::string(" : ") + this->purpose);
    
    if (glIsRenderbuffer(rbo.ID) == GL_FALSE) {
        LGDLOG::start << "Framebuffer::setRenderbuffer : RBO is not valid!" + std::string(" : ") + this->purpose << LGDLOG::end;
    }

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, rbo.attachment, GL_RENDERBUFFER, rbo.ID);
    LigidGL::testGLError("Framebuffer::setRenderbuffer : attaching renderbuffer to FBO" + std::string(" : ") + this->purpose);
}

void Framebuffer::removeRenderbuffer(){
    LigidGL::cleanGLErrors();
    
    this->renderBuffer.ID = 0;
    
    glBindFramebuffer(GL_FRAMEBUFFER, this->ID);
    LigidGL::testGLError("Framebuffer::removeRenderbuffer : Binding FBO" + std::string(" : ") + this->purpose);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
    LigidGL::testGLError("Framebuffer::removeRenderbuffer : attaching renderbuffer to FBO" + std::string(" : ") + this->purpose);
    
    this->renderBuffer.attachment = 0;
    this->renderBuffer.internalformat = 0;
}

void Framebuffer::deleteBuffers(bool delColorBuffer, bool delRenderBuffer){
    LigidGL::cleanGLErrors();
    
    glDeleteRenderbuffers(1, &this->ID);
    LigidGL::testGLError("Framebuffer::deleteBuffers : Deleting the FBO" + std::string(" : ") + this->purpose);
    
    if(delColorBuffer){
        glDeleteTextures(1, &this->colorBuffer.ID);
        LigidGL::testGLError("Framebuffer::deleteBuffers : Deleting the FBO" + std::string(" : ") + this->purpose);
    }

    if(delRenderBuffer){
        glDeleteRenderbuffers(1, &this->renderBuffer.ID);
        LigidGL::testGLError("Framebuffer::deleteBuffers : Deleting the RBO" + std::string(" : ") + this->purpose);
    }
}

void Framebuffer::makeCurrentlyBindedFBO(){
    GLuint currentFramebuffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&currentFramebuffer);

    this->ID = currentFramebuffer;
}