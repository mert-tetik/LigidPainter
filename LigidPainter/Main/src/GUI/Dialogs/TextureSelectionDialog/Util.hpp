/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "LibrarySystem/Library.hpp"
#include "MouseSystem/Mouse.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

// ---------- UTILITY FUNCTIONS -----------
static void initTextureSelectionDialog(
                                        int &selectedTextureMode, 
                                        Panel& subPanel, 
                                        Panel& subPanelTxtrPack, 
                                        Panel& subPanelSmartTextures, 
                                        int& selectedTextureIndex, 
                                        Texture& receivedTexture
                                    )
{

    
    selectedTextureMode = receivedTexture.proceduralProps.textureSelectionDialog_selectedMode;
    selectedTextureIndex = receivedTexture.proceduralProps.textureSelectionDialog_selectedTextureIndex;

    // Texture pack mode
    if(selectedTextureMode == 3){
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Invert_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralnverted;
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Normal_Map_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralNormalMap;
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Normal_Gray_Scale_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralNormalGrayScale;
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Normal_Strength_INDEX].rangeBar.value = receivedTexture.proceduralProps.proceduralNormalStrength;
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Gray_Scale_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralGrayScale;
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Brightness_INDEX].rangeBar.value = receivedTexture.proceduralProps.proceduralBrightness;   
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Scale_INDEX].rangeBar.value = receivedTexture.proceduralProps.txtrPackScale;   
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Count_INDEX].rangeBar.value = receivedTexture.proceduralProps.txtrPackCount;   
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Rotation_Jitter_INDEX].rangeBar.value = receivedTexture.proceduralProps.txtrPackRotation_Jitter;   
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Size_Jitter_INDEX].rangeBar.value = receivedTexture.proceduralProps.txtrPackSize_Jitter;   
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Opacity_Jitter_INDEX].rangeBar.value = receivedTexture.proceduralProps.txtrPackOpacity_Jitter;   
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Scatter_INDEX].rangeBar.value = receivedTexture.proceduralProps.txtrPackScatter;   
    }
    
    // Smart texture mode
    else if(selectedTextureMode == 4){
        subPanelSmartTextures.sections[0].elements[subPanelSmartTextures_Invert_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralnverted;
        subPanelSmartTextures.sections[0].elements[subPanelSmartTextures_Normal_Map_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralNormalMap;
        subPanelSmartTextures.sections[0].elements[subPanelSmartTextures_Normal_Gray_Scale_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralNormalGrayScale;
        subPanelSmartTextures.sections[0].elements[subPanelSmartTextures_Normal_Strength_INDEX].rangeBar.value = receivedTexture.proceduralProps.proceduralNormalStrength;
        subPanelSmartTextures.sections[0].elements[subPanelSmartTextures_Brightness_INDEX].rangeBar.value = receivedTexture.proceduralProps.proceduralBrightness;
    }

    // Other texture modes
    else{
        subPanel.sections[0].elements[subPanel_Invert_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralnverted;
        subPanel.sections[0].elements[subPanel_Scale_INDEX].rangeBar.value = receivedTexture.proceduralProps.proceduralScale * 10.f;  
        subPanel.sections[0].elements[subPanel_Normal_Map_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralNormalMap;
        subPanel.sections[0].elements[subPanel_Normal_Gray_Scale_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralNormalGrayScale;
        subPanel.sections[0].elements[subPanel_Normal_Strength_INDEX].rangeBar.value = receivedTexture.proceduralProps.proceduralNormalStrength;
        subPanel.sections[0].elements[subPanel_Gray_Scale_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralGrayScale;
        subPanel.sections[0].elements[subPanel_Brightness_INDEX].rangeBar.value = receivedTexture.proceduralProps.proceduralBrightness;
    }

}

static void drawBG(
                    unsigned int bgTexture 
                )
{
    ShaderSystem::defaultFramebufferShader().use();
    ShaderSystem::defaultFramebufferShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    ShaderSystem::defaultFramebufferShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
    ShaderSystem::defaultFramebufferShader().setVec2("scale", glm::vec2(0.5f));
    
    ShaderSystem::defaultFramebufferShader().setVec2("resolution", Settings::defaultFramebuffer()->resolution);
    ShaderSystem::defaultFramebufferShader().setInt("txtr", 0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bgTexture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    ShaderSystem::buttonShader().use();
}

static void updateTextureSelectingPanelElements(Panel& textureSelectingPanel, int selectedTextureMode, std::vector<Texture> smartTextureDisplayingTextures){
    textureSelectingPanel.sections.clear();
    std::vector<Element> sectionElements;
    if(selectedTextureMode == 0){
        for (size_t i = 0; i < Library::getTextureArraySize(); i++)
        {
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f), "", *Library::getTexture(i), 0.f,false)));
        }
    }
    else if(selectedTextureMode == 1){
        for (size_t i = 0; i < MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE; i++)
        {
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,3.f),""       , Texture(), 0.f,false)));
        }
    }
    else if(selectedTextureMode == 2){
        for (size_t i = 0; i < MAX_PROCEDURAL_NOISE_TEXTURE_SIZE; i++)
        {
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,3.f),""       , Texture(), 0.f,false)));
        }
    }
    else if(selectedTextureMode == 3){
        for (size_t i = 0; i < Library::getTexturePackArraySize(); i++)
        {
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,3.f),Library::getTexturePack(i)->title   , Texture(), 0.f,false)));
        }
    }
    else if(selectedTextureMode == 4){
        for (size_t i = 0; i < MAX_PROCEDURAL_SMART_TEXTURE_SIZE; i++)
        {
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,3.f),""       , smartTextureDisplayingTextures[i], 0.f,false)));
        }
    }
    else if(selectedTextureMode == 5){
        for (size_t i = 0; i < Library::getgetSrcLibTxtrsArraySize(); i++)
        {
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,3.f), "", Library::getSrcLibTxtr(i).displayingTexture, 0.f,false)));
        }
    }
    else if(selectedTextureMode == 6){
        for (size_t i = 0; i < getModel()->meshes.size(); i++)
        {
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,3.f),getModel()->meshes[i].materialName       , getModel()->meshes[i].uvMask, 0.f,false)));
        }
    }

    textureSelectingPanel.sections.push_back
                                            (
                                                Section
                                                        (
                                                            Element(Button()),
                                                            sectionElements
                                                        )
                                            );  
}

static void updateTextureModesPanel(Panel& textureModesPanel, int& selectedTextureMode, int& selectedTextureIndex){
    for (size_t i = 0; i < 7; i++)
    {
        if(textureModesPanel.sections[0].elements[i].button.clickState1 && selectedTextureMode != i){
            selectedTextureMode = i;
            selectedTextureIndex = 0;
            for (size_t i = 0; i < textureModesPanel.sections[0].elements.size(); i++){
                textureModesPanel.sections[0].elements[i].button.clickState1 = false;
            }
        }
        if(selectedTextureMode == i){
            textureModesPanel.sections[0].elements[i].button.clickState1 = true;
        }
        else{
            textureModesPanel.sections[0].elements[i].button.clickState1 = false;
        }
    }
}

void TextureSelectionDialog::selectTheTexture(Texture& receivedTexture, int displayingTextureRes){
    
    // ------------------ Seting the proceduralID & proceduralTextureID values ------------------

    if(this->selectedTextureMode == 0){
        receivedTexture.proceduralProps.proceduralID = -1;
        if(selectedTextureIndex < Library::getTextureArraySize())
            receivedTexture.proceduralProps.proceduralTextureID = Library::getTexture(selectedTextureIndex)->ID;
    }
    else if(this->selectedTextureMode == 1){
        receivedTexture.proceduralProps.proceduralID = this->selectedTextureIndex;
        receivedTexture.proceduralProps.proceduralTextureID = 0;
    }
    else if(this->selectedTextureMode == 2){
        receivedTexture.proceduralProps.proceduralID = this->selectedTextureIndex + MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE;
        receivedTexture.proceduralProps.proceduralTextureID = 0;
    }
    else if(this->selectedTextureMode == 3){
        receivedTexture.proceduralProps.proceduralID = 1000;
        receivedTexture.proceduralProps.proceduralTextureID = 0;
    }
    else if(this->selectedTextureMode == 4){
        receivedTexture.proceduralProps.proceduralID = this->selectedTextureIndex + MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE + MAX_PROCEDURAL_NOISE_TEXTURE_SIZE;
        receivedTexture.proceduralProps.proceduralTextureID = 0;
    }
    else if(this->selectedTextureMode == 5){
        receivedTexture.proceduralProps.proceduralID = -1;
        if(selectedTextureIndex < Library::getgetSrcLibTxtrsArraySize()){
            if(receivedTexture.ID == this->displayingTexture.ID)
                receivedTexture.proceduralProps.proceduralTextureID = Library::getSrcLibTxtr(selectedTextureIndex).displayingTexture.ID;
            else{
                receivedTexture.proceduralProps.proceduralTextureID = Library::getSrcLibTxtr(selectedTextureIndex).getTexture().ID;
            }
        }
    }
    else if(this->selectedTextureMode == 6){
        receivedTexture.proceduralProps.proceduralID = -1;
        if(selectedTextureIndex < getModel()->meshes.size())
            receivedTexture.proceduralProps.proceduralTextureID = getModel()->meshes[selectedTextureIndex].uvMask.ID;
    }
    

    // ------------------ Seting other properties related to the property panels ------------------

    // Texture packs
    if(this->selectedTextureMode == 3){
        receivedTexture.proceduralProps.proceduralScale = 1.f;
        receivedTexture.proceduralProps.proceduralnverted = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Invert_INDEX].checkBox.clickState1;
        receivedTexture.proceduralProps.proceduralNormalMap = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Normal_Map_INDEX].checkBox.clickState1;
        receivedTexture.proceduralProps.proceduralNormalGrayScale = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Normal_Gray_Scale_INDEX].checkBox.clickState1;
        receivedTexture.proceduralProps.proceduralNormalStrength = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Normal_Strength_INDEX].rangeBar.value;
        receivedTexture.proceduralProps.proceduralUseTexCoords = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Use_Texture_Coordinates_INDEX].checkBox.clickState1;
        receivedTexture.proceduralProps.proceduralGrayScale = this->subPanel.sections[0].elements[subPanelTxtrPack_Gray_Scale_INDEX].checkBox.clickState1;
        receivedTexture.proceduralProps.proceduralBrightness = this->subPanel.sections[0].elements[subPanelTxtrPack_Brightness_INDEX].rangeBar.value;
        
        receivedTexture.proceduralProps.txtrPackScale = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Scale_INDEX].rangeBar.value;
        receivedTexture.proceduralProps.txtrPackCount = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Count_INDEX].rangeBar.value;
        receivedTexture.proceduralProps.txtrPackRotation_Jitter = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Rotation_Jitter_INDEX].rangeBar.value;
        receivedTexture.proceduralProps.txtrPackSize_Jitter = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Size_Jitter_INDEX].rangeBar.value;
        receivedTexture.proceduralProps.txtrPackOpacity_Jitter = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Opacity_Jitter_INDEX].rangeBar.value;
        receivedTexture.proceduralProps.txtrPackScatter = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Scatter_INDEX].rangeBar.value;
    }
    
    // Smart textures
    else if(this->selectedTextureMode == 4){
        receivedTexture.proceduralProps.proceduralScale = 1.f;
        receivedTexture.proceduralProps.proceduralnverted = this->subPanelSmartTextures.sections[0].elements[subPanelSmartTextures_Invert_INDEX].checkBox.clickState1;
        receivedTexture.proceduralProps.proceduralBrightness = this->subPanelSmartTextures.sections[0].elements[subPanelSmartTextures_Brightness_INDEX].rangeBar.value;
        receivedTexture.proceduralProps.proceduralNormalMap = this->subPanelSmartTextures.sections[0].elements[subPanelSmartTextures_Normal_Map_INDEX].checkBox.clickState1;
        receivedTexture.proceduralProps.proceduralNormalGrayScale = this->subPanelSmartTextures.sections[0].elements[subPanelSmartTextures_Normal_Gray_Scale_INDEX].checkBox.clickState1;
        receivedTexture.proceduralProps.proceduralNormalStrength = this->subPanelSmartTextures.sections[0].elements[subPanelSmartTextures_Normal_Strength_INDEX].rangeBar.value;
        receivedTexture.proceduralProps.proceduralUseTexCoords = false;

        Panel* smartPropPanel;
        bool skipPanel = false;
        if(this->selectedTextureIndex == 0 || selectedTextureIndex == 1 || selectedTextureIndex == 2){
            smartPropPanel = &this->smartPositionTexturePanel;
        }
        else if(this->selectedTextureIndex == 3 || selectedTextureIndex == 4){
            smartPropPanel = &this->smartStripesTexturePanel;
        }
        else if(this->selectedTextureIndex == 5){
            smartPropPanel = &this->edgeWearTexturePanel;
        }
        else if(this->selectedTextureIndex == 6){
            smartPropPanel = &this->smartDistanceTexturePanel;
        }
        else{
            skipPanel = true;
        }
        if(!skipPanel){
            receivedTexture.proceduralProps.smartProperties = glm::vec4(
                                                                            smartPropPanel->sections[0].elements[0].rangeBar.value,
                                                                            smartPropPanel->sections[0].elements[1].rangeBar.value,
                                                                            smartPropPanel->sections[0].elements[2].rangeBar.value,
                                                                            smartPropPanel->sections[0].elements[3].rangeBar.value
                                                                        );
        }
    }

    // Other
    else{
        receivedTexture.proceduralProps.proceduralScale = this->subPanel.sections[0].elements[subPanel_Scale_INDEX].rangeBar.value / 10.f;
        receivedTexture.proceduralProps.proceduralnverted = this->subPanel.sections[0].elements[subPanel_Invert_INDEX].checkBox.clickState1;
        receivedTexture.proceduralProps.proceduralNormalMap = this->subPanel.sections[0].elements[subPanel_Normal_Map_INDEX].checkBox.clickState1;
        receivedTexture.proceduralProps.proceduralNormalGrayScale = this->subPanel.sections[0].elements[subPanel_Normal_Gray_Scale_INDEX].checkBox.clickState1;
        receivedTexture.proceduralProps.proceduralNormalStrength = this->subPanel.sections[0].elements[subPanel_Normal_Strength_INDEX].rangeBar.value;
        receivedTexture.proceduralProps.proceduralUseTexCoords = this->subPanel.sections[0].elements[subPanel_Use_Texture_Coordinates_INDEX].checkBox.clickState1;
        receivedTexture.proceduralProps.proceduralGrayScale = this->subPanel.sections[0].elements[subPanel_Gray_Scale_INDEX].checkBox.clickState1;
        receivedTexture.proceduralProps.proceduralBrightness = this->subPanel.sections[0].elements[subPanel_Brightness_INDEX].rangeBar.value;
    }

    // ------------------ Finish ------------------

    // Set other data
    receivedTexture.proceduralProps.textureSelectionDialog_selectedMode = this->selectedTextureMode;
    receivedTexture.proceduralProps.textureSelectionDialog_selectedTextureIndex = this->selectedTextureIndex;
    receivedTexture.title = "SelectedTexture";

    // Generate the displaying texture of the selected texture
    receivedTexture.generateProceduralDisplayingTexture(displayingTextureRes);
    
}

void TextureSelectionDialog::renderPanels(Timer& timer, glm::mat4 guiProjection){
    // Background panel
    this->bgPanel.render(timer, true);

    // Texture property panel (right handed panel)
    if(this->selectedTextureMode == 3)
        this->subPanelTxtrPack.render(timer, true);
    else if(this->selectedTextureMode == 4)
        this->subPanelSmartTextures.render(timer, true);
    else
        this->subPanel.render(timer, true);

    // Texture modes panel (left handed panel)
    textureModesPanel.render(timer, true);

    // Positioning the selected texture displayer panel & the texture selection panel (to the center)
    selectedTextureDisplayingPanel.scale.x = this->scale.x - subPanel.scale.x - textureModesPanel.scale.x;
    selectedTextureDisplayingPanel.pos.x = this->pos.x + subPanel.scale.x - textureModesPanel.scale.x;
    textureSelectingPanel.pos.x = selectedTextureDisplayingPanel.pos.x;
    textureSelectingPanel.scale.x = selectedTextureDisplayingPanel.scale.x;

    // Update the displaying panel's displaying button's texture
    selectedTextureDisplayingPanel.sections[0].elements[0].button.texture = displayingTexture;

    // Seting the selected texture displaying panel's row count
    if(this->selectedTextureMode == 4)
        this->textureSelectingPanel.rowCount = 7;    
    else
        this->textureSelectingPanel.rowCount = 12;    
    this->textureSelectingPanel.render(timer, true);

    // Selected texture displaying panel
    this->selectedTextureDisplayingPanel.render(timer, false);

    // Rendering the procedural textures on top of the texture selection panel's buttons
    if(this->selectedTextureMode == 1 || this->selectedTextureMode == 2){
        for (size_t i = 0; i < this->textureSelectingPanel.sections[0].elements.size(); i++)
        {
            ShaderSystem::proceduralDisplayerShader().use();
            ShaderSystem::proceduralDisplayerShader().setVec3("pos"         ,       this->textureSelectingPanel.sections[0].elements[i].button.resultPos);
            ShaderSystem::proceduralDisplayerShader().setVec2("scale"       ,       glm::vec2(std::min(this->textureSelectingPanel.sections[0].elements[i].button.resultScale.x, this->textureSelectingPanel.sections[0].elements[i].button.resultScale.y)));
            ShaderSystem::proceduralDisplayerShader().setMat4("projection"  ,       guiProjection);
            
            if(this->selectedTextureMode == 0)
                ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", -1);
            else if(this->selectedTextureMode == 1)
                ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", i);
            else if(this->selectedTextureMode == 2)
                ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", i + MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE);
            else if(this->selectedTextureMode == 4)
                ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", i + MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE + MAX_PROCEDURAL_NOISE_TEXTURE_SIZE);

            if(this->selectedTextureMode == 0)
                ShaderSystem::proceduralDisplayerShader().setFloat("proceduralScale", 1.f);
            else
                ShaderSystem::proceduralDisplayerShader().setFloat("proceduralScale", 2.5f);
            
            ShaderSystem::proceduralDisplayerShader().setInt("proceduralInverted", 0);
            ShaderSystem::proceduralDisplayerShader().setInt("proceduralGrayScale", 0);
            ShaderSystem::proceduralDisplayerShader().setFloat("proceduralBrightness", 1.f);
            
            ShaderSystem::proceduralDisplayerShader().setInt("proceduralTexture", 0);
            glActiveTexture(GL_TEXTURE0);
            if(this->selectedTextureMode == 0){
                if(i < Library::getTextureArraySize())
                    glBindTexture(GL_TEXTURE_2D, Library::getTexture(i)->ID);
            }
            else{
                if(i < getModel()->meshes.size())
                    glBindTexture(GL_TEXTURE_2D, getModel()->meshes[i].uvMask.ID);
            }
            

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }

    // Set the button shader program back
    ShaderSystem::buttonShader().use();

    // ------------- Rendering the smart texture property panels ------------

    if(smartPositionTexturePanelActive){
        this->smartPositionTexturePanel.render(timer, true);
        if((getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)) || (!this->smartPositionTexturePanel.hover && *Mouse::LClick())){
            smartPositionTexturePanelActive = false;
        }
    }

    if(smartStripesTexturePanelActive){
        this->smartStripesTexturePanel.render(timer, true);
        if((getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)) || (!this->smartStripesTexturePanel.hover && *Mouse::LClick())){
            smartStripesTexturePanelActive = false;
        }
    }
    if(edgeWearTexturePanelActive){
        this->edgeWearTexturePanel.render(timer, true);
        if((getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)) || (!this->edgeWearTexturePanel.hover && *Mouse::LClick())){
            edgeWearTexturePanelActive = false;
        }
    }
    if(smartDistanceTexturePanelActive){
        this->smartDistanceTexturePanel.render(timer, true);
        if((getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)) || (!this->smartDistanceTexturePanel.hover && *Mouse::LClick())){
            smartDistanceTexturePanelActive = false;
        }
    }
}