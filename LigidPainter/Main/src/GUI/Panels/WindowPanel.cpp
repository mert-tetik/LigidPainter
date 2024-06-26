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

Panel panel_window;

void panel_window_render(Timer& timer, bool doMouseTracking){
    panel_window.render(timer, doMouseTracking);
    if(panel_window.resizingDone){
        panels_transform();
    }

    if(panel_window.sections[0].elements[0].button.clicked){//Pressed to the 3D painting button of the window panel
        twoD_painting_mode = false;
    }
    else if(panel_window.sections[0].elements[1].button.clicked){//Pressed to the 2D painting button of the window panel
        twoD_painting_mode = true;
    }
    else if(panel_window.sections[0].elements[2].button.clicked){//Pressed to the export button of the window panel
        dialog_export.show(timer);
    }
    else if(panel_window.sections[0].elements[3].button.clicked){//Pressed to the displayer button of the window panel
        dialog_displayer.show(timer, getScene()->skybox);
    }
    else if(panel_window.sections[0].elements[4].button.clicked){//Pressed to the settings button of the window panel
        dialog_settings.show(timer);
    }
    else if(panel_window.sections[0].elements[5].button.clicked){//Pressed to the settings button of the window panel
        dialog_baking.show(timer, getScene()->skybox);
    }
    else if(panel_window.sections[0].elements[6].button.clicked){//Pressed to the materials button of the window panel
        dialog_materialSelection.show(timer, nullptr);
    }
}