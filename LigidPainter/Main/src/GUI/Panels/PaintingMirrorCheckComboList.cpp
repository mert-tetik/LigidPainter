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
#include "3D/ThreeD.hpp"

#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"

#include "Toolkit/Layers/Layers.hpp"

#include "GUI/GUI.hpp"
#include "GUI/Panels.hpp"

#include <string>
#include <vector>

CheckComboList checkComboList_painting_mirror;

void checkComboList_painting_mirror_render(Timer& timer, bool doMouseTracking, CheckComboList& checkComboList_painting_mirror){
    
    checkComboList_painting_mirror.checkButton.color2 = checkComboList_painting_mirror.checkButton.color;
    checkComboList_painting_mirror.checkButton.clickState1 = true;
    checkComboList_painting_mirror.render(timer, doMouseTracking);

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

    if(checkComboList_painting_mirror.panel.sections[0].elements[0].checkBox.clickState1 && !checkComboList_painting_mirror.arrowButton.clickState1)
        btnX.render(timer, false);
    if(checkComboList_painting_mirror.panel.sections[0].elements[2].checkBox.clickState1 && !checkComboList_painting_mirror.arrowButton.clickState1)
        btnY.render(timer, false);
    if(checkComboList_painting_mirror.panel.sections[0].elements[4].checkBox.clickState1 && !checkComboList_painting_mirror.arrowButton.clickState1)
        btnZ.render(timer, false);

}