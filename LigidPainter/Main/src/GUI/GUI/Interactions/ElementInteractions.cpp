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
void libraryPanelDisplayerInteraction(Panel &libraryPanelDisplayer, Mouse &mouse, Panel &paintingPanel, Painter &painter, Library &library, Model &model, ColorPalette& colorPalette, Shaders &shaders, int &textureRes,NewTextureDialog &newTextureDialog,AppTextures &appTextures,int frameCounter);
void updateLibraryPanelDisplayerElements(Panel &libraryPanelDisplayer, Library &library, ColorPalette& colorPalette, Shaders &shaders,int frameCounter);
void paintingPanelInteraction(Panel &paintingPanel, Mouse &mouse, Painter &painter, Dropper &dropper,ColorPalette colorPalette,Shader buttonShader, AppTextures appTextures, Model &model, std::vector<Node> &meshNodeScene);
void windowPanelInteraction(Panel &windowPanel, Mouse &mouse, Painter &painter, SettingsDialog &settingsDialog, DisplayerDialog &displayerDialog,ExportDialog &exportDialog);
void paintingModesPanelInteraction(Panel &paintingModesPanel, Painter &painter);
void nodeInteraction(Model &model, std::vector<Node> &meshNodeScene,Shaders shaders,ColorPalette colorPalette,glm::vec2 videoScale);
void displayingModesPanelInteraction(Panel &displayingModesPanel, Painter &painter);

void UI::elementInteraction(
                                Painter &painter,
                                Mouse &mouse, 
                                Library &library,
                                std::vector<ContextMenu> &contextMenus,
                                std::vector<Node> &meshNodeScene,
                                Context &context,
                                glm::vec2 &videoScale,
                                TextRenderer &textRenderer, 
                                Timer &timer, 
                                int &textureRes,
                                float screenGapPerc,
                                Model &model, 
                                Project& project
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

    this->contextMenuInteraction(contextMenus,mouse,library,meshNodeScene,context,videoScale,timer,textRenderer,project,textureRes, painter, shaders.heightToNormalMap);
    
    updateLibraryPanelDisplayerElements(libraryPanelDisplayer,library,colorPalette,shaders,frameCounter);
    
    libraryPanelDisplayerInteraction(libraryPanelDisplayer,mouse,paintingPanel,painter,library,model,colorPalette,shaders,textureRes,newTextureDialog,appTextures,frameCounter);
    
    this->libraryPanelLeftInteraction(libraryPanelLeft,library,mouse);

    paintingPanelInteraction(paintingPanel,mouse,painter,dropper,colorPalette,shaders.buttonShader,appTextures,model, meshNodeScene);
    
    windowPanelInteraction(windowPanel, mouse, painter, settingsDialog, displayerDialog,exportDialog);

    paintingModesPanelInteraction(paintingModesPanel,painter);

    displayingModesPanelInteraction(displayingModesPanel, painter);


    nodeInteraction(model,meshNodeScene,shaders,colorPalette,videoScale);
    
    this->panelPositioning(screenGapPerc,library,painter);
}