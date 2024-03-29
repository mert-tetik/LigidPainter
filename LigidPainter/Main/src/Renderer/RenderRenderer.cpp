/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <iostream>
#include <filesystem>

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "Renderer.h"

#include "GUI/Elements.hpp"
#include "GUI/GUI.hpp"
#include "GUI/Panels.hpp"

#include "3D/ThreeD.hpp"

#include "Toolkit/Toolkits.hpp"

#include "UTIL/Util.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"

// 3D Point
extern bool aPointWasAlreadyActivated;

void Renderer::render(){

    Debugger::block("Started"); // Start
    Debugger::block("Started"); // Start

    Debugger::block("Prep"); // Start
    this->start_render();
    Debugger::block("Prep"); // End

    Debugger::block("Render : Scene"); // Start
    getScene()->render_scene();
    Debugger::block("Render : Scene"); // End
    
    //Clear the depth buffer before rendering the UI elements (prevent coliding)
    glClear(GL_DEPTH_BUFFER_BIT);

    //Bind 2D square vertex buffers
    getBox()->bindBuffers();
    
    Debugger::block("Render : Toolkits"); // Start
    render_toolkits(timer);
    Debugger::block("Render : Toolkits"); // End
    
    Debugger::block("Render : Panels"); // Start
    panels_render(timer);
    Debugger::block("Render : Panels"); // End

    Debugger::block("Painting"); // Start
    //Painting
    if(
            *Mouse::LPressed() && 
            !panels_any_hovered() && 
            (panel_displaying_modes.selectedElement == 1 || panel_displaying_modes.selectedElement == 2) && panel_painting_modes.selectedElement != 5 &&
            !checkComboList_painting_over.panel.sections[0].elements[1].checkBox.clickState1 &&
            !getScene()->get_selected_mesh()->face_selection_data.editMode &&
            !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) &&
            !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_ALT)
        )
    {
        //Paint
        painter.doPaint(    
                            painter.wrapMode,
                            *Mouse::LClick(),
                            panel_painting_modes.selectedElement,
                            false || painter.wrapMode
                        );
    }

    //Painting done (refresh)
    if(((painter.refreshable && !*Mouse::LPressed()) || (painter.refreshable && (*Mouse::RClick() || *Mouse::MClick()))) && panel_painting_modes.selectedElement != 5){ //Last frame painting done or once mouse right click or mouse wheel click
        //Paint
        painter.doPaint(    
                            painter.wrapMode,
                            true,
                            panel_painting_modes.selectedElement,
                            true
                        );

        //Update the selected texture after painting
        painter.updateTexture(panel_painting_modes.selectedElement);
        
        //Refresh the 2D painting texture
        painter.refreshPainting();

        painter.refreshable = false;
    }

    Debugger::block("Painting"); // End
}