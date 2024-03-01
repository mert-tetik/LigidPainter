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

static bool __enteredPanelOnce = false;
Panel alphaSettingsPanel = Panel(
                                    {
                                        Section(
                                            Element(),
                                            {   
                                                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,2), "General Opacity"  , Texture(), 1.f, 0.f, 1.f, 1.f), //0
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "General Opacity Map"  , Texture(), 0.f, false), //1
                                                
                                                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,2), "Albedo Opacity"  , Texture(), 4.f, 0.f, 1.f, 1.f), //2
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "Albedo Opacity Map"  , Texture(), 0.f, false), //3
                                                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,2), "Roughness Opacity"  , Texture(), 1.f, 0.f, 1.f, 1.f), //4
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "Roughness Opacity Map"  , Texture(), 0.f, false), //5
                                                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,2), "Metallic Opacity"  , Texture(), 1.f, 0.f, 1.f, 1.f), //6
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "Metallic Opacity Map"  , Texture(), 0.f, false), //7
                                                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,2), "Normal Map Opacity"  , Texture(), 1.f, 0.f, 1.f, 1.f), //8
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "Normal Map Opacity Map"  , Texture(), 0.f, false), //9
                                                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,2), "Height Map Opacity"  , Texture(), 1.f, 0.f, 1.f, 1.f), //10
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "Height Map Opacity Map"  , Texture(), 0.f, false), //11
                                                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,2), "Ambient Occlusion Opacity"  , Texture(), 1.f, 0.f, 1.f, 1.f), //12
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2), "Ambient Occlusion Opacity Map"  , Texture(), 0.f, false), //13
                                            }
                                        )
                                    }, 
                                    glm::vec2(15.f, 20.f), 
                                    glm::vec3(50.f, 50.f, 0.7), 
                                    ColorPalette::secondColor, 
                                    ColorPalette::thirdColor, 
                                    true, 
                                    true, 
                                    true, 
                                    true, 
                                    true, 
                                    1.f, 
                                    2, 
                                    {}, 
                                    20.f, 
                                    true
                                );

void Layer::renderAlphaSettingsPanel(Timer& timer, bool doMouseTracking){
    

    alphaSettingsPanel.sections[0].elements[0].rangeBar.value = this->alpha.general_Alpha.alphaValue;
    alphaSettingsPanel.sections[0].elements[1].button.texture = this->alpha.general_Alpha.alphaMap;
    alphaSettingsPanel.sections[0].elements[2].rangeBar.value = this->alpha.albedo_Alpha.alphaValue;
    alphaSettingsPanel.sections[0].elements[3].button.texture = this->alpha.albedo_Alpha.alphaMap;
    alphaSettingsPanel.sections[0].elements[4].rangeBar.value = this->alpha.roughness_Alpha.alphaValue;
    alphaSettingsPanel.sections[0].elements[5].button.texture = this->alpha.roughness_Alpha.alphaMap;
    alphaSettingsPanel.sections[0].elements[6].rangeBar.value = this->alpha.metallic_Alpha.alphaValue;
    alphaSettingsPanel.sections[0].elements[7].button.texture = this->alpha.metallic_Alpha.alphaMap;
    alphaSettingsPanel.sections[0].elements[8].rangeBar.value = this->alpha.normalMap_Alpha.alphaValue;
    alphaSettingsPanel.sections[0].elements[9].button.texture = this->alpha.normalMap_Alpha.alphaMap;
    alphaSettingsPanel.sections[0].elements[10].rangeBar.value = this->alpha.heightMap_Alpha.alphaValue;
    alphaSettingsPanel.sections[0].elements[11].button.texture = this->alpha.heightMap_Alpha.alphaMap;
    alphaSettingsPanel.sections[0].elements[12].rangeBar.value = this->alpha.ambientOcclusion_Alpha.alphaValue;
    alphaSettingsPanel.sections[0].elements[13].button.texture = this->alpha.ambientOcclusion_Alpha.alphaMap;

    alphaSettingsPanel.pos = this->layerButton.pos;
    alphaSettingsPanel.pos.x -= this->layerButton.scale.x + alphaSettingsPanel.scale.x;
    alphaSettingsPanel.pos.y -= this->layerButton.scale.y - alphaSettingsPanel.scale.y;
    alphaSettingsPanel.render(timer, doMouseTracking);

    if(alphaSettingsPanel.hover)
        __enteredPanelOnce = true;

    if(*Mouse::LPressed())
        __enteredPanelOnce = false;

    if(*Mouse::LClick() && !__enteredPanelOnce && !alphaSettingsPanel.hover){
        this->alphaSettingsMode = false;
        __enteredPanelOnce = false;
    }

    if(__enteredPanelOnce && !alphaSettingsPanel.hover){
        this->alphaSettingsMode = false;
        __enteredPanelOnce = false;
    }
    
    this->alpha.general_Alpha.alphaValue = alphaSettingsPanel.sections[0].elements[0].rangeBar.value;
    this->alpha.general_Alpha.alphaMap = alphaSettingsPanel.sections[0].elements[1].button.texture;
    this->alpha.albedo_Alpha.alphaValue = alphaSettingsPanel.sections[0].elements[2].rangeBar.value;
    this->alpha.albedo_Alpha.alphaMap = alphaSettingsPanel.sections[0].elements[3].button.texture;
    this->alpha.roughness_Alpha.alphaValue = alphaSettingsPanel.sections[0].elements[4].rangeBar.value;
    this->alpha.roughness_Alpha.alphaMap = alphaSettingsPanel.sections[0].elements[5].button.texture;
    this->alpha.metallic_Alpha.alphaValue = alphaSettingsPanel.sections[0].elements[6].rangeBar.value;
    this->alpha.metallic_Alpha.alphaMap = alphaSettingsPanel.sections[0].elements[7].button.texture;
    this->alpha.normalMap_Alpha.alphaValue = alphaSettingsPanel.sections[0].elements[8].rangeBar.value;
    this->alpha.normalMap_Alpha.alphaMap = alphaSettingsPanel.sections[0].elements[9].button.texture;
    this->alpha.heightMap_Alpha.alphaValue = alphaSettingsPanel.sections[0].elements[10].rangeBar.value;
    this->alpha.heightMap_Alpha.alphaMap = alphaSettingsPanel.sections[0].elements[11].button.texture;
    this->alpha.ambientOcclusion_Alpha.alphaValue = alphaSettingsPanel.sections[0].elements[12].rangeBar.value;
    this->alpha.ambientOcclusion_Alpha.alphaMap = alphaSettingsPanel.sections[0].elements[13].button.texture;
}
