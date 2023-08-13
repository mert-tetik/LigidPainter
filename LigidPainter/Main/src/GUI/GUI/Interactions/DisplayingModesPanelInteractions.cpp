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

void displayingModesPanelInteraction(
                                        Panel &displayingModesPanel, 
                                        Painter &painter,
                                        std::vector<Node> &meshNodeScene, 
                                        Model &model, 
                                        Scene scene, 
                                        int textureRes
                                    )
{
    //!PAINTING MODES PANEL
    for (size_t i = 0; i < displayingModesPanel.sections[0].elements.size(); i++)
    {
        if(displayingModesPanel.sections[0].elements[i].button.clickState1){ //Painting mode pressed
            if(painter.selectedDisplayingModeIndex != i){
                displayingModesPanel.sections[0].elements[painter.selectedDisplayingModeIndex].button.clickState1 = false;
                painter.selectedDisplayingModeIndex = i;
                if(painter.selectedDisplayingModeIndex == 0)
                    UTIL::updateNodeResults(meshNodeScene, model, scene, textureRes, -1);
                break;
            }
        }

        if(painter.selectedDisplayingModeIndex == i){
            displayingModesPanel.sections[0].elements[painter.selectedDisplayingModeIndex].button.clickState1 = true;
        }
    }
}
