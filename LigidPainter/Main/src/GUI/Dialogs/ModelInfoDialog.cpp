
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 (c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------

    Display settings dialog.
    (Mostly manipulates the skybox )

 */
#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

#include "tinyfiledialogs.h"

ModelInfoDialog::ModelInfoDialog(int){
    //Create the panel
    this->panel = Panel(
        
        {
            {
                Section(
                    Element(Button()),
                    {
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,2),"",  Texture(), 0.f, false)), // Model title
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,2),"",  Texture(), 0.f, false)), // Path to model
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,10),"",  Texture(), 0.f, false)), // Model display texture
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,2),"",  Texture(), 0.f, false)), // Total info
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,2),"Meshes : ",  Texture(), 0.f, false)) // Meshes : text
                    }
                )
            }
        },
        glm::vec2(20.f, 34.f),
        glm::vec3(50.f,50.f,0.8f),
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
        0.25f,
        false
    );

    this->panel.sections[0].elements[0].button.textScale = 0.75f;
}

void ModelInfoDialog::show(Timer& timer, Model* model){
    
    dialogControl.activate();

    while (!getContext()->window.shouldClose())
    {
        dialogControl.updateStart(true);

        if(dialogControl.firstFrameActivated){
            int model_material_size = model->meshes.size();
            int model_total_vertex_array_size = 0;
            int model_total_index_array_size = 0;

            for (size_t i = 0; i < model->meshes.size(); i++)
            {
                model_total_vertex_array_size += model->meshes[i].vertices.size();
                model_total_index_array_size += model->meshes[i].indices.size();
            }
            
            this->panel.sections[0].elements[0].button.text = "Model title : " + model->title;
            this->panel.sections[0].elements[1].button.text = "Path to the model : " + model->path;
            this->panel.sections[0].elements[2].button.texture = model->displayingTxtr;
            this->panel.sections[0].elements[3].button.text = std::string("Material size : ") + std::to_string(model_material_size) + std::string(" , Total vertex array size : ") + std::to_string(model_total_vertex_array_size) + std::string(" , Total index array size : ") + std::to_string(model_total_index_array_size);
            
            if(this->panel.sections.size() > 1)
                this->panel.sections.erase(this->panel.sections.begin() + 1, this->panel.sections.end());

            for (size_t i = 0; i < model->meshes.size(); i++)
            {
                this->panel.sections.push_back(
                                                    Section(
                                                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,0), "",  Texture(), 2.f, true)),
                                                        {
                                                            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,2), model->meshes[i].materialName,  Texture(), 8.f, false)), 
                                                            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,10), "",  model->meshes[i].displayingTxtr, 0.f, false)), 
                                                            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "Object count : " + std::to_string(model->meshes[i].objects.size()),  Texture(), 0.f, false)), 
                                                            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,6), "Texture coordinates Mask",  model->meshes[i].uvMask, 0.f, false)), 
                                                            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "Vertices array size : " + std::to_string(model->meshes[i].vertices.size()),  Texture(), 0.f, false)), 
                                                            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "Indices array size : " + std::to_string(model->meshes[i].indices.size()),  Texture(), 0.f, false)), 
                                                            Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,6), "Material ID Texture",  model->meshes[i].materialIDTxtr, 0.f, false)), 
                                                            Element(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(2,2), "Add new material ID texture",  Texture(), 0.f, false)), 
                                                        }
                                                    )
                                                );

                this->panel.sections[this->panel.sections.size() - 1].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "Layers : ",  Texture(), 0.f, false));
                for (int layerI = model->meshes[i].layerScene.layers.size() - 1; layerI >= 0; layerI--)
                {
                    this->panel.sections[this->panel.sections.size() - 1].elements.push_back(model->meshes[i].layerScene.layers[layerI]->layerGUI.layerButton);
                }
            }
        }
        

        //Render the panel
        panel.render(timer, !dialog_log.isHovered());

        for (size_t i = 1; i < panel.sections.size(); i++)
        {
            if(panel.sections[i].elements[7].button.clicked){
                std::string test = showFileSystemObjectSelectionDialog("Select a texture file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_TEXTURE, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);

                if(test.size()){   
                    model->meshes[i - 1].materialIDTxtr.load(test.c_str());
                    panel.sections[i].elements[6].button.texture = model->meshes[i - 1].materialIDTxtr;
                }
            }

        }
        
        //End the dialog
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!panel.hover && !dialog_log.isHovered() && *Mouse::LClick())){
            if(!dialogControl.firstFrameActivated){
                panel.sections[0].elements[0].button.clickState1 = false;
                dialogControl.unActivate();
            }
        }

        dialogControl.updateEnd(timer,0.15f);
        if(dialogControl.mixVal == 0.f)
            break;
    }
}