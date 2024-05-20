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

#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/Painting/Painter.hpp"

#include "3D/ThreeD.hpp"

#include "Toolkit/Layers/Layers.hpp"

#include "GUI/GUI.hpp"
#include "GUI/Panels.hpp"

#include <string>
#include <vector>

Panel panel_selected_texture_display;

extern MirrorSide O_side;
extern Framebuffer painting_projected_painting_FBO;

void panel_selected_texture_display_render(Timer& timer, bool doMouseTracking){
    panel_selected_texture_display.sections[0].elements[0].button.texture = panel_library_selected_texture;
    
    panel_selected_texture_display.render(timer, doMouseTracking);
    if(panel_selected_texture_display.resizingDone){
        panels_transform();
    }
}