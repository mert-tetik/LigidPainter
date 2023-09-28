
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https:github.com/mert-tetik/LigidPainter
Official Web Page : https:ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "3D/ThreeD.hpp" 
#include "GUI/GUI.hpp" 
#include "MouseSystem/Mouse.hpp" 
#include "LibrarySystem/Library.hpp" 
#include "NodeSystem/Node/Node.hpp" 
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp" 

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

static void rendering(
                        Panel& panel, 
                        Button& logBtn, 
                        Button& logBtnR, 
                        Button& logBtnL, 
                        glm::vec2& pos, 
                        float& panelXAxisMixVal, 
                        float& panelYAxisMixVal, 
                        bool& messagesActive, 
                        bool& actionHistoryActive, 
                        DialogControl& dialogControl,
                        Timer& timer
                    )
{


    if(logBtnL.clicked){
        messagesActive = !messagesActive;
        actionHistoryActive = false;
    }
    
    if(logBtnR.clicked){
        actionHistoryActive = !actionHistoryActive;
        messagesActive = false;
    }
    
    if(logBtn.clicked){
        messagesActive = false;
        actionHistoryActive = false;
    }

    if(logBtn.clickState1){ 
        pos.x += Mouse::mouseOffset()->x / Settings::videoScale()->x * 100.f;
        pos.y += Mouse::mouseOffset()->y / Settings::videoScale()->y * 100.f;
    }

    logBtn.pos.x = pos.x;
    logBtn.pos.y = pos.y;
    
    logBtnL.pos.x = pos.x - logBtn.scale.x;
    if(messagesActive)
        logBtnL.pos.x -= panel.scale.x * 2.f;
    logBtnL.pos.y = pos.y;

    logBtnR.pos.x = pos.x + logBtn.scale.x;
    if(actionHistoryActive)
        logBtnR.pos.x += panel.scale.x * 2.f;
    logBtnR.pos.y = pos.y;

    timer.transition(messagesActive || actionHistoryActive, panelXAxisMixVal, 0.1f);
    timer.transition(messagesActive || actionHistoryActive, panelYAxisMixVal, 0.4f);
    
    panel.scale.x = panelXAxisMixVal * 10.f;
    panel.scale.y = panelYAxisMixVal * 10.f;
    
    if(messagesActive)
        panel.pos.x = pos.x - panel.scale.x;
    else
        panel.pos.x = pos.x + panel.scale.x;

    panel.pos.y = pos.y + panel.scale.y - panelXAxisMixVal * 2.f;

    panel.render(timer, true);
    logBtn.render(timer, true);
    logBtnL.render(timer, true);
    logBtnR.render(timer, true);
}