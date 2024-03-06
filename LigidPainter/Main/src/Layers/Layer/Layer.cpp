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
    alphaSettingsPanel.sections[0].elements[1].button.textureSelection3D = true;
    alphaSettingsPanel.sections[0].elements[3].button.textureSelection3D = true;
    alphaSettingsPanel.sections[0].elements[5].button.textureSelection3D = true;
    alphaSettingsPanel.sections[0].elements[7].button.textureSelection3D = true;
    alphaSettingsPanel.sections[0].elements[9].button.textureSelection3D = true;
    alphaSettingsPanel.sections[0].elements[11].button.textureSelection3D = true;
    alphaSettingsPanel.sections[0].elements[13].button.textureSelection3D = true;


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

Panel infoPanel = Panel(
                                    {
                                        Section(
                                            Element(),
                                            {   
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1), "Title : "  , Texture(), 0.f, false), //0
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1), "Type : "  , Texture(), 0.f, false), //1
                                                
                                                Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "ALBEDO"  , Texture(), 2.f, false), //2
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,7), ""  , Texture(), 0.f, false), //3

                                                Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "ROUGHNESS"  , Texture(), 1.f, false), //4
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,7), ""  , Texture(), 0.f, false), //5

                                                Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "METALLIC"  , Texture(), 1.f, false), //6
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,7), ""  , Texture(), 0.f, false), //7

                                                Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "NORMAL MAP"  , Texture(), 1.f, false), //8
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,7), ""  , Texture(), 0.f, false), //9

                                                Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "HEIGHT MAP"  , Texture(), 1.f, false), //10
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,7), ""  , Texture(), 0.f, false), //11

                                                Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1), "AMBIENT OCCLUSION"  , Texture(), 1.f, false), //12
                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,7), ""  , Texture(), 0.f, false), //13
                                            }
                                        )
                                    }, 
                                    glm::vec2(15.f, 20.f), 
                                    glm::vec3(50.f, 50.f, 0.7), 
                                    ColorPalette::mainColor, 
                                    ColorPalette::thirdColor, 
                                    true, 
                                    true, 
                                    true, 
                                    true, 
                                    true, 
                                    1.f, 
                                    1, 
                                    {}, 
                                    20.f, 
                                    true
                                );

void Layer::renderInfoPanel(Timer& timer, bool doMouseTracking){
    infoPanel.sections[0].elements[0].button.text = "Title : " + this->title;
    infoPanel.sections[0].elements[1].button.text = "Type : " + this->layerType;
    infoPanel.sections[0].elements[1].button.texture = this->layerIcon;
    
    infoPanel.sections[0].elements[3].button.texture = this->result.albedo;
    infoPanel.sections[0].elements[5].button.texture = this->result.roughness;
    infoPanel.sections[0].elements[7].button.texture = this->result.metallic;
    infoPanel.sections[0].elements[9].button.texture = this->result.normalMap;
    infoPanel.sections[0].elements[11].button.texture = this->result.heightMap;
    infoPanel.sections[0].elements[13].button.texture = this->result.ambientOcclusion;
    
    infoPanel.pos = this->layerButton.pos;
    infoPanel.pos.x -= this->layerButton.scale.x + infoPanel.scale.x;
    infoPanel.pos.y -= this->layerButton.scale.y - infoPanel.scale.y;
    infoPanel.render(timer, doMouseTracking);

    if(infoPanel.hover)
        __enteredPanelOnce = true;

    if(*Mouse::LPressed())
        __enteredPanelOnce = false;

    if(*Mouse::LClick() && !__enteredPanelOnce && !infoPanel.hover){
        this->infoMode = false;
        __enteredPanelOnce = false;
    }

    if(__enteredPanelOnce && !infoPanel.hover){
        this->infoMode = false;
        __enteredPanelOnce = false;
    }
}

int Layer::render_graphics(Timer& timer, bool doMosueTracking, glm::vec3 pos, glm::vec2 scale, float opacity, MaterialSelectionDialog &materialSelectionDialog){
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", opacity);
    layerButton.pos = pos;
    layerButton.scale = scale;
    if(renamingMode)
        layerButton.text = "";
    else    
        layerButton.text = this->title;
    layerButton.render(timer, doMosueTracking && !eyeBtn.hover);

    if(scale.x > 6.f && !renamingMode){
        ShaderSystem::buttonShader().setFloat("properties.groupOpacity", opacity / 2.f);
        Button layerIconDisplayer = Button(ELEMENT_STYLE_SOLID, glm::vec2(1, scale.y), "", this->layerIcon, 0.f, false);
        layerIconDisplayer.color = glm::vec4(0.f);
        layerIconDisplayer.outline = false;
        layerIconDisplayer.pos = pos;
        layerIconDisplayer.pos.x -= scale.x - layerIconDisplayer.scale.x;
        layerIconDisplayer.render(timer, false);
        ShaderSystem::buttonShader().setFloat("properties.groupOpacity", opacity);
        
        if(hiden)
            eyeBtn.texture = appTextures.eyeClosedIcon;
        else{
            eyeBtn.texture = appTextures.eyeOpenedIcon;
        }
        eyeBtn.color = glm::vec4(0.f);
        eyeBtn.color2 = glm::vec4(0.f);
        eyeBtn.outline = false;
        eyeBtn.pos = layerIconDisplayer.pos;
        eyeBtn.pos.x += layerIconDisplayer.scale.x + 0.7f * 1.5f;
        eyeBtn.scale = glm::vec2(0.7f, scale.y) + glm::vec2(0.1f) * eyeBtn.hoverMixVal + glm::vec2(0.1f) * eyeBtn.clickedMixVal;
        if(eyeBtn.clicked)
            this->hiden = !this->hiden;

        eyeBtn.render(timer, doMosueTracking);
        
        Button generalOpacityMapDisplayer = Button(ELEMENT_STYLE_SOLID, glm::vec2(1, scale.y), "", this->alpha.general_Alpha.alphaMap, 0.f, false);
        generalOpacityMapDisplayer.color = glm::vec4(0.f);
        generalOpacityMapDisplayer.outline = false;
        generalOpacityMapDisplayer.pos = pos;
        generalOpacityMapDisplayer.pos.x += scale.x - generalOpacityMapDisplayer.scale.x;
        generalOpacityMapDisplayer.render(timer, false);
        
        Button generalOpacityDisplayer = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.5f, scale.y), "", Texture(), 0.f, false);
        generalOpacityDisplayer.color = glm::vec4(0.f);
        generalOpacityDisplayer.outline = false;
        std::string generalOpacityDisplayerTxt = std::to_string(this->alpha.general_Alpha.alphaValue);
        generalOpacityDisplayerTxt.erase(generalOpacityDisplayerTxt.begin() + 4, generalOpacityDisplayerTxt.end());
        generalOpacityDisplayer.text = generalOpacityDisplayerTxt; 
        generalOpacityDisplayer.pos = generalOpacityMapDisplayer.pos;
        generalOpacityDisplayer.pos.x -= generalOpacityMapDisplayer.scale.x + generalOpacityDisplayer.scale.x;
        generalOpacityDisplayer.render(timer, false);
    }

    if(renamingMode){
        renamingTextBox.active = true;
        renamingTextBox.scale = this->layerButton.scale;
        renamingTextBox.pos = this->layerButton.pos;
        this->renamingTextBox.text = this->title;
        renamingTextBox.render(timer, true);
        this->title = this->renamingTextBox.text;
        renamingTextBox.outlineColor2 = glm::vec4(0.f);

        if(*Mouse::LClick() || *Mouse::RClick() || getContext()->window.isKeyClicked(LIGIDGL_KEY_ESCAPE) || getContext()->window.isKeyClicked(LIGIDGL_KEY_ENTER)){
            renamingMode = false;
        }
    }

    if(mainSelected){
        layerButton.color = ColorPalette::themeColor;
        layerButton.color.r /= 2.0f;
        layerButton.color.g /= 2.0f;
        layerButton.color.b /= 2.0f;
        
        layerButton.textColor = ColorPalette::oppositeColor;
    }
    else if(subSelected){
        layerButton.color = ColorPalette::themeColor;
        layerButton.color.r /= 4.0f;
        layerButton.color.g /= 4.0f;
        layerButton.color.b /= 4.0f;

        layerButton.textColor = ColorPalette::oppositeColor;
        layerButton.textColor.r /= 1.3f;
        layerButton.textColor.g /= 1.3f;
        layerButton.textColor.b /= 1.3f;
    }
    else{
        layerButton.color = ColorPalette::secondColor;
        layerButton.textColor = ColorPalette::oppositeColor;
    }


    if(layerButton.hover && *Mouse::RClick()){
        rightClicked = true;
        contextMenu.pos = glm::vec3(*Mouse::cursorPos() / *Settings::videoScale() * 100.f, pos.z + 0.04f);
    }

    if(rightClicked){
        contextMenu.clearDepthBuffer = false;
        contextMenu.render(timer, true);
        if(contextMenu.sections[0].elements[0].button.clicked){
            infoMode = true;
            rightClicked = false;
        }
        if(contextMenu.sections[0].elements[1].button.clicked){
            alphaSettingsMode = true;
            rightClicked = false;
        }
        if(contextMenu.sections[0].elements[2].button.clicked){
            renamingMode = true;
            rightClicked = false;
        }
        if(contextMenu.sections[0].elements[3].button.clicked){
            rightClicked = false;
            return 1;
        }
        if(contextMenu.sections[0].elements.size() > 4){
            if(contextMenu.sections[0].elements[4].button.clicked){
                elementSelectionMode = true;
                rightClicked = false;
            }
        }

        if(!contextMenu.hover)
            rightClicked = false;
    }

    if(elementSelectionMode)
        this->render_element_selection_panel(timer, true, materialSelectionDialog);

    if(alphaSettingsMode)
        this->renderAlphaSettingsPanel(timer, true);

    if(infoMode)
        this->renderInfoPanel(timer, true);
    
    
    if(layerButton.clicked || (layerButton.hover && *Mouse::RClick() && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && !subSelected && !mainSelected)){
        mainSelected = true;
        subSelected = true;
        return 2;
    }
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);

    return 0;
}

void Layer::updateLayerButton(){
    this->layerButton = Button(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f), this->title, Texture(), 0.f, false);
}