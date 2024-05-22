
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

#include "UTIL/Library/Library.hpp" 
#include "UTIL/Settings/Settings.hpp"

#include "./HistoryActions.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

TextureFieldScene* HistoryActionRecords::get_active_textureFieldScene(){
    if(GET_TEXTURE_FIELD_SCENE_RENDERING_CONDITION){
        return getTextureFieldScene();
    }
    return nullptr;
}

void HistoryActionRecords::undo_texture_field_actions(){
    TextureFieldScene* active_texture_field_scene = this->get_active_textureFieldScene(); 

    if(active_texture_field_scene == nullptr){
        LGDLOG::start << "ERROR : HistoryActionRecords::undo_vector_actions : No vector is current" << LGDLOG::end;
        return; 
    }

    if(!this->actions_TextureFields[active_texture_field_scene].size()){
        LGDLOG::start << "WARNING : HistoryActionRecords::undo_vector_actions : Vector history is empty" << LGDLOG::end;
        return;
    }
    
    for (size_t i = 0; i < active_texture_field_scene->texture_fields.size(); i++)
    {
        if(!active_texture_field_scene->texture_fields[i].wrapMode){
            if(i < this->actions_TextureFields[active_texture_field_scene][this->actions_TextureFields[active_texture_field_scene].size() - 1].texture_field_scene.texture_fields.size()){
                active_texture_field_scene->texture_fields[i].texture.flipTexture(
                                                                                    active_texture_field_scene->texture_fields[i].flippedH != this->actions_TextureFields[active_texture_field_scene][this->actions_TextureFields[active_texture_field_scene].size() - 1].texture_field_scene.texture_fields[i].flippedH,
                                                                                    active_texture_field_scene->texture_fields[i].flippedV != this->actions_TextureFields[active_texture_field_scene][this->actions_TextureFields[active_texture_field_scene].size() - 1].texture_field_scene.texture_fields[i].flippedV
                                                                                );
            }
        }
    }

    // Assign the changed vector scene 
    *active_texture_field_scene = this->actions_TextureFields[active_texture_field_scene][this->actions_TextureFields[active_texture_field_scene].size() - 1].texture_field_scene;

    for (auto field : active_texture_field_scene->texture_fields)
    {
        if(field.wrapMode)
            field.updateWrapBox();
    }
    
    active_texture_field_scene->update_painting_over_texture(false);

    this->actions_TextureFields[active_texture_field_scene].pop_back();
}