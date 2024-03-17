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

CheckComboList checkComboList_painting_color;

void checkComboList_painting_color_render(Timer& timer, bool doMouseTracking){
    checkComboList_painting_color.checkButton.color2 = checkComboList_painting_color.checkButton.color;
    checkComboList_painting_color.checkButton.clickState1 = true;
    checkComboList_painting_color.render(timer, doMouseTracking);

    /*painter.materialPainting = painter.selectedDisplayingModeIndex == 1;
    painter.enableAlbedoChannel = colorSection.elements[1].checkBox.clickState1;
    painter.enableRoughnessChannel = colorSection.elements[6].checkBox.clickState1;
    painter.roughnessVal = colorSection.elements[7].rangeBar.value;
    painter.enableMetallicChannel = colorSection.elements[8].checkBox.clickState1;
    painter.metallicVal = colorSection.elements[9].rangeBar.value;
    painter.enableNormalMapChannel = colorSection.elements[10].checkBox.clickState1;
    painter.normalMapStrengthVal = colorSection.elements[11].rangeBar.value;
    painter.enableHeightMapChannel = colorSection.elements[12].checkBox.clickState1;
    painter.heightMapVal = colorSection.elements[13].rangeBar.value;
    painter.enableAOChannel = colorSection.elements[14].checkBox.clickState1;
    painter.ambientOcclusionVal = colorSection.elements[15].rangeBar.value;
    painter.useCustomMaterial = colorSection.elements[16].checkBox.clickState1;*/
}