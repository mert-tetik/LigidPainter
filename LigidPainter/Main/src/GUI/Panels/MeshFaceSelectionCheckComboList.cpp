/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/


#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "GUI/Panels.hpp"

#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"

#include "Toolkit/Layers/Layers.hpp"

#include "3D/ThreeD.hpp"

#include <string>
#include <vector>

CheckComboList checkComboList_mesh_face_selection;

void checkComboList_mesh_face_selection_render(Timer& timer, bool doMouseTracking){
    checkComboList_mesh_face_selection.checkButton.clickState1 = true;
    checkComboList_mesh_face_selection.checkButton.color2 = checkComboList_mesh_face_selection.checkButton.color;

    checkComboList_mesh_face_selection.panel.sections[0].elements[0].checkBox.clickState1 = getScene()->get_selected_mesh()->face_selection_data.activated;
    checkComboList_mesh_face_selection.panel.sections[0].elements[1].checkBox.clickState1 = getScene()->get_selected_mesh()->face_selection_data.editMode;
    checkComboList_mesh_face_selection.panel.sections[0].elements[2].comboBox.selectedIndex = getScene()->get_selected_mesh()->face_selection_data.selectionModeIndex;
    checkComboList_mesh_face_selection.panel.sections[0].elements[3].rangeBar.value = getScene()->get_selected_mesh()->face_selection_data.radius;
    checkComboList_mesh_face_selection.panel.sections[0].elements[5].button.texture = getScene()->get_selected_mesh()->face_selection_data.meshMask_display_txtr;
    checkComboList_mesh_face_selection.panel.sections[0].elements[7].checkBox.clickState1 = getScene()->get_selected_mesh()->face_selection_data.hideUnselected;
    
    checkComboList_mesh_face_selection.render(timer, doMouseTracking);

    if(shortcuts_F2())
        checkComboList_mesh_face_selection.panel.sections[0].elements[0].checkBox.clickState1 = !checkComboList_mesh_face_selection.panel.sections[0].elements[0].checkBox.clickState1;
    if(checkComboList_mesh_face_selection.panel.sections[0].elements[0].checkBox.clickState1 && getContext()->window.isKeyClicked(LIGIDGL_KEY_TAB))
        checkComboList_mesh_face_selection.panel.sections[0].elements[1].checkBox.clickState1 = !checkComboList_mesh_face_selection.panel.sections[0].elements[1].checkBox.clickState1;

    // Generate mesh mask 
    if(checkComboList_mesh_face_selection.panel.sections[0].elements[5].button.clicked){          
        if(getScene()->get_selected_mesh()->face_selection_data.meshMask.getResolution().x != 1024){
            getScene()->get_selected_mesh()->face_selection_data.meshMask.update((char*)nullptr, 1024, 1024);
        }  
        checkComboList_mesh_face_selection.panel.sections[0].elements[5].button.texture.generateProceduralTexture(*getScene()->get_selected_mesh(), getScene()->get_selected_mesh()->face_selection_data.meshMask, 1024); 
    }
    
    if(checkComboList_mesh_face_selection.panel.sections[0].elements[6].button.clicked){
        checkComboList_mesh_face_selection.panel.sections[0].elements[5].button.texture.proceduralProps.proceduralID = 24;
        checkComboList_mesh_face_selection.panel.sections[0].elements[5].button.texture.proceduralProps.proceduralnverted = false;
        checkComboList_mesh_face_selection.panel.sections[0].elements[5].button.texture.generateProceduralDisplayingTexture(512, 1);
        char whitePx[] = {127, 127, 127, 127};
        getScene()->get_selected_mesh()->face_selection_data.meshMask.update(whitePx, 1, 1);
    }
    
    getScene()->get_selected_mesh()->face_selection_data.activated = checkComboList_mesh_face_selection.panel.sections[0].elements[0].checkBox.clickState1;
    getScene()->get_selected_mesh()->face_selection_data.editMode = checkComboList_mesh_face_selection.panel.sections[0].elements[1].checkBox.clickState1;
    getScene()->get_selected_mesh()->face_selection_data.selectionModeIndex = checkComboList_mesh_face_selection.panel.sections[0].elements[2].comboBox.selectedIndex;
    getScene()->get_selected_mesh()->face_selection_data.radius = checkComboList_mesh_face_selection.panel.sections[0].elements[3].rangeBar.value;
    getScene()->get_selected_mesh()->face_selection_data.meshMask_display_txtr = checkComboList_mesh_face_selection.panel.sections[0].elements[5].button.texture;
    getScene()->get_selected_mesh()->face_selection_data.hideUnselected = checkComboList_mesh_face_selection.panel.sections[0].elements[7].checkBox.clickState1;
    
}