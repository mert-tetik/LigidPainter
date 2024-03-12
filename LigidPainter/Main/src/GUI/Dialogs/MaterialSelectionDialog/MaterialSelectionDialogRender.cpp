
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
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include <map>

std::map<std::string, std::vector<Material>> matSelection_materials;

void MaterialSelectionDialog::render(Timer timer){
    
    dialogControl.updateStart();

    if(dialogControl.firstFrameActivated || !this->matModePanel.sections[0].elements.size()){
        this->updateMaterialTypes();   
    }
    
    if(dialogControl.firstFrameActivated){
        selectedMatMode = 0;
        selectedMatIndex = -1;
        this->selectedMatPanel.sections[0].elements[1].button.text = "Unselected";
        this->selectedMatPanel.sections[0].elements[0].button.texture.ID = 0;
    }

    //Render the panels
    this->bgPanel.render(timer,true);
    bool anyMatClicked = this->renderMatDisplayer(timer);
    if(anyMatClicked){
        if(!this->selectedMatPanel.sections[0].elements[2].checkBox.clickState1)
            this->selectedMatPanel.sections[0].elements[0].button.texture = this->displayingFBO.colorBuffer;
        else if(selectedMatMode < matModePanel.sections[0].elements.size()){
            if(selectedMatIndex < matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text].size() && selectedMatIndex != -1){
                this->selectedMatPanel.sections[0].elements[0].button.texture = matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text][selectedMatIndex].displayingTexture;
            }
        }
        
        if(selectedMatMode < matModePanel.sections[0].elements.size()){
            if(selectedMatIndex < matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text].size() && selectedMatIndex != -1)
                this->selectedMatPanel.sections[0].elements[1].button.text = matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text][selectedMatIndex].title;
        }
    }

    if(this->material == nullptr)
        this->selectedMatPanel.sections[0].elements[4].scale.y = 0.f;
    else
        this->selectedMatPanel.sections[0].elements[4].scale.y = 2.f;
    
    this->selectedMatPanel.render(timer,true);
    this->matModePanel.render(timer, true);

    if(this->selectedMatPanel.sections[0].elements[3].button.clicked){
        dialog_materialEditor.activate();
        dialog_materialEditor.material = &matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text][selectedMatIndex];
    }
    else if(this->selectedMatPanel.sections[0].elements[4].button.clicked){
        if(this->material != nullptr){
            this->material->deleteBuffers();
            *this->material = matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text][selectedMatIndex].duplicateMaterial();
        }
        this->dialogControl.unActivate();
    }
    
    this->updateSelectedMaterialInPanel();

    if(selectedMatMode < matModePanel.sections[0].elements.size()){
        if(!matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text].size()){
            assignMaterialsToMap();
        }
        if(selectedMatIndex < matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text].size() && selectedMatIndex != -1){
            if(!this->selectedMatPanel.sections[0].elements[2].checkBox.clickState1)
                matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text][selectedMatIndex].updateMaterialDisplayingTexture(512, anyMatClicked, this->displayingCam, 0, true, this->displayingFBO, *getMaterialDisplayerModel(), -1);
        }
    }    

    //Close the dialog
    if(getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!bgPanel.hover && *Mouse::LClick())){
        if(!dialogControl.firstFrameActivated){
            this->dialogControl.unActivate();
        }
    }

    dialogControl.updateEnd(timer, 0.15f);
}