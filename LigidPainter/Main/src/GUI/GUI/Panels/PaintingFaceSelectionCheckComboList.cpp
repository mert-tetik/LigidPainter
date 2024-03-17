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

CheckComboList checkComboList_painting_face_selection;

void checkComboList_painting_face_selection_render(Timer& timer, Painter& painter, bool doMouseTracking){
    checkComboList_painting_face_selection.checkButton.clickState1 = true;
    checkComboList_painting_face_selection.checkButton.color2 = checkComboList_painting_face_selection.checkButton.color;

    checkComboList_painting_face_selection.render(timer, doMouseTracking);

    if(shortcuts_F3())
        checkComboList_painting_face_selection.panel.sections[0].elements[0].checkBox.clickState1 = !checkComboList_painting_face_selection.panel.sections[0].elements[0].checkBox.clickState1;
    if(shortcuts_F4())
        checkComboList_painting_face_selection.panel.sections[0].elements[1].checkBox.clickState1 = !checkComboList_painting_face_selection.panel.sections[0].elements[1].checkBox.clickState1;

    /*
    painter.faceSelection.activated = meshSection.elements[0].checkBox.clickState1;
    painter.faceSelection.editMode = meshSection.elements[1].checkBox.clickState1;
    painter.faceSelection.selectionModeIndex = meshSection.elements[2].comboBox.selectedIndex;
    painter.faceSelection.radius = meshSection.elements[3].rangeBar.value;
    painter.faceSelection.hideUnselected = meshSection.elements[7].checkBox.clickState1;
    */
    
    // Generate mesh mask 
    if(checkComboList_painting_face_selection.panel.sections[0].elements[5].button.clicked){            
        checkComboList_painting_face_selection.panel.sections[0].elements[5].button.texture.generateProceduralTexture(*painter.getSelectedMesh(), painter.faceSelection.meshMask, 1024); 
    }
    
    if(checkComboList_painting_face_selection.panel.sections[0].elements[6].button.clicked || !checkComboList_painting_face_selection.panel.sections[0].elements[5].button.texture.ID){
        checkComboList_painting_face_selection.panel.sections[0].elements[5].button.texture.proceduralProps.proceduralID = 24;
        checkComboList_painting_face_selection.panel.sections[0].elements[5].button.texture.proceduralProps.proceduralnverted = false;
        checkComboList_painting_face_selection.panel.sections[0].elements[5].button.texture.generateProceduralTexture(*painter.getSelectedMesh(), painter.faceSelection.meshMask, 1024); 
        checkComboList_painting_face_selection.panel.sections[0].elements[5].button.texture.generateProceduralDisplayingTexture(512, 1);
    }

    bool applyBoxSelection = false;
    if(doMouseTracking && painter.faceSelection.editMode && painter.faceSelection.selectionModeIndex == 1)
        applyBoxSelection = painter.faceSelection.boxSelectionInteraction(timer);

}