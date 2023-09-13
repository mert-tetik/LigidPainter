/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

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

#include "GUI/Elements/Elements.hpp"
#include "GUI/Dialogs/Dialogs.hpp"

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
    

    this->doMouseTracking = doMouseTracking;

    //Get the real position value (0 - 100 to screen location)
    resultPos = glm::vec3( 
                          UTIL::getPercent(*Settings::videoScale(), glm::vec2(pos.x,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value

    //Get the real scale value
    resultScale = UTIL::getPercent(*Settings::videoScale(), scale);
    
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
        if(this->textureSelection){
            showTextureSelectionDialog(this->texture, 512);
        }
        if(this->filterSelection){
            showFilterSelectionDialog(this->filter, 512);
            this->texture.ID = this->filter.displayingTxtr;
        }
        if(this->colorSelection){
            unsigned char defRGB[4] = {0, 0, 0, 0}; // Black color (RGB = 0, 0, 0), alpha = 0
            Color clrObj;
            clrObj.loadRGB_normalized(glm::vec3(this->color.r, this->color.g, this->color.b));
            const char* hex0Val = clrObj.getHEX().c_str();
            auto check = tinyfd_colorChooser("Select a color",hex0Val,defRGB,defRGB);
            if(check){
                Color clr(check);
                this->color.r = clr.getRGB_normalized().r;
                this->color.g = clr.getRGB_normalized().g;
                this->color.b = clr.getRGB_normalized().b;
            }        
        }
        if(this->meshSelection){
            showMeshSelectionDialog();
        }
    }

    //Render the text and the texture
    renderTextAndTexture(resultScaleText);
}