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

void layers_render(Timer& timer, Panel &layerPanel){
    int count = 0;
    for (int i = __layers.size() -1; i >= 0; i--)
    {
        glm::vec2 btnScale = glm::vec2(layerPanel.scale.x, 2.f); 
        glm::vec3 btnPos = glm::vec3(layerPanel.pos.x, layerPanel.pos.y - layerPanel.scale.y  + btnScale.y + btnScale.y * (count * 2), layerPanel.pos.z);
        int layerMSG = __layers[i]->render_graphics(timer, true, btnPos, btnScale);
        
        if(layerMSG == 1){
            __layers.erase(__layers.begin() + i);
            i--;
        }
        
        count++;
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

    // Bind FBO
    layers_update_FBO.bind();

    //TODO Init channel textures

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
        ShaderSystem::layersUpdate().setInt("roughnessTxtr", 1); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, __layers[i]->result.roughness.ID);
        ShaderSystem::layersUpdate().setInt("metallicTxtr", 2); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, __layers[i]->result.metallic.ID);
        ShaderSystem::layersUpdate().setInt("normalMapTxtr", 3); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, __layers[i]->result.normalMap.ID);
        ShaderSystem::layersUpdate().setInt("heightMapTxtr", 4); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, __layers[i]->result.heightMap.ID);
        ShaderSystem::layersUpdate().setInt("ambientOcclusionTxtr", 5); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, __layers[i]->result.ambientOcclusion.ID);

        ShaderSystem::layersUpdate().setInt("generalMaskTxtr", 6); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, __layers[i]->alpha.general_Alpha.alphaMap.ID);
        ShaderSystem::layersUpdate().setFloat("generalOpacity", __layers[i]->alpha.general_Alpha.alphaValue);
        ShaderSystem::layersUpdate().setInt("albedoMaskTxtr", 7); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, __layers[i]->alpha.albedo_Alpha.alphaMap.ID);
        ShaderSystem::layersUpdate().setFloat("albedoOpacity", __layers[i]->alpha.albedo_Alpha.alphaValue);
        ShaderSystem::layersUpdate().setInt("roughnessMaskTxtr", 8); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, __layers[i]->alpha.roughness_Alpha.alphaMap.ID);
        ShaderSystem::layersUpdate().setFloat("roughnessOpacity", __layers[i]->alpha.roughness_Alpha.alphaValue);
        ShaderSystem::layersUpdate().setInt("metallicMaskTxtr", 9); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, __layers[i]->alpha.metallic_Alpha.alphaMap.ID);
        ShaderSystem::layersUpdate().setFloat("metallicOpacity", __layers[i]->alpha.metallic_Alpha.alphaValue);
        ShaderSystem::layersUpdate().setInt("normalMapMaskTxtr", 10); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, __layers[i]->alpha.normalMap_Alpha.alphaMap.ID);
        ShaderSystem::layersUpdate().setFloat("normalMapOpacity", __layers[i]->alpha.normalMap_Alpha.alphaValue);
        ShaderSystem::layersUpdate().setInt("heightMapMaskTxtr", 11); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, __layers[i]->alpha.heightMap_Alpha.alphaMap.ID);
        ShaderSystem::layersUpdate().setFloat("heightMapOpacity", __layers[i]->alpha.heightMap_Alpha.alphaValue);
        ShaderSystem::layersUpdate().setInt("ambientOcclusionMaskTxtr", 12); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, __layers[i]->alpha.ambientOcclusion_Alpha.alphaMap.ID);
        ShaderSystem::layersUpdate().setFloat("ambientOcclusionOpacity", __layers[i]->alpha.ambientOcclusion_Alpha.alphaValue);
   
        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "layers_update_result");
    }
    
}