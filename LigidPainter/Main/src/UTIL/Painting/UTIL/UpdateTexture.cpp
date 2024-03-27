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

static void captureTxtrToSourceTxtr(unsigned int &captureTexture, glm::ivec2 textureRes, unsigned int &selectedTextureID){
    //Bind the capture texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, captureTexture);
    
    //Get the pixels of the capture texture
    char* pixels = new char[textureRes.x * textureRes.y * 4];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_BYTE, pixels);
    
    Texture selectedTextureOBJ = Texture(selectedTextureID);
    selectedTextureOBJ.update(pixels, textureRes.x, textureRes.y);

    delete[] pixels; //Remove the capture texture's pixels out of the memory
    glDeleteTextures(1, &captureTexture);
}

void Painter::updateTheTexture(Texture txtr, int paintingMode, glm::vec3 paintingColor, int channelI, float channelStr){
    glm::vec2 destScale = glm::vec2(txtr.getResolution());

    glActiveTexture(GL_TEXTURE0);

    Texture captureTexture = Texture(nullptr, destScale.x, destScale.y, GL_LINEAR);
    Framebuffer captureFBO = Framebuffer(captureTexture, GL_TEXTURE_2D, "Painter::updateTheTexture");
    
    captureFBO.bind();

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Bind the selected texture (painted texture) to the albedo channel (to paint over that texture)
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, txtr.ID);

    //Set the viewport to the resolution of the texture
    glViewport(0,0,destScale.x,destScale.y);
    
    //Since the UV is between 0 - 1
    glm::mat4 orthoProjection = glm::ortho(0.f,1.f,0.f,1.f);

    ShaderSystem::buttonShader().use();
    
    ShaderSystem::buttonShader().setMat4("projection", glm::ortho(0.f, destScale.x, destScale.y, 0.f));
    ShaderSystem::buttonShader().setVec3("pos", glm::vec3(destScale.x  / 2.f, destScale.y / 2.f, 0.1));
    ShaderSystem::buttonShader().setVec2("scale", glm::vec2(destScale / 2.f));
    ShaderSystem::buttonShader().setFloat("properties.colorMixVal", 0.f);
    ShaderSystem::buttonShader().setInt("states.renderTexture",     1    );
    ShaderSystem::buttonShader().setVec2("properties.txtrScale", glm::vec2(1.f));
    ShaderSystem::buttonShader().setInt("properties.txtr",     0    );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, txtr.ID);
    
    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Painter::updateTheTexture : Rendering the original texture (background)");

    ShaderSystem::buttonShader().setInt("states.renderTexture"  ,     0    );

    ShaderSystem::textureUpdatingShader().use();

    //*Fragment
    ShaderSystem::textureUpdatingShader().setInt("txtr", 5);
    ShaderSystem::textureUpdatingShader().setInt("paintingTexture", 6);
    ShaderSystem::textureUpdatingShader().setInt("brushModeState", paintingMode);
    ShaderSystem::textureUpdatingShader().setInt("usePaintingOver", checkComboList_painting_over.panel.sections[0].elements[0].checkBox.clickState1);
    ShaderSystem::textureUpdatingShader().setFloat("smearTransformStrength", panel_smear_painting_properties.sections[0].elements[0].rangeBar.value);
    ShaderSystem::textureUpdatingShader().setFloat("smearBlurStrength", panel_smear_painting_properties.sections[0].elements[1].rangeBar.value);
    ShaderSystem::textureUpdatingShader().setInt("multiChannelsPaintingMod", panel_displaying_modes.selectedElement == 1);
    ShaderSystem::textureUpdatingShader().setInt("channelI", channelI);
    ShaderSystem::textureUpdatingShader().setFloat("channelStrength", channelStr);

    //* Bind the textures
    //painted texture
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, txtr.ID);
    
    ///@ref paintingTexture 
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, this->projectedPaintingTexture.ID);
    
    
    if(!twoD_painting_mode){
        //Draw the UV of the selected model
        {
            //*Vertex
            ShaderSystem::textureUpdatingShader().setMat4("orthoProjection", orthoProjection);
            ShaderSystem::textureUpdatingShader().setMat4("perspectiveProjection", getScene()->projectionMatrix);
            ShaderSystem::textureUpdatingShader().setMat4("view", getScene()->camera.viewMatrix);

            getScene()->get_selected_mesh()->Draw(false);         
        }
    }
    else{
        //*Fragment
        ShaderSystem::projectingPaintedTextureShader().setInt("doDepthTest", 0);

        //*Vertex
        ShaderSystem::projectingPaintedTextureShader().setMat4("orthoProjection", glm::ortho(0.f,1.f,0.f,1.f));
        ShaderSystem::projectingPaintedTextureShader().setMat4("perspectiveProjection", getContext()->ortho_projection);
        ShaderSystem::projectingPaintedTextureShader().setMat4("view", glm::mat4(1.));
        
        twoD_painting_box.bindBuffers();
        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Painter::updateTheTexture : Applying painting to the texture");
    }
    
    if(!twoD_painting_mode){
        captureTexture.removeSeams(*getScene()->get_selected_mesh(), destScale);
    }

    //Delete the capture framebuffer
    captureFBO.deleteBuffers(false, false);

    //Copy capture texture into the source texture (painted texture)
    captureTxtrToSourceTxtr(captureTexture.ID, destScale, txtr.ID);
}

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
        if(panel_displaying_modes.selectedElement == 1){
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