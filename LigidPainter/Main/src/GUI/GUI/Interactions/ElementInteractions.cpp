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
void libraryPanelDisplayerInteraction(Panel &libraryPanelDisplayer, Mouse &mouse, Panel &paintingPanel, Painter &painter,  Model &model, ColorPalette& colorPalette, int &textureRes,NewTextureDialog &newTextureDialog,AppTextures &appTextures, AppMaterialModifiers& appMaterialModifiers, int frameCounter);
void updateLibraryPanelDisplayerElements(Panel &libraryPanelDisplayer,  ColorPalette& colorPalette, int frameCounter);
void paintingPanelInteraction(Panel &paintingPanel, Mouse &mouse, Painter &painter, Dropper &dropper,ColorPalette colorPalette, AppTextures appTextures, Model &model, std::vector<Node> &meshNodeScene);
void windowPanelInteraction(Panel &windowPanel, Mouse &mouse, Painter &painter, SettingsDialog &settingsDialog, DisplayerDialog &displayerDialog,ExportDialog &exportDialog);
void paintingModesPanelInteraction(Panel &paintingModesPanel, Painter &painter);
void nodeInteraction(Model &model, std::vector<Node> &meshNodeScene,ColorPalette colorPalette,glm::vec2 videoScale, Scene scene, int textureRes);
void displayingModesPanelInteraction(Panel &displayingModesPanel, Painter &painter, std::vector<Node> &meshNodeScene, Model &model, Scene scene, int textureRes);

void UI::elementInteraction(
                                Painter &painter,
                                Mouse &mouse, 
                                
                                std::vector<ContextMenu> &contextMenus,
                                std::vector<Node> &meshNodeScene,
                                Context &context,
                                glm::vec2 &videoScale,
                                TextRenderer &textRenderer, 
                                Timer &timer, 
                                int &textureRes,
                                float screenGapPerc,
                                Model &model, 
                                Project& project,
                                Scene scene,
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
                    loadProjectDialog.dialogControl.isActive();   

    this->anyPanelHover = 
                    navigationPanel.hover               ||
                    windowPanel.hover                   ||
                    paintingPanel.hover                 ||
                    libraryPanelLeft.hover              ||
                    libraryPanelDisplayer.hover         ||
                    nodeEditorDisplayer.hover           ||
                    selectedTextureDisplayer.hover      || 
                    paintingModesPanel.hover;

    this->contextMenuInteraction(contextMenus,mouse,meshNodeScene,context,videoScale,timer,textRenderer,project,textureRes, painter);
    
    updateLibraryPanelDisplayerElements(libraryPanelDisplayer,colorPalette,frameCounter);
    
    libraryPanelDisplayerInteraction(libraryPanelDisplayer,mouse,paintingPanel,painter,model,colorPalette,textureRes,newTextureDialog,appTextures, appMaterialModifiers,frameCounter);
    
    this->libraryPanelLeftInteraction(libraryPanelLeft,mouse);

    paintingPanelInteraction(paintingPanel,mouse,painter,dropper,colorPalette,appTextures,model, meshNodeScene);
    
    windowPanelInteraction(windowPanel, mouse, painter, settingsDialog, displayerDialog,exportDialog);

    paintingModesPanelInteraction(paintingModesPanel,painter);

    displayingModesPanelInteraction(displayingModesPanel, painter, meshNodeScene, model, scene, textureRes);

    nodeInteraction(model,meshNodeScene,colorPalette,videoScale, scene, textureRes);
    
    this->panelPositioning(screenGapPerc,painter);
}