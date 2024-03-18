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
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "GUI/Panels.hpp"

#include <string>
#include <vector>

Panel panel_smear_painting_properties;

void panel_smear_painting_properties_render(Timer& timer, Painter& painter, bool doMouseTracking)
{
    panel_smear_painting_properties.render(timer, doMouseTracking); 

    painter.smearTransformStrength = panel_smear_painting_properties.sections[0].elements[0].rangeBar.value;
    painter.smearBlurStrength = panel_smear_painting_properties.sections[0].elements[1].rangeBar.value;  
}