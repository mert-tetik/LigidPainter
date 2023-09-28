
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

#ifndef LOGDIALOG_REGISTERING_HPP
#define LOGDIALOG_REGISTERING_HPP

struct BtnAction{
    Button* button;
    Button previousBtn;
    Texture previousBtnTexture;

    BtnAction(){}
    BtnAction(Button* button, Button previousBtn, Texture previousBtnTexture){
        this->button = button; 
        this->previousBtn = previousBtn; 
        this->previousBtnTexture = previousBtnTexture; 
    }
};

struct Action{
    std::string title;
    unsigned int ID;
    Texture icon;
    Texture texture;
    int textureIndex;
    std::vector<Node> nodeScene;
    BtnAction button;

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
        this->nodeScene = nodeScene;
    }

    Action(std::string title, unsigned int ID, Texture icon, BtnAction btnAction){
        this->title = title;
        this->ID = ID;
        this->icon = icon;
        this->button = btnAction;
    }
};

extern std::vector<Action> __actions;

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

void registerButtonAction(const std::string title, const Texture icon, Button* button, Button previousButton){
    Texture previousBtnTexture = previousButton.texture.duplicateTexture();
    previousBtnTexture.proceduralProps = previousButton.texture.proceduralProps;
    previousBtnTexture.title = previousButton.texture.title;

    __actions.push_back(Action(title, BUTTON_ACTION, icon, BtnAction(button, previousButton, previousBtnTexture)));
}

#endif // LOGDIALOG_REGISTERING_HPP