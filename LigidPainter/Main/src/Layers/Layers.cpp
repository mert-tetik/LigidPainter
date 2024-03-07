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

std::vector<Layer*> __layers;

static bool btnMoving = false;
static std::vector<Layer*> movingLayers;
static int btnMovingI = 0;

static void moveLayer(int src, int dest){
    if(src >= __layers.size() && src < 0)
        return;

    Layer* layer = __layers[src];
    __layers.erase(__layers.begin() + src);

    if(src < dest)
        dest--;

    if(dest >= __layers.size())
        __layers.push_back(layer);
    else if(dest < 0)
        __layers.insert(__layers.begin(), layer);
    else
        __layers.insert(__layers.begin() + dest, layer);
}

void layers_render(Timer& timer, Panel &layerPanel, MaterialSelectionDialog &materialSelectionDialog, Painter& painter, bool doMouseTracking){
    int count = 0;

    bool anyBtnClickState1 = false;
    for (int i = __layers.size() -1; i >= 0; i--)
    {
        glm::vec2 btnScale = glm::vec2(layerPanel.scale.x, 2.f); 
        glm::vec3 btnPos = glm::vec3(layerPanel.pos.x, layerPanel.pos.y - layerPanel.scale.y  + btnScale.y + btnScale.y * (count * 2), layerPanel.pos.z);
        int layerMSG = __layers[i]->render_graphics(timer, doMouseTracking, btnPos, btnScale, 1.f, materialSelectionDialog, painter);
        
        if(__layers[i]->layerButton.clickState1 && (Mouse::mouseOffset()->x || Mouse::mouseOffset()->y))
            btnMoving = true;
        if(__layers[i]->layerButton.clickState1)
            anyBtnClickState1 = true;

        if(__layers[i]->layerButton.clickState1 && btnMoving){
            glm::vec2 crsPos = glm::vec2(Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f, Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f);
            for (size_t cI = 0; cI < __layers.size(); cI++){
                if(crsPos.y > __layers[cI]->layerButton.pos.y || (cI == __layers.size() - 1 && crsPos.y < __layers[cI]->layerButton.pos.y)){
                    Button btn = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(__layers[cI]->layerButton.scale.x, 0.2f), "", Texture(), 0.f, false);
                    btn.radius = 0.05f;
                    btn.outlineThickness /= 2.f;
                    btn.color = glm::vec4(1.f);
                    btn.pos = glm::vec3(__layers[cI]->layerButton.pos.x, __layers[cI]->layerButton.pos.y + __layers[cI]->layerButton.scale.y + btn.scale.y, __layers[cI]->layerButton.pos.z + 0.05f);
                    if(!(crsPos.y > __layers[cI]->layerButton.pos.y) && (cI == __layers.size() - 1 && crsPos.y < __layers[cI]->layerButton.pos.y)){
                        btn.pos.y = __layers[cI]->layerButton.pos.y - __layers[cI]->layerButton.scale.y - btn.scale.y;
                        btnMovingI = cI + 1;                    
                    }
                    else{
                        btnMovingI = cI;                    
                    }

                    btn.render(timer, false);
                    break;
                }
            }

            std::vector<Layer*> __layersCopy = __layers;
            int copyCount = 0;
            for (int cI = __layersCopy.size() -1; cI >= 0; cI--)
            {
                glm::vec3 btnPos = glm::vec3(crsPos.x, crsPos.y + btnScale.y * (copyCount * 2), layerPanel.pos.z + 0.05f);
                if(layerPanel.hover)
                    btnPos.x = layerPanel.pos.x;
                
                if(__layers[i]->subSelected && __layersCopy[cI]->subSelected || cI == i){
                    if(copyCount == 0)
                        movingLayers.clear();    
                    movingLayers.push_back(__layers[cI]);
                    __layersCopy[cI]->render_graphics(timer, false, btnPos, btnScale, 0.5f, materialSelectionDialog, painter);
                    copyCount++;
                }
            }
            
        }

        if(layerPanel.hover && shortcuts_CTRL_A()){
            for (size_t cI = 0; cI < __layers.size(); cI++){
                __layers[cI]->subSelected = true;
            }
        }

        if(layerMSG == 1){
            for (size_t cI = 0; cI < __layers.size(); cI++){
                if(__layers[cI]->subSelected || __layers[cI]->mainSelected){
                    __layers.erase(__layers.begin() + cI);
                    cI--;
                }
            }

            break;
        }

        if(layerMSG == 2){
            for (int cI = 0; cI < __layers.size(); cI++)
            {
                if(cI != i){
                    if(getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) && __layers[cI]->mainSelected){
                        for (int cII = 0; cII < __layers.size(); cII++){
                            if(cII < std::max(cI, i) && cII > std::min(cI, i))
                                __layers[cII]->subSelected = true;
                        }
                    }
                    
                    __layers[cI]->mainSelected = false;   
                    
                    
                    if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL))
                        __layers[cI]->subSelected = false;
                }
            }
        }
        
        count++;
    }

    if(!anyBtnClickState1){
        if(btnMoving){
            Layer* movingLayer;
            int movingI = 0;
            
            if(btnMovingI != __layers.size()){
                movingLayer = __layers[btnMovingI];
            }
            else{
                movingI = __layers.size();
            }
            
            for (int i = movingLayers.size() - 1; i >= 0 ; i--)
            {
                int movingLayerI = 0;
                for (size_t ii = 0; ii < __layers.size(); ii++)
                {
                    if(btnMovingI != __layers.size()){
                        if(__layers[ii] == movingLayer){
                            movingI = ii;
                        }
                    }

                    if(__layers[ii] == movingLayers[i]){
                        movingLayerI = ii;
                    }
                }
                
                moveLayer(movingLayerI, movingI);
            }
        }
        btnMoving = false;
    }
}

void layers_add_new(Layer* layer){
    __layers.push_back(layer);
}

static Framebuffer layers_update_FBO;

void layers_update_result(unsigned int resolution, glm::vec3 baseColor){
    
    // Init FBO
    if(!layers_update_FBO.ID){
        layers_update_FBO.generate();
        layers_update_FBO.purpose = "layers_update_FBO";
    }

    if(!getModel()->meshes[0].albedo.ID){
        getModel()->meshes[0].albedo = Texture(nullptr, resolution, resolution);
        getModel()->meshes[0].roughness = Texture(nullptr, resolution, resolution);
        getModel()->meshes[0].metallic = Texture(nullptr, resolution, resolution);
        getModel()->meshes[0].normalMap = Texture(nullptr, resolution, resolution);
        getModel()->meshes[0].heightMap = Texture(nullptr, resolution, resolution);
        getModel()->meshes[0].ambientOcclusion = Texture(nullptr, resolution, resolution);
    }
    else{
        glm::vec2 albedoRes = getModel()->meshes[0].albedo.getResolution();
        if(albedoRes.x != resolution){
            getModel()->meshes[0].albedo.update(nullptr, resolution, resolution);
            getModel()->meshes[0].roughness.update(nullptr, resolution, resolution);
            getModel()->meshes[0].metallic.update(nullptr, resolution, resolution);
            getModel()->meshes[0].normalMap.update(nullptr, resolution, resolution);
            getModel()->meshes[0].heightMap.update(nullptr, resolution, resolution);
            getModel()->meshes[0].ambientOcclusion.update(nullptr, resolution, resolution);
        }
    }
    
    // Bind FBO
    layers_update_FBO.bind();

    // Update FBO
    layers_update_FBO.setColorBuffer(
                                        {
                                            getModel()->meshes[0].albedo, 
                                            getModel()->meshes[0].roughness, 
                                            getModel()->meshes[0].metallic, 
                                            getModel()->meshes[0].normalMap, 
                                            getModel()->meshes[0].heightMap, 
                                            getModel()->meshes[0].ambientOcclusion
                                        },
                                        GL_TEXTURE_2D
                                    );

    glViewport(0, 0, resolution, resolution);

    glClearColor(baseColor.r, baseColor.g, baseColor.b, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    getBox()->bindBuffers();

    for (size_t i = 0; i < __layers.size(); i++)
    {
        ShaderSystem::layersUpdate().use();
        ShaderSystem::layersUpdate().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
        ShaderSystem::layersUpdate().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.7f));
        ShaderSystem::layersUpdate().setVec2("scale", glm::vec2(0.5f));    
    
        ShaderSystem::layersUpdate().setInt("albedoTxtr", 0); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, __layers[i]->result.albedo.ID);
        ShaderSystem::layersUpdate().setInt("roughnessTxtr", 1); glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, __layers[i]->result.roughness.ID);
        ShaderSystem::layersUpdate().setInt("metallicTxtr", 2); glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, __layers[i]->result.metallic.ID);
        ShaderSystem::layersUpdate().setInt("normalMapTxtr", 3); glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, __layers[i]->result.normalMap.ID);
        ShaderSystem::layersUpdate().setInt("heightMapTxtr", 4); glActiveTexture(GL_TEXTURE4); glBindTexture(GL_TEXTURE_2D, __layers[i]->result.heightMap.ID);
        ShaderSystem::layersUpdate().setInt("ambientOcclusionTxtr", 5); glActiveTexture(GL_TEXTURE5); glBindTexture(GL_TEXTURE_2D, __layers[i]->result.ambientOcclusion.ID);

        ShaderSystem::layersUpdate().setInt("generalMaskTxtr", 6); glActiveTexture(GL_TEXTURE6); glBindTexture(GL_TEXTURE_2D, __layers[i]->alpha.general_Alpha.alphaMapProceduralTxtr.ID);
        ShaderSystem::layersUpdate().setFloat("generalOpacity", __layers[i]->alpha.general_Alpha.alphaValue);
        ShaderSystem::layersUpdate().setInt("albedoMaskTxtr", 7); glActiveTexture(GL_TEXTURE7); glBindTexture(GL_TEXTURE_2D, __layers[i]->alpha.albedo_Alpha.alphaMapProceduralTxtr.ID);
        ShaderSystem::layersUpdate().setFloat("albedoOpacity", __layers[i]->alpha.albedo_Alpha.alphaValue);
        ShaderSystem::layersUpdate().setInt("roughnessMaskTxtr", 8); glActiveTexture(GL_TEXTURE8); glBindTexture(GL_TEXTURE_2D, __layers[i]->alpha.roughness_Alpha.alphaMapProceduralTxtr.ID);
        ShaderSystem::layersUpdate().setFloat("roughnessOpacity", __layers[i]->alpha.roughness_Alpha.alphaValue);
        ShaderSystem::layersUpdate().setInt("metallicMaskTxtr", 9); glActiveTexture(GL_TEXTURE9); glBindTexture(GL_TEXTURE_2D, __layers[i]->alpha.metallic_Alpha.alphaMapProceduralTxtr.ID);
        ShaderSystem::layersUpdate().setFloat("metallicOpacity", __layers[i]->alpha.metallic_Alpha.alphaValue);
        ShaderSystem::layersUpdate().setInt("normalMapMaskTxtr", 10); glActiveTexture(GL_TEXTURE10); glBindTexture(GL_TEXTURE_2D, __layers[i]->alpha.normalMap_Alpha.alphaMapProceduralTxtr.ID);
        ShaderSystem::layersUpdate().setFloat("normalMapOpacity", __layers[i]->alpha.normalMap_Alpha.alphaValue);
        ShaderSystem::layersUpdate().setInt("heightMapMaskTxtr", 11); glActiveTexture(GL_TEXTURE11); glBindTexture(GL_TEXTURE_2D, __layers[i]->alpha.heightMap_Alpha.alphaMapProceduralTxtr.ID);
        ShaderSystem::layersUpdate().setFloat("heightMapOpacity", __layers[i]->alpha.heightMap_Alpha.alphaValue);
        ShaderSystem::layersUpdate().setInt("ambientOcclusionMaskTxtr", 12); glActiveTexture(GL_TEXTURE12); glBindTexture(GL_TEXTURE_2D, __layers[i]->alpha.ambientOcclusion_Alpha.alphaMapProceduralTxtr.ID);
        ShaderSystem::layersUpdate().setFloat("ambientOcclusionOpacity", __layers[i]->alpha.ambientOcclusion_Alpha.alphaValue);
   
        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "layers_update_result");
    }
    
}

bool layers_any_dialog_active(){
    for (size_t i = 0; i < __layers.size(); i++)
    {
        if(__layers[i]->elementSelectionMode || __layers[i]->alphaSettingsMode || __layers[i]->infoMode || __layers[i]->renamingMode || __layers[i]->rightClicked)
            return true;
    }
    return false;
}

MaterialChannels layers_get_painting_channels(bool* success){
    for (size_t i = 0; i < __layers.size(); i++)
    {
        if(__layers[i]->mainSelected){
            *success = true;
            if(__layers[i]->layerType == "painting")
                return __layers[i]->result;
        }
    }
    
    *success = false;
    return MaterialChannels();
}