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

void TextureFieldScene::render(Timer& timer, bool doMouseTracking, bool threeD_only, bool editMode){
    
    glClear(GL_DEPTH_BUFFER_BIT);
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();

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

        if((checkBox_wrap_mode.clickState1 && this->texture_fields[i].wrapMode) || !checkBox_wrap_mode.clickState1 || editMode){
            this->texture_fields[i].render(
                                                timer, 
                                                this, 
                                                i, 
                                                editMode, 
                                                false, 
                                                doMouseTracking
                                            );
        }
    }    
}

void TextureFieldScene::add_new(TextureField texture_field){
    this->texture_fields.push_back(texture_field);
}

void TextureFieldScene::update_painting_over_texture(bool threeD_only){
    if(!this->painting_over_texture.ID || glIsTexture(this->painting_over_texture.ID) == GL_FALSE){
        LGDLOG::start << "ERROR : Texture field scene : Updating painting over texture : Invalid texture ID" << LGDLOG::end;
        return;
    }

    Framebuffer FBO = FBOPOOL::requestFBO_with_RBO(this->painting_over_texture, this->painting_over_texture.getResolution(), "TextureFieldScene::update_painting_over_texture");

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rendering all the painting over texture fields
    for (int i = 0; i < this->texture_fields.size(); i++)
    {

        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glDepthFunc(GL_LEQUAL);

        if((threeD_only && this->texture_fields[i].wrapMode) || !threeD_only){
            this->texture_fields[i].render(Timer(), this, i, false, true, false);
            glClear(GL_DEPTH_BUFFER_BIT);
        }
    }    

    // Finish
    FBOPOOL::releaseFBO(FBO);
}