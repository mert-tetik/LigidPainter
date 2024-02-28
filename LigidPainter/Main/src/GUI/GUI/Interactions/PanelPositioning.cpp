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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "LibrarySystem/Library.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>


void UI::panelPositioning(
                            float &screenGapPerc, 
                            Painter &painter
                        )
{
    //!Positioning the panels

    windowPanel.scale.y = 50.f - navigationPanel.scale.y;

    windowPanel.pos.x = 100.f - windowPanel.scale.x - screenGapPerc; //Keep on the right side
    libraryPanelDisplayer.pos.x = libraryPanelLeft.pos.x + libraryPanelLeft.scale.x + libraryPanelDisplayer.scale.x; //Keep on the left side of the window panel 
    
    windowPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + windowPanel.scale.y; //Keep beneath the navigation bar
    windowPanel.scale.y = 50.f - navigationPanel.scale.y; //Keep beneath the navigation bar
    
    layersPanel.pos = windowPanel.pos;
    layersPanel.pos.x -= windowPanel.scale.x + layersPanel.scale.x;
    layersPanel.scale.y = windowPanel.scale.y;

    libraryPanelLeft.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + windowPanel.scale.y; //Keep beneath the navigation bar
    libraryPanelDisplayer.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + libraryPanelDisplayer.scale.y; //Keep beneath the navigation bar
    libraryPanelDisplayer.scale.y = 50 - navigationPanel.scale.y - selectedTextureDisplayer.scale.y;
    libraryPanelLeft.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + libraryPanelDisplayer.scale.y; //Keep beneath the navigation bar
    libraryPanelLeft.scale.y = 50 - navigationPanel.scale.y - selectedTextureDisplayer.scale.y;
    selectedTextureDisplayer.pos.x = libraryPanelDisplayer.pos.x - libraryPanelLeft.pos.x;
    selectedTextureDisplayer.scale.x = libraryPanelDisplayer.scale.x + libraryPanelLeft.scale.x;
    
    
    twoDPaintingPanel.scale.x = 50 - screenGapPerc/2.f - selectedTextureDisplayer.scale.x - (windowPanel.scale.x);
    twoDPaintingPanel.scale.y = 50 - navigationPanel.scale.y;
    twoDPaintingPanel.pos.x = windowPanel.pos.x - windowPanel.scale.x - twoDPaintingPanel.scale.x; //Keep on the left side of the window panel 
    twoDPaintingPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + twoDPaintingPanel.scale.y; //Keep beneath the navigation bar
    paintingModesPanel.pos.x = libraryPanelDisplayer.pos.x + libraryPanelDisplayer.scale.x + paintingModesPanel.scale.x + 1;
    paintingModesPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + paintingModesPanel.scale.y;

    faceSelectionCheckComboList.pos = paintingModesPanel.pos;
    faceSelectionCheckComboList.pos.x += paintingModesPanel.scale.x + faceSelectionCheckComboList.scale.x + 0.5f;
    faceSelectionCheckComboList.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + faceSelectionCheckComboList.scale.y;
    paintingOverCheckComboList.pos = faceSelectionCheckComboList.pos;  
    paintingOverCheckComboList.pos.x += faceSelectionCheckComboList.scale.x + paintingOverCheckComboList.scale.x + 0.25f;  
    colorCheckComboList.pos = paintingOverCheckComboList.pos;  
    colorCheckComboList.pos.x += paintingOverCheckComboList.scale.x + colorCheckComboList.scale.x + 0.25f;  
    mirrorCheckComboList.pos = colorCheckComboList.pos;  
    mirrorCheckComboList.pos.x += colorCheckComboList.scale.x + mirrorCheckComboList.scale.x + 0.25f;  
    paintingBrushButton.scale.y = mirrorCheckComboList.scale.y;  
    paintingBrushButton.pos = mirrorCheckComboList.pos;  
    paintingBrushButton.pos.x += mirrorCheckComboList.scale.x + paintingBrushButton.scale.x + 0.25f;  

    //faceSelectionCheckComboList
    //paintingOverCheckComboList

    vectorPaintingModePropertyPanel.pos.x = libraryPanelDisplayer.pos.x + libraryPanelDisplayer.scale.x + vectorPaintingModePropertyPanel.scale.x + 1;
    vectorPaintingModePropertyPanel.pos.y = paintingModesPanel.pos.y + paintingModesPanel.scale.y + vectorPaintingModePropertyPanel.scale.y + 1;

    vectorPaintingMode2DModeWrapCheckBox.scale.x = vectorPaintingModePropertyPanel.scale.x; 
    vectorPaintingMode2DModeWrapCheckBox.pos = vectorPaintingModePropertyPanel.pos; 
    vectorPaintingMode2DModeWrapCheckBox.pos.y += vectorPaintingModePropertyPanel.scale.y + vectorPaintingMode2DModeWrapCheckBox.scale.y; 
    
    smearPaintingModePropertyPanel.pos.x = libraryPanelDisplayer.pos.x + libraryPanelDisplayer.scale.x + smearPaintingModePropertyPanel.scale.x + 1;
    smearPaintingModePropertyPanel.pos.y = paintingModesPanel.pos.y + paintingModesPanel.scale.y + smearPaintingModePropertyPanel.scale.y + 1;
    
    filterPaintingModeFilterBtn.pos.x = libraryPanelDisplayer.pos.x + libraryPanelDisplayer.scale.x + filterPaintingModeFilterBtn.scale.x + 1;
    filterPaintingModeFilterBtn.pos.y = paintingModesPanel.pos.y + paintingModesPanel.scale.y + filterPaintingModeFilterBtn.scale.y + 1;
    filterPaintingModeFilterBtn.pos.z = libraryPanelDisplayer.pos.z;
    
    textureSelectedObjectsButton.pos.x = libraryPanelDisplayer.pos.x + libraryPanelDisplayer.scale.x + textureSelectedObjectsButton.scale.x + 1;
    textureSelectedObjectsButton.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + textureSelectedObjectsButton.scale.y + 1;
    textureSelectedObjectsButton.pos.z = libraryPanelDisplayer.pos.z;
    
    objectsPanel.pos = textureSelectedObjectsButton.pos;
    objectsPanel.pos.x = libraryPanelDisplayer.pos.x + libraryPanelDisplayer.scale.x + objectsPanel.scale.x + 1;
    objectsPanel.pos.y += textureSelectedObjectsButton.scale.y + objectsPanel.scale.y + 1.f;

    displayingModesPanel.pos.x = layersPanel.pos.x - layersPanel.scale.x - displayingModesPanel.scale.x - 0.5f;
    displayingModesPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + displayingModesPanel.scale.y;
    
    currentModeDisplayer.pos = displayingModesPanel.pos;
    currentModeDisplayer.pos.x = layersPanel.pos.x - layersPanel.scale.x - currentModeDisplayer.scale.x;
    currentModeDisplayer.pos.y += displayingModesPanel.scale.y + currentModeDisplayer.scale.y + 1.f;
    
    currentModeHintDisplayer.pos = currentModeDisplayer.pos;
    currentModeHintDisplayer.pos.x = layersPanel.pos.x - layersPanel.scale.x - currentModeHintDisplayer.scale.x;
    currentModeHintDisplayer.pos.y += currentModeDisplayer.scale.y + currentModeHintDisplayer.scale.y;
    
    wrapModeCheckbox.pos = currentModeHintDisplayer.pos;
    wrapModeCheckbox.pos.x = layersPanel.pos.x - layersPanel.scale.x - wrapModeCheckbox.scale.x * 2.f;
    wrapModeCheckbox.pos.y += currentModeHintDisplayer.scale.y + wrapModeCheckbox.scale.y;

    sceneGizmo.pos = wrapModeCheckbox.pos;
    sceneGizmo.pos.x = displayingModesPanel.pos.x;
    sceneGizmo.pos.y += wrapModeCheckbox.scale.y + sceneGizmo.scale.y + 1.f;

    selectedTextureDisplayer.sections[0].elements[0].scale.y = selectedTextureDisplayer.scale.y;

    selectedTextureDisplayer.sections[0].elements[0].button.texture = painter.selectedTexture;
    twoDPaintingPanel.sections[0].elements[0].scale.y = twoDPaintingPanel.scale.y;

}