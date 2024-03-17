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

Panel panel_painting_modes;

void panel_painting_modes_render(Timer& timer, Painter& painter, bool doMouseTracking){
    panel_painting_modes.render(timer, doMouseTracking);
    if(panel_painting_modes.resizingDone){
        panels_transform();
    }
    
    // Handle interactions related to painting modes panel (change selected painting mode index & make sure selected index highlighted)
    for (size_t i = 0; i < panel_painting_modes.sections[0].elements.size(); i++)
    {
        if(!painter.getSelectedMesh()->layerScene.any_vector_editing()){
            if(panel_painting_modes.sections[0].elements[i].button.clickState1){ //Painting mode pressed
                if(painter.selectedPaintingModeIndex != i){
                    panel_painting_modes.sections[0].elements[painter.selectedPaintingModeIndex].button.clickState1 = false;
                    painter.selectedPaintingModeIndex = i;
                    break;
                }
            }
        }
        else{
            panel_painting_modes.sections[0].elements[i].button.clickState1 = false;
        }
        if(painter.selectedPaintingModeIndex == i){
            panel_painting_modes.sections[0].elements[i].button.clickState1 = true;
        }
    }
}