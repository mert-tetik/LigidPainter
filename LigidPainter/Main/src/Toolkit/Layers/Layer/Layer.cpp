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
#include "Toolkit/Layers/Layer_Dialogs/Layer_Dialogs.hpp" // Dialogs designed specifically for layers. "LAYERDIALOG_*dialog name*"

bool Layer::render_graphics(Timer& timer, bool doMosueTracking, glm::vec3 pos, glm::vec2 scale, float opacity, const unsigned int resolution, Mesh& mesh, LayerScene* src_layer_scene, int src_layer_index){

    this->layerGUI.layerButton.pos = pos;
    this->layerGUI.layerButton.scale = scale;

    update_layer_buffers_resolution_values(resolution, mesh);

    this->layerGUI.layerButton.update_result_transform_values();

    /* Render layer background*/
    if(opacity == 1.f)
        this->layerGUI.render_layer_bg(this);

    /* Render layer button*/
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", opacity);
    this->layerGUI.layerButton.color.a = 0.4f;
    this->layerGUI.layerButton.text = this->title;
    this->layerGUI.layerButton.render(timer, doMosueTracking && !this->layerGUI.eyeBtn.hover);
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);

    /* Render layer elements*/
    if(scale.x > 8.f){
        this->layerGUI.render_info_and_modification_elements(timer, doMosueTracking, this->layerIcon, &this->hiden, this->alpha.general_Alpha.alphaMap, this->alpha.general_Alpha.alphaValue, this->layerGUI.layerButton.pos, this->layerGUI.layerButton.scale, opacity);
    }

    // Update clickedMixVal value
    timer.transition(this->mainSelected || this->subSelected, this->layerGUI.clickedMixVal, 0.2f);

    // Highlight the selected layers
    if(this->layerGUI.clickedMixVal){
        Button hightlight_btn = Button(ELEMENT_STYLE_SOLID, glm::vec2(this->layerGUI.layerButton.scale.x, this->layerGUI.layerButton.scale.y / 10.f * this->layerGUI.clickedMixVal), "", Texture(), 0.f, false);
        hightlight_btn.pos = glm::vec3(this->layerGUI.layerButton.pos.x, this->layerGUI.layerButton.pos.y - this->layerGUI.layerButton.scale.y + hightlight_btn.scale.y, this->layerGUI.layerButton.pos.z);
        hightlight_btn.color = ColorPalette::themeColor;
        if(!mainSelected)
            hightlight_btn.color /= 2.f;
        hightlight_btn.outlineExtra = false;
        hightlight_btn.render(timer, false);

        // Render the same for the bottom part
        hightlight_btn.pos = glm::vec3(this->layerGUI.layerButton.pos.x, this->layerGUI.layerButton.pos.y + this->layerGUI.layerButton.scale.y - hightlight_btn.scale.y, this->layerGUI.layerButton.pos.z);
        hightlight_btn.render(timer, false);

        this->layerGUI.layerButton.textColor = hightlight_btn.color;
    }
    else{
        this->layerGUI.layerButton.textColor = ColorPalette::oppositeColor;
    }

    /* Selected the layer if clicked to the layer button*/
    if(this->layerGUI.layerButton.clicked){
        src_layer_scene->select_layer(
                                        src_layer_index, 
                                        !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL), 
                                        getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT)
                                    );
    }

    /* Show the context menu if right clicked to the layer button*/
    if(this->layerGUI.layerButton.hover && *Mouse::RClick() && src_layer_scene != nullptr){
        src_layer_scene->select_layer(src_layer_index, !(this->mainSelected || this->subSelected), false);
        if(this->layerGUI.show_layer_context_menu(timer, this, src_layer_scene, resolution, mesh))
            return true;
    }

    /* Quit vector editing if the layer is not main selected*/
    if(!this->mainSelected)
        this->type_specific_modification_enabled = false;

    /* Render type specific modification thingies*/
    if(type_specific_modification_enabled)
        this->type_specific_modification(timer, true, resolution, mesh);


    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);

    return false;
}




















void Layer::generate_layer_buffers(const unsigned int resolution){
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

void LayerAlpha::update_buffers(const unsigned int resolution, Mesh& mesh){
    this->general_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);
    this->albedo_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);
    this->roughness_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);
    this->metallic_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);
    this->normalMap_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);
    this->heightMap_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);
    this->ambientOcclusion_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);

    this->general_Alpha.alphaMap.generateProceduralTexture(mesh, this->general_Alpha.alphaMapProceduralTxtr, resolution);
    this->albedo_Alpha.alphaMap.generateProceduralTexture(mesh, this->albedo_Alpha.alphaMapProceduralTxtr, resolution);
    this->roughness_Alpha.alphaMap.generateProceduralTexture(mesh, this->roughness_Alpha.alphaMapProceduralTxtr, resolution);
    this->metallic_Alpha.alphaMap.generateProceduralTexture(mesh, this->metallic_Alpha.alphaMapProceduralTxtr, resolution);
    this->normalMap_Alpha.alphaMap.generateProceduralTexture(mesh, this->normalMap_Alpha.alphaMapProceduralTxtr, resolution);
    this->heightMap_Alpha.alphaMap.generateProceduralTexture(mesh, this->heightMap_Alpha.alphaMapProceduralTxtr, resolution);
    this->ambientOcclusion_Alpha.alphaMap.generateProceduralTexture(mesh, this->ambientOcclusion_Alpha.alphaMapProceduralTxtr, resolution);
} 

void Layer::update_result_buffers(const unsigned int resolution, Mesh& mesh){
    if(this->layerType == "painting"){
        result.albedo.resize(glm::ivec2(resolution));
        result.roughness.resize(glm::ivec2(resolution));
        result.metallic.resize(glm::ivec2(resolution));
        result.normalMap.resize(glm::ivec2(resolution));
        result.heightMap.resize(glm::ivec2(resolution));
        result.ambientOcclusion.resize(glm::ivec2(resolution));
    }
    else{
        result.albedo.update(nullptr, resolution, resolution);
        result.roughness.update(nullptr, resolution, resolution);
        result.metallic.update(nullptr, resolution, resolution);
        result.normalMap.update(nullptr, resolution, resolution);
        result.heightMap.update(nullptr, resolution, resolution);
        result.ambientOcclusion.update(nullptr, resolution, resolution);

        this->type_specific_generate_result(resolution, mesh);
    }
}

void Layer::update_layer_buffers_resolution_values(unsigned int resolution, Mesh& mesh){
    if(this->result.albedo.getResolution().x != resolution)
        this->update_result_buffers(resolution, mesh);
    if(this->alpha.general_Alpha.alphaMapProceduralTxtr.getResolution().x != resolution)
        this->alpha.update_buffers(resolution, mesh);
}

bool LayerGUI::show_layer_context_menu(Timer& timer, Layer* layer, LayerScene* src_layer_scene, unsigned int resolution, Mesh& mesh){

    std::vector<std::string> context_menu_elements;

    std::string layer_info_txt = "Layer Info", 
                opacity_settings_txt = "Opacity Settings", 
                rename_txt = "Rename", 
                hide_txt = "Hide", 
                unhide_txt = "Unhide", 
                hide_unhide_all_txt = "Hide - Unhide All Selected", 
                modify_elements_txt = "Modify Elements", 
                delete_txt = "Delete", 
                delete_all_txt = "Delete All Selected";

    // Multiple layers selected
    if(src_layer_scene->get_selected_layers().size() > 1){
        context_menu_elements = {hide_unhide_all_txt, delete_all_txt};
    }
    else{
        context_menu_elements = {layer_info_txt, opacity_settings_txt, rename_txt, (layer->hiden) ? unhide_txt : hide_txt, modify_elements_txt, delete_txt};
    }

    int res = show_context_menu(timer, context_menu_elements);
    
    // No context menu buttons were selected
    if(res == -1)
        return false;

    // Pressed to layer info button 
    if(context_menu_elements[res] == layer_info_txt){
        LAYERDIALOG_show_info_dialog(timer, layer, layer->layerGUI.layerButton, resolution, mesh);
    }
    
    // Pressed to opacity settings button 
    if(context_menu_elements[res] == opacity_settings_txt){
        LAYERDIALOG_show_alpha_settings_dialog(timer, &layer->alpha, layer->layerGUI.layerButton, resolution, mesh);
    }
    
    // Pressed to rename button
    if(context_menu_elements[res] == rename_txt){
        dialog_renaming.show(timer, layer->layerGUI.layerButton.pos, layer->layerGUI.layerButton.scale.x, &layer->title, 25);
    }

    // Pressed to hide - unhide button
    if(context_menu_elements[res] == hide_txt || context_menu_elements[res] == unhide_txt || context_menu_elements[res] == hide_unhide_all_txt){
        src_layer_scene->hide_unhide_selected_layers();
    }   

    // Pressed to set elements button
    if(context_menu_elements[res] == modify_elements_txt){
        layer->type_specific_modification_enabled = true;
    }

    // Pressed to delete button
    if(context_menu_elements[res] == delete_txt || context_menu_elements[res] == delete_all_txt){
        src_layer_scene->delete_all_selected_layers();
        return true;
    }

    return false;
}
void LayerGUI::render_layer_bg(Layer* layer){
    {
        ShaderSystem::color2d().use();
        ShaderSystem::color2d().setMat4("projection", getContext()->ortho_projection);
        ShaderSystem::color2d().setVec4("color" , glm::vec4(0.f)); 
        //Bottom
        ShaderSystem::color2d().setVec3("pos", glm::vec3(layerButton.resultPos.x, layerButton.resultPos.y + layerButton.resultScale.y + layerButton.resultScale.y * 4.f,   1.f)); 
        ShaderSystem::color2d().setVec2("scale", glm::vec2(layerButton.resultScale * 4.f));
        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Panel : Barrier bottom");
        //Top
        ShaderSystem::color2d().setVec3("pos", glm::vec3(layerButton.resultPos.x, layerButton.resultPos.y - layerButton.resultScale.y - layerButton.resultScale.y * 4.f,   1.f)); 
        ShaderSystem::color2d().setVec2("scale", glm::vec2(layerButton.resultScale * 4.f));
        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Panel : Barrier bottom");
    }

    if(layer->layerType == "material"){
        Material* material;
        layer->get_type_specific_variable(&material, nullptr, nullptr, nullptr, nullptr);
        ShaderSystem::textureRenderingShader().use();
        ShaderSystem::textureRenderingShader().setMat4("projection", getContext()->ortho_projection);
        ShaderSystem::textureRenderingShader().setVec2("scale", glm::vec2(layerButton.resultScale.x/1.4f, layerButton.resultScale.x/ 1.4f));
        ShaderSystem::textureRenderingShader().setVec3("pos", layerButton.resultPos);
        ShaderSystem::textureRenderingShader().setInt("txtr", 0); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, material->displayingTexture.ID);
        ShaderSystem::textureRenderingShader().setFloat("opacity", 1.0f);
        ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
        ShaderSystem::textureRenderingShader().setFloat("depthToleranceValue", 0);

        if(material->displayingTexture.ID)
            LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Panel : Pre-rendering mode : Rendering result texture");
        
        ShaderSystem::buttonShader().use();    
    }   

    if(layer->layerType == "vector"){
        std::vector<VectorStroke3D>* strokes;
        Brush* brush;
        layer->get_type_specific_variable(nullptr, &strokes, &brush, nullptr, nullptr);
        ShaderSystem::textureRenderingShader().use();
        ShaderSystem::textureRenderingShader().setMat4("projection", getContext()->ortho_projection);
        ShaderSystem::textureRenderingShader().setVec2("scale", glm::vec2(layerButton.resultScale.x/1.4f, layerButton.resultScale.x/ 1.4f));
        ShaderSystem::textureRenderingShader().setVec3("pos", layerButton.resultPos);
        ShaderSystem::textureRenderingShader().setInt("txtr", 0); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, brush->displayingTexture.ID);
        ShaderSystem::textureRenderingShader().setFloat("opacity", 1.0f);
        ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
        ShaderSystem::textureRenderingShader().setFloat("depthToleranceValue", 0);

        if(brush->displayingTexture.ID)
            LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Panel : Pre-rendering mode : Rendering result texture");
        
        ShaderSystem::buttonShader().use();    
    }   

    if(layer->layerType == "painting"){
        Texture* capture_txtr;
        layer->get_type_specific_variable(nullptr, nullptr, nullptr, nullptr, &capture_txtr);
        ShaderSystem::textureRenderingShader().use();
        ShaderSystem::textureRenderingShader().setMat4("projection", getContext()->ortho_projection);
        ShaderSystem::textureRenderingShader().setVec2("scale", glm::vec2(layerButton.resultScale.x/1.4f, layerButton.resultScale.x/ 1.4f));
        ShaderSystem::textureRenderingShader().setVec3("pos", layerButton.resultPos);
        ShaderSystem::textureRenderingShader().setInt("txtr", 0); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, capture_txtr->ID);
        ShaderSystem::textureRenderingShader().setFloat("opacity", 1.0f);
        ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
        ShaderSystem::textureRenderingShader().setFloat("depthToleranceValue", 0);

        if(capture_txtr->ID)
            LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Panel : Pre-rendering mode : Rendering result texture");
        
        ShaderSystem::buttonShader().use();    
    }   

    if(layer->layerType == "texture"){
        MaterialChannels* materialChannels;
        layer->get_type_specific_variable(nullptr, nullptr, nullptr, &materialChannels, nullptr);
        
        for (size_t i = 0; i < 6; i++)
        {
            Texture txtr;

            if(i == 0)
                txtr = materialChannels->albedo;
            if(i == 1)
                txtr = materialChannels->roughness;
            if(i == 2)
                txtr = materialChannels->metallic;
            if(i == 3)
                txtr = materialChannels->normalMap;
            if(i == 4)
                txtr = materialChannels->heightMap;
            if(i == 5)
                txtr = materialChannels->ambientOcclusion;

            glm::vec2 scale = glm::vec2(layerButton.resultScale.x / 1.4f);
            scale.x /= 6;
            scale.y = layerButton.resultScale.y;

            glm::vec3 pos = layerButton.resultPos;
            pos.x = pos.x - (layerButton.resultScale.x / 1.4f) + scale.x + (scale.x * 2.f * i);

            ShaderSystem::textureRenderingShader().use();
            ShaderSystem::textureRenderingShader().setMat4("projection", getContext()->ortho_projection);
            ShaderSystem::textureRenderingShader().setVec2("scale", scale);
            ShaderSystem::textureRenderingShader().setVec3("pos", pos);
            ShaderSystem::textureRenderingShader().setInt("txtr", 0); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, txtr.ID);
            ShaderSystem::textureRenderingShader().setFloat("opacity", 1.0f);
            ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
            ShaderSystem::textureRenderingShader().setFloat("depthToleranceValue", 0);
        
            if(txtr.ID)
                LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Panel : Pre-rendering mode : Rendering result texture");
        }

        ShaderSystem::buttonShader().use();    
    }   

    glClear(GL_DEPTH_BUFFER_BIT);
}

void LayerGUI::render_info_and_modification_elements(Timer& timer, bool doMouseTracking, Texture icon, bool* hidden, Texture alpha_map_texture, float alpha_value, glm::vec3 pos, glm::vec2 scale, float opacity){
    
    /* Render the layer icon*/
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", opacity / 2.f);
    Button layerIconDisplayer = Button(ELEMENT_STYLE_SOLID, glm::vec2(1, scale.y), "", icon, 0.f, false);
    layerIconDisplayer.color = glm::vec4(0.f);
    layerIconDisplayer.outline = false;
    layerIconDisplayer.pos = pos;
    layerIconDisplayer.pos.x -= scale.x - layerIconDisplayer.scale.x;
    layerIconDisplayer.render(timer, false);
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", opacity);
    
    /* Render the eye icon*/
    eyeBtn.texture = (*hidden) ? appTextures.eyeClosedIcon : appTextures.eyeOpenedIcon;
    eyeBtn.color = glm::vec4(0.f);
    eyeBtn.color2 = glm::vec4(0.f);
    eyeBtn.outline = false;
    eyeBtn.pos = layerIconDisplayer.pos;
    eyeBtn.pos.x += layerIconDisplayer.scale.x + 0.7f * 1.5f;
    eyeBtn.scale = glm::vec2(0.7f, scale.y) + glm::vec2(0.1f) * eyeBtn.hoverMixVal + glm::vec2(0.1f) * eyeBtn.clickedMixVal;
    if(eyeBtn.clicked)
        *hidden = !*hidden;
    eyeBtn.render(timer, doMouseTracking);
    
    /* Render the general opacity map*/
    Button generalOpacityMapDisplayer = Button(ELEMENT_STYLE_SOLID, glm::vec2(1, scale.y), "", alpha_map_texture, 0.f, false);
    generalOpacityMapDisplayer.color = glm::vec4(0.f);
    generalOpacityMapDisplayer.outline = false;
    generalOpacityMapDisplayer.pos = pos;
    generalOpacityMapDisplayer.pos.x += scale.x - generalOpacityMapDisplayer.scale.x;
    generalOpacityMapDisplayer.render(timer, false);

    /* Render the general opacity value*/   
    Button generalOpacityDisplayer = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.5f, scale.y), "", Texture(), 0.f, false);
    generalOpacityDisplayer.color = glm::vec4(0.f);
    generalOpacityDisplayer.outline = false;
    std::string generalOpacityDisplayerTxt = std::to_string(alpha_value);
    generalOpacityDisplayerTxt.erase(generalOpacityDisplayerTxt.begin() + 4, generalOpacityDisplayerTxt.end());
    generalOpacityDisplayer.text = generalOpacityDisplayerTxt; 
    generalOpacityDisplayer.pos = generalOpacityMapDisplayer.pos;
    generalOpacityDisplayer.pos.x -= generalOpacityMapDisplayer.scale.x + generalOpacityDisplayer.scale.x;
    generalOpacityDisplayer.render(timer, false);

    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);
}