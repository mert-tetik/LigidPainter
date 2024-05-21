
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
#include "UTIL/Texture/Texture.hpp"

#include "./HistoryActions.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

static size_t ID_val = 3975392845189;
static size_t generate_unique_ID(){
    ID_val++;
    return ID_val;
}

PaintingAction::PaintingAction(std::string title, Texture painted_texture)
{
    this->title = title;
    this->painted_texture = painted_texture;

    this->unique_ID = generate_unique_ID();
}

void HistoryActionRecords::undo_painting_actions(){
    unsigned int texture_ID = panel_library_selected_texture.ID; 

    if(texture_ID == 0 && glIsTexture(texture_ID) == GL_TRUE){
        LGDLOG::start << "ERROR : HistoryActionRecords::undo_painting_actions : Selected texture is not valid" << LGDLOG::end;
        return; 
    }

    if(!this->actions_Painting[texture_ID].size()){
        LGDLOG::start << "WARNING : HistoryActionRecords::undo_painting_actions : Texture history is empty" << LGDLOG::end;
        return;
    }

    panel_library_selected_texture.readTMP(std::to_string(this->actions_Painting[texture_ID][this->actions_Painting[texture_ID].size() - 1].unique_ID));

    this->actions_Painting[texture_ID].pop_back();
}