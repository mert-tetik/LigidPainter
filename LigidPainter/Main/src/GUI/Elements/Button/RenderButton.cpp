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

#include "GUI/Elements.hpp"
#include "GUI/Dialogs.hpp"
#include "UTIL/Mouse/Mouse.hpp"

#include "tinyfiledialogs.h"

#include <string>
#include <iostream>
#include <vector>

/// @brief renders the button element
/// @param videoScale resolution value of the primary monitor
/// @param mouse local mouse class (manage mouse activities)
/// @param timer local timer class (manage the time (for transitions))
/// @param textRenderer local textRenderer class (manage text rendering & keyboard inputs)
/// @param doMouseTracking don't check mouse hover if set to false
void Button::render(
                        Timer &timer,
                        bool doMouseTracking
                    ){
    
    ShaderSystem::buttonShader().use();

    this->doMouseTracking = doMouseTracking;

    //Get the real position value (0 - 100 to screen location)
    resultPos = glm::vec3( 
                          UTIL::getPercent(*Settings::videoScale(), glm::vec2(pos.x,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value

    //Get the real scale value
    resultScale = UTIL::getPercent(*Settings::videoScale(), glm::abs(scale));
    resultScale *= glm::sign(scale);
    
    //Get the real radius value
    float resultRadius = UTIL::getPercent(Settings::videoScale()->x,radius);
    
    //Get the real text scale value
    float resultScaleText = Settings::videoScale()->x/1920/2*textScale;
    
    //Get the real outline thickness value
    float resultOutlineThickness = Settings::videoScale()->x/1920/2*outlineThickness;

    //Manage mouse activites
    manageMouseActivity();

    //Manage the hover animation of the button 
    timer.transition(hover,hoverMixVal,0.2f); 
    
    //Manage the click animation of the button 
    if(keepPressingState)
        timer.transition(clickState1,clickedMixVal,0.2f); //Change the color by the state of being clicked
    else
        timer.transition(false,clickedMixVal,0.2f); //Take the value down repedatedly (since clickedMixVal will be set to 1. when clicking is done)
    
    //Render the button
    render(resultPos,resultScale,resultRadius,resultOutlineThickness);
    
    if(this->clicked){
        this->click_action(timer);
    }

    if(this->meshSelection){
        if(getScene()->model->newModelAdded)
            this->selectedMeshI = 0;

        if(this->selectedMeshI < getScene()->model->meshes.size()){
            this->texture = getScene()->model->meshes[this->selectedMeshI].displayingTxtr; 
            this->text = getScene()->model->meshes[this->selectedMeshI].materialName; 
        }
        else
            this->selectedMeshI = 0;
    }

    //Render the text and the texture
    renderTextAndTexture(resultScaleText);

    if(!this->hover)
        hoverDuration = 0;
    else{
        if(timer.tick){
            hoverDuration++;
        }
    }

    if(hoverDuration > 2 && this->infoText.size()){
        glDisable(GL_DEPTH_TEST);
        Button infoBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(6.f, 1.3f), this->infoText, Texture(), 0.f, false);
        infoBtn.pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
        infoBtn.pos.y = this->pos.y + this->scale.y + infoBtn.scale.y;
        infoBtn.pos.z = this->pos.z + 0.03f;
        infoBtn.render(timer, false);
        glEnable(GL_DEPTH_TEST);
    }
}


void Button::click_action(Timer& timer){
    if(this->textureSelection3D){
        dialog_textureSelection.show(timer, this->texture, selectionDialogTxtrRes, false);
    }
    if(this->textureSelection2D){
        dialog_textureSelection.show(timer, this->texture, selectionDialogTxtrRes, true);
    }
    if(this->filterSelection){
        dialog_filterSelection.show(timer, this->filter, selectionDialogTxtrRes);
        
        this->texture.ID = this->filter.displayingTxtr.ID;
    }
    if(this->colorSelection){
        unsigned char defRGB[4] = {0, 0, 0, 0}; // Black color (RGB = 0, 0, 0), alpha = 0
        Color clrObj;
        clrObj.loadRGB_normalized(glm::vec3(this->color.r, this->color.g, this->color.b));
        std::string hex0Val = clrObj.getHEX();
        auto check = tinyfd_colorChooser("Select a color", hex0Val.c_str(), defRGB,defRGB);
        if(check){
            Color clr(check);
            this->color.r = clr.getRGB_normalized().r;
            this->color.g = clr.getRGB_normalized().g;
            this->color.b = clr.getRGB_normalized().b;
        }        
    }
    if(this->meshSelection){
        dialog_meshSelection.show(timer, this->selectedMeshI);
    }
    if(this->brushModification){
        dialog_brushModification.show(timer, &this->brush.properties);
    
        this->brush.updateDisplayTexture(this->brush.properties.radius);
    }
    if(materialSelection){
        dialog_materialSelection.show(timer, &this->material);
    }
}