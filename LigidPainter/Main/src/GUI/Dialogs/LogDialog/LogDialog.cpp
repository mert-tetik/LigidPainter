
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

#include <string>
#include <iostream>
#include <vector>
   
struct Action{
    std::string title;
    unsigned int ID;
    Texture icon;

    Action(std::string title, unsigned int ID, Texture icon){
        this->title = title;
        this->ID = ID;
        this->icon = icon;
    }
};

std::vector<Action> __actions;

void registerAction(const std::string title, const unsigned int id, const Texture icon){
    __actions.push_back(Action(title, id, icon));
}

LogDialog::LogDialog(){

}

namespace LGDLOG{
    LogMsg start;
    std::string end = "$#";
}

LogDialog::LogDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,AppTextures appTextures, AppMaterialModifiers& appMaterialModifiers){
    this->panel = Panel(
        colorPalette,
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
        colorPalette.mainColor,
        colorPalette.thirdColor,
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

    this->logBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(3.f), colorPalette, "", appTextures.logButtonM, 0., false);
    this->logBtn.color = glm::vec4(0);
    this->logBtn.color2 = glm::vec4(0);
    this->logBtn.outlineColor = glm::vec4(0);
    this->logBtn.outlineColor2 = glm::vec4(0);
    this->logBtn.pos.z = 0.9f;
    
    this->logBtnL = Button(ELEMENT_STYLE_SOLID, glm::vec2(3.f), colorPalette, "", appTextures.logButtonL, 0., false);
    this->logBtnL.color = glm::vec4(0);
    this->logBtnL.color2 = glm::vec4(0);
    this->logBtnL.outlineColor = glm::vec4(0);
    this->logBtnL.outlineColor2 = glm::vec4(0);
    this->logBtnL.pos.z = 0.9f;
    
    this->logBtnR = Button(ELEMENT_STYLE_SOLID, glm::vec2(3.f), colorPalette, "", appTextures.logButtonR, 0., false);
    this->logBtnR.color = glm::vec4(0);
    this->logBtnR.color2 = glm::vec4(0);
    this->logBtnR.outlineColor = glm::vec4(0);
    this->logBtnR.outlineColor2 = glm::vec4(0);
    this->logBtnR.pos.z = 0.9f;

    this->panel.hasSlider = true;
}

void LogDialog::render(LigidWindow originalWindow, ColorPalette colorPalette,Timer timer, TextRenderer &textRenderer, glm::vec2 videoScale){
    
    if(this->logBtn.clicked)
        actionHistoryActive = !actionHistoryActive;

    if(this->logBtn.clickState1){ 
        this->pos.x += Mouse::mouseOffset()->x / videoScale.x * 100.f;
        this->pos.y += Mouse::mouseOffset()->y / videoScale.y * 100.f;
    }

    this->logBtn.pos.x = this->pos.x;
    this->logBtn.pos.y = this->pos.y;
    
    this->logBtnL.pos.x = this->pos.x;
    if(this->messagesActive)
        this->logBtnL.pos.x -= this->panel.scale.x * 2.f;
    this->logBtnL.pos.y = this->pos.y;

    this->logBtnR.pos.x = this->pos.x;
    if(this->actionHistoryActive)
        this->logBtnR.pos.x += this->panel.scale.x * 2.f;
    this->logBtnR.pos.y = this->pos.y;

    timer.transition(messagesActive || actionHistoryActive, this->panelXAxisMixVal, 0.1f);
    timer.transition(messagesActive || actionHistoryActive, this->panelYAxisMixVal, 0.4f);
    
    this->panel.scale.x = this->panelXAxisMixVal * 10.f;
    this->panel.scale.y = this->panelYAxisMixVal * 10.f;
    
    if(messagesActive)
        this->panel.pos.x = this->pos.x - this->panel.scale.x;
    else
        this->panel.pos.x = this->pos.x + this->panel.scale.x;

    this->panel.pos.y = this->pos.y + this->panel.scale.y - this->panelXAxisMixVal * 2.f;

    if(messagesActive || actionHistoryActive || this->panelYAxisMixVal){
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
                logSections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), colorPalette, messages[i], Texture(), 0., false));
                logSections[0].elements[logSections[0].elements.size() - 1].button.color = glm::vec4(0.56f, 0.37f, 0.11f, 1.f);
            }
            
            this->panel.sections = logSections;
        }
        
        if(actionHistoryActive){
            std::vector<Section> logSections;
            logSections.push_back(Section(
                                            Element(),
                                            {
                                                
                                            }
                                        )
                                );
            for (size_t i = 0; i < __actions.size(); i++)
            {
                logSections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), colorPalette, __actions[i].title, Texture(), 0., false));
                logSections[0].elements[logSections[0].elements.size() - 1].button.color = glm::vec4(0.11f, 0.55f, 0.38f, 1.f);
            }
            
            this->panel.sections = logSections;
        }

        this->panel.render(videoScale, timer, textRenderer, true);
    }

    
    this->logBtn.render(videoScale, timer, textRenderer, true);
    this->logBtnL.render(videoScale, timer, textRenderer, true);
    this->logBtnR.render(videoScale, timer, textRenderer, true);
}