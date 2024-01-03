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
void windowPanelInteraction(Panel &windowPanel, Painter &painter, SettingsDialog &settingsDialog, DisplayerDialog &displayerDialog,ExportDialog &exportDialog,BakingDialog &bakingDialog, MaterialSelectionDialog &materialSelectionDialog);
void paintingModesPanelInteraction(Panel &paintingModesPanel, Panel& vectorPaintingModePropertyPanel, Panel& smearPaintingModePropertyPanel, Painter &painter, Panel& twoDPaintingPanel, glm::mat4 windowOrtho, float twoDSceneScroll, glm::vec2 twoDScenePos, Button filterPaintingModeFilterBtn, Box twoDPaintingBox, Material& paintingCustomMat);
void displayingModesPanelInteraction(Panel &displayingModesPanel, Painter &painter);

size_t lastSecAnyDialogActiveWasTrue = 0;

void UI::elementInteraction(
                                Painter &painter,
                                Timer &timer, 
                                float screenGapPerc,
                                Project& project
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
                    objectTexturingDialog.dialogControl.isActive() ||
                    materialSelectionDialog.dialogControl.isActive() ||
                    paintingChannelsAutoCreateTexturesDialog.dialogControl.isActive() ||
                    paintingChannelsTextureSelectionPanelActive ||
                    texturePackEditorDialog.dialogControl.isActive() ||
                    timer.seconds - lastSecAnyDialogActiveWasTrue <= 2; 

    if(this->anyDialogActive && !(timer.seconds - lastSecAnyDialogActiveWasTrue <= 3)){
        lastSecAnyDialogActiveWasTrue = timer.seconds;
    }

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
                    textureSelectedObjectsButton.hover   ||
                    objectsPanel.hover   ||
                    smearPaintingModePropertyPanel.hover||
                    logDialog.isHovered() ||
                    vectorPaintingModePropertyPanel.hover;

    Debugger::block("GUI : Interactions : contextMenuInteraction"); // Start
    this->contextMenuInteraction(timer, project, painter);
    Debugger::block("GUI : Interactions : contextMenuInteraction"); // End
    
    Debugger::block("GUI : Interactions : updateLibraryPanelDisplayerElements"); // Start
    updateLibraryPanelDisplayerElements(libraryPanelDisplayer, frameCounter);
    Debugger::block("GUI : Interactions : updateLibraryPanelDisplayerElements"); // End
    
    Debugger::block("GUI : Interactions : libraryPanelDisplayerInteraction"); // Start
    this->libraryPanelDisplayerInteraction(painter, timer, project);
    Debugger::block("GUI : Interactions : libraryPanelDisplayerInteraction"); // End
    
    Debugger::block("GUI : Interactions : libraryPanelLeftInteraction"); // Start
    this->libraryPanelLeftInteraction(libraryPanelLeft);
    Debugger::block("GUI : Interactions : libraryPanelLeftInteraction"); // End

    Debugger::block("GUI : Interactions : paintingPanelInteraction"); // Start
    this->paintingPanelInteraction(painter);
    Debugger::block("GUI : Interactions : paintingPanelInteraction"); // End
    
    Debugger::block("GUI : Interactions : windowPanelInteraction"); // Start
    windowPanelInteraction(windowPanel, painter, this->settingsDialog, this->displayerDialog, this->exportDialog, this->bakingDialog, this->materialSelectionDialog);
    Debugger::block("GUI : Interactions : windowPanelInteraction"); // End

    Debugger::block("GUI : Interactions : paintingModesPanelInteraction"); // Start
    paintingModesPanelInteraction(this->paintingModesPanel, this->vectorPaintingModePropertyPanel, this->smearPaintingModePropertyPanel, painter, twoDPaintingPanel, this->projection, this->twoDPaintingSceneScroll, this->twoDPaintingScenePos, this->filterPaintingModeFilterBtn, this->twoDPaintingBox, this->paintingCustomMat);
    Debugger::block("GUI : Interactions : paintingModesPanelInteraction"); // End

    Debugger::block("GUI : Interactions : displayingModesPanelInteraction"); // Start
    displayingModesPanelInteraction(displayingModesPanel, painter);
    Debugger::block("GUI : Interactions : displayingModesPanelInteraction"); // End

    Debugger::block("GUI : Interactions : panelPositioning"); // Start
    this->panelPositioning(screenGapPerc,painter);
    Debugger::block("GUI : Interactions : panelPositioning"); // End
}