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
#include <fstream>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/GL/GL.hpp"
#include "UTIL/Framebuffers/Framebuffer_Pool.hpp"
#include "UTIL/Texture/Texture.hpp"

//------
Texture::Texture(){

}
Texture::Texture(unsigned int ID){
    this->ID = ID;
}
//------

//------
Texture::Texture(char* pixels, int w, int h){
    *this = Texture(pixels, w, h, GL_LINEAR, GL_RGBA, GL_RGBA8);
}
Texture::Texture(unsigned char* pixels, int w, int h){
    *this = Texture(pixels, w, h, GL_LINEAR, GL_RGBA, GL_RGBA8);
}
//------

//------
Texture::Texture(char* pixels, int w, int h, GLenum filterParam){
    *this = Texture(pixels, w, h, filterParam, GL_RGBA, GL_RGBA8);
}
Texture::Texture(unsigned char* pixels, int w, int h, GLenum filterParam){
    *this = Texture(pixels, w, h, filterParam, GL_RGBA, GL_RGBA8);
}
//------

//------
Texture::Texture(char* pixels, int w, int h, GLenum filterParam, int proceduralID){
    this->proceduralProps.proceduralID = proceduralID;
    *this = Texture(pixels, w, h, filterParam);
}
Texture::Texture(unsigned char* pixels, int w, int h, GLenum filterParam, int proceduralID){
    this->proceduralProps.proceduralID = proceduralID;
    *this = Texture(pixels, w, h, filterParam);
}
//------

//------
Texture::Texture(char* pixels, int w, int h, GLenum filterParam, GLenum format, GLenum internalFormat){
    *this = Texture(pixels, nullptr, false, w, h, filterParam, format, internalFormat, GL_MIRRORED_REPEAT, GL_TEXTURE_2D);
}
Texture::Texture(unsigned char* pixels, int w, int h, GLenum filterParam, GLenum format, GLenum internalFormat){
    *this = Texture((char*)nullptr, pixels, true, w, h, filterParam, format, internalFormat, GL_MIRRORED_REPEAT, GL_TEXTURE_2D);
}
//------

Texture::Texture(char* pixels, unsigned char* uPixels, bool use_unsigned, int w, int h, GLenum filterParam, GLenum format, GLenum internalFormat, GLenum textureWrap, GLenum target){
    LigidGL::cleanGLErrors();
    
    glGenTextures(1, &ID);
    LigidGL::testGLError("Texture::Texture : Generate the texture");

    int txtr_slot = GL::bindTexture_2D(ID, "Texture::Texture");
    
    if(filterParam){
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filterParam);
        LigidGL::testGLError("Texture::Texture : glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filterParam)");
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filterParam);
        LigidGL::testGLError("Texture::Texture : glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filterParam)");
    }
    if(textureWrap){
        glTexParameteri(target, GL_TEXTURE_WRAP_S, textureWrap);
        LigidGL::testGLError("Texture::Texture : glTexParameteri(target, GL_TEXTURE_WRAP_S, textureWrap)");
        glTexParameteri(target, GL_TEXTURE_WRAP_T, textureWrap);
        LigidGL::testGLError("Texture::Texture : glTexParameteri(target, GL_TEXTURE_WRAP_T, textureWrap)");
        glTexParameteri(target, GL_TEXTURE_WRAP_R, textureWrap);
        LigidGL::testGLError("Texture::Texture : glTexParameteri(target, GL_TEXTURE_WRAP_R, textureWrap)");
    }
    
    if(use_unsigned){
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, uPixels);
        LigidGL::testGLError("Texture::Texture : Allocate memory for the texture");
    }
    else{
        glTexImage2D(target, 0, internalFormat, w, h, 0, format, GL_BYTE, pixels);
        LigidGL::testGLError("Texture::Texture : Allocate memory for the texture");
    }
	
    glGenerateMipmap(target);
    LigidGL::testGLError("Texture::Texture : Generate mipmap");

    GL::releaseTextureFromSlot(txtr_slot, "Texture::Texture");
}


// ------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------

//------
void Texture::update(char* pixels, int w, int h){
    this->update(pixels, w, h, GL_LINEAR, GL_RGBA, GL_RGBA8);
}
void Texture::update(unsigned char* pixels, int w, int h){
    this->update(pixels, w, h, GL_LINEAR, GL_RGBA, GL_RGBA8);
}
//------

//------
void Texture::update(char* pixels, int w, int h, unsigned int filterParam){
    this->update(pixels, w, h, filterParam, GL_RGBA, GL_RGBA8);
}
void Texture::update(unsigned char* pixels, int w, int h, unsigned int filterParam){
    this->update(pixels, w, h, filterParam, GL_RGBA, GL_RGBA8);
}
//------

//------
void Texture::update(char* pixels, int w, int h, unsigned int filterParam, unsigned int format){
    this->update(pixels, w, h, filterParam, format, format);
}
void Texture::update(unsigned char* pixels, int w, int h, unsigned int filterParam, unsigned int format){
    this->update(pixels, w, h, filterParam, format, format);
}
//------

//------
void Texture::update(char* pixels, int w, int h, unsigned int filterParam, unsigned int format, unsigned int internalFormat){
    this->update(pixels, w, h, filterParam, format, internalFormat, GL_MIRRORED_REPEAT);
}
void Texture::update(unsigned char* pixels, int w, int h, unsigned int filterParam, unsigned int format, unsigned int internalFormat){
    this->update(pixels, w, h, filterParam, format, internalFormat, GL_MIRRORED_REPEAT);
}
//------

void Texture::update(char* pixels, int w, int h, unsigned int filterParam, unsigned int format, unsigned int internalFormat, unsigned int wrap){
    this->update(pixels, nullptr, false, w, h, filterParam, format, internalFormat, wrap);
}
void Texture::update(unsigned char* pixels, int w, int h, unsigned int filterParam, unsigned int format, unsigned int internalFormat, unsigned int wrap){
    this->update((char*)nullptr, pixels, true, w, h, filterParam, format, internalFormat, wrap);
}
//------


void Texture::update(char* pixels, unsigned char* uPixels, bool use_unsigned, int w, int h, unsigned int filterParam, unsigned int format, unsigned int internalFormat, unsigned int wrap) {
    
    if (!this->ID) {
        std::cout << "ERROR : Updating texture : Invalid ID : " << ID << std::endl;
        return;
    }

    LigidGL::cleanGLErrors();

    int txtr_slot = GL::bindTexture_2D(ID, "Texture::update");
    
    // Check if the texture size matches the provided data size
    if(glIsTexture(ID) == GL_TRUE){
        GLint currentWidth, currentHeight, currentInternalFormat, currentFilterParam, currentWrap;
        
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &currentWidth);
        LigidGL::testGLError("Texture::update : Get texture's width data");
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &currentHeight);
        LigidGL::testGLError("Texture::update : Get texture's height data");

        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &currentInternalFormat);
        LigidGL::testGLError("Texture::update : Get texture's internal format data");
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &currentFilterParam);
        LigidGL::testGLError("Texture::update: Get texture's min filter");
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &currentWrap);
        LigidGL::testGLError("Texture::update: Get texture's wrap s");

        // Check if size, internal format, min filter, and wrap are the same
        if (currentWidth == w && currentHeight == h && currentInternalFormat == static_cast<int>(internalFormat) && currentFilterParam == static_cast<int>(filterParam) && currentWrap == static_cast<int>(wrap)) {
            
            if((pixels == nullptr && !use_unsigned) || (uPixels == nullptr && use_unsigned)){
                Framebuffer bound_fbo;
                bound_fbo.makeCurrentlyBindedFBO();

                Framebuffer FBO = FBOPOOL::requestFBO(*this, "Texture::update");

                // Pixels was nullptr
                glClearColor(0,0,0,0);
                glClear(GL_COLOR_BUFFER_BIT);

                FBOPOOL::releaseFBO(FBO);

                bound_fbo.bind();
            }
            else{
                // Use glTexSubImage2D if the parameters match
                if(use_unsigned)
                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, format, GL_UNSIGNED_BYTE, uPixels);
                else
                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, format, GL_BYTE, pixels);
                
                LigidGL::testGLError("Texture::update : glTexSubImage2D");
            }
        } 
        else {
            // Otherwise, use glTexImage2D
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterParam);
            LigidGL::testGLError("Texture::update : glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterParam)");
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterParam);
            LigidGL::testGLError("Texture::update : glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterParam)");
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
            LigidGL::testGLError("Texture::update : glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap)");
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
            LigidGL::testGLError("Texture::update : glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap)");
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrap);
            LigidGL::testGLError("Texture::update : glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrap)");

            if(use_unsigned)
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, uPixels);
            else
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_BYTE, pixels);

            LigidGL::testGLError("Texture::update : Allocate new memory for the texture : Texture is already initialized");
            
            glGenerateMipmap(GL_TEXTURE_2D);
            LigidGL::testGLError("Texture::update : Generate mipmap");
        }
    }
    else {
        // Otherwise, use glTexImage2D
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterParam);
        LigidGL::testGLError("Texture::update : glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterParam)");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterParam);
        LigidGL::testGLError("Texture::update : glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterParam)");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        LigidGL::testGLError("Texture::update : glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap)");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
        LigidGL::testGLError("Texture::update : glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap)");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrap);
        LigidGL::testGLError("Texture::update : glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrap)");

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_BYTE, pixels);
        LigidGL::testGLError("Texture::update : Allocate new memory for the texture : Texture was not initialized");
    
        glGenerateMipmap(GL_TEXTURE_2D);
        LigidGL::testGLError("Texture::update : Generate mipmap");
    }

    GL::releaseTextureFromSlot(txtr_slot, "Texture::update");
}