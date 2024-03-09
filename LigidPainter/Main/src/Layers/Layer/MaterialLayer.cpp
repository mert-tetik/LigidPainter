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

#include "GUI/Elements/Elements.hpp"
#include "GUI/Dialogs/Dialogs.hpp"
#include "ContextMenuSystem/ContextMenus.hpp"
#include "Layers/Layers.hpp"

MaterialLayer::MaterialLayer(const unsigned int resolution){
    this->title = "Material Layer";
    this->layerType = "material";
    this->layerIcon = appTextures.materialIcon;
    this->updateLayerButton();
    this->genResultChannels(resolution);
}

static bool __enteredPanelOnce = false;

Panel materialSelectPanel = Panel(
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

void MaterialLayer::render_element_selection_panel(Timer& timer, bool doMouseTracking, MaterialSelectionDialog &materialSelectionDialog, Painter& painter, const unsigned int resolution){
    materialSelectPanel.sections[0].elements[0].button.texture = this->material.displayingTexture;
    
    if(materialSelectPanel.sections[0].elements[0].button.clicked){
        materialSelectionDialog.material = &this->material;
        materialSelectionDialog.dialogControl.activate();
    }
    materialSelectPanel.pos = this->layerButton.pos;
    materialSelectPanel.pos.x -= this->layerButton.scale.x + materialSelectPanel.scale.x;
    materialSelectPanel.pos.y -= this->layerButton.scale.y - materialSelectPanel.scale.y;
    materialSelectPanel.render(timer, doMouseTracking);
    
    if(materialSelectPanel.hover)
        __enteredPanelOnce = true;

    if(materialSelectionDialog.dialogControl.isActive())
        __enteredPanelOnce = false;

    if(*Mouse::LPressed())
        __enteredPanelOnce = false;

    if(*Mouse::LClick() && !__enteredPanelOnce && !materialSelectPanel.hover && !materialSelectionDialog.dialogControl.isActive()){
        this->elementSelectionMode = false;
        __enteredPanelOnce = false;
    }

    if(__enteredPanelOnce && !materialSelectPanel.hover){
        this->elementSelectionMode = false;
        __enteredPanelOnce = false;
    }

    if(!this->elementSelectionMode){
        this->render(painter, resolution);
    }
}

void MaterialLayer::render(Painter& painter, const unsigned int resolution){
    Mesh resMesh = getModel()->meshes[0];
    resMesh.albedo = this->result.albedo;
    resMesh.roughness = this->result.roughness;
    resMesh.metallic = this->result.metallic;
    resMesh.normalMap = this->result.normalMap;
    resMesh.heightMap = this->result.heightMap;
    resMesh.ambientOcclusion = this->result.ambientOcclusion;

    for (int i = this->material.materialModifiers.size() - 1; i >= 0; --i)    
    {
        this->material.materialModifiers[i].updateMaterialChannels(this->material, resMesh, resolution, i, appTextures.white, 0, false, Model());
    }

    painter.getSelectedMesh()->layerScene.update_result(resolution, glm::vec3(0.f));
}