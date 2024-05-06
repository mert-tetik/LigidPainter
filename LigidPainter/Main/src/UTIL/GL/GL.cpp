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
#include <mutex>

#include "UTIL/GL/GL.hpp"

#include "GUI/GUI.hpp"

// Define a mutex to protect access to texture_slot_data
static std::map<int, std::pair<std::pair<unsigned int, std::string>, LigidWindow*>> texture_slot_data;
static int maxTextureUnits = 0;

std::mutex gl_mutex;

bool is_texture_bound_in_different_context(unsigned int texture, LigidWindow* bound_context){
    std::lock_guard<std::mutex> lock(gl_mutex);

    bool result = false;
    for (auto& texture_slot : texture_slot_data)
    {
        if(texture_slot.second.first.first == texture && texture_slot.second.second != bound_context){
            result = true;
        }
    }
    return result;
}

int find_unused_texture_slot() {
    std::lock_guard<std::mutex> lock(gl_mutex);

    int unusedKey = 0;
    while (texture_slot_data.find(unusedKey) != texture_slot_data.end()) {
        unusedKey++;
    }

    if(unusedKey >= maxTextureUnits && maxTextureUnits > 0)
        return -1;

    return unusedKey;
}

int GL::bindTexture(unsigned int texture, unsigned int target, std::string location){

    LigidGL::cleanGLErrors();
    
    if(maxTextureUnits <= 0){
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
        bool success = LigidGL::testGLError(location + " : GL::bindTexture : glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits)");
        if(!success)
            maxTextureUnits = 0;
    }
    
    // Get already bound OpenGL context
    LigidWindow* bound_context = LigidGL::getBoundContext();
    
    int slot = find_unused_texture_slot();

    if(slot == -1){
        LGDLOG::start << "ERROR *CRITICAL* : GL::bindTexture : No more texture units available left! LigidPainter will be waiting until sufficient amount of units are free. " << texture_slot_data.size() << LGDLOG::end;

        /*
        for (auto& texture_slot : texture_slot_data){
            LGDLOG::start << texture_slot.second.first.second << LGDLOG::end; 
        }
        */

        while (slot == -1)
        {
            slot = find_unused_texture_slot();
        }
    }
    
    // If no OpenGL context is bound
    if(bound_context == nullptr){
        LGDLOG::start << "ERROR : GL::bindTexture : No OpenGL context is bound! At the location : " << location  << LGDLOG::end; 
        return slot;
    }

    // Don't let the code proceed while the texture is bound in another context 😡
    if(is_texture_bound_in_different_context(texture, bound_context))
        std::cout << "WARNING : GL::bindTexture : " + location + " : Texture is already bound in a different context. Waiting until it's released!" << std::endl;
            
    while(is_texture_bound_in_different_context(texture, bound_context)){}

    bool success = true;

    glActiveTexture(GL_TEXTURE0 + slot);
    LigidGL::testGLError(location + " : GL::bindTexture : glActiveTexture(GL_TEXTURE0 + slot) : slot = " + std::to_string(slot));

    glBindTexture(target, texture);
    success = LigidGL::testGLError(location + " : GL::bindTexture : glBindTexture(target, texture) : slot = " + std::to_string(slot));

    {        
        std::lock_guard<std::mutex> lock(gl_mutex);

        if(success){
            if(texture != 0){
                texture_slot_data[slot].first = std::make_pair(texture, location);
                texture_slot_data[slot].second = bound_context;
            }
            else
                texture_slot_data.erase(slot);
        }
        else{
            glBindTexture(target, 0);
            texture_slot_data.erase(slot);
        }
    }

    return slot;
}

int GL::bindTexture_2D(unsigned int texture, std::string location){
    return GL::bindTexture(texture, GL_TEXTURE_2D, location);
}

int GL::bindTexture_CubeMap(unsigned int texture, std::string location){
    return GL::bindTexture(texture, GL_TEXTURE_CUBE_MAP, location);
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
    
    {
        std::lock_guard<std::mutex> lock(gl_mutex);
        
        for (auto it = texture_slot_data.begin(); it != texture_slot_data.end(); ) {
            // Unbind the texture if contexts matches
            if(bound_context == it->second.second){
                glActiveTexture(GL_TEXTURE0 + it->first);
                LigidGL::testGLError(location + " : GL::releaseBoundTextures : glActiveTexture(GL_TEXTURE0 + slot)");

                glBindTexture(GL_TEXTURE_2D, 0);
                LigidGL::testGLError(location + " : GL::releaseBoundTextures : glActiveTexture(GL_TEXTURE0)");
                
                it = texture_slot_data.erase(it);
            }
            else    
                it++;
        }
    }
}

void GL::releaseTextureFromSlot(int slot, std::string location){


    // Get already bound OpenGL context
    LigidWindow* bound_context = LigidGL::getBoundContext();
    
    // If no OpenGL context is bound
    if(bound_context == nullptr){
        LGDLOG::start << "ERROR : GL::releaseTextureFromSlot : No OpenGL context is bound! At the location : " << location  << LGDLOG::end; 
        return;
    }

    LigidGL::cleanGLErrors();

    {
        std::lock_guard<std::mutex> lock(gl_mutex);
        
        auto it = texture_slot_data.find(slot);
        if (it != texture_slot_data.end()) {
            glActiveTexture(GL_TEXTURE0 + it->first);
            LigidGL::testGLError(location + " : GL::releaseTextureFromSlot : glActiveTexture(GL_TEXTURE0 + it->first)");

            glBindTexture(GL_TEXTURE_2D, 0);
            LigidGL::testGLError(location + " : GL::releaseTextureFromSlot : glActiveTexture(GL_TEXTURE0)");
            
            texture_slot_data.erase(it);
        }
    }

}

bool GL::makeDrawCall(GLenum mode, GLint first, GLsizei count, std::string debugTitle){
    LigidGL::cleanGLErrors();

    glDrawArrays(mode, first, count);
    return !LigidGL::testGLError("Draw call : " + debugTitle);
}