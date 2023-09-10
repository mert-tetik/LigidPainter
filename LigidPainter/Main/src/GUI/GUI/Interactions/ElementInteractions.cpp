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
void libraryPanelDisplayerInteraction(Panel &libraryPanelDisplayer, Panel &paintingPanel, Painter &painter, NewTextureDialog &newTextureDialog, AppMaterialModifiers& appMaterialModifiers, MaterialDisplayerDialog &materialDisplayerDialog, FilterDisplayerDialog &filterDisplayerDialog, Button& zoomingDisplayingButton, Timer& timer);
void updateLibraryPanelDisplayerElements(Panel &libraryPanelDisplayer, int frameCounter);
void paintingPanelInteraction(Panel &paintingPanel, Painter &painter, Dropper &dropper);
void windowPanelInteraction(Panel &windowPanel, Painter &painter, SettingsDialog &settingsDialog, DisplayerDialog &displayerDialog,ExportDialog &exportDialog,BakingDialog &bakingDialog);
void paintingModesPanelInteraction(Panel &paintingModesPanel, Panel& vectorPaintingModePropertyPanel, Painter &painter, Panel& twoDPaintingPanel, glm::mat4 windowOrtho, float twoDSceneScroll, glm::vec2 twoDScenePos);
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
                    textureSelectionDialog.dialogControl.isActive() || 
                    materialEditorDialog.dialogControl.isActive()   || 
                    anyContextMenuActive                            || 
                    displayerDialog.dialogControl.isActive()        || 
                    greetingDialog.dialogControl.isActive()         || 
                    settingsDialog.dialogControl.isActive()         || 
                    newProjectDialog.dialogControl.isActive()       || 
                    newTextureDialog.dialogControl.isActive()       || 
                    textureEditorDialog.dialogControl.isActive()    || 
                    materialDisplayerDialog.dialogControl.isActive()|| 
                    filterDisplayerDialog.dialogControl.isActive()  || 
                    loadProjectDialog.dialogControl.isActive();   

    this->anyPanelHover = 
                    navigationPanel.hover               ||
                    windowPanel.hover                   ||
                    paintingPanel.hover                 ||
                    libraryPanelLeft.hover              ||
                    libraryPanelDisplayer.hover         ||
                    nodeEditorDisplayer.hover           ||
                    selectedTextureDisplayer.hover      || 
                    paintingModesPanel.hover            ||
                    displayingModesPanel.hover          ||
                    vectorPaintingModePropertyPanel.hover;

    this->contextMenuInteraction(timer, project, painter);
    
    updateLibraryPanelDisplayerElements(libraryPanelDisplayer, frameCounter);
    
    libraryPanelDisplayerInteraction(libraryPanelDisplayer, paintingPanel, painter, newTextureDialog, appMaterialModifiers, materialDisplayerDialog, filterDisplayerDialog, zoomingDisplayingButton, timer);
    
    this->libraryPanelLeftInteraction(libraryPanelLeft);

    paintingPanelInteraction(paintingPanel, painter, dropper);
    
    windowPanelInteraction(windowPanel, painter, this->settingsDialog, this->displayerDialog, this->exportDialog, this->bakingDialog);

    paintingModesPanelInteraction(paintingModesPanel, this->vectorPaintingModePropertyPanel, painter, twoDPaintingPanel, this->projection, this->twoDPaintingSceneScroll, this->twoDPaintingScenePos);

    displayingModesPanelInteraction(displayingModesPanel, painter);

    nodeInteraction();
    
    this->panelPositioning(screenGapPerc,painter);
}