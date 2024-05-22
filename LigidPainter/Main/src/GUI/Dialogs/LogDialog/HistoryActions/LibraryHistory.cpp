
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

extern std::vector<Texture> __textures;
extern std::vector<Material> __materials;
extern std::vector<Brush> __brushes;
static const unsigned int brushTextureResolution = 256; 

void HistoryActionRecords::undo_library_actions(){

    if(!this->actions_Library.size()){
        LGDLOG::start << "WARNING : HistoryActionRecords::undo_library_actions : Library history is empty" << LGDLOG::end;
        return;
    }

    // Assign the changed vector scene 
    if(this->actions_Library[this->actions_Library.size() - 1].action_ID == 0){
        __textures = this->actions_Library[this->actions_Library.size() - 1].textures;
    }
    if(this->actions_Library[this->actions_Library.size() - 1].action_ID == 1){
        __materials = this->actions_Library[this->actions_Library.size() - 1].materials;
    }
    if(this->actions_Library[this->actions_Library.size() - 1].action_ID == 2){
        __brushes = this->actions_Library[this->actions_Library.size() - 1].brushes;
        
        for(Brush& brush : __brushes){
            brush.updateDisplayTexture(0.1f);
            brush.properties.brushTexture.generateProceduralDisplayingTexture(brushTextureResolution, false);
        }
    }
    if(this->actions_Library[this->actions_Library.size() - 1].action_ID == 3){
        this->actions_Library[this->actions_Library.size() - 1].texture.readTMP(std::to_string(this->actions_Library[this->actions_Library.size() - 1].unique_id));
    }

    this->actions_Library.pop_back();
}