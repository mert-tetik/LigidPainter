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

#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/Painting/Painter.hpp"

#include "GUI/GUI.hpp"
#include "GUI/Panels.hpp"

#include "3D/ThreeD.hpp"

#include "Toolkit/Toolkits.hpp"

#include <string>
#include <iostream>
#include <vector>

static bool prevStraightLinePaintingCondition = false;

void render_toolkits_before_panels(Timer& timer){
    
    if(checkComboList_painting_over.panel.sections[0].elements[1].checkBox.clickState1){
        Debugger::block("GUI : Rendering dotes"); // Start
        render_dots(*Settings::videoScale() / 2.f, *Settings::videoScale() / 2.f, glm::vec2(0.f), 1.f);
        Debugger::block("GUI : Rendering dotes"); // End
    }

    if(GET_TEXTURE_FIELD_SCENE_RENDERING_CONDITION){
        Debugger::block("GUI : Texture fields"); // Start
        getTextureFieldScene()->render(timer, !panels_any_hovered() && !getScene()->get_selected_mesh()->face_selection_data.editMode, checkBox_wrap_mode.clickState1, checkComboList_painting_over.panel.sections[0].elements[1].checkBox.clickState1);
        Debugger::block("GUI : Texture fields"); // End
    }
    
    if(GET_VECTOR_SCENE_RENDERING_CONDITION){
        Debugger::block("GUI : Vectors"); // Start
        getVectorScene()->render_scene(timer, !panels_any_hovered() && !getScene()->get_selected_mesh()->face_selection_data.editMode, checkBox_wrap_mode.clickState1);
        Debugger::block("GUI : Vectors"); // End
    }

    bool straightLinePaintingCondition = panel_displaying_modes.selectedElement != 0 && 
                                         panel_painting_modes.selectedElement != 5 && 
                                         !getScene()->get_selected_mesh()->face_selection_data.editMode && 
                                         (getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) || getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_ALT)) && 
                                         *Mouse::LPressed(); 

    if(straightLinePaintingCondition || prevStraightLinePaintingCondition)
    {
        Debugger::block("GUI : Line painting"); // Start
        line_painting(
                        timer, 
                        prevStraightLinePaintingCondition && !straightLinePaintingCondition, 
                        !prevStraightLinePaintingCondition && straightLinePaintingCondition
                    );
        Debugger::block("GUI : Line painting"); // End
    }
    prevStraightLinePaintingCondition = straightLinePaintingCondition;
}

void render_toolkits_after_panels(Timer& timer){
    //Render the brush cursor
    if(
            painting_paintable_condition() &&
            panel_painting_modes.selectedElement != 6 // If not bucket painting mode
        )
    {
        if(!checkBox_wrap_mode.clickState1){
            render_painting_cursor(button_painting_brush.brush.properties.radius);
        }
        else{
            render_3D_cursor(button_painting_brush.brush.properties.radius);
        }
    }
    else{
        getContext()->window.setCursorVisibility(true);
        // Set the cursor as the bucket cursor if bucket painting mode
        if(panel_painting_modes.selectedElement == 6 && painting_paintable_condition()){
            Mouse::setCursor(*Mouse::bucketCursor());
        }
    }
}