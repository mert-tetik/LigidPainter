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

Texture Texture::get_temp_copy_txtr(){
    if(getContext()->window.isContextCurrent()){
        if(!txtr_for_getContext.ID){
            txtr_for_getContext = Texture((char*)nullptr, 1, 1);
        }
        if(this->getResolution() != txtr_for_getContext.getResolution()){
            txtr_for_getContext.update((char*)nullptr, this->getResolution().x, this->getResolution().y);
        }
        
        this->duplicateTextureSub(txtr_for_getContext);

        return txtr_for_getContext;
    }
    if(getSecondContext()->window.isContextCurrent()){
        if(!txtr_for_getSecondContext.ID){
            txtr_for_getSecondContext =   Texture((char*)nullptr, 1, 1);
        }
        if(this->getResolution() != txtr_for_getSecondContext.getResolution()){
            txtr_for_getSecondContext.update((char*)nullptr, this->getResolution().x, this->getResolution().y);
        }
        
        this->duplicateTextureSub(txtr_for_getSecondContext);

        return txtr_for_getSecondContext;
    }
    if(getCopyContext()->window.isContextCurrent()){
        if(!txtr_for_getCopyContext.ID){
            txtr_for_getCopyContext = Texture((char*)nullptr, 1, 1);
        }
        if(this->getResolution() != txtr_for_getCopyContext.getResolution()){
            txtr_for_getCopyContext.update((char*)nullptr, this->getResolution().x, this->getResolution().y);
        }
        
        this->duplicateTextureSub(txtr_for_getCopyContext);

        return txtr_for_getCopyContext;
    }
}

void Texture::duplicateTextureSub(Texture& txtr){
    
    LigidGL::cleanGLErrors();

    //Get the resolution data of the texture
    GL::bindTexture_2D(this->ID, "Texture::duplicateTextureSub");
    
    // Get wrap data
    GLint wrapParam_S, wrapParam_T, wrapParam_R;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapParam_S);
    LigidGL::testGLError("glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapParam_S)");
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapParam_T);
    LigidGL::testGLError("glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapParam_T)");
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, &wrapParam_R);
    LigidGL::testGLError("glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, &wrapParam_R)");
    
    // Get pixel data
    GLint width, height, internalFormat;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    LigidGL::testGLError("glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width)");
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    LigidGL::testGLError("glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height)");
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
    LigidGL::testGLError("glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat)");
    
    // Bind the requested texture
    GL::bindTexture_2D(txtr.ID, "Texture::duplicateTextureSub");

    // Set texture params to txtr.ID    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam_S);
    LigidGL::testGLError("glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam_S)");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam_T);
    LigidGL::testGLError("glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam_T)");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapParam_R);
    LigidGL::testGLError("glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapParam_R)");
    
    //Copy the texture
    Framebuffer FBO = FBOPOOL::requestFBO(*this, "Texture::duplicateTextureSub");
    glCopyTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 0, 0, width, height, 0);
    LigidGL::testGLError("glCopyTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 0, 0, width, height, 0)");

    // Finish
    FBOPOOL::releaseFBO(FBO);
    GL::releaseBoundTextures("Texture::duplicateTextureSub");

    // Set variables
    txtr.proceduralProps = this->proceduralProps;
}

Texture Texture::duplicateTexture(){

    // Return a 1x1 rgba 0 texture if the requested texture is not valid
    if(this->ID == 0 || glIsTexture(this->ID) == GL_FALSE){
        return Texture((char*)nullptr, 1, 1).ID;
    }

    Texture newTexture = Texture((char*)nullptr, this->getResolution().x, this->getResolution().y);
    this->duplicateTextureSub(newTexture);

    return newTexture;
}

void Texture::duplicateTexture(Texture& txtr){
    if(!txtr.ID)
        txtr = Texture((char*)nullptr, this->getResolution().x, this->getResolution().y);
    else
        txtr.update((char*)nullptr, this->getResolution().x, this->getResolution().y);

    this->duplicateTextureSub(txtr);
}
