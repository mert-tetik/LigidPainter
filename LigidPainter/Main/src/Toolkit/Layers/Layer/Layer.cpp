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

#include "GUI/Elements.hpp"
#include "GUI/Dialogs.hpp"
#include "Toolkit/Layers/Layers.hpp"

static bool __enteredPanelOnce = false;

void Layer::renderAlphaSettingsPanel(Timer& timer, bool doMouseTracking, const unsigned int resolution, Mesh& mesh){
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

    if(!this->alphaSettingsMode){
        this->updateProceduralMaskTextures(resolution, mesh);
    }
}

void Layer::renderInfoPanel(Timer& timer, bool doMouseTracking){
    infoPanel.sections[0].elements[0].button.text = "Title : " + this->title;
    infoPanel.sections[0].elements[1].button.text = "Type : " + this->layerType;
    infoPanel.sections[0].elements[1].button.texture = this->layerIcon;
    
    infoPanel.sections[0].elements[2].button.texture = this->alpha.albedo_Alpha.alphaMapProceduralTxtr;
    infoPanel.sections[0].elements[4].button.texture = this->alpha.albedo_Alpha.alphaMapProceduralTxtr;
    infoPanel.sections[0].elements[7].button.texture = this->alpha.roughness_Alpha.alphaMapProceduralTxtr;
    infoPanel.sections[0].elements[10].button.texture = this->alpha.metallic_Alpha.alphaMapProceduralTxtr;
    infoPanel.sections[0].elements[13].button.texture = this->alpha.normalMap_Alpha.alphaMapProceduralTxtr;
    infoPanel.sections[0].elements[16].button.texture = this->alpha.heightMap_Alpha.alphaMapProceduralTxtr;
    infoPanel.sections[0].elements[19].button.texture = this->alpha.ambientOcclusion_Alpha.alphaMapProceduralTxtr;
    
    infoPanel.sections[0].elements[5].button.texture = this->result.albedo;
    infoPanel.sections[0].elements[8].button.texture = this->result.roughness;
    infoPanel.sections[0].elements[11].button.texture = this->result.metallic;
    infoPanel.sections[0].elements[14].button.texture = this->result.normalMap;
    infoPanel.sections[0].elements[17].button.texture = this->result.heightMap;
    infoPanel.sections[0].elements[20].button.texture = this->result.ambientOcclusion;
    
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

int Layer::render_graphics(Timer& timer, bool doMosueTracking, glm::vec3 pos, glm::vec2 scale, float opacity, const unsigned int resolution, Mesh& mesh){
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", opacity);
    layerButton.pos = pos;
    layerButton.scale = scale;
    layerButton.text = this->title;
    layerButton.render(timer, doMosueTracking && !eyeBtn.hover);

    if(scale.x > 6.f){
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
        int res;
        if(this->layerType == "painting")
            res = show_context_menu(timer, {"Layer Info", "Opacity Settings", "Rename" , "Delete"});
        else
            res = show_context_menu(timer, {"Layer Info", "Opacity Settings", "Rename" , "Delete" , "Set Elements"});
        
        if(res == 0){
            infoMode = true;
        }
        if(res == 1){
            alphaSettingsMode = true;
        }
        if(res == 2){
            dialog_renaming.show(timer, this->layerButton.pos, this->layerButton.scale.x, &this->title);
        }
        if(res == 3){
            return 1;
        }
        if(res == 4){
            elementSelectionMode = true;
        }
    }

    if(elementSelectionMode)
        this->render_element_selection_panel(timer, true, resolution, mesh);

    if(alphaSettingsMode)
        this->renderAlphaSettingsPanel(timer, true, resolution, mesh);

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

void Layer::genResultChannels(const unsigned int resolution){
    result.albedo = Texture(nullptr, resolution, resolution);
    result.roughness = Texture(nullptr, resolution, resolution);
    result.metallic = Texture(nullptr, resolution, resolution);
    result.normalMap = Texture(nullptr, resolution, resolution);
    result.heightMap = Texture(nullptr, resolution, resolution);
    result.ambientOcclusion = Texture(nullptr, resolution, resolution);

    this->alpha.general_Alpha.genTxtrs();
    this->alpha.albedo_Alpha.genTxtrs();
    this->alpha.roughness_Alpha.genTxtrs();
    this->alpha.metallic_Alpha.genTxtrs();
    this->alpha.normalMap_Alpha.genTxtrs();
    this->alpha.heightMap_Alpha.genTxtrs();
    this->alpha.ambientOcclusion_Alpha.genTxtrs();
}

void Layer::updateProceduralMaskTextures(const unsigned int resolution, Mesh& mesh){
    this->alpha.general_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);
    this->alpha.albedo_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);
    this->alpha.roughness_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);
    this->alpha.metallic_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);
    this->alpha.normalMap_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);
    this->alpha.heightMap_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);
    this->alpha.ambientOcclusion_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);

    this->alpha.general_Alpha.alphaMap.generateProceduralTexture(mesh, this->alpha.general_Alpha.alphaMapProceduralTxtr, resolution);
    this->alpha.albedo_Alpha.alphaMap.generateProceduralTexture(mesh, this->alpha.albedo_Alpha.alphaMapProceduralTxtr, resolution);
    this->alpha.roughness_Alpha.alphaMap.generateProceduralTexture(mesh, this->alpha.roughness_Alpha.alphaMapProceduralTxtr, resolution);
    this->alpha.metallic_Alpha.alphaMap.generateProceduralTexture(mesh, this->alpha.metallic_Alpha.alphaMapProceduralTxtr, resolution);
    this->alpha.normalMap_Alpha.alphaMap.generateProceduralTexture(mesh, this->alpha.normalMap_Alpha.alphaMapProceduralTxtr, resolution);
    this->alpha.heightMap_Alpha.alphaMap.generateProceduralTexture(mesh, this->alpha.heightMap_Alpha.alphaMapProceduralTxtr, resolution);
    this->alpha.ambientOcclusion_Alpha.alphaMap.generateProceduralTexture(mesh, this->alpha.ambientOcclusion_Alpha.alphaMapProceduralTxtr, resolution);
} 

void Layer::updateResultTextureResolutions(const unsigned int resolution, Mesh& mesh){
    if(result.albedo.getResolution().x != resolution){
        result.albedo.resize(glm::ivec2(resolution));
        result.roughness.resize(glm::ivec2(resolution));
        result.metallic.resize(glm::ivec2(resolution));
        result.normalMap.resize(glm::ivec2(resolution));
        result.heightMap.resize(glm::ivec2(resolution));
        result.ambientOcclusion.resize(glm::ivec2(resolution));

        this->updateProceduralMaskTextures(resolution, mesh);
    }
}