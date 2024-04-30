/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

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

#include "UTIL/GL/GL.hpp"

#include "GUI/GUI.hpp"

static std::map<int, std::pair<unsigned int, LigidWindow*>> texture_slot_data;

bool is_texture_bound_in_different_context(unsigned int texture, LigidWindow* bound_context){
    bool result = false;
    for (auto& texture_slot : texture_slot_data)
    {
        if(texture_slot.second.first == texture && texture_slot.second.second != bound_context){
            result = true;
        }
    }
    return result;
}

void GL::bindTexture(unsigned int texture, int slot, unsigned int target, std::string location){

    // Get already bound OpenGL context
    LigidWindow* bound_context = LigidGL::getBoundContext();
    
    // If no OpenGL context is bound
    if(bound_context == nullptr){
        LGDLOG::start << "ERROR : GL::bindTexture : No OpenGL context is bound! At the location : " << location  << LGDLOG::end; 
        return;
    }

    // Don't let the code proceed while the texture is bound in another context 😡
    while(is_texture_bound_in_different_context(texture, bound_context)){}

    LigidGL::cleanGLErrors();

    bool success = true;

    glActiveTexture(GL_TEXTURE0 + slot);
    LigidGL::testGLError(location + " : GL::bindTexture : glActiveTexture(GL_TEXTURE0 + slot) : slot = " + std::to_string(slot));

    glBindTexture(target, texture);
    success = LigidGL::testGLError(location + " : GL::bindTexture : glBindTexture(target, texture) : slot = " + std::to_string(slot));

    if(success){
        if(texture != 0)
            texture_slot_data[slot] = std::make_pair(texture, bound_context);
        else
            texture_slot_data.erase(slot);
    }
    else{
        glBindTexture(target, 0);
        texture_slot_data.erase(slot);
    }
}

void GL::bindTexture_2D(unsigned int texture, int slot, std::string location){
    GL::bindTexture(texture, slot, GL_TEXTURE_2D, location);
}

void GL::bindTexture_CubeMap(unsigned int texture, int slot, std::string location){
    GL::bindTexture(texture, slot, GL_TEXTURE_CUBE_MAP, location);
}

void GL::releaseBoundTextures(std::string location){

    // Get already bound OpenGL context
    LigidWindow* bound_context = LigidGL::getBoundContext();
    
    // If no OpenGL context is bound
    if(bound_context == nullptr){
        LGDLOG::start << "ERROR : GL::releaseBoundTextures : No OpenGL context is bound! At the location : " << location  << LGDLOG::end; 
        return;
    }

    LigidGL::cleanGLErrors();
    
    for (auto& texture_slot : texture_slot_data)
    {
        // Unbind the texture if contexts matches
        if(bound_context == texture_slot.second.second){
            glActiveTexture(GL_TEXTURE0 + texture_slot.first);
            LigidGL::testGLError(location + " : GL::releaseBoundTextures : glActiveTexture(GL_TEXTURE0 + slot)");

            glBindTexture(GL_TEXTURE_2D, 0);
            LigidGL::testGLError(location + " : GL::releaseBoundTextures : glActiveTexture(GL_TEXTURE0 + slot)");

            texture_slot_data.erase(texture_slot.first);    
        }
    }
}