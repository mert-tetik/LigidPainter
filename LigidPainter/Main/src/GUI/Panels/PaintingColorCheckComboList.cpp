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

#include "GUI/GUI.hpp"
#include "GUI/Panels.hpp"

#include "3D/ThreeD.hpp"

#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/Threads/Threads.hpp"

#include "Toolkit/Layers/Layers.hpp"

#include <string>
#include <vector>

CheckComboList checkComboList_painting_color;

static Material display_mat;
static Material prev_display_mat;

void checkComboList_painting_color_render(Timer& timer, bool doMouseTracking, CheckComboList& checkComboList_painting_color){
    checkComboList_painting_color.checkButton.color2 = checkComboList_painting_color.checkButton.color;
    checkComboList_painting_color.checkButton.clickState1 = true;
    checkComboList_painting_color.render(timer, doMouseTracking);
 
    if(!display_mat.title.size()){
        display_mat = Material("checkComboList_painting_color display mat", {MaterialModifier(SOLID_MATERIAL_MODIFIER)});
    }

    display_mat.materialModifiers[0].sections[0].elements[0].button.color = glm::vec4(checkComboList_painting_color.panel.sections[0].elements[2].painterColorSelection.getSelectedColor().getRGB_normalized(), 1.f); 
    display_mat.materialModifiers[0].sections[0].elements[2].button.color = glm::vec4(glm::vec3(checkComboList_painting_color.panel.sections[0].elements[4].rangeBar.value), 1.f); 
    display_mat.materialModifiers[0].sections[0].elements[4].button.color = glm::vec4(glm::vec3(checkComboList_painting_color.panel.sections[0].elements[6].rangeBar.value), 1.f); 
    display_mat.materialModifiers[0].sections[0].elements[6].button.color = glm::vec4(glm::vec3(checkComboList_painting_color.panel.sections[0].elements[8].rangeBar.value), 1.f); 
    display_mat.materialModifiers[0].sections[0].elements[8].button.color = glm::vec4(glm::vec3(checkComboList_painting_color.panel.sections[0].elements[10].rangeBar.value), 1.f); 
    display_mat.materialModifiers[0].sections[0].elements[10].button.color = glm::vec4(glm::vec3(checkComboList_painting_color.panel.sections[0].elements[12].rangeBar.value), 1.f); 

    if(prev_display_mat != display_mat){
        material_thread.update_material_displaying_texture(&display_mat, getMaterialDisplayerModel(), &getMaterialDisplayerModel()->meshes[0], &getMaterialDisplayerModel()->meshes[0].material_channels, 512);
    }
    
    prev_display_mat = display_mat;

    if(checkComboList_painting_color.panel.sections[0].elements[13].checkBox.clickState1)
        checkComboList_painting_color.panel.sections[0].elements[0].button.texture = checkComboList_painting_color.panel.sections[0].elements[14].button.material.displayingTexture;
    else
        checkComboList_painting_color.panel.sections[0].elements[0].button.texture = display_mat.displayingTexture;
}