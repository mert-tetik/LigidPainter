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

#include "Renderer.h"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "Toolkit/Layers/Layers.hpp"
#include "GUI/Panels.hpp"

#include <string>
#include <vector>

void current_mode_hint_displayer_render(Timer& timer)
{
    Button currentModeDisplayer(ELEMENT_STYLE_SOLID, glm::vec2(6.f, 1.f), ""  , Texture(), 0.f, true);
    currentModeDisplayer.color = glm::vec4(0.f);
    currentModeDisplayer.outline = false;
    
    Button currentModeHintDisplayer(ELEMENT_STYLE_SOLID, glm::vec2(6.f, 1.f), ""  , Texture(), 0.f, true);
    currentModeHintDisplayer.color = glm::vec4(0.f);
    currentModeHintDisplayer.outline = false;

    if(panel_displaying_modes.selectedElement == 0){
        currentModeDisplayer.text = "Solid Mode";
    }
    else if(panel_displaying_modes.selectedElement == 1){
        currentModeDisplayer.text = "Material Painting mode";
        currentModeHintDisplayer.text = "Regular Painting";
    }
    else if(panel_displaying_modes.selectedElement == 2){
        currentModeDisplayer.text = "Single Texture Painting mode";
        currentModeHintDisplayer.text = "Regular Painting";
    }
    else{
        currentModeDisplayer.text = "Unknown mode";
        currentModeHintDisplayer.text = "Unknown mode";
    }

    bool mirror_X = checkComboList_painting_mirror.panel.sections[0].elements[0].checkBox.clickState1;
    bool mirror_Y = checkComboList_painting_mirror.panel.sections[0].elements[2].checkBox.clickState1;
    bool mirror_Z = checkComboList_painting_mirror.panel.sections[0].elements[4].checkBox.clickState1;

    if(mirror_X || mirror_Y || mirror_Z){
        
        if(mirror_X && mirror_Y && mirror_Z)
            currentModeHintDisplayer.text = "Mirror XYZ";
        
        else if(mirror_X && mirror_Y)
            currentModeHintDisplayer.text = "Mirror XY";
        
        else if(mirror_Y && mirror_Z)
            currentModeHintDisplayer.text = "Mirror YZ";
        
        else if(mirror_X && mirror_Z)
            currentModeHintDisplayer.text = "Mirror XZ";
        
        else if(mirror_X)
            currentModeHintDisplayer.text = "Mirror X";
        
        else if(mirror_Y)
            currentModeHintDisplayer.text = "Mirror Y";
        
        else if(mirror_Z)
            currentModeHintDisplayer.text = "Mirror Z";

    }


    if(getScene()->get_selected_mesh()->face_selection_data.activated){
        currentModeHintDisplayer.text = "Masking To Selected Mesh";
    }
    if(getScene()->get_selected_mesh()->face_selection_data.editMode){
        currentModeDisplayer.text = "Face Selection Mode (Edit)";
        currentModeHintDisplayer.text = "Faces Total : " + std::to_string(getScene()->get_selected_mesh()->indices.size() / 3);
    }
    
    if(checkComboList_painting_over.panel.sections[0].elements[0].checkBox.clickState1){
        currentModeHintDisplayer.text = "Painting Over Mode";
    }
    if(checkComboList_painting_over.panel.sections[0].elements[1].checkBox.clickState1){
        currentModeDisplayer.text = "Painting Over Edit Mode";
        
        currentModeHintDisplayer.text = std::to_string(getTextureFieldScene()->texture_fields.size()) + " Textures";
    }

    currentModeDisplayer.pos = panel_displaying_modes.pos;
    currentModeDisplayer.pos.x = panel_layers.pos.x - panel_layers.scale.x - currentModeDisplayer.scale.x;
    currentModeDisplayer.pos.y += panel_displaying_modes.scale.y + currentModeDisplayer.scale.y + 1.f;
    
    currentModeHintDisplayer.pos = currentModeDisplayer.pos;
    currentModeHintDisplayer.pos.x = panel_layers.pos.x - panel_layers.scale.x - currentModeHintDisplayer.scale.x;
    currentModeHintDisplayer.pos.y += currentModeDisplayer.scale.y + currentModeHintDisplayer.scale.y;

    currentModeDisplayer.render(timer, false);
    currentModeHintDisplayer.render(timer, false);
}