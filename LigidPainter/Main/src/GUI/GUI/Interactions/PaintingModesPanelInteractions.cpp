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
                                    glm::mat4 windowOrtho, 
                                    float twoDSceneScroll, 
                                    glm::vec2 twoDScenePos,
                                    Button filterPaintingModeFilterBtn,
                                    Box twoDPaintingBox,
                                    Material& paintingCustomMat
                                )
{
    //!PAINTING MODES PANEL
    for (size_t i = 0; i < paintingModesPanel.sections[0].elements.size(); i++)
    {
        if(paintingModesPanel.sections[0].elements[i].button.clickState1){ //Painting mode pressed
            if(painter.selectedPaintingModeIndex != i){
                paintingModesPanel.sections[0].elements[painter.selectedPaintingModeIndex].button.clickState1 = false;
                painter.selectedPaintingModeIndex = i;
                break;
            }
        }
        if(painter.selectedPaintingModeIndex == i){
            paintingModesPanel.sections[0].elements[painter.selectedPaintingModeIndex].button.clickState1 = true;
        }
    }

    if(vectorPaintingModePropertyPanel.sections[0].elements[1].button.clicked){

        registerVectorAction("New point between the selected points", painter.vectorStrokes);

        for (size_t i = 0; i < painter.vectorStrokes.size(); i++)
        {
            if(painter.vectorStrokes[i].endPointClicked && painter.vectorStrokes[i].startPointClicked){
                glm::vec2 strokeCenter = (painter.vectorStrokes[i].startPos + painter.vectorStrokes[i].endPos + painter.vectorStrokes[i].offsetPos) / 3.0f;
                glm::vec2 offsetPointDistance = strokeCenter - painter.vectorStrokes[i].offsetPos;

                VectorStroke newStroke;
                newStroke.startPos = strokeCenter;
                newStroke.endPos = painter.vectorStrokes[i].endPos;
                newStroke.offsetPos = newStroke.startPos - (newStroke.startPos - newStroke.endPos) / 2.f - offsetPointDistance;

                painter.vectorStrokes[i].endPos  = strokeCenter;

                painter.vectorStrokes.insert(painter.vectorStrokes.begin() + i + 1, newStroke);
                i++;
            }
        }
    }
    if(vectorPaintingModePropertyPanel.sections[0].elements[2].button.clicked){
        registerVectorAction("Vector strokes cleared", painter.vectorStrokes);
        painter.vectorStrokes.clear();
    }
    if(vectorPaintingModePropertyPanel.sections[0].elements[3].button.clicked){
        registerVectorAction("Selected vector point deleted", painter.vectorStrokes);
        for (size_t i = 0; i < painter.vectorStrokes.size(); i++)
        {
            if(i == 0){
                if(painter.vectorStrokes[i].startPointClicked){
                    painter.vectorStrokes.erase(painter.vectorStrokes.begin() + i);
                    break;
                }
                else if(painter.vectorStrokes[i].endPointClicked && painter.vectorStrokes.size() > 1){
                    painter.vectorStrokes[i].endPos = painter.vectorStrokes[i + 1].endPos; 
                    painter.vectorStrokes.erase(painter.vectorStrokes.begin() + i + 1);
                    break;
                }
            }
            else{
                if(painter.vectorStrokes[i].endPointClicked){
                    painter.vectorStrokes.erase(painter.vectorStrokes.begin() + i);
                    if(i - 1 < painter.vectorStrokes.size() && i < painter.vectorStrokes.size())
                        painter.vectorStrokes[i].startPos = painter.vectorStrokes[i - 1].endPos; 
                    break;
                }
            }
        }
    }
    if(vectorPaintingModePropertyPanel.sections[0].elements[4].button.clicked){
        painter.applyVectorStrokes(painter.vectorStrokes, twoDPaintingPanel, windowOrtho, vectorPaintingModePropertyPanel.sections[0].elements[0].comboBox.selectedIndex, filterPaintingModeFilterBtn.filter, twoDPaintingBox, paintingCustomMat);
    }

    painter.smearTransformStrength = smearPaintingModePropertyPanel.sections[0].elements[0].rangeBar.value;
    painter.smearBlurStrength = smearPaintingModePropertyPanel.sections[0].elements[1].rangeBar.value;
}