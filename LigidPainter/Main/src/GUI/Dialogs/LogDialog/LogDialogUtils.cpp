
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
                if(!getContext()->window.shouldClose())
                    this->make_cat_talk(timer, res, notification_duration);
                this->messages.push_back(res);
            }
        }
    }
}

int LogDialog::get_active_history_mode(){
    
    
    if(this->history_action_records.get_active_vectorScene() != nullptr){
        return HISTORY_VECTORS_MODE;
    }
    
    if(this->history_action_records.get_active_face_selected_mesh() != nullptr){
        return HISTORY_FACESELECTION_MODE;
    }

    if(panel_displaying_modes.selectedElement == 1){
        return HISTORY_MULTI_CHANNEL_PAINTING_MODE;
    }

    if(panel_displaying_modes.selectedElement == 2){
        return HISTORY_PAINTING_MODE;
    }

    if(checkComboList_painting_over.panel.sections[0].elements[1].checkBox.clickState1){
        return HISTORY_TEXTUREFIELDS_MODE;
    }

    if(dialog_materialEditor.dialogControl.isActive()){
        return HISTORY_MATERIALEDITOR_MODE; 
    }

    return -1;
}

bool LogDialog::isHovered(){
    return  this->menu_bar.hover || 
            this->messagesPanel.hover || 
            this->historyPanel.hover || 
            this->libraryHistoryPanel.hover || 
            this->multiThreadInfoPanel.hover || 
            this->logBtn.hover;
}

void LogDialog::undo_general_history(){
    int active_history_mode = this->get_active_history_mode();

    if(active_history_mode == -1)
        return;

    if(active_history_mode == HISTORY_PAINTING_MODE){
        this->history_action_records.undo_painting_actions();   
    }
    if(active_history_mode == HISTORY_MULTI_CHANNEL_PAINTING_MODE){
        this->history_action_records.undo_multi_channel_painting_actions();   
    }
    if(active_history_mode == HISTORY_VECTORS_MODE){
        this->history_action_records.undo_vector_actions();   
    }
    if(active_history_mode == HISTORY_FACESELECTION_MODE){
        this->history_action_records.undo_face_selection_actions();   
    }
    if(active_history_mode == HISTORY_TEXTUREFIELDS_MODE){
        //this->history_action_records.undo_texture_field_actions();   
    }
    if(active_history_mode == HISTORY_MATERIALEDITOR_MODE){
        //this->history_action_records.undo_material_editor_actions();   
    }
}