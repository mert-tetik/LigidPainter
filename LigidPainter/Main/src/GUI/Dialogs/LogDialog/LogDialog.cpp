
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 Copyright (c) 2022-2023, Mert Tetik

 All rights reserved.

 Official GitHub Link : https:github.com/mert-tetik/LigidPainter
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
#include "MouseSystem/Mouse.hpp" 
#include "LibrarySystem/Library.hpp" 
#include "NodeSystem/Node/Node.hpp" 
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp" 

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

#include "GUI/Dialogs/LogDialog/Registering.hpp"
#include "GUI/Dialogs/LogDialog/Rendering.hpp"
#include "GUI/Dialogs/LogDialog/Undo.hpp"

std::vector<Action> __actions;


namespace LGDLOG{
    LogMsg start;
    std::string end = "$#";
}

LogDialog::LogDialog(){

}

LogDialog::LogDialog(AppMaterialModifiers& appMaterialModifiers){
    this->panel = Panel(
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

    this->logBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.5f), "", Settings::appTextures().logButtonM, 0., false);
    this->logBtn.color = glm::vec4(0);
    this->logBtn.color2 = glm::vec4(0);
    this->logBtn.outlineColor = glm::vec4(0);
    this->logBtn.outlineColor2 = glm::vec4(0);
    this->logBtn.pos.z = 0.9f;
    
    this->logBtnL = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.8f), "", Settings::appTextures().logButtonL, 0., false);
    this->logBtnL.color = glm::vec4(0);
    this->logBtnL.color2 = glm::vec4(0);
    this->logBtnL.outlineColor = glm::vec4(0);
    this->logBtnL.outlineColor2 = glm::vec4(0);
    this->logBtnL.pos.z = 0.9f;
    
    this->logBtnR = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.8f), "", Settings::appTextures().logButtonR, 0., false);
    this->logBtnR.color = glm::vec4(0);
    this->logBtnR.color2 = glm::vec4(0);
    this->logBtnR.outlineColor = glm::vec4(0);
    this->logBtnR.outlineColor2 = glm::vec4(0);
    this->logBtnR.pos.z = 0.9f;

    this->panel.hasSlider = true;
}

void LogDialog::render(Timer timer){

    rendering(
                this->panel, this->logBtn, this->logBtnR, this->logBtnL, 
                this->pos, this->panelXAxisMixVal, this->panelYAxisMixVal, 
                this->messagesActive, this->actionHistoryActive, this->dialogControl, 
                timer
            );

    if(messagesActive){
        std::vector<std::string> messages;

        std::string a;
        LGDLOG::start >> a;

        for (size_t i = 0; i < a.size(); i++)
        {
            if(i < a.size()-1){
                if(a[i] == '$' && a[i+1] == '#'){
                    std::string res(a.begin(), a.begin() + i );

                    a.erase(a.begin(), a.begin() + i + 2);
                    i = 0;
                    messages.push_back(res);
                }
            }
        }

        std::vector<Section> logSections;
        logSections.push_back(Section(
                                        Element(),
                                        {
                                            
                                        }
                                    )
                            );
        for (size_t i = 0; i < messages.size(); i++)
        {
            logSections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), messages[i], Texture(), 0., false));
            logSections[0].elements[logSections[0].elements.size() - 1].button.color = glm::vec4(0.56f, 0.37f, 0.11f, 1.f);
        }
        
        this->panel.sections = logSections;
            
    }
    else if(actionHistoryActive){
        std::vector<Section> logSections;
        logSections.push_back(Section(
                                        Element(),
                                        {
                                            
                                        }
                                    )
                            );
        for (size_t i = 0; i < __actions.size(); i++)
        {
            logSections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), __actions[i].title, Texture(), 0., false));
            logSections[0].elements[logSections[0].elements.size() - 1].button.color = glm::vec4(0.11f, 0.55f, 0.38f, 1.f);
        }


        if(this->panel.sections[0].elements.size()){
            if(this->panel.sections[0].elements[this->panel.sections[0].elements.size()-1].button.text == "Undo"){
                logSections[0].elements.push_back(this->panel.sections[0].elements[this->panel.sections[0].elements.size()-1].button);
            }
            else
                logSections[0].elements.push_back(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(1), "Undo", Texture(), 0., false));
        }
        else
            logSections[0].elements.push_back(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(1), "Undo", Texture(), 0., false));
        
        this->panel.sections = logSections;
    }
    else if(this->panelYAxisMixVal < 0.2)
        this->panel.sections.clear();

    if(actionHistoryActive && this->panel.sections.size()){
        if(this->panel.sections[0].elements[this->panel.sections[0].elements.size() - 1].button.clicked){
            undo();
        }
    }
}

bool LogDialog::isHovered(){
    return this->panel.hover || this->logBtn.hover || this->logBtnL.hover || this->logBtnR.hover;
}