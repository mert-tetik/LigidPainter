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

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "Toolkit/Toolkits.hpp"
#include "GUI/Panels.hpp"

#include <string>
#include <iostream>
#include <vector>

static bool prevStraightLinePaintingCondition = false;

void render_toolkits(Timer& timer, Painter& painter){
    
    if(painter.paintingoverTextureEditorMode){
        Debugger::block("GUI : Rendering dotes"); // Start
        render_dots();
        Debugger::block("GUI : Rendering dotes"); // End
    }

    if(painter.usePaintingOver){
        Debugger::block("GUI : Texture fields"); // Start
        getTextureFieldScene()->render(timer, painter, !panels_any_hovered(), checkBox_wrap_mode.clickState1);
        Debugger::block("GUI : Texture fields"); // End
    }
    
    if(painter.selectedPaintingModeIndex == 5 && painter.selectedDisplayingModeIndex != 0 && !painter.paintingoverTextureEditorMode){
        Debugger::block("GUI : Vectors"); // Start
        getVectorScene()->render_scene(timer, !panels_any_hovered(), checkBox_wrap_mode.clickState1);
        Debugger::block("GUI : Vectors"); // End
    }

    bool straightLinePaintingCondition = painter.selectedDisplayingModeIndex != 0 && 
                                         painter.selectedPaintingModeIndex != 5 && 
                                         !painter.faceSelection.editMode && 
                                         (getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) || getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_ALT)) && 
                                         *Mouse::LPressed(); 

    if(straightLinePaintingCondition || prevStraightLinePaintingCondition)
    {
        Debugger::block("GUI : Line painting"); // Start
        line_painting(
                        timer, 
                        painter, 
                        prevStraightLinePaintingCondition && !straightLinePaintingCondition, 
                        !prevStraightLinePaintingCondition && straightLinePaintingCondition
                    );
        Debugger::block("GUI : Line painting"); // End
    }
    prevStraightLinePaintingCondition = straightLinePaintingCondition;

    //Render the brush cursor
    if(
            !panels_any_hovered() && 
            (painter.selectedDisplayingModeIndex == 1 || painter.selectedDisplayingModeIndex == 2) && painter.selectedPaintingModeIndex != 5 && painter.selectedPaintingModeIndex != 6 &&
            !painter.paintingoverTextureEditorMode &&
            !painter.faceSelection.editMode &&
            !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) &&
            !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_ALT)
        )
    {
        if(!painter.wrapMode){
            render_painting_cursor(painter.brushProperties.radius);
        }
        else{
            render_3D_cursor(painter);
        }
        
    }
    else{
        getContext()->window.setCursorVisibility(true);
    }
}