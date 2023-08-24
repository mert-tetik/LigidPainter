
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
   
struct Action{
    std::string title;
    unsigned int ID;
    Texture icon;
    Texture texture;
    int textureIndex;
    std::vector<Node> nodeScene;

    Action(std::string title, unsigned int ID, Texture icon, Texture texture){
        this->title = title;
        this->ID = ID;
        this->icon = icon;
        this->texture = texture;
    }
    
    Action(std::string title, unsigned int ID, Texture icon, Texture texture, int textureIndex){
        this->title = title;
        this->ID = ID;
        this->icon = icon;
        this->texture = texture;
        this->textureIndex = textureIndex;
    }
    
    Action(std::string title, unsigned int ID, Texture icon, std::vector<Node> nodeScene){
        this->title = title;
        this->ID = ID;
        this->icon = icon;
        this->texture = texture;
        this->textureIndex = textureIndex;
        this->nodeScene = nodeScene;
    }
};

std::vector<Action> __actions;

void registerTextureAction(const std::string title, const Texture icon, Texture texture){
    texture.writeTMP("_history_" + std::to_string(__actions.size()) + "_" + std::to_string(texture.uniqueId));
    
    __actions.push_back(Action(title, TEXTURE_UPDATING_ACTION, icon, texture));
}

void registerTextureDeletionAction(const std::string title, const Texture icon, Texture texture, const int index){
    texture.writeTMP("_history_" + std::to_string(__actions.size()) + "_" + std::to_string(texture.uniqueId));

    __actions.push_back(Action(title, TEXTURE_DELETION_ACTION, icon, texture, index));
}

void registerTextureAdditionAction(const std::string title, const Texture icon, Texture texture, const int index){
    __actions.push_back(Action(title, TEXTURE_ADDITION_ACTION, icon, texture, index));
}

void registerNodeAction(const std::string title, const Texture icon){
    __actions.push_back(Action(title, NODE_ACTION, icon, *NodeScene::getNodeArrayPointer()));
}

LogDialog::LogDialog(){

}

namespace LGDLOG{
    LogMsg start;
    std::string end = "$#";
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

void LogDialog::render(Timer timer, TextRenderer &textRenderer){
    
    if(this->logBtnL.clicked)
        messagesActive = !messagesActive;
    if(this->logBtnR.clicked)
        actionHistoryActive = !actionHistoryActive;
    if(this->logBtn.clicked){
        messagesActive = false;
        actionHistoryActive = false;
    }

    if(this->logBtn.clickState1){ 
        this->pos.x += Mouse::mouseOffset()->x / Settings::videoScale()->x * 100.f;
        this->pos.y += Mouse::mouseOffset()->y / Settings::videoScale()->y * 100.f;
    }

    this->logBtn.pos.x = this->pos.x;
    this->logBtn.pos.y = this->pos.y;
    
    this->logBtnL.pos.x = this->pos.x - this->logBtn.scale.x;
    if(this->messagesActive)
        this->logBtnL.pos.x -= this->panel.scale.x * 2.f;
    this->logBtnL.pos.y = this->pos.y;

    this->logBtnR.pos.x = this->pos.x + this->logBtn.scale.x;
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

        this->panel.render(timer, textRenderer, true);

        if(actionHistoryActive && this->panel.sections.size()){
            if(this->panel.sections[0].elements[this->panel.sections[0].elements.size() - 1].button.clicked){
                if(__actions[__actions.size()-1].ID == TEXTURE_UPDATING_ACTION || __actions[__actions.size()-1].ID == TEXTURE_DELETION_ACTION){
                    for (const auto& entry : std::filesystem::directory_iterator("./tmp")) {
                        if (entry.is_regular_file()) {
                            std::string fileName = entry.path().filename().string();

                            // Check if the file starts with "_history_"
                            if (fileName.find("_history_") == 0) {
                                // Use string stream to split the filename into parts
                                std::istringstream iss(fileName);
                                std::string part;
                                std::getline(iss, part, '_'); // Skip the first part "_history_"
                                std::getline(iss, part, '_'); // Read the first integer value
                                std::getline(iss, part, '_'); // Read the first integer value
                                int indexVal = std::stoi(part);

                                std::getline(iss, part, '_'); // Read the second integer value
                                int IDVal = std::stoi(part);
                                
                                if(indexVal == __actions.size() - 1){
                                    if(__actions[__actions.size()-1].ID == TEXTURE_UPDATING_ACTION){
                                        for (size_t i = 0; i < Library::getTextureArraySize(); i++)
                                        {   
                                            if(Library::getTexture(i)->uniqueId == IDVal){
                                                Library::getTexture(i)->readTMP("_history_" + std::to_string(indexVal) + "_" + std::to_string(IDVal));
                                            }
                                        }
                                    }
                                    else if(__actions[__actions.size()-1].ID == TEXTURE_DELETION_ACTION){
                                        Texture regeneratedTxtr = Texture(nullptr, 1, 1); 
                                        regeneratedTxtr.readTMP("_history_" + std::to_string(indexVal) + "_" + std::to_string(IDVal));
                                        regeneratedTxtr.title = __actions[__actions.size()-1].texture.title;
                                        Library::getTextureVectorPointer()->insert(Library::getTextureVectorPointer()->begin() + __actions[__actions.size()-1].textureIndex, regeneratedTxtr);
                                        Library::setChanged(true);
                                    }
                                }
                            }
                        }
                    }
                }
                else if(__actions[__actions.size()-1].ID == TEXTURE_ADDITION_ACTION){
                    Library::getTextureVectorPointer()->erase(Library::getTextureVectorPointer()->begin() + __actions[__actions.size()-1].textureIndex);
                    Library::setChanged(true);
                }
                else if(__actions[__actions.size()-1].ID == NODE_ACTION){
                    *NodeScene::getNodeArrayPointer() = __actions[__actions.size()-1].nodeScene;
                    //NodeScene::updateNodeResults();
                }
                
                __actions.pop_back();
            }
        }
    }

    
    this->logBtn.render(timer, textRenderer, true);
    this->logBtnL.render(timer, textRenderer, true);
    this->logBtnR.render(timer, textRenderer, true);
}