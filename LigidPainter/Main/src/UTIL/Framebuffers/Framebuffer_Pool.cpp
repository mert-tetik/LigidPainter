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
#include <mutex>

#include "UTIL/Util.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Framebuffers/Framebuffer_Pool.hpp"
#include "UTIL/Texture/Texture.hpp"
#include "GUI/GUI.hpp"

static std::vector<std::pair<Framebuffer, bool>> FBO_POOL;
static double wait_time = 0.;

std::mutex FBOPOOL_mutex;

static bool is_texture_already_bound_to_a_FBO(Texture txtr, std::string location){
    /* Check all the framebuffers in the pool */
    for(auto& fbo : FBO_POOL){
        /* If the requested texture matches with a FBO's color buffer in the pool*/
        if(fbo.first.colorBuffer.ID == txtr.ID && txtr.ID && fbo.second)
        {
            return true;
        }
    }

    return false;
}

static Framebuffer request_from_FBO_POOL(Texture txtr, glm::ivec2 resolution, std::string location){

    std::lock_guard<std::mutex> lock(FBOPOOL_mutex);

    Framebuffer* res = nullptr;

    /* Wait until the texture is no longer bound to any FBO (wait the other thread) */
    if(is_texture_already_bound_to_a_FBO(txtr, location)){
        wait_time = LigidGL::getTime();
        std::cout << "WARNING : request_from_FBO_POOL : " + location + " : Texture is already bound to a framebuffer. Waiting until it's released!" << std::endl;
    }

    while(is_texture_already_bound_to_a_FBO(txtr, location)){
        if(LigidGL::getTime() - wait_time > 5){
            std::cout << "BROKE AHH NEGE" << std::endl;
            break;
        }
    }

    /* Find proper FBO for the request */
    for(auto& fbo : FBO_POOL)
    {   
        /* If the FBO is not being used */
        if(!fbo.second){
            /* If renderbuffer is not requested*/
            if(resolution == glm::ivec2(0)){
                res = &fbo.first;
                fbo.second = true;
            }
            
            /* If renderbuffer is requested*/
            else{
                /* If the renderbuffer has it's renderbuffer*/
                if(fbo.first.renderBuffer.ID){
                    /* If the resolution values of the requested and renderbuffer's matches*/
                    if(fbo.first.renderBuffer.getResolution() == resolution){
                        res = &fbo.first;
                        fbo.second = true;
                    }   
                }
            }
        }
    }

    /* If proper FBO found in the pool create one and add to the pool */
    if(true){
        Framebuffer new_FBO;

        /* If a renderbuffer is not requested*/
        if(resolution == glm::ivec2(0)){
            new_FBO = Framebuffer(txtr, GL_TEXTURE_2D, "");
        }
        else{
            new_FBO = Framebuffer(txtr, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, resolution),"");
        }

        
        /* Add generated FBO to pool */
        FBO_POOL.push_back(std::make_pair(new_FBO, true));
        
        /* Provide the generated FBO from the pool */
        res = &FBO_POOL[FBO_POOL.size()-1].first;
        
    }

    /* Update the purpose of the fbo*/
    res->purpose = location;
    
    /* Update the color buffer of the FBO*/
    res->setColorBuffer(txtr, GL_TEXTURE_2D);

    /* Bind the requested FBO*/
    res->bind();

    // Update the viewport according to requested txtr
    glViewport(0, 0, txtr.getResolution().x, txtr.getResolution().y);

    /* Provide the requested FBO */
    return *res;
} 

Framebuffer FBOPOOL::requestFBO(Texture txtr, std::string location){
    return request_from_FBO_POOL(txtr, glm::ivec2(0), location);
}

Framebuffer FBOPOOL::requestFBO_with_RBO(Texture txtr, glm::ivec2 resolution, std::string location){
    return request_from_FBO_POOL(txtr, resolution, location);
}

void FBOPOOL::releaseFBO(Framebuffer released_FBO){
    std::lock_guard<std::mutex> lock(FBOPOOL_mutex);

    /* Updating the currently used flag */
    for (auto& FBO_from_pool : FBO_POOL)
    {
        /* If adresses matches */
        if(FBO_from_pool.first.ID == released_FBO.ID){
            FBO_from_pool.first.setColorBuffer(0, GL_TEXTURE_2D); // Set currently using flag to false
            FBO_from_pool.second = false; // Set currently using flag to false
        }
    }
    
    /* Bind the default framebuffer if main OpenGL context is current */
    if(getContext()->window.isContextCurrent()){
        Settings::defaultFramebuffer()->FBO.bind();
        Settings::defaultFramebuffer()->setViewport();
    }
    else{
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}