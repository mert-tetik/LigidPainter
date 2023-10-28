/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

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

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>

//Forward declarations for the interaction functions defined in the Interactions directory 
void updateLibraryPanelDisplayerElements(Panel &libraryPanelDisplayer, int frameCounter);
void windowPanelInteraction(Panel &windowPanel, Painter &painter, SettingsDialog &settingsDialog, DisplayerDialog &displayerDialog,ExportDialog &exportDialog,BakingDialog &bakingDialog);
void paintingModesPanelInteraction(Panel &paintingModesPanel, Panel& vectorPaintingModePropertyPanel, Panel& smearPaintingModePropertyPanel, Painter &painter, Panel& twoDPaintingPanel, glm::mat4 windowOrtho, float twoDSceneScroll, glm::vec2 twoDScenePos, Button filterPaintingModeFilterBtn, Box twoDPaintingBox, Section paintingChannelsSection);
void nodeInteraction();
void displayingModesPanelInteraction(Panel &displayingModesPanel, Painter &painter);

void UI::elementInteraction(
                                Painter &painter,
                                Timer &timer, 
                                float screenGapPerc,
                                Project& project,
                                AppMaterialModifiers& appMaterialModifiers
                            )
{
    //!Dialog & panel state
    this->anyDialogActive = 
                    anyContextMenuActive || 
                    greetingDialog.dialogControl.isActive() ||
                    newProjectDialog.dialogControl.isActive() ||
                    exportDialog.dialogControl.isActive() ||
                    materialDisplayerDialog.dialogControl.isActive() ||
                    filterDisplayerDialog.dialogControl.isActive() ||
                    loadProjectDialog.dialogControl.isActive() ||
                    materialEditorDialog.dialogControl.isActive() ||
                    textureSelectionDialog.dialogControl.isActive() ||
                    bakingDialog.dialogControl.isActive() ||
                    filterSelectionDialog.dialogControl.isActive() ||
                    newTextureDialog.dialogControl.isActive() ||
                    settingsDialog.dialogControl.isActive() ||
                    displayerDialog.dialogControl.isActive() ||
                    textureEditorDialog.dialogControl.isActive() ||
                    texturePackEditorDialog.dialogControl.isActive();

    this->anyPanelHover = 
                    navigationPanel.hover               ||
                    windowPanel.hover                   ||
                    paintingPanel.hover                 ||
                    paintingPanelModePanel.hover        ||
                    libraryPanelLeft.hover              ||
                    libraryPanelDisplayer.hover         ||
                    nodeEditorDisplayer.hover           ||
                    selectedTextureDisplayer.hover      || 
                    paintingModesPanel.hover            ||
                    displayingModesPanel.hover          ||
                    filterPaintingModeFilterBtn.hover   ||
                    smearPaintingModePropertyPanel.hover||
                    logDialog.isHovered() ||
                    vectorPaintingModePropertyPanel.hover;

    this->contextMenuInteraction(timer, project, painter, appMaterialModifiers);
    
    updateLibraryPanelDisplayerElements(libraryPanelDisplayer, frameCounter);
    
    this->libraryPanelDisplayerInteraction(painter, appMaterialModifiers, timer, project);
    
    this->libraryPanelLeftInteraction(libraryPanelLeft);

    this->paintingPanelInteraction(painter);
    
    windowPanelInteraction(windowPanel, painter, this->settingsDialog, this->displayerDialog, this->exportDialog, this->bakingDialog);

    paintingModesPanelInteraction(this->paintingModesPanel, this->vectorPaintingModePropertyPanel, this->smearPaintingModePropertyPanel, painter, twoDPaintingPanel, this->projection, this->twoDPaintingSceneScroll, this->twoDPaintingScenePos, this->filterPaintingModeFilterBtn, this->twoDPaintingBox, this->paintingChannelsSection);

    displayingModesPanelInteraction(displayingModesPanel, painter);

    nodeInteraction();
    
    this->panelPositioning(screenGapPerc,painter);
}