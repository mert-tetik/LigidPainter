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
#include "MouseSystem/Mouse.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>

void windowPanelInteraction(
                                Panel &windowPanel, 
                                Painter &painter, 
                                Timer& timer
                            )
{
    //!WINDOW PANEL BUTTONS
    if(windowPanel.sections[0].elements[0].button.clicked){//Pressed to the 3D painting button of the window panel
        painter.threeDimensionalMode = true;
    }
    else if(windowPanel.sections[0].elements[1].button.clicked){//Pressed to the 2D painting button of the window panel
        painter.threeDimensionalMode = false;
    }
    else if(windowPanel.sections[0].elements[2].button.clicked){//Pressed to the export button of the window panel
        dialog_export.show(timer, Project());
    }
    else if(windowPanel.sections[0].elements[3].button.clicked){//Pressed to the displayer button of the window panel
        dialog_displayer.show(timer, Skybox());
    }
    else if(windowPanel.sections[0].elements[4].button.clicked){//Pressed to the settings button of the window panel
        dialog_settings.show(timer, painter);
    }
    else if(windowPanel.sections[0].elements[5].button.clicked){//Pressed to the settings button of the window panel
        dialog_baking.show(timer, Skybox());
    }
    else if(windowPanel.sections[0].elements[6].button.clicked){//Pressed to the materials button of the window panel
        dialog_materialSelection.show(timer, nullptr);
    }
}