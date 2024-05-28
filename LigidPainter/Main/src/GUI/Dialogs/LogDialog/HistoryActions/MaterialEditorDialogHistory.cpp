
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

void HistoryActionRecords::undo_material_editor_actions(){
    Material* active_material = dialog_materialEditor.currently_edited_materialPtr; 

    if(active_material == nullptr){
        LGDLOG::start << "ERROR : HistoryActionRecords::undo_material_editor_actions : Material is nullptr" << LGDLOG::end;
        return; 
    }

    if(!this->actions_MaterialEditor[active_material].size()){
        LGDLOG::start << "WARNING : HistoryActionRecords::undo_vector_actions : Material editor is empty" << LGDLOG::end;
        return;
    }
    Material history_material = this->actions_MaterialEditor[active_material][this->actions_MaterialEditor[active_material].size() - 1].material;

    // Assign the changed vector scene 
    *active_material = history_material;
    dialog_materialEditor.updateTheMaterial = true;

    if(dialog_materialEditor.selectedMaterialModifierIndex < history_material.materialModifiers.size())
        dialog_materialEditor.modifiersPanel.sections = history_material.materialModifiers[dialog_materialEditor.selectedMaterialModifierIndex].sections;

    for (size_t i = 0; i < active_material->materialShortcuts.size(); i++)
    {
        active_material->materialShortcuts[i].updateElement(*active_material, active_material->materialShortcuts[i].modI);
    }

    Library::setChanged(true);

    // Remove the undone history
    this->actions_MaterialEditor[active_material].pop_back();
}