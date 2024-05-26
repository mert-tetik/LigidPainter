/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "UTIL/Threads/Threads.hpp"

#include "GUI/Elements.hpp"
#include "GUI/Dialogs.hpp"

#include "Toolkit/Layers/Layers.hpp"

MaterialLayer::MaterialLayer(const unsigned int resolution){
    this->title = "Material Layer";
    this->layerType = "material";
    this->layerIcon = appTextures.materialIcon;
    this->generate_layer_buffers(resolution);

    this->materialSelectPanel = Panel(
                                    {
                                        Section(
                                            Element(),
                                            {   
                                                Button(ELEMENT_STYLE_SOLID, glm::vec2(10.f, 8.f), "Select Material"  , Texture(), 0.f, false), //0
                                            }
                                        )
                                    }, 
                                    glm::vec2(10.f, 8.f), 
                                    glm::vec3(50.f, 50.f, 0.7), 
                                    ColorPalette::mainColor, 
                                    ColorPalette::thirdColor, 
                                    true, 
                                    true, 
                                    true, 
                                    true, 
                                    true, 
                                    1.f, 
                                    1.f, 
                                    {}, 
                                    20.f, 
                                    true
                                );
}

static DialogControl material_select_dialog_control;

void MaterialLayer::type_specific_modification(Timer& timer, bool doMouseTracking, const unsigned int resolution, Mesh& mesh){
    material_select_dialog_control.activate();

    while (!getContext()->window.shouldClose())
    {
        material_select_dialog_control.updateStart(true);

        materialSelectPanel.sections[0].elements[0].button.texture = this->material.displayingTexture;
        
        if(materialSelectPanel.sections[0].elements[0].button.clicked){
            dialog_materialSelection.show(timer, &this->material); 
        }
        materialSelectPanel.pos = this->layerGUI.layerButton.pos;
        materialSelectPanel.pos.x -= this->layerGUI.layerButton.scale.x + materialSelectPanel.scale.x;
        materialSelectPanel.pos.y -= this->layerGUI.layerButton.scale.y - materialSelectPanel.scale.y;
        materialSelectPanel.render(timer, doMouseTracking);

        if((!materialSelectPanel.hover && *Mouse::LClick()) || getContext()->window.isKeyClicked(LIGIDGL_KEY_ESCAPE))
            material_select_dialog_control.unActivate();
        
        material_select_dialog_control.updateEnd(timer,0.15f);

        /* Quit the layer info dialog*/
        if(material_select_dialog_control.mixVal == 0.f){
            this->update_result_buffers(resolution, mesh);
            this->type_specific_modification_enabled = false;
            break;
        }
    }

}

void MaterialLayer::type_specific_generate_result(const unsigned int resolution, Mesh& mesh){
    //this->material.apply_material(Model(), resMesh, resolution, false);
    material_thread.layer_stuff(&this->material, getScene()->model, &mesh, &this->result, resolution);
}