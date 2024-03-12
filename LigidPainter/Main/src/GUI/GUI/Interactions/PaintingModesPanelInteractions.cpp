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
#include "Layers/Layers.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>

void paintingModesPanelInteraction(
                                    Panel &paintingModesPanel, 
                                    Panel &vectorPaintingModePropertyPanel, 
                                    Panel &smearPaintingModePropertyPanel, 
                                    Painter &painter,
                                    Panel& twoDPaintingPanel, 
                                    float twoDSceneScroll, 
                                    glm::vec2 twoDScenePos,
                                    Button filterPaintingModeFilterBtn,
                                    Box twoDPaintingBox,
                                    Material& paintingCustomMat,
                                    std::vector<TextureField> textureFields,
                                    CheckBox vectorPaintingMode2DModeWrapCheckBox
                                )
{
    //!PAINTING MODES PANEL
    for (size_t i = 0; i < paintingModesPanel.sections[0].elements.size(); i++)
    {
        if(!painter.getSelectedMesh()->layerScene.any_vector_editing()){
            if(paintingModesPanel.sections[0].elements[i].button.clickState1){ //Painting mode pressed
                if(painter.selectedPaintingModeIndex != i){
                    paintingModesPanel.sections[0].elements[painter.selectedPaintingModeIndex].button.clickState1 = false;
                    painter.selectedPaintingModeIndex = i;
                    break;
                }
            }
        }
        else{
            paintingModesPanel.sections[0].elements[i].button.clickState1 = false;
        }
        if(painter.selectedPaintingModeIndex == i){
            paintingModesPanel.sections[0].elements[i].button.clickState1 = true;
        }
    }

    if(vectorPaintingModePropertyPanel.sections[0].elements[1].button.clicked){
        painter.subdivideSelectedPoints();
    }
    if(vectorPaintingModePropertyPanel.sections[0].elements[2].button.clicked){
        painter.clearSelectedVectorPoints();
    }
    if(vectorPaintingModePropertyPanel.sections[0].elements[3].button.clicked || shortcuts_CTRL_X()){
        painter.deleteSelectedVectorPoints();
    }
    if(vectorPaintingModePropertyPanel.sections[0].elements[4].button.clicked){
        painter.applyVectorStrokes(
                                        painter.vectorStrokes, 
                                        twoDPaintingPanel, 
                                        vectorPaintingModePropertyPanel.sections[0].elements[0].comboBox.selectedIndex, 
                                        filterPaintingModeFilterBtn.filter, 
                                        twoDPaintingBox, 
                                        paintingCustomMat, 
                                        textureFields,
                                        vectorPaintingMode2DModeWrapCheckBox.clickState1
                                    );
    }

    painter.smearTransformStrength = smearPaintingModePropertyPanel.sections[0].elements[0].rangeBar.value;
    painter.smearBlurStrength = smearPaintingModePropertyPanel.sections[0].elements[1].rangeBar.value;
}