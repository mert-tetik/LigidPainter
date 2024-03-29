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

static bool btnMoving = false;
static std::vector<Layer*> movingLayers;
static int btnMovingI = 0;

static void moveLayer(int src, int dest, std::vector<Layer*> layers){
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

void LayerScene::render(Timer& timer, Panel &layerPanel, Painter& painter, bool doMouseTracking, const unsigned int resolution, Mesh& mesh){
    int count = 0;

    bool anyBtnClickState1 = false;
    for (int i = this->layers.size() -1; i >= 0; i--)
    {
        glm::vec2 btnScale = glm::vec2(layerPanel.scale.x, 1.5f); 
        glm::vec3 btnPos = glm::vec3(layerPanel.pos.x, layerPanel.pos.y - layerPanel.scale.y  + btnScale.y + btnScale.y * (count * 2), layerPanel.pos.z);
        int layerMSG = this->layers[i]->render_graphics(timer, doMouseTracking, btnPos, btnScale, 1.f, painter, resolution, mesh);
        
        if(this->layers[i]->layerButton.clickState1 && (Mouse::mouseOffset()->x || Mouse::mouseOffset()->y))
            btnMoving = true;

        if(this->layers[i]->layerButton.clickState1)
            anyBtnClickState1 = true;

        if(this->layers[i]->layerButton.clickState1 && btnMoving){
            glm::vec2 crsPos = glm::vec2(Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f, Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f);
            for (size_t cI = 0; cI < this->layers.size(); cI++){
                if(crsPos.y > this->layers[cI]->layerButton.pos.y || (cI == this->layers.size() - 1 && crsPos.y < this->layers[cI]->layerButton.pos.y)){
                    Button btn = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(this->layers[cI]->layerButton.scale.x, 0.2f), "", Texture(), 0.f, false);
                    btn.radius = 0.05f;
                    btn.outlineThickness /= 2.f;
                    btn.color = glm::vec4(1.f);
                    btn.pos = glm::vec3(this->layers[cI]->layerButton.pos.x, this->layers[cI]->layerButton.pos.y + this->layers[cI]->layerButton.scale.y + btn.scale.y, this->layers[cI]->layerButton.pos.z + 0.05f);
                    if(!(crsPos.y > this->layers[cI]->layerButton.pos.y) && (cI == this->layers.size() - 1 && crsPos.y < this->layers[cI]->layerButton.pos.y)){
                        btn.pos.y = this->layers[cI]->layerButton.pos.y - this->layers[cI]->layerButton.scale.y - btn.scale.y;
                        btnMovingI = cI + 1;                    
                    }
                    else{
                        btnMovingI = cI;                    
                    }

                    btn.render(timer, false);
                    break;
                }
            }

            std::vector<Layer*> __layersCopy = this->layers;
            int copyCount = 0;
            for (int cI = __layersCopy.size() -1; cI >= 0; cI--)
            {
                glm::vec3 btnPos = glm::vec3(crsPos.x, crsPos.y + btnScale.y * (copyCount * 2), layerPanel.pos.z + 0.05f);
                if(layerPanel.hover)
                    btnPos.x = layerPanel.pos.x;
                
                if(this->layers[i]->subSelected && __layersCopy[cI]->subSelected || cI == i){
                    if(copyCount == 0)
                        movingLayers.clear();    
                    movingLayers.push_back(this->layers[cI]);
                    __layersCopy[cI]->render_graphics(timer, false, btnPos, btnScale, 0.5f, painter, resolution, mesh);
                    copyCount++;
                }
            }
            
        }

        if(layerPanel.hover && shortcuts_CTRL_A()){
            for (size_t cI = 0; cI < this->layers.size(); cI++){
                this->layers[cI]->subSelected = true;
            }
        }

        if(layerMSG == 1){
            for (size_t cI = 0; cI < this->layers.size(); cI++){
                if(this->layers[cI]->subSelected || this->layers[cI]->mainSelected){
                    this->layers.erase(this->layers.begin() + cI);
                    cI--;
                }
            }

            break;
        }

        if(layerMSG == 2){
            for (int cI = 0; cI < this->layers.size(); cI++)
            {
                if(cI != i){
                    if(getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) && this->layers[cI]->mainSelected){
                        for (int cII = 0; cII < this->layers.size(); cII++){
                            if(cII < std::max(cI, i) && cII > std::min(cI, i))
                                this->layers[cII]->subSelected = true;
                        }
                    }
                    
                    this->layers[cI]->mainSelected = false;   
                    
                    
                    if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL))
                        this->layers[cI]->subSelected = false;
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
            }
        }
        btnMoving = false;
    }
}

void LayerScene::add_new(Layer* layer){
    this->layers.push_back(layer);
}

static Framebuffer layers_update_FBO;

void LayerScene::update_result(unsigned int resolution, glm::vec3 baseColor, Mesh& mesh){
    
    // Init FBO
    if(!layers_update_FBO.ID){
        layers_update_FBO.generate();
        layers_update_FBO.purpose = "layers_update_FBO";
    }

    if(!mesh.albedo.ID){
        mesh.albedo = Texture(nullptr, resolution, resolution);
        mesh.roughness = Texture(nullptr, resolution, resolution);
        mesh.metallic = Texture(nullptr, resolution, resolution);
        mesh.normalMap = Texture(nullptr, resolution, resolution);
        mesh.heightMap = Texture(nullptr, resolution, resolution);
        mesh.ambientOcclusion = Texture(nullptr, resolution, resolution);
    }
    else{
        glm::vec2 albedoRes = mesh.albedo.getResolution();
        if(albedoRes.x != resolution){
            mesh.albedo.update(nullptr, resolution, resolution);
            mesh.roughness.update(nullptr, resolution, resolution);
            mesh.metallic.update(nullptr, resolution, resolution);
            mesh.normalMap.update(nullptr, resolution, resolution);
            mesh.heightMap.update(nullptr, resolution, resolution);
            mesh.ambientOcclusion.update(nullptr, resolution, resolution);
        }
    }
    
    // Bind FBO
    layers_update_FBO.bind();

    // Update FBO
    layers_update_FBO.setColorBuffer(
                                        {
                                            mesh.albedo, 
                                            mesh.roughness, 
                                            mesh.metallic, 
                                            mesh.normalMap, 
                                            mesh.heightMap, 
                                            mesh.ambientOcclusion
                                        },
                                        GL_TEXTURE_2D
                                    );

    glViewport(0, 0, resolution, resolution);

    glClearColor(baseColor.r, baseColor.g, baseColor.b, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    getBox()->bindBuffers();

    for (size_t i = 0; i < this->layers.size(); i++)
    {
        ShaderSystem::layersUpdate().use();
        ShaderSystem::layersUpdate().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
        ShaderSystem::layersUpdate().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.7f));
        ShaderSystem::layersUpdate().setVec2("scale", glm::vec2(0.5f));    
    
        ShaderSystem::layersUpdate().setInt("albedoTxtr", 0); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, this->layers[i]->result.albedo.ID);
        ShaderSystem::layersUpdate().setInt("roughnessTxtr", 1); glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, this->layers[i]->result.roughness.ID);
        ShaderSystem::layersUpdate().setInt("metallicTxtr", 2); glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, this->layers[i]->result.metallic.ID);
        ShaderSystem::layersUpdate().setInt("normalMapTxtr", 3); glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, this->layers[i]->result.normalMap.ID);
        ShaderSystem::layersUpdate().setInt("heightMapTxtr", 4); glActiveTexture(GL_TEXTURE4); glBindTexture(GL_TEXTURE_2D, this->layers[i]->result.heightMap.ID);
        ShaderSystem::layersUpdate().setInt("ambientOcclusionTxtr", 5); glActiveTexture(GL_TEXTURE5); glBindTexture(GL_TEXTURE_2D, this->layers[i]->result.ambientOcclusion.ID);

        ShaderSystem::layersUpdate().setInt("generalMaskTxtr", 6); glActiveTexture(GL_TEXTURE6); glBindTexture(GL_TEXTURE_2D, this->layers[i]->alpha.general_Alpha.alphaMapProceduralTxtr.ID);
        ShaderSystem::layersUpdate().setFloat("generalOpacity", this->layers[i]->alpha.general_Alpha.alphaValue);
        ShaderSystem::layersUpdate().setInt("albedoMaskTxtr", 7); glActiveTexture(GL_TEXTURE7); glBindTexture(GL_TEXTURE_2D, this->layers[i]->alpha.albedo_Alpha.alphaMapProceduralTxtr.ID);
        ShaderSystem::layersUpdate().setFloat("albedoOpacity", this->layers[i]->alpha.albedo_Alpha.alphaValue);
        ShaderSystem::layersUpdate().setInt("roughnessMaskTxtr", 8); glActiveTexture(GL_TEXTURE8); glBindTexture(GL_TEXTURE_2D, this->layers[i]->alpha.roughness_Alpha.alphaMapProceduralTxtr.ID);
        ShaderSystem::layersUpdate().setFloat("roughnessOpacity", this->layers[i]->alpha.roughness_Alpha.alphaValue);
        ShaderSystem::layersUpdate().setInt("metallicMaskTxtr", 9); glActiveTexture(GL_TEXTURE9); glBindTexture(GL_TEXTURE_2D, this->layers[i]->alpha.metallic_Alpha.alphaMapProceduralTxtr.ID);
        ShaderSystem::layersUpdate().setFloat("metallicOpacity", this->layers[i]->alpha.metallic_Alpha.alphaValue);
        ShaderSystem::layersUpdate().setInt("normalMapMaskTxtr", 10); glActiveTexture(GL_TEXTURE10); glBindTexture(GL_TEXTURE_2D, this->layers[i]->alpha.normalMap_Alpha.alphaMapProceduralTxtr.ID);
        ShaderSystem::layersUpdate().setFloat("normalMapOpacity", this->layers[i]->alpha.normalMap_Alpha.alphaValue);
        ShaderSystem::layersUpdate().setInt("heightMapMaskTxtr", 11); glActiveTexture(GL_TEXTURE11); glBindTexture(GL_TEXTURE_2D, this->layers[i]->alpha.heightMap_Alpha.alphaMapProceduralTxtr.ID);
        ShaderSystem::layersUpdate().setFloat("heightMapOpacity", this->layers[i]->alpha.heightMap_Alpha.alphaValue);
        ShaderSystem::layersUpdate().setInt("ambientOcclusionMaskTxtr", 12); glActiveTexture(GL_TEXTURE12); glBindTexture(GL_TEXTURE_2D, this->layers[i]->alpha.ambientOcclusion_Alpha.alphaMapProceduralTxtr.ID);
        ShaderSystem::layersUpdate().setFloat("ambientOcclusionOpacity", this->layers[i]->alpha.ambientOcclusion_Alpha.alphaValue);
   
        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "layers_update_result");
    }
    
}

bool LayerScene::any_dialog_active(){
    for (size_t i = 0; i < this->layers.size(); i++)
    {
        if((this->layers[i]->elementSelectionMode && this->layers[i]->layerType != "vector") || this->layers[i]->alphaSettingsMode || this->layers[i]->infoMode || this->layers[i]->renamingMode || this->layers[i]->rightClicked)
            return true;
    }
    return false;
}

bool LayerScene::any_vector_editing(){
    for (size_t i = 0; i < this->layers.size(); i++)
    {
        if((this->layers[i]->elementSelectionMode && this->layers[i]->layerType == "vector"))
            return true;
    }
    return false;
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

void LayerScene::update_all_layers(const unsigned int resolution, glm::vec3 baseColor, Painter& painter, Mesh& mesh){
    for (size_t i = 0; i < this->layers.size(); i++)
    {
        this->layers[i]->render(painter, resolution, mesh);
    }
    this->update_result(resolution, baseColor, mesh);
}