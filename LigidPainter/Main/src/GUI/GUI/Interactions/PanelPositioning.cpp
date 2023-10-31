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
    paintingPanel.pos.x = windowPanel.pos.x - windowPanel.scale.x - paintingPanel.scale.x; //Keep on the left side of the window panel 
    windowPanel.pos.x = 100.f - windowPanel.scale.x - screenGapPerc; //Keep on the right side
    libraryPanelDisplayer.pos.x = libraryPanelLeft.pos.x + libraryPanelLeft.scale.x + libraryPanelDisplayer.scale.x; //Keep on the left side of the window panel 
    
    paintingPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + paintingPanel.scale.y; //Keep beneath the navigation bar
    windowPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + windowPanel.scale.y; //Keep beneath the navigation bar
    libraryPanelLeft.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + windowPanel.scale.y; //Keep beneath the navigation bar
    libraryPanelDisplayer.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + libraryPanelDisplayer.scale.y; //Keep beneath the navigation bar
    libraryPanelDisplayer.scale.y = 50 - navigationPanel.scale.y - nodeEditorDisplayer.scale.y;
    libraryPanelLeft.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + libraryPanelDisplayer.scale.y; //Keep beneath the navigation bar
    libraryPanelLeft.scale.y = 50 - navigationPanel.scale.y - nodeEditorDisplayer.scale.y;
    nodeEditorDisplayer.pos.x = paintingPanelModePanel.pos.x - paintingPanelModePanel.scale.x - nodeEditorDisplayer.scale.x; //Keep on the left side of the window panel 
    selectedTextureDisplayer.pos.x = libraryPanelDisplayer.pos.x - libraryPanelLeft.pos.x;
    selectedTextureDisplayer.pos.y = nodeEditorDisplayer.pos.y;
    selectedTextureDisplayer.scale.y = nodeEditorDisplayer.scale.y;
    selectedTextureDisplayer.scale.x = libraryPanelDisplayer.scale.x + libraryPanelLeft.scale.x;
    
    nodeEditorDisplayer.scale.x = 50 - screenGapPerc/2.f - selectedTextureDisplayer.scale.x - (paintingPanelModePanel.scale.x+windowPanel.scale.x+paintingPanel.scale.x);
    
    twoDPaintingPanel.scale.x = 50 - screenGapPerc/2.f - selectedTextureDisplayer.scale.x - (paintingPanelModePanel.scale.x+windowPanel.scale.x+paintingPanel.scale.x);
    twoDPaintingPanel.scale.y = 50 - navigationPanel.scale.y - nodeEditorDisplayer.scale.y;
    twoDPaintingPanel.pos.x = paintingPanelModePanel.pos.x - paintingPanelModePanel.scale.x - twoDPaintingPanel.scale.x; //Keep on the left side of the window panel 
    twoDPaintingPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + twoDPaintingPanel.scale.y; //Keep beneath the navigation bar
    paintingModesPanel.pos.x = libraryPanelDisplayer.pos.x + libraryPanelDisplayer.scale.x + paintingModesPanel.scale.x + 1;
    paintingModesPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + paintingModesPanel.scale.y;

    vectorPaintingModePropertyPanel.pos.x = libraryPanelDisplayer.pos.x + libraryPanelDisplayer.scale.x + vectorPaintingModePropertyPanel.scale.x + 1;
    vectorPaintingModePropertyPanel.pos.y = paintingModesPanel.pos.y + paintingModesPanel.scale.y + vectorPaintingModePropertyPanel.scale.y + 1;
    
    smearPaintingModePropertyPanel.pos.x = libraryPanelDisplayer.pos.x + libraryPanelDisplayer.scale.x + smearPaintingModePropertyPanel.scale.x + 1;
    smearPaintingModePropertyPanel.pos.y = paintingModesPanel.pos.y + paintingModesPanel.scale.y + smearPaintingModePropertyPanel.scale.y + 1;
    
    filterPaintingModeFilterBtn.pos.x = libraryPanelDisplayer.pos.x + libraryPanelDisplayer.scale.x + filterPaintingModeFilterBtn.scale.x + 1;
    filterPaintingModeFilterBtn.pos.y = paintingModesPanel.pos.y + paintingModesPanel.scale.y + filterPaintingModeFilterBtn.scale.y + 1;
    filterPaintingModeFilterBtn.pos.z = libraryPanelDisplayer.pos.z;

    displayingModesPanel.pos.x = paintingPanelModePanel.pos.x - paintingPanelModePanel.scale.x - displayingModesPanel.scale.x - 0.5f;
    displayingModesPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + displayingModesPanel.scale.y;

    selectedTextureDisplayer.sections[0].elements[0].scale.y = selectedTextureDisplayer.scale.y;
    if(Library::getTextureArraySize())
        selectedTextureDisplayer.sections[0].elements[0].button.texture = painter.selectedTexture;
    twoDPaintingPanel.sections[0].elements[0].scale.y = twoDPaintingPanel.scale.y;

    paintingPanelModePanel.pos = paintingPanel.pos;
    paintingPanelModePanel.pos.x -= paintingPanel.scale.x + paintingPanelModePanel.scale.x;

    paintingPanelModeDisplayer.pos = paintingPanel.pos;
    paintingPanelModeDisplayer.scale.x = 5.f;
    if(paintingPanel.scale.x < 6.f)
        paintingPanelModeDisplayer.scale.x = paintingPanel.scale.x;
    paintingPanelModeDisplayer.pos.x = paintingPanel.pos.x - paintingPanel.scale.x + paintingPanelModeDisplayer.scale.x;
    paintingPanelModeDisplayer.pos.y -= paintingPanel.scale.y - paintingPanelModeDisplayer.scale.y - 2.f;
}