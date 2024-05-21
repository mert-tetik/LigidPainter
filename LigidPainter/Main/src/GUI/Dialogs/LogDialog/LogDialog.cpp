
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 (c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------
 
    TODO USE ALL THE PROJECT SETTINGS WHILE CREATING A PROJECT 

 */

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "3D/ThreeD.hpp" 

#include "GUI/GUI.hpp" 

#include "UTIL/Mouse/Mouse.hpp" 
#include "UTIL/Library/Library.hpp" 
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp" 
#include "UTIL/Project/Project.hpp" 
#include "UTIL/Threads/Threads.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

#define MAX_MATERIAL_HISTORY 8
#define MAX_PAINTING_HISTORY 25

namespace LGDLOG{
    LogMsg start;
    std::string end = "$#";
}

int prevpPainterDisplayIndex = 0;
unsigned int prevpPainterSelectedTxtr = 0;
std::string lastProjectPath;

void LogDialog::render(Timer& timer)
{
    glClear(GL_DEPTH_BUFFER_BIT);

    this->render_elements(timer);

    this->update_messages_array(timer);

    this->talking_interaction(timer);    
    
    prevpPainterDisplayIndex = panel_displaying_modes.selectedElement;
    prevpPainterSelectedTxtr = panel_library_selected_texture.ID;

    if(
            shortcuts_CTRL_Z()
        )
    {
        //this->undo();
    }

    lastProjectPath = project_path();
}