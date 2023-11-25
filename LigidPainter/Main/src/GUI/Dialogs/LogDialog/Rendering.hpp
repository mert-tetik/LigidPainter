
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
                        Button& messageInfoBtn, 
                        float messageInfoBtnMixVal, 
                        bool& messageInfoActive,
                        glm::vec2& pos, 
                        float& panelXAxisMixVal, 
                        float& panelYAxisMixVal, 
                        bool& messagesActive, 
                        bool& actionHistoryActive, 
                        DialogControl& dialogControl,
                        Timer& timer
                    )
{


    if(*Mouse::LClick()){
        if(logBtnL.hover && !messageInfoActive){
            messagesActive = !messagesActive;
            actionHistoryActive = false;
        }
        
        else if(logBtnR.hover && !messageInfoActive){
            actionHistoryActive = !actionHistoryActive;
            messagesActive = false;
        }
        
        else{
            messagesActive = false;
            actionHistoryActive = false;
        }
    }

    if(logBtn.clickState1){ 
        pos.x += Mouse::mouseOffset()->x / Settings::videoScale()->x * 100.f;
        pos.y += Mouse::mouseOffset()->y / Settings::videoScale()->y * 100.f;
    }

    logBtn.pos.x = pos.x;
    logBtn.pos.y = pos.y;
    
    logBtnL.pos.x = pos.x - logBtn.scale.x;
    if(messagesActive)
        logBtnL.pos.x -= panel.scale.x * 1.85f;
    else
        logBtnL.pos.x -= messageInfoBtn.scale.x * 1.9f;
    
    logBtnL.pos.y = pos.y + std::sin(LigidGL::getTime() * 2.f + 1.f) / 4.f;

    logBtnR.pos.x = pos.x + logBtn.scale.x;
    if(actionHistoryActive)
        logBtnR.pos.x += panel.scale.x * 1.85f;
    logBtnR.pos.y = pos.y + std::sin(LigidGL::getTime() * 2.f) / 4.f;

    timer.transition(messagesActive || actionHistoryActive, panelXAxisMixVal, 0.1f);
    timer.transition(messagesActive || actionHistoryActive, panelYAxisMixVal, 0.4f);
    
    panel.scale.x = panelXAxisMixVal * 10.f;
    panel.scale.y = panelYAxisMixVal * 10.f;
    
    if(messagesActive)
        panel.pos.x = pos.x - panel.scale.x;
    else
        panel.pos.x = pos.x + panel.scale.x;

    panel.pos.y = pos.y + panel.scale.y - panelXAxisMixVal * 2.f;

    messageInfoBtn.scale.x = messageInfoBtnMixVal * 15.f;
    messageInfoBtn.pos.x = pos.x - messageInfoBtn.scale.x;
    messageInfoBtn.pos.y = pos.y;

    panel.render(timer, true);
    ShaderSystem::buttonShader().setFloat("rotation", std::sin(LigidGL::getTime() * 2.f) * 10.f + (std::sin(logBtn.clickedMixVal) * 360.f));
    logBtn.render(timer, true);
    ShaderSystem::buttonShader().setFloat("rotation", 0.f);
    logBtnL.render(timer, true);
    logBtnR.render(timer, true);
    messageInfoBtn.render(timer, true);

    if(logBtn.hover && !logBtnL.hover && !logBtnR.hover)
        logBtn.texture = Settings::appTextures().mascotCat_smile;
    else if(messagesActive || actionHistoryActive)
        logBtn.texture = Settings::appTextures().mascotCat_relaxed;
    else
        logBtn.texture = Settings::appTextures().mascotCat_default;
}