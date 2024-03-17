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
#include "ShaderSystem/Shader.hpp"
#include "LibrarySystem/Library.hpp"
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"
#include "Layers/Layers.hpp"
#include "GUI/Panels.hpp"

#include <string>
#include <vector>

Panel panel_library_modes;

void panel_library_modes_render(Timer& timer, bool doMouseTracking){
    
    panel_library_modes.render(timer, doMouseTracking);
    if(panel_library_modes.resizingDone){
        panels_transform();
    }

    //Check all the library element button if they are pressed
    for (size_t i = 0; i < panel_library_modes.sections[0].elements.size(); i++) 
    {
        if(panel_library_modes.sections[0].elements[i].button.hover && *Mouse::LClick()){//If any button element is pressed
            if(Library::getSelectedElementIndex() != i){
                panel_library_modes.sections[0].elements[Library::getSelectedElementIndex()].button.clickState1 = false;
                Library::changeSelectedElementIndex(i);
                break;
            }
        } 
    }
    
    //Keep the selected element as selected
    for (size_t i = 0; i < panel_library_modes.sections[0].elements.size(); i++) 
    {
        if(Library::getSelectedElementIndex() == i)
            panel_library_modes.sections[0].elements[i].button.clickState1 = true;
        else
            panel_library_modes.sections[0].elements[i].button.clickState1 = false;
    }
}