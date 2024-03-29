/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Updating the source texture after painting is released

*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "GUI/GUI.hpp"
#include "GUI/Panels.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Library/Library.hpp"
#include "Toolkit/Layers/Layers.hpp"

#include <string>
#include <iostream>
#include <vector>

// Defined in the RenderPanel.cpp
extern bool updateThePreRenderedPanels;

static Mesh customMatMesh;

void Painter::updateTexture(int paintingMode){
    
    if(twoD_painting_mode && panel_displaying_modes.selectedElement != 2){
        LGDLOG::start << "ERROR : Painting : Invalid displaying mode for the 2D painting" << LGDLOG::end;
        return;
    }

    if(button_mesh_selection.selectedMeshI >= getScene()->model->meshes.size()){
        LGDLOG::start << "ERROR : Painting : Invalid selected mesh" << LGDLOG::end;
        return;
    }
    
    if(panel_displaying_modes.selectedElement == 2 && (!panel_library_selected_texture.ID || glIsTexture(panel_library_selected_texture.ID) == GL_FALSE)){
        LGDLOG::start << "ERROR : Painting : Selected texture is invalid" << LGDLOG::end;
        return;
    }

    bool channelsSuccesss;
    MaterialChannels channels = getScene()->get_selected_mesh()->layerScene.get_painting_channels(&channelsSuccesss);
    if(panel_displaying_modes.selectedElement == 1 && !channelsSuccesss){
        LGDLOG::start << "ERROR : Painting : Select a painting layer to paint" << LGDLOG::end;
        return;
    }

    // Selected texture index in the library
    int txtrI = -1;
    for (size_t i = 0; i < Library::getTextureArraySize(); i++)
    {
        if(panel_library_selected_texture.ID == Library::getTexture(i)->ID)
            txtrI = i;
    }
    
    if(checkComboList_painting_color.panel.sections[0].elements[13].checkBox.clickState1){
        glm::vec2 res = getScene()->get_selected_mesh()->albedo.getResolution();
        customMatMesh.EBO = getScene()->get_selected_mesh()->EBO;
        customMatMesh.VBO = getScene()->get_selected_mesh()->VBO;
        customMatMesh.VAO = getScene()->get_selected_mesh()->VAO;
        customMatMesh.indices = getScene()->get_selected_mesh()->indices;
        
        if(!customMatMesh.albedo.ID){
            customMatMesh.albedo = Texture(nullptr, res.x, res.y);
            customMatMesh.roughness = Texture(nullptr, res.x, res.y);
            customMatMesh.metallic = Texture(nullptr, res.x, res.y);
            customMatMesh.normalMap = Texture(nullptr, res.x, res.y);
            customMatMesh.heightMap = Texture(nullptr, res.x, res.y);
            customMatMesh.ambientOcclusion = Texture(nullptr, res.x, res.y);
        }
        else{
            customMatMesh.albedo.update(nullptr, res.x, res.y);
            customMatMesh.roughness.update(nullptr, res.x, res.y);
            customMatMesh.metallic.update(nullptr, res.x, res.y);
            customMatMesh.normalMap.update(nullptr, res.x, res.y);
            customMatMesh.heightMap.update(nullptr, res.x, res.y);
            customMatMesh.ambientOcclusion.update(nullptr, res.x, res.y);
        }

        Material* painting_custom_mat = &checkComboList_painting_color.panel.sections[0].elements[14].button.material;

        for (int i = painting_custom_mat->materialModifiers.size() - 1; i >= 0; --i)    
        {
            painting_custom_mat->materialModifiers[i].updateMaterialChannels(*painting_custom_mat, customMatMesh, res.x, i, false, *getScene()->model);
        }
    }

    if(panel_displaying_modes.selectedElement == 1){
        registerPaintingAction(
                                    "Multi-channel painting", 
                                    Texture(), 
                                    channels.albedo, checkComboList_painting_color.panel.sections[0].elements[1].checkBox.clickState1, 
                                    channels.roughness, checkComboList_painting_color.panel.sections[0].elements[3].checkBox.clickState1,
                                    channels.metallic, checkComboList_painting_color.panel.sections[0].elements[5].checkBox.clickState1,
                                    channels.normalMap, checkComboList_painting_color.panel.sections[0].elements[7].checkBox.clickState1,
                                    channels.heightMap, checkComboList_painting_color.panel.sections[0].elements[9].checkBox.clickState1,
                                    channels.ambientOcclusion, checkComboList_painting_color.panel.sections[0].elements[11].checkBox.clickState1
                                );
    }
    else if(txtrI != -1){
        
        std::string actionTitle = "Unknown painting mode";
        
        if(paintingMode == 0)
            actionTitle = "Painting a texture";
        if(paintingMode == 1)
            actionTitle = "Softening a texture";
        if(paintingMode == 2)
            actionTitle = "Smearing a texture";
        if(paintingMode == 3)
            actionTitle = "Normal painting a texture";
        if(paintingMode == 4)
            actionTitle = "Filter painting a texture";

        registerPaintingAction(
                                    actionTitle, 
                                    Texture(), 
                                    panel_library_selected_texture, true, 
                                    Texture(), false,
                                    Texture(), false,
                                    Texture(), false,
                                    Texture(), false,
                                    Texture(), false
                                );
    }

    if(paintingMode == 4){
        button_painting_filter_mode_filter.filter.applyFilter(panel_library_selected_texture.ID, this->projectedPaintingTexture, 0);
    }
    else{
        if(panel_displaying_modes.selectedElement == 1)
        {
            for (size_t i = 0; i < 6; i++)
            {
                glm::vec3 clr;
                bool enableChannel;
                Texture txtr;
                Texture customMatTxtr;
                if(i == 0){
                    clr = checkComboList_painting_color.panel.sections[0].elements[2].painterColorSelection.getSelectedColor().getRGB_normalized();
                    enableChannel = checkComboList_painting_color.panel.sections[0].elements[1].checkBox.clickState1;
                    txtr = channels.albedo;
                    customMatTxtr = customMatMesh.albedo;
                }
                if(i == 1){
                    clr = glm::vec3(checkComboList_painting_color.panel.sections[0].elements[4].rangeBar.value);
                    enableChannel = checkComboList_painting_color.panel.sections[0].elements[3].checkBox.clickState1;
                    txtr = channels.roughness;
                    customMatTxtr = customMatMesh.roughness;
                }
                if(i == 2){
                    clr = glm::vec3(checkComboList_painting_color.panel.sections[0].elements[6].rangeBar.value);
                    enableChannel = checkComboList_painting_color.panel.sections[0].elements[5].checkBox.clickState1;
                    txtr = channels.metallic;
                    customMatTxtr = customMatMesh.metallic;
                }
                if(i == 3){
                    clr = glm::vec3(checkComboList_painting_color.panel.sections[0].elements[8].rangeBar.value);
                    enableChannel = checkComboList_painting_color.panel.sections[0].elements[7].checkBox.clickState1;
                    txtr = channels.normalMap;
                    customMatTxtr = customMatMesh.normalMap;
                }
                if(i == 4){
                    clr = glm::vec3(checkComboList_painting_color.panel.sections[0].elements[10].rangeBar.value);
                    enableChannel = checkComboList_painting_color.panel.sections[0].elements[9].checkBox.clickState1;
                    txtr = channels.heightMap;
                    customMatTxtr = customMatMesh.heightMap;
                }
                if(i == 5){
                    clr = glm::vec3(checkComboList_painting_color.panel.sections[0].elements[12].rangeBar.value);
                    enableChannel = checkComboList_painting_color.panel.sections[0].elements[11].checkBox.clickState1;
                    txtr = channels.ambientOcclusion;
                    customMatTxtr = customMatMesh.ambientOcclusion;
                }

                if(enableChannel){
                    if(!checkComboList_painting_color.panel.sections[0].elements[13].checkBox.clickState1)
                        updateTheTexture(txtr, paintingMode, clr, i, clr.r);
                    else{
                        txtr.mix(customMatTxtr, projectedPaintingTexture, true, false, false);
                        txtr.removeSeams(*getScene()->get_selected_mesh(), txtr.getResolution());
                    }
                }
            }
        }
        else{
            updateTheTexture(panel_library_selected_texture, paintingMode, checkComboList_painting_color.panel.sections[0].elements[2].painterColorSelection.getSelectedColor().getRGB_normalized(), 0, 1.f);
        }
    }

    if(txtrI != -1){
        Library::getTexture(txtrI)->copyDataToTheCopyContext();
        projectUpdatingThreadElements.updateTextures = true;
    }

    updateThePreRenderedPanels = true;
}

//Clear the painting texture
void Painter::refreshPainting(){
    glClearColor(0,0,0,0);

    this->paintingFBO.bind();
    this->paintingFBO.setColorBuffer(this->paintingTexture16f, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    
    this->paintingFBO.setColorBuffer(this->paintingTexture8, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    
    this->paintingFBO.setColorBuffer(this->projectedPaintingTexture.ID, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    
    this->paintingFBO.setColorBuffer(this->oSide.projectedPaintingTexture.ID, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    
    this->paintingFBO.setColorBuffer(this->oXSide.projectedPaintingTexture.ID, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    
    this->paintingFBO.setColorBuffer(this->oYSide.projectedPaintingTexture.ID, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    
    this->paintingFBO.setColorBuffer(this->oXYSide.projectedPaintingTexture.ID, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    
    this->paintingFBO.setColorBuffer(this->oZSide.projectedPaintingTexture.ID, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    
    this->paintingFBO.setColorBuffer(this->oXZSide.projectedPaintingTexture.ID, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    
    this->paintingFBO.setColorBuffer(this->oYZSide.projectedPaintingTexture.ID, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    
    this->paintingFBO.setColorBuffer(this->oXYZSide.projectedPaintingTexture.ID, GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    
    Settings::defaultFramebuffer()->FBO.bind();
}