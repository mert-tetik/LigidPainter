
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

void LogDialog::update_messages_array(Timer& timer){
    std::string a;
    LGDLOG::start >> a;
    LGDLOG::start.clear();
    for (size_t i = 0; i < a.size(); i++)
    {
        if(i < a.size()-1){
            if(a[i] == '$' && a[i+1] == '#'){
                std::string res(a.begin(), a.begin() + i );

                a.erase(a.begin(), a.begin() + i + 2);
                i = 0;
                const int notification_duration = 5;
                this->make_cat_talk(timer, res, notification_duration);
                this->messages.push_back(res);
            }
        }
    }
}

void LogDialog::update_active_history_mode(){
    if(panel_displaying_modes.selectedElement != 0){
        this->activeHistoryMode = HISTORY_PAINTING_MODE;
    }
    if(panel_painting_modes.selectedElement == 5){
        this->activeHistoryMode = HISTORY_VECTORS_MODE;
    }
    if(getScene()->get_selected_mesh()->face_selection_data.editMode){
        this->activeHistoryMode = HISTORY_FACESELECTION_MODE;
    }
    if(checkComboList_painting_over.panel.sections[0].elements[1].checkBox.clickState1){
        this->activeHistoryMode = HISTORY_TEXTUREFIELDS_MODE;
    }
    if(dialog_materialEditor.dialogControl.isActive()){
        this->activeHistoryMode = HISTORY_MATERIALEDITOR_MODE; 
    }
}

bool LogDialog::isHovered(){
    return  this->menu_bar.hover || 
            this->messagesPanel.hover || 
            this->historyPanel.hover || 
            this->libraryHistoryPanel.hover || 
            this->multiThreadInfoPanel.hover || 
            this->logBtn.hover;
}