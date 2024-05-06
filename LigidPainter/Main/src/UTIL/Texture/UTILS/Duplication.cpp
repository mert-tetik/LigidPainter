/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/GL/GL.hpp"
#include "UTIL/Framebuffers/Framebuffer_Pool.hpp"
#include "UTIL/Texture/Texture.hpp"

static Texture txtr_for_getContext;
static Texture txtr_for_getSecondContext;
static Texture txtr_for_getCopyContext;

Texture Texture::get_temp_copy_txtr(std::string location){
    if(getContext()->window.isContextCurrent()){
        if(!txtr_for_getContext.ID){
            txtr_for_getContext = Texture((char*)nullptr, 1, 1);
        }
        if(this->getResolution() != txtr_for_getContext.getResolution()){
            txtr_for_getContext.update((char*)nullptr, this->getResolution().x, this->getResolution().y);
        }
        
        this->duplicateTextureSub(txtr_for_getContext, location + " : get_temp_copy_txtr");

        return txtr_for_getContext;
    }
    if(getSecondContext()->window.isContextCurrent()){
        if(!txtr_for_getSecondContext.ID){
            txtr_for_getSecondContext =   Texture((char*)nullptr, 1, 1);
        }
        if(this->getResolution() != txtr_for_getSecondContext.getResolution()){
            txtr_for_getSecondContext.update((char*)nullptr, this->getResolution().x, this->getResolution().y);
        }
        
        this->duplicateTextureSub(txtr_for_getSecondContext, location + " : get_temp_copy_txtr");

        return txtr_for_getSecondContext;
    }
    if(getCopyContext()->window.isContextCurrent()){
        if(!txtr_for_getCopyContext.ID){
            txtr_for_getCopyContext = Texture((char*)nullptr, 1, 1);
        }
        if(this->getResolution() != txtr_for_getCopyContext.getResolution()){
            txtr_for_getCopyContext.update((char*)nullptr, this->getResolution().x, this->getResolution().y);
        }
        
        this->duplicateTextureSub(txtr_for_getCopyContext, location + " : get_temp_copy_txtr");

        return txtr_for_getCopyContext;
    }

    return txtr_for_getCopyContext;
}

void Texture::duplicateTextureSub(Texture& txtr, std::string location){
    
    LigidGL::cleanGLErrors();

    //Get the resolution data of the texture
    int ID_slot = GL::bindTexture_2D(this->ID, "Texture::duplicateTextureSub " + location);
    
    // Get wrap data
    GLint wrapParam_S, wrapParam_T, wrapParam_R;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapParam_S);
    LigidGL::testGLError("Texture::duplicateTextureSub " + location + "glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapParam_S)");
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapParam_T);
    LigidGL::testGLError("Texture::duplicateTextureSub " + location + "glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapParam_T)");
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, &wrapParam_R);
    LigidGL::testGLError("Texture::duplicateTextureSub " + location + "glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, &wrapParam_R)");
    
    // Get pixel data
    GLint width, height, internalFormat;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    LigidGL::testGLError("Texture::duplicateTextureSub " + location + "glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width)");
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    LigidGL::testGLError("Texture::duplicateTextureSub " + location + "glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height)");
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
    LigidGL::testGLError("Texture::duplicateTextureSub " + location + "glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat)");
    
    GL::releaseTextureFromSlot(ID_slot, "Texture::duplicateTextureSub " + location);
    
    //Copy the texture
    Framebuffer FBO = FBOPOOL::requestFBO(*this, "Texture::duplicateTextureSub " + location);

    // Bind the requested texture
    int txtrID_slot = GL::bindTexture_2D(txtr.ID, "Texture::duplicateTextureSub " + location);

    // Set texture params to txtr.ID    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam_S);
    LigidGL::testGLError("Texture::duplicateTextureSub " + location + "glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam_S)");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam_T);
    LigidGL::testGLError("Texture::duplicateTextureSub " + location + "glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam_T)");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapParam_R);
    LigidGL::testGLError("Texture::duplicateTextureSub " + location + "glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapParam_R)");
    
    glCopyTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 0, 0, width, height, 0);
    
    GL::releaseTextureFromSlot(txtrID_slot, "Texture::duplicateTextureSub " + location);
    FBOPOOL::releaseFBO(FBO);

    // Set variables
    txtr.proceduralProps = this->proceduralProps;
}

Texture Texture::duplicateTexture(std::string location){

    // Return a 1x1 rgba 0 texture if the requested texture is not valid
    if(this->ID == 0 || glIsTexture(this->ID) == GL_FALSE){
        return Texture((char*)nullptr, 1, 1).ID;
    }

    Texture newTexture = Texture((char*)nullptr, this->getResolution().x, this->getResolution().y);
    this->duplicateTextureSub(newTexture, location + " : Texture::duplicateTexture()");

    return newTexture;
}

void Texture::duplicateTexture(Texture& txtr, std::string location){
    if(!txtr.ID)
        txtr = Texture((char*)nullptr, this->getResolution().x, this->getResolution().y);
    else
        txtr.update((char*)nullptr, this->getResolution().x, this->getResolution().y);

    this->duplicateTextureSub(txtr, location + " : Texture::duplicateTexture(Texture)");
}
