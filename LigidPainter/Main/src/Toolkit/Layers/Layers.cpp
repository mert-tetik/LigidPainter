/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>
#include <mutex>

#include "UTIL/GL/GL.hpp"
#include "UTIL/Wait/Wait.hpp"

#include "GUI/Elements.hpp"
#include "GUI/Panels.hpp"
#include "GUI/Dialogs.hpp"

#include "Toolkit/Layers/Layers.hpp"

static bool btnMoving = false;
static std::vector<Layer*> movingLayers;
static int btnMovingI = 0;

static void moveLayer(int src, int dest, std::vector<Layer*>& layers){
    // Wait until no thread is in progress
    WAIT_WHILE(dialog_log.any_thread_in_progress());

    if(src >= layers.size() && src < 0)
        return;

    Layer* layer = layers[src];
    layers.erase(layers.begin() + src);

    if(src < dest)
        dest--;

    if(dest >= layers.size())
        layers.push_back(layer);
    else if(dest < 0)
        layers.insert(layers.begin(), layer);
    else
        layers.insert(layers.begin() + dest, layer);
}

void LayerScene::render(Timer& timer, Panel &layerPanel, bool doMouseTracking, const unsigned int resolution, Mesh& mesh){
    int count = 0;
    
    if(layerPanel.hover && *Mouse::LClick() && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL)){
        for (Layer* layer : this->layers){
            layer->mainSelected = false;
            layer->subSelected = false;
        }
    }

    const float btn_height = 2.5f;

    float slider_val = this->slider.render(doMouseTracking, glm::vec3(layerPanel.pos.x + layerPanel.scale.x - this->slider.bg_btn.scale.x, layerPanel.pos.y, layerPanel.pos.z), layerPanel.scale.y, layerPanel.scale.y, this->layers.size() * btn_height, layerPanel.hover);

    bool anyBtnClickState1 = false;
    for (int i = this->layers.size() -1; i >= 0; i--)
    {
        glm::vec2 btnScale = glm::vec2(layerPanel.scale.x - this->slider.bg_btn.scale.x, btn_height); 
        glm::vec3 btnPos = glm::vec3(layerPanel.pos.x - this->slider.bg_btn.scale.x, layerPanel.pos.y - layerPanel.scale.y + btnScale.y + btnScale.y * (count * 2) - slider_val, layerPanel.pos.z);
        
        if(btnPos.y + btnScale.y > layerPanel.pos.y - layerPanel.scale.y){
            //Bottom
            ShaderSystem::color2d().use();
            ShaderSystem::color2d().setMat4("projection", getContext()->ortho_projection);
            ShaderSystem::color2d().setVec3("pos", glm::vec3(layerPanel.resultPos.x, layerPanel.resultPos.y + layerPanel.resultScale.y + layerPanel.resultScale.y * 2.f,   1.f)); 
            ShaderSystem::color2d().setVec2("scale", glm::vec2(layerPanel.resultScale.y * 2.f));
            ShaderSystem::color2d().setVec4("color" , glm::vec4(0.f)); 
            getBox()->draw("Panel : Barrier bottom");
            
            //Top
            ShaderSystem::color2d().setVec3("pos", glm::vec3(layerPanel.resultPos.x, layerPanel.resultPos.y - layerPanel.resultScale.y - layerPanel.resultScale.y * 2.f, 1.f));
            ShaderSystem::color2d().setVec2("scale", glm::vec2(layerPanel.resultScale.y * 2.f));
            getBox()->draw("Panel : Barrier top");
        
            bool layerDeleted = this->layers[i]->render_graphics(timer, doMouseTracking && !panel_add_layer.hover && !comboBox_PBR_displaying_mode.hover[0] && !comboBox_layers_resolution.hover[0], btnPos, btnScale, 1.f, resolution, mesh, this, i);
            
            if(layerDeleted){
                return;
            }

            if(this->layers[i]->layerGUI.layerButton.clickState1 && (Mouse::mouseOffset()->x || Mouse::mouseOffset()->y))
                btnMoving = true;

            if(this->layers[i]->layerGUI.layerButton.clickState1)
                anyBtnClickState1 = true;

            if(this->layers[i]->layerGUI.layerButton.clickState1 && btnMoving){
                glm::vec2 crsPos = glm::vec2(Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f, Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f);
                for (size_t cI = 0; cI < this->layers.size(); cI++){
                    if(crsPos.y > this->layers[cI]->layerGUI.layerButton.pos.y || (cI == this->layers.size() - 1 && crsPos.y < this->layers[cI]->layerGUI.layerButton.pos.y)){
                        Button btn = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(this->layers[cI]->layerGUI.layerButton.scale.x, 0.2f), "", Texture(), 0.f, false);
                        btn.radius = 0.05f;
                        btn.outlineThickness /= 2.f;
                        btn.color = glm::vec4(1.f);
                        btn.pos = glm::vec3(this->layers[cI]->layerGUI.layerButton.pos.x, this->layers[cI]->layerGUI.layerButton.pos.y + this->layers[cI]->layerGUI.layerButton.scale.y + btn.scale.y, this->layers[cI]->layerGUI.layerButton.pos.z + 0.05f);
                        if(!(crsPos.y > this->layers[cI]->layerGUI.layerButton.pos.y) && (cI == this->layers.size() - 1 && crsPos.y < this->layers[cI]->layerGUI.layerButton.pos.y)){
                            btn.pos.y = this->layers[cI]->layerGUI.layerButton.pos.y - this->layers[cI]->layerGUI.layerButton.scale.y - btn.scale.y;
                            btnMovingI = cI + 1;                    
                        }
                        else{
                            btnMovingI = cI;                    
                        }

                        btn.render(timer, false);
                        break;
                    }
                }

                int copyCount = 0;
                for (int cI = this->layers.size() -1; cI >= 0; cI--)
                {
                    glm::vec3 btnPos = glm::vec3(crsPos.x, crsPos.y + btnScale.y * (copyCount * 2), layerPanel.pos.z + 0.05f);
                    if(layerPanel.hover)
                        btnPos.x = layerPanel.pos.x;
                    
                    if(this->layers[i]->subSelected && this->layers[cI]->subSelected || cI == i){
                        if(copyCount == 0)
                            movingLayers.clear();    
                        movingLayers.push_back(this->layers[cI]);
                        this->layers[cI]->render_graphics(timer, false, btnPos, btnScale, 0.5f, resolution, mesh, nullptr, 0);
                        copyCount++;
                    }
                }
                
            }

            if(layerPanel.hover && shortcuts_CTRL_A()){
                for (size_t cI = 0; cI < this->layers.size(); cI++){
                    this->layers[cI]->subSelected = true;
                }
            }
        }
        
        count++;
    }

    if(!anyBtnClickState1){
        if(btnMoving){
            Layer* movingLayer;
            int movingI = 0;
            
            if(btnMovingI != this->layers.size()){
                movingLayer = this->layers[btnMovingI];
            }
            else{
                movingI = this->layers.size();
            }
            
            for (int i = movingLayers.size() - 1; i >= 0 ; i--)
            {
                int movingLayerI = 0;
                for (size_t ii = 0; ii < this->layers.size(); ii++)
                {
                    if(btnMovingI != this->layers.size()){
                        if(this->layers[ii] == movingLayer){
                            movingI = ii;
                        }
                    }

                    if(this->layers[ii] == movingLayers[i]){
                        movingLayerI = ii;
                    }
                }
                
                moveLayer(movingLayerI, movingI, this->layers);
            
                if(movingLayerI != movingI){
                    this->update_result(resolution, glm::vec3(0.f), mesh);
                }
            }
        }
        btnMoving = false;
    }

    glClear(GL_DEPTH_BUFFER_BIT);
}

void LayerScene::add_new(Layer* layer){
    this->layers.push_back(layer);
}

std::mutex layerscene_update_result_mutex;

static MaterialChannels result_channels;

void LayerScene::update_result(unsigned int resolution, glm::vec3 baseColor, Mesh& mesh){
    
    std::lock_guard<std::mutex> lock(layerscene_update_result_mutex);

    if(!result_channels.albedo.ID){
        result_channels.generate_channels(glm::ivec2(resolution));
    }
    else{
        result_channels.update_channels(glm::ivec2(resolution));
    }
    
    // Update FBO
    Framebuffer FBO = FBOPOOL::requestFBO(
                                            {
                                                result_channels.albedo, 
                                                result_channels.roughness, 
                                                result_channels.metallic, 
                                                result_channels.normalMap, 
                                                result_channels.heightMap, 
                                                result_channels.ambientOcclusion
                                            },
                                            "LayerScene::updateResult"
                                        );

    glClearColor(baseColor.r, baseColor.g, baseColor.b, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);

    for (size_t i = 0; i < this->layers.size(); i++)
    {
        if(!this->layers[i]->hiden){

            ShaderSystem::layersUpdate().use();
            ShaderSystem::layersUpdate().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
            ShaderSystem::layersUpdate().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.7f));
            ShaderSystem::layersUpdate().setVec2("scale", glm::vec2(0.5f));    
        
            ShaderSystem::layersUpdate().setInt("albedoTxtr", GL::bindTexture_2D(this->layers[i]->result.albedo.ID, "LayerScene::update_result"));
            ShaderSystem::layersUpdate().setInt("roughnessTxtr", GL::bindTexture_2D(this->layers[i]->result.roughness.ID, "LayerScene::update_result"));
            ShaderSystem::layersUpdate().setInt("metallicTxtr", GL::bindTexture_2D(this->layers[i]->result.metallic.ID, "LayerScene::update_result"));
            ShaderSystem::layersUpdate().setInt("normalMapTxtr", GL::bindTexture_2D(this->layers[i]->result.normalMap.ID, "LayerScene::update_result"));
            ShaderSystem::layersUpdate().setInt("heightMapTxtr", GL::bindTexture_2D(this->layers[i]->result.heightMap.ID, "LayerScene::update_result"));
            ShaderSystem::layersUpdate().setInt("ambientOcclusionTxtr", GL::bindTexture_2D(this->layers[i]->result.ambientOcclusion.ID, "LayerScene::update_result"));

            ShaderSystem::layersUpdate().setInt("generalMaskTxtr", GL::bindTexture_2D(this->layers[i]->alpha.general_Alpha.alphaMapProceduralTxtr.ID, "LayerScene::update_result"));
            ShaderSystem::layersUpdate().setFloat("generalOpacity", this->layers[i]->alpha.general_Alpha.alphaValue);
            ShaderSystem::layersUpdate().setInt("albedoMaskTxtr", GL::bindTexture_2D(this->layers[i]->alpha.albedo_Alpha.alphaMapProceduralTxtr.ID, "LayerScene::update_result"));
            ShaderSystem::layersUpdate().setFloat("albedoOpacity", this->layers[i]->alpha.albedo_Alpha.alphaValue);
            ShaderSystem::layersUpdate().setInt("roughnessMaskTxtr", GL::bindTexture_2D(this->layers[i]->alpha.roughness_Alpha.alphaMapProceduralTxtr.ID, "LayerScene::update_result"));
            ShaderSystem::layersUpdate().setFloat("roughnessOpacity", this->layers[i]->alpha.roughness_Alpha.alphaValue);
            ShaderSystem::layersUpdate().setInt("metallicMaskTxtr", GL::bindTexture_2D(this->layers[i]->alpha.metallic_Alpha.alphaMapProceduralTxtr.ID, "LayerScene::update_result"));
            ShaderSystem::layersUpdate().setFloat("metallicOpacity", this->layers[i]->alpha.metallic_Alpha.alphaValue);
            ShaderSystem::layersUpdate().setInt("normalMapMaskTxtr", GL::bindTexture_2D(this->layers[i]->alpha.normalMap_Alpha.alphaMapProceduralTxtr.ID, "LayerScene::update_result"));
            ShaderSystem::layersUpdate().setFloat("normalMapOpacity", this->layers[i]->alpha.normalMap_Alpha.alphaValue);
            ShaderSystem::layersUpdate().setInt("heightMapMaskTxtr", GL::bindTexture_2D(this->layers[i]->alpha.heightMap_Alpha.alphaMapProceduralTxtr.ID, "LayerScene::update_result"));
            ShaderSystem::layersUpdate().setFloat("heightMapOpacity", this->layers[i]->alpha.heightMap_Alpha.alphaValue);
            ShaderSystem::layersUpdate().setInt("ambientOcclusionMaskTxtr", GL::bindTexture_2D(this->layers[i]->alpha.ambientOcclusion_Alpha.alphaMapProceduralTxtr.ID, "LayerScene::update_result"));
            ShaderSystem::layersUpdate().setFloat("ambientOcclusionOpacity", this->layers[i]->alpha.ambientOcclusion_Alpha.alphaValue);
    
            getBox()->draw("layers_update_result");

            GL::releaseBoundTextures("LayerScene::update_result");
            ShaderUTIL::release_bound_shader();
        }
    }

    FBOPOOL::releaseFBO(FBO);
    
    if(!mesh.material_channels.albedo.ID){
        mesh.material_channels.generate_channels(glm::ivec2(resolution));
    }

    mesh.material_channels.apply_another_material_channels(result_channels, {true, true, true, true, true, true});
}

bool LayerScene::any_dialog_hovered(){
    for (size_t i = 0; i < this->layers.size(); i++)
    {
        if(
            (this->layers[i]->type_specific_modification_enabled && this->layers[i]->is_type_specific_panels_hovered())
        )
            return true;
    }
    return false;
}

Layer* LayerScene::get_edited_vector_layer(){
    for (size_t i = 0; i < this->layers.size(); i++)
    {
        if((this->layers[i]->type_specific_modification_enabled && this->layers[i]->layerType == "vector"))
            return this->layers[i];
    }

    return nullptr;
}

MaterialChannels LayerScene::get_painting_channels(bool* success){
    for (size_t i = 0; i < this->layers.size(); i++)
    {
        if(this->layers[i]->mainSelected){
            *success = true;
            if(this->layers[i]->layerType == "painting" || this->layers[i]->layerType == "vector")
                return this->layers[i]->result;
        }
    }
    
    *success = false;
    return MaterialChannels();
}

Layer* LayerScene::get_selected_layer(int* layer_index){
    for (size_t i = 0; i < this->layers.size(); i++)
    {
        if(this->layers[i]->mainSelected){
            if(layer_index != nullptr)
                *layer_index = i;
            return this->layers[i];
        }
    }
    
    return nullptr;
}

std::vector<Layer*> LayerScene::get_selected_layers(){
    std::vector<Layer*> selected_layers;

    for (size_t i = 0; i < this->layers.size(); i++)
    {
        if(this->layers[i]->mainSelected || this->layers[i]->subSelected){
            selected_layers.push_back(this->layers[i]);
        }
    }

    return selected_layers;
}

void LayerScene::update_all_layers(const unsigned int resolution, glm::vec3 baseColor, Mesh& mesh){
    for (size_t i = 0; i < this->layers.size(); i++)
    {
        this->layers[i]->type_specific_generate_result(resolution, mesh);
    }

    this->update_result(resolution, baseColor, mesh);
}

void LayerScene::unselect_all_layers(){
    for(Layer* layer : this->layers){
        layer->mainSelected = false;
        layer->subSelected = false;
    }
}

void LayerScene::delete_all_selected_layers(){
    // Wait until no thread is in progress
    WAIT_WHILE(dialog_log.any_thread_in_progress());

    for (size_t layerI = 0; layerI < this->layers.size(); layerI++){
        if(this->layers[layerI]->subSelected || this->layers[layerI]->mainSelected){
            this->layers.erase(this->layers.begin() + layerI);
            layerI--;
        }
    }
}

void LayerScene::select_layer(int layer_index, bool unselect_selected_ones, bool select_between){
    
    // Check if the layer_index is valid
    if(layer_index < 0 || layer_index >= this->layers.size()){
        LGDLOG::start << "Error : Invalid LayerScene::select_layer index" << LGDLOG::end;
        return;
    }

    int already_main_selected_index = -1;
    this->get_selected_layer(&already_main_selected_index);

    // Loop thru all the layers
    for (Layer* layer : this->layers)
    {
        // Sub-select all the layers between
        if(already_main_selected_index != -1 && select_between){
            for (int cII = 0; cII < this->layers.size(); cII++){
                if(cII < std::max(already_main_selected_index, layer_index) && cII > std::min(already_main_selected_index, layer_index))
                    this->layers[cII]->subSelected = true;
            }
        }
        
        // None of the layers will be main selected but the especially selected one
        layer->mainSelected = false;   
        
        // None of the layers will be selected but the especially selected one 
        if(unselect_selected_ones)
            layer->subSelected = false;
    }

    // Select the layer 
    this->layers[layer_index]->mainSelected = true;
    this->layers[layer_index]->subSelected = true;
}

void LayerScene::hide_unhide_selected_layers(){
    for (Layer* layer : this->get_selected_layers()){
        layer->hiden = !layer->hiden;
    }
}