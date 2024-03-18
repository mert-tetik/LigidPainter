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

Panel panel_displaying_modes;

void panel_displaying_modes_render(Timer& timer, Painter& painter, bool doMouseTracking)
{
    
    panel_displaying_modes.render(timer, doMouseTracking);
    if(panel_displaying_modes.resizingDone){
        panels_transform();
    }

    //!PAINTING MODES PANEL
    for (size_t i = 0; i < panel_displaying_modes.sections[0].elements.size(); i++)
    {
        if(!painter.getSelectedMesh()->layerScene.any_vector_editing()){
            if(panel_displaying_modes.sections[0].elements[i].button.clickState1){ //Painting mode pressed
                    if(painter.selectedDisplayingModeIndex != i){
                        panel_displaying_modes.sections[0].elements[painter.selectedDisplayingModeIndex].button.clickState1 = false;
                        painter.selectedDisplayingModeIndex = i;
                        break;
                    }
            }
        }
        else{
            panel_displaying_modes.sections[0].elements[i].button.clickState1 = false;
        }

        if(painter.selectedDisplayingModeIndex == i){
            panel_displaying_modes.sections[0].elements[i].button.clickState1 = true;
        }
    }
}