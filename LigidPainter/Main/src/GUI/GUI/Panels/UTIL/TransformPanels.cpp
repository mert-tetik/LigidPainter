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

void panels_transform()
{
    //Calculate the total screen gap
    float screenGap = Settings::videoScale()->x - getContext()->windowScale.x; //Use that value to keep panels on the right side
    
    //Calculate the screen gap in 0 - 100 range
    float screenGapPerc = screenGap / Settings::videoScale()->x * 100.f; 

    //!Positioning the panels

    windowPanel.scale.y = 50.f - navigationPanel.scale.y;

    windowPanel.pos.x = 100.f - windowPanel.scale.x - screenGapPerc; //Keep on the right side
    panel_library.pos.x = libraryModesPanel.pos.x + libraryModesPanel.scale.x + panel_library.scale.x; //Keep on the left side of the window panel 
    
    windowPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + windowPanel.scale.y; //Keep beneath the navigation bar
    windowPanel.scale.y = 50.f - navigationPanel.scale.y; //Keep beneath the navigation bar
    
    layersPanel.pos = windowPanel.pos;
    layersPanel.pos.x -= windowPanel.scale.x + layersPanel.scale.x;
    layersPanel.pos.y += addLayerPanel.scale.y + meshSelectionButton.scale.y + layersResolutionComboBox.scale.y;
    layersPanel.scale.y = windowPanel.scale.y - addLayerPanel.scale.y - meshSelectionButton.scale.y - layersResolutionComboBox.scale.y;

    meshSelectionButton.scale.x = layersPanel.scale.x;
    meshSelectionButton.pos = layersPanel.pos;
    meshSelectionButton.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + meshSelectionButton.scale.y;
    
    layersResolutionComboBox.scale.x = layersPanel.scale.x;
    layersResolutionComboBox.pos = meshSelectionButton.pos;
    layersResolutionComboBox.pos.y = meshSelectionButton.pos.y + meshSelectionButton.scale.y + layersResolutionComboBox.scale.y;
    
    addLayerPanel.scale.x = layersPanel.scale.x;
    addLayerPanel.pos = layersResolutionComboBox.pos;
    addLayerPanel.pos.y = layersResolutionComboBox.pos.y + layersResolutionComboBox.scale.y + addLayerPanel.scale.y;
    
    libraryModesPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + windowPanel.scale.y; //Keep beneath the navigation bar
    libraryPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + libraryPanel.scale.y; //Keep beneath the navigation bar
    libraryPanel.scale.y = 50 - navigationPanel.scale.y - selectedTextureDisplayer.scale.y;
    libraryModesPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + libraryPanel.scale.y; //Keep beneath the navigation bar
    libraryModesPanel.scale.y = 50 - navigationPanel.scale.y - selectedTextureDisplayer.scale.y;
    selectedTextureDisplayer.pos.x = libraryPanel.pos.x - libraryModesPanel.pos.x;
    selectedTextureDisplayer.scale.x = libraryPanel.scale.x + libraryModesPanel.scale.x; 
    
    twoDPaintingPanel.scale.x = 50 - screenGapPerc/2.f - selectedTextureDisplayer.scale.x - (windowPanel.scale.x);
    twoDPaintingPanel.scale.y = 50 - navigationPanel.scale.y;
    twoDPaintingPanel.pos.x = windowPanel.pos.x - windowPanel.scale.x - twoDPaintingPanel.scale.x; //Keep on the left side of the window panel 
    twoDPaintingPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + twoDPaintingPanel.scale.y; //Keep beneath the navigation bar
    paintingModesPanel.pos.x = libraryPanel.pos.x + libraryPanel.scale.x + paintingModesPanel.scale.x + 1;
    paintingModesPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + paintingModesPanel.scale.y;

    faceSelectionCheckComboList.pos = paintingModesPanel.pos;
    faceSelectionCheckComboList.pos.x += paintingModesPanel.scale.x + faceSelectionCheckComboList.scale.x + 0.5f;
    faceSelectionCheckComboList.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + faceSelectionCheckComboList.scale.y;
    paintingOverCheckComboList.pos = faceSelectionCheckComboList.pos;  
    paintingOverCheckComboList.pos.x += faceSelectionCheckComboList.scale.x + paintingOverCheckComboList.scale.x + 0.25f;  
    paintingColorCheckComboList.pos = paintingOverCheckComboList.pos;  
    paintingColorCheckComboList.pos.x += paintingOverCheckComboList.scale.x + paintingColorCheckComboList.scale.x + 0.25f;  
    mirrorCheckComboList.pos = paintingColorCheckComboList.pos;  
    mirrorCheckComboList.pos.x += paintingColorCheckComboList.scale.x + mirrorCheckComboList.scale.x + 0.25f;  
    paintingBrushButton.scale.y = mirrorCheckComboList.scale.y;  
    paintingBrushButton.pos = mirrorCheckComboList.pos;  
    paintingBrushButton.pos.x += mirrorCheckComboList.scale.x + paintingBrushButton.scale.x + 0.25f;  

    //faceSelectionCheckComboList
    //paintingOverCheckComboList
    
    smearPaintingModePropertyPanel.pos.x = libraryPanel.pos.x + libraryPanel.scale.x + smearPaintingModePropertyPanel.scale.x + 1;
    smearPaintingModePropertyPanel.pos.y = paintingModesPanel.pos.y + paintingModesPanel.scale.y + smearPaintingModePropertyPanel.scale.y + 1;
    
    filterPaintingModeFilterBtn.pos.x = libraryPanel.pos.x + libraryPanel.scale.x + filterPaintingModeFilterBtn.scale.x + 1;
    filterPaintingModeFilterBtn.pos.y = paintingModesPanel.pos.y + paintingModesPanel.scale.y + filterPaintingModeFilterBtn.scale.y + 1;
    filterPaintingModeFilterBtn.pos.z = libraryPanel.pos.z;
    
    textureSelectedObjectsButton.pos.x = libraryPanel.pos.x + libraryPanel.scale.x + textureSelectedObjectsButton.scale.x + 1;
    textureSelectedObjectsButton.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + textureSelectedObjectsButton.scale.y + 1;
    textureSelectedObjectsButton.pos.z = libraryPanel.pos.z;
    
    objectsPanel.pos = textureSelectedObjectsButton.pos;
    objectsPanel.pos.x = libraryPanel.pos.x + libraryPanel.scale.x + objectsPanel.scale.x + 1;
    objectsPanel.pos.y += textureSelectedObjectsButton.scale.y + objectsPanel.scale.y + 1.f;

    displayingModesPanel.pos.x = layersPanel.pos.x - layersPanel.scale.x - displayingModesPanel.scale.x - 0.5f;
    displayingModesPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + displayingModesPanel.scale.y;
    
    wrapModeCheckbox.pos = currentModeHintDisplayer.pos;
    wrapModeCheckbox.pos.x = layersPanel.pos.x - layersPanel.scale.x - wrapModeCheckbox.scale.x * 2.f;
    wrapModeCheckbox.pos.y += currentModeHintDisplayer.scale.y + wrapModeCheckbox.scale.y;

    sceneGizmo.pos = wrapModeCheckbox.pos;
    sceneGizmo.pos.x = displayingModesPanel.pos.x;
    sceneGizmo.pos.y += wrapModeCheckbox.scale.y + sceneGizmo.scale.y + 1.f;

    selectedTextureDisplayer.sections[0].elements[0].scale.y = selectedTextureDisplayer.scale.y;

    twoDPaintingPanel.sections[0].elements[0].scale.y = twoDPaintingPanel.scale.y;

}