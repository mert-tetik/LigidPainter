/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef FRAMEBUFFER_POOL_HPP
#define FRAMEBUFFER_POOL_HPP

//OpenGL & window 
#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

//OpenGL Math Library GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector> //std::vector
#include <string> //std::string
#include <map> //std::map
#include <atomic>

#include "UTIL/Shader/Shader.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/Texture/Texture.hpp"

class Renderbuffer{
public:
    unsigned int ID = 0, internalformat = 0, attachment = 0;
    
    Renderbuffer();
    Renderbuffer(unsigned int ID, unsigned int internalformat, unsigned int attachment);
    Renderbuffer(unsigned int internalformat, unsigned int attachment, glm::ivec2 resolution);
    Renderbuffer(unsigned int internalformat, unsigned int attachment, glm::ivec2 resolution, int samples);

    void update(unsigned int internalformat, unsigned int attachment, glm::ivec2 resolution);
    void update(unsigned int internalformat, unsigned int attachment, glm::ivec2 resolution, int samples);

    glm::ivec2 getResolution();
};

class Framebuffer{
public:
    unsigned int ID = 0;

    Texture colorBuffer;
    Renderbuffer renderBuffer;
    std::string purpose;

    Framebuffer();
    Framebuffer(Texture colorBuffer, unsigned int textureTarget, std::string purpose);
    Framebuffer(Texture colorBuffer, unsigned int textureTarget, Renderbuffer renderbuffer, std::string purpose);
    void update(Texture colorBuffer, unsigned int textureTarget, Renderbuffer renderbuffer);


    void generate();
    void bind();
    void setColorBuffer(Texture colorBuffer, unsigned int textureTarget);
    void setColorBuffer(std::vector<Texture> colorBuffers, unsigned int textureTarget);
    void setRenderbuffer(Renderbuffer rbo);
    void removeRenderbuffer();
    void deleteBuffers(bool delColorBuffer, bool delRenderBuffer);
    
    void makeCurrentlyBindedFBO();
};

namespace FBOPOOL{
    Framebuffer requestFBO(Texture txtr, std::string location);
    Framebuffer requestFBO(std::vector<Texture> txtrs, std::string location);
    Framebuffer requestFBO_with_RBO(Texture txtr, glm::ivec2 resolution, std::string location);

    void releaseFBO(Framebuffer FBO);
};

#endif