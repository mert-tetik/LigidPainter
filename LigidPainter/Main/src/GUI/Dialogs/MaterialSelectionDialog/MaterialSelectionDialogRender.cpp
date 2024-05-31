
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
#include "UTIL/Wait/Wait.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include <map>

std::map<std::string, std::vector<Material>> matSelection_materials;

Material* MaterialSelectionDialog::get_selected_material(){
    if(matModePanel.sections.size()){
        if(selectedMatMode >= 0 && selectedMatMode < matModePanel.sections[0].elements.size()){
            if(selectedMatIndex >= 0 && selectedMatIndex < matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text].size()){
                return &matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text][selectedMatIndex];
            }
        }
    }

    return nullptr;
}

void MaterialSelectionDialog::show(Timer& timer, Material* material){
    
    dialogControl.activate();
    
    while (!getContext()->window.shouldClose())
    {
        dialogControl.updateStart(true);

        this->displayingCam.interaction(*Mouse::mouseScroll(), *Mouse::mouseOffset(), true);

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
        this->bgPanel.render(timer, !dialog_log.isHovered());
        bool anyMatClicked = this->renderMatDisplayer(timer);
        if(anyMatClicked){
            if(!this->selectedMatPanel.sections[0].elements[2].checkBox.clickState1)
                this->selectedMatPanel.sections[0].elements[0].button.texture = this->displayingTexture;
            else{
                Material* selected_material = this->get_selected_material();
                if(selected_material != nullptr){
                    this->selectedMatPanel.sections[0].elements[0].button.texture = selected_material->displayingTexture;
                }
            }
            
            if(selectedMatMode < matModePanel.sections[0].elements.size()){
                
                Material* selected_material = this->get_selected_material();
                if(selected_material != nullptr){
                    this->selectedMatPanel.sections[0].elements[1].button.text = selected_material->title;
                }
            }
        }

        if(material == nullptr)
            this->selectedMatPanel.sections[0].elements[4].scale.y = 0.f;
        else
            this->selectedMatPanel.sections[0].elements[4].scale.y = 2.f;
        
        this->selectedMatPanel.render(timer, !dialog_log.isHovered());
        this->matModePanel.render(timer, !dialog_log.isHovered());
        
        if(material_thread.active){
            Material* selected_material = this->get_selected_material();
            if(selected_material != nullptr){
                if(((material_thread.actions.size()) ? material_thread.actions[0].material : nullptr) == selected_material){
                    Element* btn = &this->selectedMatPanel.sections[0].elements[0];
                    appVideos.loading.render(timer, btn->pos, glm::vec2(btn->scale.y / (Settings::videoScale()->x / Settings::videoScale()->y), btn->scale.y) / 1.2f, 1.f, 1, true);
                    btn->button.textureSizeScale = 100000000.f;
                }
            }

        }
        else{
            Element* btn = &this->selectedMatPanel.sections[0].elements[0];
            btn->button.textureSizeScale = 1.f;
        }

        if(this->selectedMatPanel.sections[0].elements[3].button.clicked){

            Material* selected_material = this->get_selected_material();
            WAIT_WHILE(material_thread.actions.size());

            if(selected_material != nullptr){
                if(selected_material->material_selection_dialog_initialized)
                    dialog_materialEditor.show(timer, selected_material);
            }
        }
        else if(this->selectedMatPanel.sections[0].elements[4].button.clicked){
            if(material != nullptr){
                
                Material* selected_material = this->get_selected_material();
                WAIT_WHILE(material_thread.actions.size());

                material->deleteBuffers();

                if(selected_material != nullptr){
                    if(selected_material->material_selection_dialog_initialized)
                        *material = selected_material->duplicateMaterial();
                }
            }
            this->dialogControl.unActivate();
        }
        
        this->updateSelectedMaterialInPanel();

        if(selectedMatMode < matModePanel.sections[0].elements.size()){
            if(!matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text].size()){
                assignMaterialsToMap();
            }
            Material* selected_material = this->get_selected_material();
            if(selected_material != nullptr){
                if(!this->selectedMatPanel.sections[0].elements[2].checkBox.clickState1){
                    if(anyMatClicked){
                        material_thread.update_material_displaying_texture(selected_material, getMaterialDisplayerModel(), &getMaterialDisplayerModel()->meshes[0], &getMaterialDisplayerModel()->meshes[0].material_channels, 2048);
                    }
                    else{
                        selected_material->updateMaterialDisplayingTexture(512, false, this->displayingCam, 0, true, getMaterialDisplayerModel()->meshes[0], getMaterialDisplayerModel()->meshes[0].material_channels, this->displayingTexture, -1);
                    }
                }
            }
        }    

        for (Material& material : matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text])
        {
            if(!material.material_selection_dialog_initialized){
                if(!material_thread.active && !material_thread.readyToService){
                    material.material_selection_dialog_initialized = true;
                    material_thread.read_material_file(&material, getMaterialDisplayingModel(), &getMaterialDisplayingModel()->meshes[0], &getMaterialDisplayingModel()->meshes[0].material_channels, material.material_selection_dialog_path, 512);
                }
            }
        }

        bool update_frame_value = true;
        int matI = 0;
        for (Element element : this->matDisplayerPanel.sections[0].elements)
        {
            if(matI < matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text].size()){
                if(material_thread.actions.size()){
                    if(
                            !matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text][matI].material_selection_dialog_initialized ||
                            (((material_thread.actions.size()) ? material_thread.actions[0].material : nullptr) == &matSelection_materials[matModePanel.sections[0].elements[selectedMatMode].button.text][matI] && 
                            material_thread.active)
                            )
                    {
                        appVideos.loading.render(timer, element.pos, glm::vec2(element.scale.y / (Settings::videoScale()->x / Settings::videoScale()->y), element.scale.y) / 1.2f, 1.f, 1, update_frame_value);
                        update_frame_value = false;
                    }
                }
            }
        
            matI++;
        }
        

        //Close the dialog
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!bgPanel.hover && !dialog_log.isHovered() && *Mouse::LClick())){
            if(!dialogControl.firstFrameActivated){
                this->dialogControl.unActivate();
            }
        }

        dialogControl.updateEnd(timer, 0.15f);
        if(dialogControl.mixVal == 0.f)
            break;

    }   
}