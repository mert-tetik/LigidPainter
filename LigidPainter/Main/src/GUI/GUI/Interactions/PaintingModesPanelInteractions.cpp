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

void paintingModesPanelInteraction(
                                    Panel &paintingModesPanel, 
                                    Panel &vectorPaintingModePropertyPanel, 
                                    Painter &painter,
                                    Panel& twoDPaintingPanel, 
                                    glm::mat4 windowOrtho, 
                                    float twoDSceneScroll, 
                                    glm::vec2 twoDScenePos
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

                painter.vectorStrokes.insert(painter.vectorStrokes.begin() + i - 1, newStroke);
                i++;
            }
        }
    }
    if(vectorPaintingModePropertyPanel.sections[0].elements[2].button.clicked){
        painter.vectorStrokes.clear();
    }
    if(vectorPaintingModePropertyPanel.sections[0].elements[3].button.clicked){
        for (size_t i = 0; i < painter.vectorStrokes.size(); i++)
        {
            if(i == 0 && painter.vectorStrokes[i].startPointClicked)
                painter.vectorStrokes.erase(painter.vectorStrokes.begin() + i);
            else if(painter.vectorStrokes[i].endPointClicked){
                painter.vectorStrokes.erase(painter.vectorStrokes.begin() + i);
                painter.vectorStrokes[i].startPos = painter.vectorStrokes[i - 1].endPos; 
                i--;
            }
        }
    }
    if(vectorPaintingModePropertyPanel.sections[0].elements[4].button.clicked){
        painter.applyVectorStrokes(twoDPaintingPanel, windowOrtho, twoDSceneScroll, twoDScenePos);
    }
}