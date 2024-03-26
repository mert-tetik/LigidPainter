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
#include <glm/gtx/string_cast.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/Mouse/Mouse.hpp"

#include <string>
#include <iostream>
#include <vector>

/* Defined in the TextureField.cpp */
extern bool textureFields_decidingWrapPointsMode;
extern bool textureField_alreadyInteracted;

void TextureFieldScene::render(Timer& timer, Painter& painter, bool doMouseTracking, bool threeD_only, bool editMode){
    
    getBox()->bindBuffers();
    glClear(GL_DEPTH_BUFFER_BIT);
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
    ShaderSystem::buttonShader().use();
    ShaderSystem::buttonShader().setMat4("projection", getContext()->ortho_projection);

    textureFields_decidingWrapPointsMode = false;
    textureField_alreadyInteracted = false;

    // Rendering all the painting over texture fields
    for (int i = 0; i < this->texture_fields.size(); i++)
    {
        bool anyHover = false;
        for (int ii = 0; ii < this->texture_fields.size(); ii++){
            if(ii > i){
                if(this->texture_fields[ii].is2DModeHovered()){
                    anyHover = true;
                }    
            }
        }

        if((painter.wrapMode && this->texture_fields[i].wrapMode) || !painter.wrapMode || editMode){
            this->texture_fields[i].render(
                                                timer, 
                                                painter,
                                                this->texture_fields, 
                                                i, 
                                                editMode, 
                                                false, 
                                                doMouseTracking
                                            );
        }
    }    

    ShaderSystem::buttonShader().use();   
}

void TextureFieldScene::add_new(TextureField texture_field){
    this->texture_fields.push_back(texture_field);
}

void TextureFieldScene::update_painting_over_texture(bool threeD_only, Painter& painter){
    if(!this->FBO.ID || glIsFramebuffer(this->FBO.ID) == GL_FALSE){
        LGDLOG::start << "ERROR : Texture field scene : Updating painting over texture : Invalid framebuffer ID" << LGDLOG::end;
        return;
    }
    
    if(!this->painting_over_texture.ID || glIsTexture(this->painting_over_texture.ID) == GL_FALSE){
        LGDLOG::start << "ERROR : Texture field scene : Updating painting over texture : Invalid texture ID" << LGDLOG::end;
        return;
    }

    glm::vec2 resolution = this->painting_over_texture.getResolution();

    this->FBO.bind();
    this->FBO.setColorBuffer(this->painting_over_texture, GL_TEXTURE_2D);

    glViewport(0, 0, resolution.x, resolution.y);

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rendering all the painting over texture fields
    for (int i = 0; i < this->texture_fields.size(); i++)
    {
        if((threeD_only && this->texture_fields[i].wrapMode) || !threeD_only){
            this->texture_fields[i].render(Timer(), painter, this->texture_fields, i, false, true, false);
        }
    }    

    // Finish
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
}