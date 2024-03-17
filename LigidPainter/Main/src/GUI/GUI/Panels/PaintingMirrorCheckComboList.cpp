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

CheckComboList checkComboList_painting_mirror;

void checkComboList_painting_mirror_render(Timer& timer, Painter& painter, bool doMouseTracking){
    
    checkComboList_painting_mirror.checkButton.color2 = checkComboList_painting_mirror.checkButton.color;
    checkComboList_painting_mirror.checkButton.clickState1 = true;
    checkComboList_painting_mirror.render(timer, doMouseTracking);

    /*
    painter.oSide.active = true;
    
    painter.oXSide.active = checkComboList_painting_mirror.panel.sections[0].elements[0].checkBox.clickState1; 
    painter.mirrorXOffset = checkComboList_painting_mirror.panel.sections[0].elements[1].rangeBar.value;
    painter.oYSide.active = checkComboList_painting_mirror.panel.sections[0].elements[2].checkBox.clickState1; 
    painter.mirrorYOffset = checkComboList_painting_mirror.panel.sections[0].elements[3].rangeBar.value;
    painter.oZSide.active = checkComboList_painting_mirror.panel.sections[0].elements[4].checkBox.clickState1; 
    painter.mirrorZOffset = checkComboList_painting_mirror.panel.sections[0].elements[5].rangeBar.value;

    painter.oXYSide.active = painter.oXSide.active && painter.oYSide.active;     
    painter.oXZSide.active = painter.oXSide.active && painter.oZSide.active; 
    painter.oYZSide.active = painter.oYSide.active && painter.oZSide.active; 
    painter.oXYZSide.active = painter.oXSide.active && painter.oYSide.active && painter.oZSide.active; 
    */

    Button btnX = Button(ELEMENT_STYLE_SOLID, glm::vec2(checkComboList_painting_mirror.scale.x / 3.f, checkComboList_painting_mirror.scale.y), "X", Texture(), 0.f, false);
    btnX.pos = checkComboList_painting_mirror.pos;
    btnX.pos.x -= btnX.scale.x * 2.f;
    btnX.pos.y += btnX.scale.y + checkComboList_painting_mirror.scale.y;
    Button btnY = Button(ELEMENT_STYLE_SOLID, glm::vec2(checkComboList_painting_mirror.scale.x / 3.f, checkComboList_painting_mirror.scale.y), "Y", Texture(), 0.f, false);
    btnY.pos = checkComboList_painting_mirror.pos;
    btnY.pos.y += btnY.scale.y + checkComboList_painting_mirror.scale.y;
    Button btnZ = Button(ELEMENT_STYLE_SOLID, glm::vec2(checkComboList_painting_mirror.scale.x / 3.f, checkComboList_painting_mirror.scale.y), "Z", Texture(), 0.f, false);
    btnZ.pos = checkComboList_painting_mirror.pos;
    btnZ.pos.x += btnX.scale.x * 2.f;
    btnZ.pos.y += btnZ.scale.y + checkComboList_painting_mirror.scale.y;

    if(painter.oXSide.active && !checkComboList_painting_mirror.arrowButton.clickState1)
        btnX.render(timer, false);
    if(painter.oYSide.active && !checkComboList_painting_mirror.arrowButton.clickState1)
        btnY.render(timer, false);
    if(painter.oZSide.active && !checkComboList_painting_mirror.arrowButton.clickState1)
        btnZ.render(timer, false);

    // Updating the depth texture if interacted with the gui elements related to mirroring
    if( 
        checkComboList_painting_mirror.panel.sections[0].elements[0].isInteracted() || 
        checkComboList_painting_mirror.panel.sections[0].elements[1].isInteracted() || 
        checkComboList_painting_mirror.panel.sections[0].elements[2].isInteracted() || 
        checkComboList_painting_mirror.panel.sections[0].elements[3].isInteracted() || 
        checkComboList_painting_mirror.panel.sections[0].elements[4].isInteracted() || 
        checkComboList_painting_mirror.panel.sections[0].elements[5].isInteracted()
    ) 
    {
        painter.updateDepthTexture();
    }
}