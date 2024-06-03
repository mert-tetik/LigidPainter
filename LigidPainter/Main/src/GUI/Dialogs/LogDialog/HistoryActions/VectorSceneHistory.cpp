
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

VectorScene* HistoryActionRecords::get_active_vectorScene(){
    if(GET_VECTOR_SCENE_RENDERING_CONDITION){
        return getVectorScene();
    }
    else{
        Layer* layer = getScene()->get_selected_mesh()->layerScene.get_edited_vector_layer();
    
        if(layer != nullptr){
            VectorScene* vector_scene;
            layer->get_type_specific_variable(nullptr, &vector_scene, nullptr, nullptr, nullptr, nullptr, nullptr);
        
            return vector_scene;
        }
    }

    return nullptr;
}

void HistoryActionRecords::undo_vector_actions(){
    VectorScene* active_vector_scene = this->get_active_vectorScene(); 

    if(active_vector_scene == nullptr){
        LGDLOG::start << "ERROR : HistoryActionRecords::undo_vector_actions : No vector is current" << LGDLOG::end;
        return; 
    }

    if(!this->actions_Vectors[active_vector_scene].size()){
        LGDLOG::start << "WARNING : HistoryActionRecords::undo_vector_actions : Vector history is empty" << LGDLOG::end;
        return;
    }

    // Assign the changed vector scene 
    *active_vector_scene = this->actions_Vectors[active_vector_scene][this->actions_Vectors[active_vector_scene].size() - 1].vectorScene;
    active_vector_scene->update3DVectorBuffers();

    this->actions_Vectors[active_vector_scene].pop_back();
}