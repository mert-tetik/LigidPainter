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

CheckComboList checkComboList_painting_color;

void checkComboList_painting_color_render(Timer& timer, bool doMouseTracking){
    checkComboList_painting_color.checkButton.color2 = checkComboList_painting_color.checkButton.color;
    checkComboList_painting_color.checkButton.clickState1 = true;
    checkComboList_painting_color.render(timer, doMouseTracking);

    painter.enableRoughnessChannel = checkComboList_painting_color.panel.sections[0].elements[3].checkBox.clickState1;
    painter.roughnessVal = checkComboList_painting_color.panel.sections[0].elements[4].rangeBar.value;
    
    painter.enableMetallicChannel = checkComboList_painting_color.panel.sections[0].elements[5].checkBox.clickState1;
    painter.metallicVal = checkComboList_painting_color.panel.sections[0].elements[6].rangeBar.value;

    painter.enableNormalMapChannel = checkComboList_painting_color.panel.sections[0].elements[7].checkBox.clickState1;
    painter.normalMapStrengthVal = checkComboList_painting_color.panel.sections[0].elements[8].rangeBar.value;

    painter.enableHeightMapChannel = checkComboList_painting_color.panel.sections[0].elements[9].checkBox.clickState1;
    painter.heightMapVal = checkComboList_painting_color.panel.sections[0].elements[10].rangeBar.value;

    painter.enableAOChannel = checkComboList_painting_color.panel.sections[0].elements[11].checkBox.clickState1;
    painter.ambientOcclusionVal = checkComboList_painting_color.panel.sections[0].elements[12].rangeBar.value;

    painter.useCustomMaterial = checkComboList_painting_color.panel.sections[0].elements[13].checkBox.clickState1;
}