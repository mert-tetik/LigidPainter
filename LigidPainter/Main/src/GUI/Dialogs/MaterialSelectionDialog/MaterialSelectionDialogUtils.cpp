
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 (c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
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
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/Library/Library.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include <map>

extern std::map<std::string, std::vector<Material>> matSelection_materials;

void MaterialSelectionDialog::updateMaterialTypes(){
    std::vector<std::string> titles;
    try
    {
        for (const auto& entry : std::filesystem::directory_iterator("./LigidPainter/Resources/Materials")) {
            std::string title = "";
            title = UTIL::getLastWordBySeparatingWithChar(entry.path().string(), UTIL::folderDistinguisher());
            title = UTIL::removeLastWordBySeparatingWithChar(title, '_');
            
            bool contains = false;
            for (size_t i = 0; i < titles.size(); i++)
            {
                if(titles[i] == title)
                    contains = true;
            }

            if(!contains)
                titles.push_back(title);
        }            
    }
    catch (const std::filesystem::filesystem_error& ex) {
        LGDLOG::start << "ERROR : Filesystem : Updating the material modes panel" << ex.what() << LGDLOG::end;
    }

    titles.push_back("Custom");

    this->matModePanel.sections[0].elements.clear();

    for (size_t i = 0; i < titles.size(); i++)
    {
        this->matModePanel.sections[0].elements.push_back(Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), titles[i], Texture(), 0.f, true)));
    }

    matSelection_materials["Custom"].clear();
    for (size_t i = 0; i < Library::getMaterialArraySize(); i++)
    {
        matSelection_materials["Custom"].push_back(Library::getMaterialObj(i));
    }
    
}

bool MaterialSelectionDialog::renderMatDisplayer(Timer& timer){
    matDisplayerPanel.sections[0].elements.clear();
    for (size_t i = 0; i < matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text].size(); i++)
    {
        matDisplayerPanel.sections[0].elements.push_back(Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(6.f), "", matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text][i].displayingTexture, 0.f, true)));
    }
    for (size_t i = 0; i < matDisplayerPanel.sections[0].elements.size(); i++)
    {
        if(i == selectedMatIndex){
            matDisplayerPanel.sections[0].elements[i].button.clickState1 = true;
            matDisplayerPanel.sections[0].elements[i].button.clickedMixVal = 1.f;
        }
        else{
            matDisplayerPanel.sections[0].elements[i].button.clickState1 = false;
            matDisplayerPanel.sections[0].elements[i].button.clickedMixVal = 0.f;
        }
    }

    bool hasElements = matDisplayerPanel.sections[0].elements.size();

    if(!matDisplayerPanel.sections[0].elements.size()){
        matDisplayerPanel.rowCount = 1;
        matDisplayerPanel.sections[0].elements.push_back(Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(matDisplayerPanel.scale.y), "No material detected! Please check the other material sections.", Texture(), 0.f, true)));
    }
    else
        matDisplayerPanel.rowCount = 5;


    bool anyPressed = false;

    this->matDisplayerPanel.render(timer, hasElements);
    for (size_t i = 0; i < matDisplayerPanel.sections[0].elements.size(); i++)
    {
        if(matDisplayerPanel.sections[0].elements[i].button.hover){
            if(*Mouse::LClick()){
                selectedMatIndex = i;
                anyPressed = true;
            }
            
            matDisplayerPanel.sections[0].elements[i].button.hoverMixVal = 1.f;
        }
    }

    return anyPressed;
}

void MaterialSelectionDialog::assignMaterialsToMap(){
    try
    {
        for (const auto& entry : std::filesystem::directory_iterator("./LigidPainter/Resources/Materials")) {
            std::string title = "";
            title = UTIL::getLastWordBySeparatingWithChar(entry.path().string(), UTIL::folderDistinguisher());
            title = UTIL::removeLastWordBySeparatingWithChar(title, '_');

            if(title == matModePanel.sections[0].elements[selectedMatMode].button.text){
                Material mat = Material(UTIL::getLastWordBySeparatingWithChar(entry.path().string(), UTIL::folderDistinguisher()), {});
                mat.material_selection_dialog_path = entry.path().string(); 

                matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text].push_back(mat);
            }    
        }            
    }
    catch (const std::filesystem::filesystem_error& ex) {
        LGDLOG::start << "ERROR : Filesystem : Updating the material modes panel" << ex.what() << LGDLOG::end;
    }
}

void MaterialSelectionDialog::updateSelectedMaterialInPanel(){
    for (size_t i = 0; i < matModePanel.sections[0].elements.size(); i++)
    {
        if(!matModePanel.sections[0].elements[i].button.clickState1 && i == selectedMatMode)
            matModePanel.sections[0].elements[i].button.clickState1 = true;
    }
    for (size_t i = 0; i < matModePanel.sections[0].elements.size(); i++)
    {
        if(matModePanel.sections[0].elements[i].button.clickState1 && i != selectedMatMode){
            selectedMatMode = i;
            selectedMatIndex = -1;
            this->selectedMatPanel.sections[0].elements[1].button.text = "Unselected";
            this->selectedMatPanel.sections[0].elements[0].button.texture.ID = 0;
            break;
        }
    }
    for (size_t i = 0; i < matModePanel.sections[0].elements.size(); i++)
    {
        if(matModePanel.sections[0].elements[i].button.clickState1 && i != selectedMatMode)
            matModePanel.sections[0].elements[i].button.clickState1 = false;
    }
}