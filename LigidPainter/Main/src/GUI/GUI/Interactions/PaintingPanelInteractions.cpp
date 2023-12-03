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


#include<glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"
#include "LibrarySystem/Library.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>

static glm::vec3 prevSelectedClr;
static bool painterColorDisplayMatInit = false;

void UI::paintingPanelInteraction(
                                Painter &painter 
                            )
{

    //Get the brush data from GUI to the painter class
    painter.setBrushProperties(brushSection);

    if(painter.selectedDisplayingModeIndex == 0){
        this->paintingOverSection.elements[0].checkBox.clickState1 = false;
        this->paintingOverSection.elements[1].checkBox.clickState1 = false;
        this->meshSection.elements[1].checkBox.clickState1 = false;
        this->meshSection.elements[2].checkBox.clickState1 = false;
        this->mirrorSection.elements[0].checkBox.clickState1 = false;
        this->mirrorSection.elements[2].checkBox.clickState1 = false;
        this->mirrorSection.elements[4].checkBox.clickState1 = false;
    }

    for (size_t i = 0; i < this->colorSection.elements.size(); i++){
        if(this->colorSection.elements[i].rangeBar.valueDoneChanging || prevSelectedClr != painter.getSelectedColor().getRGB_normalized() || !painterColorDisplayMatInit){
            painterColorDisplayMatInit = true;
            
            if(paintingSectionDisplayMat.materialModifiers.size()){
                paintingSectionDisplayMat.materialModifiers[0].sections[0].elements[0].button.color = glm::vec4(glm::vec3(painter.getSelectedColor().getRGB_normalized()), 1.f);
                paintingSectionDisplayMat.materialModifiers[0].sections[0].elements[2].button.color = glm::vec4(glm::vec3(painter.roughnessVal), 1.f);
                paintingSectionDisplayMat.materialModifiers[0].sections[0].elements[4].button.color = glm::vec4(glm::vec3(painter.metallicVal), 1.f);
                paintingSectionDisplayMat.materialModifiers[0].sections[0].elements[6].button.color = glm::vec4(glm::vec3(0.5f,0.5f,1.f), 1.f);
                paintingSectionDisplayMat.materialModifiers[0].sections[0].elements[8].button.color = glm::vec4(glm::vec3(painter.heightMapVal), 1.f);
                paintingSectionDisplayMat.materialModifiers[0].sections[0].elements[10].button.color = glm::vec4(glm::vec3(painter.ambientOcclusionVal), 1.f);
            }
            
            paintingSectionDisplayMat.updateMaterialDisplayingTexture(512, true, Camera(), 0, false);
            
            break;
        }
    }

    prevSelectedClr = painter.getSelectedColor().getRGB_normalized();

    if(!painter.useCustomMaterial)
        this->colorSection.elements[0].button.texture = paintingSectionDisplayMat.displayingTexture;
    else{
        this->colorSection.elements[0].button.texture = Library::findMaterialViaUniqueID(painter.customMaterialID).displayingTexture;
    }

    for (size_t i = 0; i < this->colorSection.elements.size(); i++)
    {
        if(painter.selectedDisplayingModeIndex == 1){
            if(painter.useCustomMaterial && i != 0 && i != 1  && i != 6 && i != 8 && i != 10 && i != 12 && i != 14 && i!= 16 && i!= 17){
                this->colorSection.elements[i].scale.y = 0.f;
            }
            else{
                if(i == 17 && !painter.useCustomMaterial){
                    this->colorSection.elements[i].scale.y = 0.f;
                }
                else{
                    this->colorSection.elements[i].scale.y = 2.f;
                    if(this->colorSection.elements[i].state == 1)
                        this->colorSection.elements[i].scale.y = 1.f;
                    if(i == 0)
                        this->colorSection.elements[i].scale.y = 4.f;
                }
            }
        }
        else{
            colorSection.elements[16].checkBox.clickState1 = false;

            if(i != 2 && i != 3 && i != 4 && i != 5){
                this->colorSection.elements[i].scale.y = 0.f;
            }
            else{
                this->colorSection.elements[i].scale.y = 2.f;
            }
        }
    }

    if(this->colorSection.elements[17].button.clicked){
        this->colorUseCustomMatSelectionPanelActive = true;
        this->colorUseCustomMatSelectionPanel.pos = paintingPanelModePanel.pos;
        this->colorUseCustomMatSelectionPanel.pos.x -= paintingPanelModePanel.scale.x + this->colorUseCustomMatSelectionPanel.scale.x;
    }

    if(colorSection.elements[2].button.hover && *Mouse::LDoubleClick()){//Pressed to first color button element
        painter.loadColor1();
    }
    if(colorSection.elements[3].button.hover && *Mouse::LDoubleClick()){//Pressed to second color button element
        painter.loadColor2();
    }
    if(colorSection.elements[4].button.hover && *Mouse::LDoubleClick()){//Pressed to third color button element
        painter.loadColor3();
    }

    //Prevent multiple selection and update the painter.selectedColorIndex for colors
    for (size_t i = 2; i < colorSection.elements.size(); i++)
    {
        if(i == 5) 
            break; //Don't bring the dropper button
        
        if(colorSection.elements[i].button.clickState1){ //If a color button is clicked
            if(painter.selectedColorIndex != i - 2){ //If the clicked button is not selected 
                colorSection.elements[painter.selectedColorIndex + 2].button.clickState1 = false; //Unselect the selected one
                painter.selectedColorIndex = i - 2; //Select the clicked color button
                break; 
            }
        }
    }

    //Keep the selected color button pressed
    for (size_t i = 0; i < colorSection.elements.size(); i++){
        if(i == painter.selectedColorIndex + 2){
            colorSection.elements[i].button.clickState1 = true;           
        }
    }
    
    //Update the color values of the color buttons
    colorSection.elements[2].button.color = glm::vec4(painter.color1.getRGB_normalized(), 1.f);
    colorSection.elements[3].button.color = glm::vec4(painter.color2.getRGB_normalized(), 1.f);
    colorSection.elements[4].button.color = glm::vec4(painter.color3.getRGB_normalized(), 1.f);
    

    //If clicked to the dropper button activate the dropper
    if(colorSection.elements[5].button.clicked){
        dropper.active = true;
    }

    if(brushSection.elements[10].button.hover && *Mouse::LClick()){ //If pressed to remove the brush texture button from brush/more
        char whitePixel[] = { 127, 127, 127, 127 };
        brushSection.elements[9].button.textureSelection2D = true;
        brushSection.elements[9].button.texture = Texture(whitePixel, 1, 1, GL_NEAREST);
        brushSection.elements[9].button.texture.proceduralProps.proceduralID = 24; //Solid white
        brushSection.elements[9].button.texture.proceduralProps.proceduralnverted = 0;
        brushSection.elements[9].button.texture.proceduralProps.proceduralScale = 1.f;
        brushSection.elements[9].button.texture.title = "AutoGeneratedMask";        
    }
    
    if(brushSection.elements[15].button.hover && *Mouse::LClick()){ //If pressed to create new brush button
        Brush newBrush
                        (    
                            0.1f,
                            painter.brushProperties.spacing,
                            painter.brushProperties.hardness,
                            painter.brushProperties.sizeJitter,
                            painter.brushProperties.scatter,
                            painter.brushProperties.fade,
                            painter.brushProperties.rotation,
                            painter.brushProperties.rotationJitter,
                            painter.brushProperties.alphaJitter,
                            painter.brushProperties.individualTexture,
                            painter.brushProperties.sinWavePattern,
                            "newBrush",
                            painter.brushProperties.brushTexture
                        );

        Library::addBrush(newBrush, "New brush via painting panel");
    }
    
    if(brushSection.elements[16].button.hover && *Mouse::LClick()){ //If pressed to export brush file button
        Brush exportBrush
                        (    
                            0.1f,
                            painter.brushProperties.spacing,
                            painter.brushProperties.hardness,
                            painter.brushProperties.sizeJitter,
                            painter.brushProperties.scatter,
                            painter.brushProperties.fade,
                            painter.brushProperties.rotation,
                            painter.brushProperties.rotationJitter,
                            painter.brushProperties.alphaJitter,
                            painter.brushProperties.individualTexture,
                            painter.brushProperties.sinWavePattern,
                            "exportBrush",
                            painter.brushProperties.brushTexture
                        );

        FileHandler::writeLGDBRUSHFile("", exportBrush);

        // Delete the OpenGL texture buffers created with the export brush
        glDeleteTextures(1, &exportBrush.texture.ID);
        glDeleteTextures(1, &exportBrush.displayingTexture.ID);
        
    }

    //Update the meshes section of the painting panel if a new model is added
    if(getModel()->newModelAdded){
        meshSection.elements[0].button.selectedMeshI = 0;
    }

    painter.selectedMeshIndex = meshSection.elements[0].button.selectedMeshI;

    painter.oSide.active = true;
    
    painter.oXSide.active = mirrorSection.elements[0].checkBox.clickState1; 
    painter.mirrorXOffset = mirrorSection.elements[1].rangeBar.value;
    painter.oYSide.active = mirrorSection.elements[2].checkBox.clickState1; 
    painter.mirrorYOffset = mirrorSection.elements[3].rangeBar.value;
    painter.oZSide.active = mirrorSection.elements[4].checkBox.clickState1; 
    painter.mirrorZOffset = mirrorSection.elements[5].rangeBar.value;


    painter.oXYSide.active = painter.oXSide.active && painter.oYSide.active;     
    painter.oXZSide.active = painter.oXSide.active && painter.oZSide.active; 
    painter.oYZSide.active = painter.oYSide.active && painter.oZSide.active; 
    painter.oXYZSide.active = painter.oXSide.active && painter.oYSide.active && painter.oZSide.active; 

    // Updating the depth texture if interacted with the gui elements related to mirroring
    if( 
        mirrorSection.elements[0].isInteracted() || 
        mirrorSection.elements[1].isInteracted() || 
        mirrorSection.elements[2].isInteracted() || 
        mirrorSection.elements[3].isInteracted() || 
        mirrorSection.elements[4].isInteracted() || 
        mirrorSection.elements[5].isInteracted()
    ) {
        painter.updateDepthTexture();
    }


    brushSection.elements[0].button.texture = painter.displayingBrush.displayingTexture; 

    if(!painter.displayingBrush.displayingTexture.ID){
        // Init brush settings displaying brush
        painter.displayingBrush = Brush(
                                            painter.brushProperties.radius * 2.f,
                                            painter.brushProperties.spacing,
                                            painter.brushProperties.hardness,
                                            painter.brushProperties.sizeJitter,
                                            painter.brushProperties.scatter,
                                            painter.brushProperties.fade,
                                            painter.brushProperties.rotation,
                                            painter.brushProperties.rotationJitter,
                                            painter.brushProperties.alphaJitter,
                                            painter.brushProperties.individualTexture,
                                            painter.brushProperties.sinWavePattern,
                                            "BrushSettingsDisplayingBrush",
                                            painter.brushProperties.brushTexture
                                        );
    }
    
    for (size_t i = 0; i < brushSection.elements.size(); i++)
    {
        if(brushSection.elements[i].isInteracted()){
            // Update brush settings displaying brush
            painter.displayingBrush.update(
                                                painter.brushProperties.radius * 2.f,
                                                painter.brushProperties.spacing,
                                                painter.brushProperties.hardness,
                                                painter.brushProperties.sizeJitter,
                                                painter.brushProperties.scatter,
                                                painter.brushProperties.fade,
                                                painter.brushProperties.rotation,
                                                painter.brushProperties.rotationJitter,
                                                painter.brushProperties.alphaJitter,
                                                painter.brushProperties.individualTexture,
                                                painter.brushProperties.sinWavePattern,
                                                painter.brushProperties.brushTexture
                                            );
        }
    }
    
    for (size_t i = 0; i < paintingPanelModePanel.sections[0].elements.size(); i++)
    {
        if(paintingPanelModePanel.sections[0].elements[i].button.clickState1 && selectedPaintingPanelMode != i){
            selectedPaintingPanelMode = i;
            for (size_t i = 0; i < paintingPanelModePanel.sections[0].elements.size(); i++){
                paintingPanelModePanel.sections[0].elements[i].button.clickState1 = false;
            }
        }
        if(selectedPaintingPanelMode == i){
            paintingPanelModePanel.sections[0].elements[i].button.clickState1 = true;
        }
        else{
            paintingPanelModePanel.sections[0].elements[i].button.clickState1 = false;
        }
    }
    for (size_t i = 0; i < paintingPanelModePanel.sections[0].elements.size(); i++){
        if(i == selectedPaintingPanelMode){
            paintingPanelModePanel.sections[0].elements[i].button.clickState1 = true;
        }
    }

    painter.materialPainting = painter.selectedDisplayingModeIndex == 1;
    painter.enableAlbedoChannel = colorSection.elements[1].checkBox.clickState1;
    painter.enableRoughnessChannel = colorSection.elements[6].checkBox.clickState1;
    painter.roughnessVal = colorSection.elements[7].rangeBar.value;
    painter.enableMetallicChannel = colorSection.elements[8].checkBox.clickState1;
    painter.metallicVal = colorSection.elements[9].rangeBar.value;
    painter.enableNormalMapChannel = colorSection.elements[10].checkBox.clickState1;
    painter.normalMapStrengthVal = colorSection.elements[11].rangeBar.value;
    painter.enableHeightMapChannel = colorSection.elements[12].checkBox.clickState1;
    painter.heightMapVal = colorSection.elements[13].rangeBar.value;
    painter.enableAOChannel = colorSection.elements[14].checkBox.clickState1;
    painter.ambientOcclusionVal = colorSection.elements[15].rangeBar.value;
    painter.useCustomMaterial = colorSection.elements[16].checkBox.clickState1;

    if(selectedPaintingPanelMode != 3)
        this->meshSection.elements[2].checkBox.clickState1 = false;
    if(selectedPaintingPanelMode != 5)
        this->paintingOverSection.elements[1].checkBox.clickState1 = false;

    if(getModel()->newModelAdded){
        paintingChannelsSection.clear(); 

        paintingChannelsSection.push_back(
                                            Section(
                                                Element(),
                                                {
                                                    Element(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(2,2), "Auto Create Textures"  , Texture(), 7.f, false)),//4
                                                }
                                            )
                                        );
        
        for (size_t i = 0; i < getModel()->meshes.size(); i++)
        {
            paintingChannelsSection.push_back(
                                                Section(
                                                    SectionHolder(ColorPalette::secondColor, 2.f, getModel()->meshes[i].materialName),
                                                    {
                                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2, 2),"Albedo Texture"  , Texture(), 1.f, true)),//4
                                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2, 2),"Roughness Texture"  , Texture(), 1.f, true)),//4
                                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2, 2),"Metallic Texture"  , Texture(), 1.f, true)),//4
                                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2, 2),"Normal Map Texture"  , Texture(), 1.f, true)),//4
                                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2, 2),"Height Map Texture"  , Texture(), 1.f, true)),//4
                                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2, 2),"Ambient Occlusion Texture"  , Texture(), 1.f, true)),//4
                                                    }
                                                )
                                            );
        }
    }

    if(paintingChannelsSection.size()){
        if(paintingChannelsSection[0].elements.size()){
            if(paintingChannelsSection[0].elements[0].button.clicked)
                paintingChannelsAutoCreateTexturesDialog.dialogControl.activate();
        }
    }

    for (size_t secI = 1; secI < paintingChannelsSection.size(); secI++)
    {
        for (size_t elI = 0; elI < paintingChannelsSection[secI].elements.size(); elI++){
            if(paintingChannelsSection[secI].elements[elI].button.clickState1){
                this->paintingChannelsTextureSelectionPanelActive = true;
                this->paintingChannelsTextureSelectionPanel.pos = paintingPanelModePanel.pos;
                this->paintingChannelsTextureSelectionPanel.pos.x -= paintingPanelModePanel.scale.x + this->paintingChannelsTextureSelectionPanel.scale.x;
            }
        }
    }

    if(!objectTexturingDialog.dialogControl.isActive()){
        for (size_t i = 0; i < getModel()->meshes.size(); i++)
        {
            if(i >= paintingChannelsSection.size())
                break;
                
            getModel()->meshes[i].albedo = paintingChannelsSection[i + 1].elements[0].button.texture;
            getModel()->meshes[i].roughness = paintingChannelsSection[i + 1].elements[1].button.texture;
            getModel()->meshes[i].metallic = paintingChannelsSection[i + 1].elements[2].button.texture;
            getModel()->meshes[i].normalMap = paintingChannelsSection[i + 1].elements[3].button.texture;
            getModel()->meshes[i].heightMap = paintingChannelsSection[i + 1].elements[4].button.texture;
            getModel()->meshes[i].ambientOcclusion = paintingChannelsSection[i + 1].elements[5].button.texture;
        }
    }
}