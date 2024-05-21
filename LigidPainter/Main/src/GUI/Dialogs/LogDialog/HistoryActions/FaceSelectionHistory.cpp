
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

Mesh* HistoryActionRecords::get_active_face_selected_mesh(){
    for(Mesh& mesh : getScene()->model->meshes){
        if(mesh.face_selection_data.editMode)
            return &mesh;
    }
    
    return nullptr;
}

void HistoryActionRecords::undo_face_selection_actions(){
    Mesh* active_face_selected_mesh = this->get_active_face_selected_mesh(); 

    if(active_face_selected_mesh == nullptr){
        LGDLOG::start << "ERROR : HistoryActionRecords::undo_face_selection_actions : No mesh is in face selection mode" << LGDLOG::end;
        return; 
    }

    if(!this->actions_FaceSelection[active_face_selected_mesh].size()){
        LGDLOG::start << "WARNING : HistoryActionRecords::undo_face_selection_actions : Face selection history is empty" << LGDLOG::end;
        return;
    }

    // Assign the changed vector scene 
    Texture mesh_mask = active_face_selected_mesh->face_selection_data.meshMask; 
    Texture selectedFaces = active_face_selected_mesh->face_selection_data.selectedFaces;

    active_face_selected_mesh->face_selection_data = this->actions_FaceSelection[active_face_selected_mesh][this->actions_FaceSelection[active_face_selected_mesh].size() - 1].face_selection_data;
    active_face_selected_mesh->update_face_selection_buffers(true);

    active_face_selected_mesh->face_selection_data.meshMask = mesh_mask; 
    active_face_selected_mesh->face_selection_data.selectedFaces = selectedFaces;

    // Remove undone history action
    actions_FaceSelection[active_face_selected_mesh].pop_back();
}