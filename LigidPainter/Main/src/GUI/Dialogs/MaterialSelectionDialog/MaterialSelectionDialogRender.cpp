
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
#include "UTIL/Threads/Threads.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include <map>

std::map<std::string, std::vector<Material>> matSelection_materials;

void MaterialSelectionDialog::show(Timer& timer, Material* material){
    
    dialogControl.activate();
    
    while (!getContext()->window.shouldClose())
    {
        dialogControl.updateStart();

        this->displayingCam.interaction(*Mouse::mouseScroll(), *Mouse::mouseOffset());

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
                this->selectedMatPanel.sections[0].elements[0].button.texture = this->displayingTexture;
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

        if(material == nullptr)
            this->selectedMatPanel.sections[0].elements[4].scale.y = 0.f;
        else
            this->selectedMatPanel.sections[0].elements[4].scale.y = 2.f;
        
        this->selectedMatPanel.render(timer,true);
        this->matModePanel.render(timer, true);

        if(this->selectedMatPanel.sections[0].elements[3].button.clicked){
            dialog_materialEditor.show(timer, &matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text][selectedMatIndex]);
        }
        else if(this->selectedMatPanel.sections[0].elements[4].button.clicked){
            if(material != nullptr){
                material->deleteBuffers();
                *material = matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text][selectedMatIndex].duplicateMaterial();
            }
            this->dialogControl.unActivate();
        }
        
        this->updateSelectedMaterialInPanel();

        if(selectedMatMode < matModePanel.sections[0].elements.size()){
            if(!matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text].size()){
                assignMaterialsToMap();
            }
            if(selectedMatIndex < matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text].size() && selectedMatIndex != -1){
                if(!this->selectedMatPanel.sections[0].elements[2].checkBox.clickState1){
                    if(anyMatClicked){
                        material_thread.update_material_displaying_texture(&matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text][selectedMatIndex], getMaterialDisplayerModel(), &getMaterialDisplayerModel()->meshes[0], &getMaterialDisplayerModel()->meshes[0].material_channels);
                    }
                    else{
                        matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text][selectedMatIndex].updateMaterialDisplayingTexture(512, false, this->displayingCam, 0, true, this->displayingTexture, *getMaterialDisplayerModel(), -1);
                    }
                }
            }
        }    

        for (Material& material : matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text])
        {
            if(!material.material_selection_dialog_initialized){
                if(!material_thread.active && !material_thread.readyToService){
                    material.material_selection_dialog_initialized = true;
                }
                material_thread.read_material_file(&material, getMaterialDisplayingModel(), &getMaterialDisplayingModel()->meshes[0], &getMaterialDisplayingModel()->meshes[0].material_channels, material.material_selection_dialog_path);
            }
        }

        bool update_frame_value = true;
        int matI = 0;
        for (Element element : this->matDisplayerPanel.sections[0].elements)
        {
            if(matI < matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text].size()){
                if(!matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text][matI].material_selection_dialog_initialized){
                    appVideos.loading.render(timer, element.pos, element.scale, 1.f, 1, update_frame_value);
                    update_frame_value = false;
                }
            }
        
            matI++;
        }
        

        //Close the dialog
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!bgPanel.hover && *Mouse::LClick())){
            if(!dialogControl.firstFrameActivated){
                this->dialogControl.unActivate();
            }
        }

        dialogControl.updateEnd(timer, 0.15f);
        if(dialogControl.mixVal == 0.f)
            break;

    }   
}