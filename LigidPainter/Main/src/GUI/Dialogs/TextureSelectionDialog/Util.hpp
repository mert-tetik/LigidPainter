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
void TextureSelectionDialog::initTextureSelectionDialog(
                                                            Texture& receivedTexture,
                                                            bool twoDMode
                                                        )
{

    
    selectedTextureMode = receivedTexture.proceduralProps.textureSelectionDialog_selectedMode;
    selectedTextureIndex = receivedTexture.proceduralProps.textureSelectionDialog_selectedTextureIndex;

    this->activeSelectedTextureDisplayingMode = !twoDMode;

    // Texture pack mode
    if(selectedTextureMode == 3){
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Invert_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralnverted;
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Normal_Map_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralNormalMap;
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Normal_Gray_Scale_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralNormalGrayScale;
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Normal_Strength_INDEX].rangeBar.value = receivedTexture.proceduralProps.proceduralNormalStrength;
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Gray_Scale_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralGrayScale;
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Brightness_INDEX].rangeBar.value = receivedTexture.proceduralProps.proceduralBrightness;   
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Scale_INDEX].rangeBar.value = receivedTexture.proceduralProps.txtrPackScale;   
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Stretch_INDEX].rangeBar.value = receivedTexture.proceduralProps.proceduralStretch;   
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_ScaleModelPos_INDEX].rangeBar.value = receivedTexture.proceduralProps.proceduralScaleModelPos;   
        subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_MirroredRepeat_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralMirroredRepeat;
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

        if(selectedTextureIndex == 0 || selectedTextureIndex == 1 || selectedTextureIndex == 2){
            this->smartPositionTexturePanel.sections[0].elements[0].rangeBar.value = receivedTexture.proceduralProps.smartProperties.x;
            this->smartPositionTexturePanel.sections[0].elements[1].rangeBar.value = receivedTexture.proceduralProps.smartProperties.y;
            this->smartPositionTexturePanel.sections[0].elements[2].rangeBar.value = receivedTexture.proceduralProps.smartProperties.z;
            this->smartPositionTexturePanel.sections[0].elements[3].rangeBar.value = receivedTexture.proceduralProps.smartProperties.w;
        }
        if(selectedTextureIndex == 3 || selectedTextureIndex == 4){
            this->smartStripesTexturePanel.sections[0].elements[0].rangeBar.value = receivedTexture.proceduralProps.smartProperties.x;
            this->smartStripesTexturePanel.sections[0].elements[1].rangeBar.value = receivedTexture.proceduralProps.smartProperties.y;
            this->smartStripesTexturePanel.sections[0].elements[2].rangeBar.value = receivedTexture.proceduralProps.smartProperties.z;
            this->smartStripesTexturePanel.sections[0].elements[3].rangeBar.value = receivedTexture.proceduralProps.smartProperties.w;
        }
        if(selectedTextureIndex == 5){
            this->edgeWearTexturePanel.sections[0].elements[0].rangeBar.value = receivedTexture.proceduralProps.smartProperties.x;
            this->edgeWearTexturePanel.sections[0].elements[1].rangeBar.value = receivedTexture.proceduralProps.smartProperties.y;
            this->edgeWearTexturePanel.sections[0].elements[2].rangeBar.value = receivedTexture.proceduralProps.smartProperties.z;
            this->edgeWearTexturePanel.sections[0].elements[3].rangeBar.value = receivedTexture.proceduralProps.smartProperties.w;
        }
        if(selectedTextureIndex == 6){
            this->smartDistanceTexturePanel.sections[0].elements[0].rangeBar.value = receivedTexture.proceduralProps.smartProperties.x;
            this->smartDistanceTexturePanel.sections[0].elements[1].rangeBar.value = receivedTexture.proceduralProps.smartProperties.y;
            this->smartDistanceTexturePanel.sections[0].elements[2].rangeBar.value = receivedTexture.proceduralProps.smartProperties.z;
            this->smartDistanceTexturePanel.sections[0].elements[3].rangeBar.value = receivedTexture.proceduralProps.smartProperties.w;
        }
    }

    // Other texture modes
    else{
        subPanel.sections[0].elements[subPanel_Invert_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralnverted;
        subPanel.sections[0].elements[subPanel_Scale_INDEX].rangeBar.value = receivedTexture.proceduralProps.proceduralScale * 10.f;  
        subPanel.sections[0].elements[subPanel_Stretch_INDEX].rangeBar.value = receivedTexture.proceduralProps.proceduralStretch;  
        subPanel.sections[0].elements[subPanel_ScaleModelPos_INDEX].rangeBar.value = receivedTexture.proceduralProps.proceduralScaleModelPos;  
        subPanel.sections[0].elements[subPanel_MirroredRepeat_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralMirroredRepeat;
        subPanel.sections[0].elements[subPanel_Normal_Map_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralNormalMap;
        subPanel.sections[0].elements[subPanel_Normal_Gray_Scale_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralNormalGrayScale;
        subPanel.sections[0].elements[subPanel_Normal_Strength_INDEX].rangeBar.value = receivedTexture.proceduralProps.proceduralNormalStrength;
        subPanel.sections[0].elements[subPanel_Gray_Scale_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralGrayScale;
        subPanel.sections[0].elements[subPanel_Brightness_INDEX].rangeBar.value = receivedTexture.proceduralProps.proceduralBrightness;
        subPanel.sections[0].elements[subPanel_Use_Texture_Coordinates_INDEX].checkBox.clickState1 = receivedTexture.proceduralProps.proceduralUseTexCoords;
    }

    this->updateProceduralDisplayingTexturesArray(twoDMode);
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

static void updateTextureSelectingPanelElements(Panel& textureSelectingPanel, int selectedTextureMode, std::vector<Texture> proceduralDisplayingTextures){
    textureSelectingPanel.sections.clear();
    std::vector<Element> sectionElements;
    if(selectedTextureMode == 0){
        for (size_t i = 0; i < Library::getTextureArraySize(); i++)
        {
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,6.f), "", *Library::getTexture(i), 0.f,false)));
        }
    }
    else if(selectedTextureMode == 1){
        for (size_t i = 0; i < MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE; i++)
        {
            Texture txtr;
            if(i < proceduralDisplayingTextures.size())
                txtr = proceduralDisplayingTextures[i];
            
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,6.f),""       , txtr, 0.f,false)));
        }
    }
    else if(selectedTextureMode == 2){
        for (size_t i = 0; i < MAX_PROCEDURAL_NOISE_TEXTURE_SIZE; i++)
        {
            Texture txtr;
            if(i < proceduralDisplayingTextures.size())
                txtr = proceduralDisplayingTextures[i];
            
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,6.f),""       , txtr, 0.f,false)));
        }
    }
    else if(selectedTextureMode == 3){
        for (size_t i = 0; i < Library::getTexturePackArraySize(); i++)
        {
            Texture txtr;
            if(i < proceduralDisplayingTextures.size())
                txtr = proceduralDisplayingTextures[i];
            
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,6.f),Library::getTexturePack(i)->title   , txtr, 0.f,false)));
        }
    }
    else if(selectedTextureMode == 4){
        for (size_t i = 0; i < MAX_PROCEDURAL_SMART_TEXTURE_SIZE; i++)
        {
            Texture txtr;
            if(i < proceduralDisplayingTextures.size())
                txtr = proceduralDisplayingTextures[i];
            
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,6.f),""       , txtr, 0.f,false)));
        }
    }
    else if(selectedTextureMode == 5){
        for (size_t i = 0; i < Library::getgetSrcLibTxtrsArraySize(); i++)
        {
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,6.f), "", Library::getSrcLibTxtr(i).displayingTexture, 0.f,false)));
        }
    }
    else if(selectedTextureMode == 6){
        for (size_t i = 0; i < getModel()->meshes.size(); i++)
        {
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,6.f),getModel()->meshes[i].materialName       , getModel()->meshes[i].uvMask, 0.f,false)));
        }
    }
    else if(selectedTextureMode == 7){
        for (size_t meshI = 0; meshI < getModel()->meshes.size(); meshI++)
        {
            for (size_t i = 0; i < getModel()->meshes[meshI].materialIDColors.size(); i++)
            {
                sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,6.f), getModel()->meshes[meshI].materialName, getModel()->meshes[meshI].materialIDColors[i].grayScaleTxtr, 0.f,false)));
                sectionElements[sectionElements.size()-1].button.textureSizeScale = 1.2f;
                sectionElements[sectionElements.size()-1].button.color = glm::vec4(getModel()->meshes[meshI].materialIDColors[i].color, 1.f);
            }
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

void TextureSelectionDialog::updateProceduralDisplayingTexturesArray(bool twoDMode){
        int elementSize = 0;

        if(selectedTextureMode == 0){
            elementSize = Library::getTextureArraySize();
        }
        else if(selectedTextureMode == 1){
            elementSize = MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE;
        }
        else if(selectedTextureMode == 2){
            elementSize = MAX_PROCEDURAL_NOISE_TEXTURE_SIZE;
        }
        else if(selectedTextureMode == 3){
            elementSize = Library::getTexturePackArraySize();
        }
        else if(selectedTextureMode == 4){
            elementSize = MAX_PROCEDURAL_SMART_TEXTURE_SIZE;
        }
        else if(selectedTextureMode == 5){
            elementSize = Library::getgetSrcLibTxtrsArraySize();
        }
        else if(selectedTextureMode == 6){
            elementSize = getModel()->meshes.size();
        }
        else if(selectedTextureMode == 7){
            for (size_t i = 0; i < getModel()->meshes.size(); i++)
            {
                elementSize += getModel()->meshes[i].materialIDColors.size();
            }
            
        }

        for (size_t i = 0; i < this->proceduralDisplayingTextures.size(); i++)
        {
            glDeleteTextures(1, &this->proceduralDisplayingTextures[i].ID); 
        }
        
        this->proceduralDisplayingTextures.clear();

        int prevSelectedTextureI = this->selectedTextureIndex;

        for (size_t i = 0; i < elementSize; i++)
        {
            Texture dispTxtr;
            this->selectedTextureIndex = i;
            this->selectTheTexture(dispTxtr, 256, !twoDMode, true);
            this->proceduralDisplayingTextures.push_back(dispTxtr);
        }

        this->selectedTextureIndex = prevSelectedTextureI;
}
void TextureSelectionDialog::updateTextureModesPanel(bool twoDMode){
    
    if(twoDMode)
        textureModesPanel.sections[0].elements[0].button.texture = appTextures.twoDIcon;
    else
        textureModesPanel.sections[0].elements[0].button.texture = appTextures.threeDIcon;
    
    for (size_t i = 1; i < 9; i++)
    {
        if(textureModesPanel.sections[0].elements[i].button.clickState1 && selectedTextureMode != i - 1){
            selectedTextureMode = i - 1;

            this->updateProceduralDisplayingTexturesArray(twoDMode);

            selectedTextureIndex = 0;
            for (size_t i = 0; i < textureModesPanel.sections[0].elements.size(); i++){
                textureModesPanel.sections[0].elements[i].button.clickState1 = false;
            }
        }
        if(selectedTextureMode == i - 1){
            textureModesPanel.sections[0].elements[i].button.clickState1 = true;
        }
        else{
            textureModesPanel.sections[0].elements[i].button.clickState1 = false;
        }
    }
}

void TextureSelectionDialog::selectTheTexture(Texture& receivedTexture, int displayingTextureRes, int displayMode, bool defaultProperties){
    
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
            if(receivedTexture.ID == this->displayingTexture.ID || displayingTextureRes <= 512)
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
    else if(this->selectedTextureMode == 7){
        receivedTexture.proceduralProps.proceduralID = -1;
        std::vector<Texture> txtrs;
        for (size_t meshI = 0; meshI < getModel()->meshes.size(); meshI++)
        {
            for (size_t i = 0; i < getModel()->meshes[meshI].materialIDColors.size(); i++)
            {
                txtrs.push_back(getModel()->meshes[meshI].materialIDColors[i].grayScaleTxtr);
            }
        }
        
        if(selectedTextureIndex < txtrs.size())
            receivedTexture.proceduralProps.proceduralTextureID = txtrs[selectedTextureIndex].ID;
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
        receivedTexture.proceduralProps.proceduralGrayScale = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Gray_Scale_INDEX].checkBox.clickState1;
        receivedTexture.proceduralProps.proceduralBrightness = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Brightness_INDEX].rangeBar.value;
        receivedTexture.proceduralProps.proceduralStretch = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Stretch_INDEX].rangeBar.value;
        receivedTexture.proceduralProps.proceduralScaleModelPos = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_ScaleModelPos_INDEX].rangeBar.value;
        receivedTexture.proceduralProps.proceduralMirroredRepeat = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_MirroredRepeat_INDEX].checkBox.clickState1;
        
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
        receivedTexture.proceduralProps.proceduralStretch = this->subPanel.sections[0].elements[subPanel_Stretch_INDEX].rangeBar.value;
        receivedTexture.proceduralProps.proceduralScaleModelPos = this->subPanel.sections[0].elements[subPanel_ScaleModelPos_INDEX].rangeBar.value;
        receivedTexture.proceduralProps.proceduralMirroredRepeat = this->subPanel.sections[0].elements[subPanel_MirroredRepeat_INDEX].checkBox.clickState1;
        receivedTexture.proceduralProps.proceduralnverted = this->subPanel.sections[0].elements[subPanel_Invert_INDEX].checkBox.clickState1;
        receivedTexture.proceduralProps.proceduralNormalMap = this->subPanel.sections[0].elements[subPanel_Normal_Map_INDEX].checkBox.clickState1;
        receivedTexture.proceduralProps.proceduralNormalGrayScale = this->subPanel.sections[0].elements[subPanel_Normal_Gray_Scale_INDEX].checkBox.clickState1;
        receivedTexture.proceduralProps.proceduralNormalStrength = this->subPanel.sections[0].elements[subPanel_Normal_Strength_INDEX].rangeBar.value;
        receivedTexture.proceduralProps.proceduralUseTexCoords = this->subPanel.sections[0].elements[subPanel_Use_Texture_Coordinates_INDEX].checkBox.clickState1;
        receivedTexture.proceduralProps.proceduralGrayScale = this->subPanel.sections[0].elements[subPanel_Gray_Scale_INDEX].checkBox.clickState1;
        receivedTexture.proceduralProps.proceduralBrightness = this->subPanel.sections[0].elements[subPanel_Brightness_INDEX].rangeBar.value;
    }

    if(defaultProperties){
        ProceduralProperties defaultProps;
        defaultProps.proceduralID = receivedTexture.proceduralProps.proceduralID;
        if(this->selectedTextureIndex == 0 || selectedTextureIndex == 1 || selectedTextureIndex == 2){
            defaultProps.smartProperties.x = 1.f;
            defaultProps.smartProperties.y = 0.f;
            defaultProps.smartProperties.z = 0.f;
            defaultProps.smartProperties.w = 0.f;
        }
        else if(this->selectedTextureIndex == 3 || selectedTextureIndex == 4){
            defaultProps.smartProperties.x = 1.f;
            defaultProps.smartProperties.y = 0.25f;
            defaultProps.smartProperties.z = 0.0005f;
            defaultProps.smartProperties.w = 0.f;
        }
        else if(this->selectedTextureIndex == 5){
            defaultProps.smartProperties.x = 0.15f;
            defaultProps.smartProperties.y = 3.37f;
            defaultProps.smartProperties.z = 0.f;
            defaultProps.smartProperties.w = 1.f;
        }
        else if(this->selectedTextureIndex == 6){
            defaultProps.smartProperties.x = 0.944f;
            defaultProps.smartProperties.y = 1.f;
            defaultProps.smartProperties.z = 13.15f;
            defaultProps.smartProperties.w = 0.f;
        }
        receivedTexture.proceduralProps = defaultProps;
    }

    // ------------------ Finish ------------------

    // Set other data
    receivedTexture.proceduralProps.textureSelectionDialog_selectedMode = this->selectedTextureMode;
    receivedTexture.proceduralProps.textureSelectionDialog_selectedTextureIndex = this->selectedTextureIndex;
    receivedTexture.title = "SelectedTexture";

    // Generate the displaying texture of the selected texture
    if(!defaultProperties){
        receivedTexture.generateProceduralDisplayingTexture(displayingTextureRes, displayMode, 4.f - (this->zoomVal / 3.f) , 360.f - this->rotationRangeBar.value);
    }
    else{
        receivedTexture.generateProceduralDisplayingTexture(displayingTextureRes, displayMode);
    }
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
    selectedTextureDisplayingPanel.scale.x = this->subPanel.scale.x;
    selectedTextureDisplayingPanel.pos.x = this->subPanel.pos.x;
    textureSelectingPanel.pos.x = this->pos.x + textureModesPanel.scale.x - subPanel.scale.x;
    textureSelectingPanel.scale.x = this->scale.x - subPanel.scale.x - textureModesPanel.scale.x;

    // Update the displaying panel's displaying button's texture
    selectedTextureDisplayingPanel.sections[0].elements[0].button.texture = displayingTexture;

    // Seting the selected texture displaying panel's row count
    this->textureSelectingPanel.rowCount = 7;    
    this->textureSelectingPanel.render(timer, true);

    // Selected texture displaying panel
    this->selectedTextureDisplayingPanel.render(timer, false);


    if(selectedTextureSolidDisplayingModeBtn.hover)
        selectedTextureSolidDisplayingModeBtn.texture = Texture();
    else
        selectedTextureSolidDisplayingModeBtn.texture = appTextures.twoDIcon;

    if(selectedTextureMaterialBallDisplayingMode.hover)
        selectedTextureMaterialBallDisplayingMode.texture = Texture();
    else
        selectedTextureMaterialBallDisplayingMode.texture = appTextures.materialIcon;

    if(selectedTextureCustomMeshDisplayingMode.hover)
        selectedTextureCustomMeshDisplayingMode.texture = Texture();
    else
        selectedTextureCustomMeshDisplayingMode.texture = appTextures.TDModelIcon;

    selectedTextureSolidDisplayingModeBtn.scale.x = 1.f + selectedTextureSolidDisplayingModeBtn.hoverMixVal * 0.6f;
    selectedTextureMaterialBallDisplayingMode.scale.x = 1.f + selectedTextureMaterialBallDisplayingMode.hoverMixVal * 1.5f;
    selectedTextureCustomMeshDisplayingMode.scale.x = 1.f + selectedTextureCustomMeshDisplayingMode.hoverMixVal * 1.6f;

    selectedTextureSolidDisplayingModeBtn.pos = selectedTextureDisplayingPanel.pos;
    selectedTextureSolidDisplayingModeBtn.pos.x -= selectedTextureDisplayingPanel.scale.x - selectedTextureSolidDisplayingModeBtn.scale.x - 1.f; 
    selectedTextureSolidDisplayingModeBtn.pos.y -= selectedTextureDisplayingPanel.scale.y - selectedTextureSolidDisplayingModeBtn.scale.y - 1.f; 

    selectedTextureMaterialBallDisplayingMode.pos.x = selectedTextureSolidDisplayingModeBtn.pos.x + selectedTextureSolidDisplayingModeBtn.scale.x + selectedTextureMaterialBallDisplayingMode.scale.x;
    selectedTextureMaterialBallDisplayingMode.pos.y = selectedTextureSolidDisplayingModeBtn.pos.y;
    selectedTextureMaterialBallDisplayingMode.pos.z = selectedTextureSolidDisplayingModeBtn.pos.z;
    selectedTextureCustomMeshDisplayingMode.pos.x = selectedTextureMaterialBallDisplayingMode.pos.x + selectedTextureMaterialBallDisplayingMode.scale.x + selectedTextureCustomMeshDisplayingMode.scale.x;
    selectedTextureCustomMeshDisplayingMode.pos.y = selectedTextureMaterialBallDisplayingMode.pos.y;
    selectedTextureCustomMeshDisplayingMode.pos.z = selectedTextureMaterialBallDisplayingMode.pos.z;

    zoomValDisplayer.pos = selectedTextureDisplayingPanel.pos;
    zoomValDisplayer.pos.x += selectedTextureDisplayingPanel.scale.x - zoomInBtn.scale.x - 2.f; 
    zoomValDisplayer.pos.y -= selectedTextureDisplayingPanel.scale.y - zoomInBtn.scale.y - 1.f; 
    zoomInBtn.pos = zoomValDisplayer.pos;
    zoomInBtn.pos.x -= zoomValDisplayer.scale.x + zoomInBtn.scale.x;
    zoomOutBtn.pos = zoomInBtn.pos;
    zoomOutBtn.pos.x -= zoomInBtn.scale.x + zoomInBtn.scale.x;
    rotationRangeBar.pos = selectedTextureDisplayingPanel.pos;
    rotationRangeBar.pos.y += selectedTextureDisplayingPanel.scale.y - rotationRangeBar.scale.y - 1.f;

    if(zoomInBtn.clicked && this->zoomVal != 10)
        this->zoomVal++;
    if(zoomOutBtn.clicked && this->zoomVal != 0)
        this->zoomVal--;

    selectedTextureSolidDisplayingModeBtn.render(timer, true);
    selectedTextureMaterialBallDisplayingMode.render(timer, true);
    selectedTextureCustomMeshDisplayingMode.render(timer, true);
    
    if(this->activeSelectedTextureDisplayingMode != 0){
        zoomInBtn.render(timer, true);
        zoomOutBtn.render(timer, true);
        zoomValDisplayer.text = "Zoom : " + std::to_string(this->zoomVal);
        zoomValDisplayer.text.erase(zoomValDisplayer.text.end() - 4, zoomValDisplayer.text.end());
        zoomValDisplayer.render(timer, true);
        rotationRangeBar.render(timer, true);
    }

    if(selectedTextureSolidDisplayingModeBtn.clicked)
        activeSelectedTextureDisplayingMode = 0;
    if(selectedTextureMaterialBallDisplayingMode.clicked)
        activeSelectedTextureDisplayingMode = 1;
    if(selectedTextureCustomMeshDisplayingMode.clicked)
        activeSelectedTextureDisplayingMode = 2;

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