
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 (c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------
 
    TODO USE ALL THE PROJECT SETTINGS WHILE CREATING A PROJECT 

 */

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "3D/ThreeD.hpp" 

#include "GUI/GUI.hpp" 

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

LogDialog::LogDialog(int){
    this->messagesPanel = Panel(
        {
            {
                Section(
                    Element(Button()),
                    {

                    }
                )
            }
        },
        glm::vec2(10.f,16.f),
        glm::vec3(50.f,50.f,0.8f),
        ColorPalette::mainColor,
        ColorPalette::thirdColor,
        true,
        true,
        true,
        true,
        true,
        1.f,
        1.f,
        {},
        0.25f,
        false
    );

    this->menu_bar = Panel(
        {
            {
                Section(
                    Element(Button()),
                    {
                        Button(ELEMENT_STYLE_SOLID, glm::vec2(2.5f, 1.5f), "", appTextures.messageIcon, 0., true),
                        Button(ELEMENT_STYLE_SOLID, glm::vec2(2.5f, 1.5f), "", appTextures.historyIcon, 0., true),
                        Button(ELEMENT_STYLE_SOLID, glm::vec2(2.5f, 1.5f), "", appTextures.historyLibraryIcon, 0., true),
                        Button(ELEMENT_STYLE_SOLID, glm::vec2(2.5f, 1.5f), "", appTextures.loadingIcon, 0., true)
                    }
                )
            }
        },
        glm::vec2(0.f, 1.5f),
        glm::vec3(50.f,50.f,0.8f),
        ColorPalette::mainColor,
        ColorPalette::thirdColor,
        false,
        true,
        true,
        true,
        true,
        1.f,
        1.f,
        {},
        0.25f,
        false
    );
    this->menu_bar.solidStyle = true;
    this->menu_bar.elementSelectionMode = true; 

    this->menu_bar.sections[0].elements[0].button.infoText = "Messages";
    this->menu_bar.sections[0].elements[1].button.infoText = "General Action History";
    this->menu_bar.sections[0].elements[2].button.infoText = "Library Action History";
    this->menu_bar.sections[0].elements[3].button.infoText = "On-going background operations";
    
    this->menu_bar.sections[0].elements[0].button.textureSizeScale = 1.25f;
    this->menu_bar.sections[0].elements[1].button.textureSizeScale = 1.25f;
    this->menu_bar.sections[0].elements[2].button.textureSizeScale = 1.25f;
    this->menu_bar.sections[0].elements[3].button.textureSizeScale = 1.25f;
    
    this->messagesPanel.solidStyle = true;
    this->messagesPanel.hasSlider = true;

    this->historyPanel = this->messagesPanel;
    this->libraryHistoryPanel = this->messagesPanel;
    this->multiThreadInfoPanel = this->messagesPanel;
    
    this->logBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "", appTextures.mascotCat_default, 0., false);
    this->logBtn.color = glm::vec4(0);
    this->logBtn.color2 = glm::vec4(0);
    this->logBtn.outlineColor = glm::vec4(0);
    this->logBtn.outlineColor2 = glm::vec4(0);
    this->logBtn.pos.z = 0.9f;
}