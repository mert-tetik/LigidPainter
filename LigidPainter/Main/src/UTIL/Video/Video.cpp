/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
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
#include "UTIL/Video/Video.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/GL/GL.hpp"

void Video::generate_color_buffer(){
    if(this->resolution.x == 0 || this->resolution.y == 0 || this->channels == 0){
        return;
    }

    unsigned int format;
    unsigned int internal_format;
    if(this->channels == 1){
        format = GL_RED; 
        internal_format = GL_R8; 
    }
    if(this->channels == 2){
        format = GL_RG; 
        internal_format = GL_RG8; 
    }
    if(this->channels == 3){
        format = GL_RGB; 
        internal_format = GL_RGB8; 
    }
    if(this->channels == 4){
        format = GL_RGBA; 
        internal_format = GL_RGBA8; 
    }

    this->color_buffer = Texture(nullptr, nullptr, false, this->resolution.x, this->resolution.y, GL_NEAREST, format, internal_format, GL_MIRRORED_REPEAT, GL_TEXTURE_2D);
}

void Video::update_color_buffer(){

    LigidGL::cleanGLErrors();

    unsigned int format;
    unsigned int internal_format;
    if(this->channels == 1){
        format = GL_RED; 
        internal_format = GL_R8; 
    }
    if(this->channels == 2){
        format = GL_RG; 
        internal_format = GL_RG8; 
    }
    if(this->channels == 3){
        format = GL_RGB; 
        internal_format = GL_RGB8; 
    }
    if(this->channels == 4){
        format = GL_RGBA; 
        internal_format = GL_RGBA8; 
    }

    this->color_buffer.update(this->frames[this->frame], this->resolution.x, this->resolution.y, GL_NEAREST, format, internal_format);
}

void Video::render(Timer& timer, glm::vec3 position, glm::vec2 scale, float opacity, int render_mode, bool update_frame_value)
{
    if(!this->frames.size()){
        return; 
    }

    Shader bound_shader = ShaderUTIL::get_bound_shader();
    
    // Pos value, % of the video scale
    glm::vec3 resultPos = glm::vec3( 
                            UTIL::getPercent(*Settings::videoScale(), glm::vec2(position.x, position.y)), //Don't include the depth
                            position.z //Use the original depth value
                        ); 
    
    // scale value % of the video scale
    glm::vec2 resultScale = UTIL::getPercent(*Settings::videoScale(), scale);

    if(update_frame_value){
        timer.transition(true, this->frame_mixVal, this->frames.size() / this->fps);

        if(this->frame_mixVal >= 1.f){
            // Keep the frame count at the last frame
            if(render_mode == 0){
                this->frame_mixVal = 1.f;
            }
            // Reset the video
            if(render_mode == 1){
                this->frame_mixVal = 0.f;
            }
        }

        this->frame = (((float)this->frames.size() - 1) * this->frame_mixVal);
    }

    this->update_color_buffer();
    
    ShaderSystem::textureRenderingShader().use();
    ShaderSystem::textureRenderingShader().setMat4("projection", getContext()->ortho_projection);
    ShaderSystem::textureRenderingShader().setVec2("scale", resultScale);
    ShaderSystem::textureRenderingShader().setVec3("pos", resultPos);

    ShaderSystem::textureRenderingShader().setInt("txtr", 0); GL::bindTexture_2D(this->color_buffer.ID, 0, "Video::render");
    ShaderSystem::textureRenderingShader().setFloat("opacity", opacity);
    ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
    ShaderSystem::textureRenderingShader().setFloat("depthToleranceValue", 0);

    getBox()->bindBuffers();
    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Video::render : Rendering result");

    // Bind already bound shader at the start
    bound_shader.use();
}

