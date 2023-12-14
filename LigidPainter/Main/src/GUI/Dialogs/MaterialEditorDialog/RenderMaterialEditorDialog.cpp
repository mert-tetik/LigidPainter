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
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "NodeSystem/Node/Node.hpp"
#include "SettingsSystem/Settings.hpp"
#include "MouseSystem/Mouse.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

// Defined in the RenderPanel.cpp
extern bool updateThePreRenderedPanels;

bool __materialEditorDialogESCPressed = false;
bool __materialEditorDialogESCFirstFramePressed = false;

bool __lastDisplayModeComboBoxPressed = false;

void MaterialEditorDialog::render
                                (
                                    Timer &timer,
                                    TextureSelectionDialog &textureSelectionDialog,
                                    LogDialog& logDialog
                                )
{
    dialogControl.updateStart();

    // ------- Positioning & Prepearing the panels to rendering -------
    navPanel.scale.x = bgPanel.scale.x - shortcutPanel.scale.x - layerPanel.scale.x - modifiersPanel.scale.x;
    navPanel.pos = shortcutPanel.pos;
    navPanel.pos.x += shortcutPanel.scale.x + navPanel.scale.x;
    navPanel.pos.y = bgPanel.pos.y - bgPanel.scale.y + navPanel.scale.y;

    this->materialDisplayer.texture = this->displayingFBO.colorBuffer;
    this->materialDisplayer.pos = navPanel.pos;
    materialDisplayer.scale.y = bgPanel.scale.y - navPanel.scale.y;
    materialDisplayer.scale.x = navPanel.scale.x;
    this->materialDisplayer.pos.y += navPanel.scale.y + materialDisplayer.scale.y;

    barButton.pos.x = bgPanel.pos.x;
    barButton.pos.y = bgPanel.pos.y - bgPanel.scale.y - barButton.scale.y;
    
    bool mouseTrackingFlag = !(textureSelectionDialog.dialogControl.isActive() || ContextMenus::materialModifier.dialogControl.isActive() || ContextMenus::addMaterialModifier.dialogControl.isActive());
    
    if(this->selectedMaterialModifierIndex < material->materialModifiers.size())
        modifiersPanel.sections = material->materialModifiers[this->selectedMaterialModifierIndex].sections;
    else
        modifiersPanel.sections.clear();
    

    displayModeComboBox.scale.y = navPanel.scale.y - 0.1;
    matDisplayerBallModeBtn.scale.y = navPanel.scale.y - 0.1;
    twoDModelModeBtn.scale.y = navPanel.scale.y - 0.1;
    customModelModeBtn.scale.y = navPanel.scale.y - 0.1;
    splitMode.scale.y = navPanel.scale.y - 0.1;
    selectedModifierResultMode.scale.y = navPanel.scale.y - 0.1;
    overallResultMode.scale.y = navPanel.scale.y - 0.1;

    matDisplayerBallModeBtn.pos = navPanel.pos;
    matDisplayerBallModeBtn.pos.x += displayModeComboBox.scale.x - splitMode.scale.x * 9.f;
    twoDModelModeBtn.pos = matDisplayerBallModeBtn.pos; 
    twoDModelModeBtn.pos.x -= matDisplayerBallModeBtn.scale.x + twoDModelModeBtn.scale.x; 
    customModelModeBtn.pos = matDisplayerBallModeBtn.pos; 
    customModelModeBtn.pos.x += matDisplayerBallModeBtn.scale.x + customModelModeBtn.scale.x; 

    splitMode.pos = navPanel.pos;
    splitMode.pos.x += navPanel.scale.x - splitMode.scale.x * 1.5f;
    selectedModifierResultMode.pos = splitMode.pos; 
    selectedModifierResultMode.pos.x -= splitMode.scale.x + selectedModifierResultMode.scale.x; 
    overallResultMode.pos = selectedModifierResultMode.pos; 
    overallResultMode.pos.x -= selectedModifierResultMode.scale.x + overallResultMode.scale.x; 

    // ------- Rendering the panels -------
    bgPanel.render(timer, mouseTrackingFlag);
    layerPanel.pos.x = modifiersPanel.pos.x - modifiersPanel.scale.x - layerPanel.scale.x; 
    layerPanel.render(timer, mouseTrackingFlag);
    modifiersPanel.render(timer, mouseTrackingFlag);
    barButton.render(timer, mouseTrackingFlag);
    shortcutPanel.render(timer, mouseTrackingFlag);
    navPanel.render(timer, mouseTrackingFlag);
    materialDisplayer.render(timer, false);

    twoDModelModeBtn.render(timer, mouseTrackingFlag);
    matDisplayerBallModeBtn.render(timer, mouseTrackingFlag);
    customModelModeBtn.render(timer, mouseTrackingFlag);
    overallResultMode.render(timer, mouseTrackingFlag);
    selectedModifierResultMode.render(timer, mouseTrackingFlag);
    splitMode.render(timer, mouseTrackingFlag);
    
    if(this->selectedMaterialModifierIndex < material->materialModifiers.size())
        material->materialModifiers[this->selectedMaterialModifierIndex].sections = modifiersPanel.sections;
    else
        modifiersPanel.sections.clear();

    if(twoDModelModeBtn.clicked)
        this->selectedModelModeIndex = 0;
    if(this->selectedModelModeIndex == 0)
        twoDModelModeBtn.color = ColorPalette::thirdColor;
    else
        twoDModelModeBtn.color = ColorPalette::secondColor;

    if(matDisplayerBallModeBtn.clicked)
        this->selectedModelModeIndex = 1;
    if(this->selectedModelModeIndex == 1)
        matDisplayerBallModeBtn.color = ColorPalette::thirdColor;
    else
        matDisplayerBallModeBtn.color = ColorPalette::secondColor;

    if(customModelModeBtn.clicked)
        this->selectedModelModeIndex = 2;
    if(this->selectedModelModeIndex == 2)
        customModelModeBtn.color = ColorPalette::thirdColor;
    else
        customModelModeBtn.color = ColorPalette::secondColor;

    if(overallResultMode.clicked)
        this->selectedResultModeIndex = 0;
    if(this->selectedResultModeIndex == 0)
        overallResultMode.color = ColorPalette::thirdColor;
    else
        overallResultMode.color = ColorPalette::secondColor;

    if(selectedModifierResultMode.clicked)
        this->selectedResultModeIndex = 1;
    if(this->selectedResultModeIndex == 1)
        selectedModifierResultMode.color = ColorPalette::thirdColor;
    else
        selectedModifierResultMode.color = ColorPalette::secondColor;

    if(splitMode.clicked)
        this->selectedResultModeIndex = 2;
    if(this->selectedResultModeIndex == 2)
        splitMode.color = ColorPalette::thirdColor;
    else
        splitMode.color = ColorPalette::secondColor;


    //If texture selection dialog is not active reset the index values used to navigate textures
    if(textureSelectionDialog.dialogControl.isActive() == false){
        textureModifierTextureSelectingButtonIndex = 1000;
        textureSelectionDialog.selectedTextureIndex = 0;
    }

    //Make sure selectedMaterialModifierIndex won't cause any vector out of range error
    if(selectedMaterialModifierIndex >= material->materialModifiers.size()){
        selectedMaterialModifierIndex = material->materialModifiers.size()-1;
        if(selectedMaterialModifierIndex < 0)
            selectedMaterialModifierIndex = 0;
    }

    //Check layerpanel if any modifier is clicked & change selectedMaterialModifierIndex if clicked
    checkLayerPanel(*material);
    
    //Check the modifier's panel and update the material if interacted with any of the GUI element (show the texture selection dialog if pressed to a texture modifier's channel button)
    checkModifiersPanel(*material, textureSelectionDialog);
    
    //Manage actions of the context menus 
    this->manageContextMenuActions(*material);

    if(dialogControl.firstFrameActivated)
        updateLayerPanel(*material);
    
    //If texture selection done
    checkTextureSelectionDialog(textureSelectionDialog, *material);

    this->displayModeComboBox.pos = navPanel.pos;
    this->displayModeComboBox.pos.x -= navPanel.scale.x - displayModeComboBox.scale.x * 1.5f;
    this->displayModeComboBox.render(timer, true);
    
    dialogControl.updateEnd(timer,0.15f);

    if(!this->updateTheMaterial && this->prevUpdateTheMaterial){
        material->updateMaterialDisplayingTexture((float)Settings::properties()->textureRes, true, this->displayerCamera, this->displayModeComboBox.selectedIndex, true, this->displayingFBO);
    }
    
    this->prevUpdateTheMaterial = this->updateTheMaterial;
    this->updateTheMaterial = false;

    if(getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS){
        if(!__materialEditorDialogESCPressed)
            __materialEditorDialogESCFirstFramePressed = true;
        __materialEditorDialogESCPressed = true;
    }
    else{
        __materialEditorDialogESCPressed = false;
    } 

    //Close the dialog
    if(__materialEditorDialogESCFirstFramePressed || ((!bgPanel.hover && !barButton.hover && !logDialog.isHovered()) && *Mouse::LClick()) || (barButton.hover && *Mouse::LDoubleClick())){
        if(!wasTextureSelectionDialogActive() && !ContextMenus::materialModifier.dialogControl.isActive() && !ContextMenus::addMaterialModifier.dialogControl.isActive()){
            this->displayModeComboBox.selectedIndex = 0;
            material->updateMaterialDisplayingTexture((float)Settings::properties()->textureRes, false, Camera(), 0, false);

            updateThePreRenderedPanels = true;

            this->deactivate(textureSelectionDialog);
        }
    }

    __materialEditorDialogESCFirstFramePressed = false; 

    if((!ContextMenus::materialModifier.dialogControl.isActive() && !ContextMenus::addMaterialModifier.dialogControl.isActive() && getContext()->window.isMouseButtonPressed(LIGIDGL_MOUSE_BUTTON_RIGHT) == LIGIDGL_PRESS) || __lastDisplayModeComboBoxPressed)
        material->updateMaterialDisplayingTexture((float)Settings::properties()->textureRes, false, this->displayerCamera, this->displayModeComboBox.selectedIndex, true, this->displayingFBO);

    __lastDisplayModeComboBoxPressed = this->displayModeComboBox.pressed;
}




















// -------------------- UTILITY FUNCTIONS --------------------
void MaterialEditorDialog::updateLayerPanel(Material &material){
    //Clear the elements of the layerPanel
    layerPanel.sections.clear();
    
    //Create the section
    Section layerPanelSection;
    
    //Give empty header to the new layer panel section
    layerPanelSection.header = Element(Button());
    
    //Push the elements one by one from the materialModifiers of the material
    for (size_t i = 0; i < material.materialModifiers.size(); i++)
    {
        Element btn = Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1.5f), material.materialModifiers[i].title , material.materialModifiers[i].maskTexture, 0.f, false));
        layerPanelSection.elements.push_back(btn);
    }

    //Give the section to the layerPanel
    layerPanel.sections.push_back(layerPanelSection);
    
    //Update the material after updating layerPanel
    material.updateMaterialDisplayingTexture((float)Settings::properties()->textureRes, true, this->displayerCamera, this->displayModeComboBox.selectedIndex, true, this->displayingFBO);
}

static bool modMoved = false; 

void MaterialEditorDialog::checkLayerPanel(Material &material){
    //Update the selected material modifier index (pressed to a modifier)
    if(layerPanel.sections.size()){
        bool anyPressed = false;
        for (size_t i = 0; i < layerPanel.sections[0].elements.size(); i++)
        {
            if(layerPanel.sections[0].elements[i].button.clicked){
                selectedMaterialModifierIndex = i; //Select the clicked button
                if(selectedMaterialModifierIndex < material.materialModifiers.size() && selectedMaterialModifierIndex >= 0)
                    modifiersPanel.sections = material.materialModifiers[selectedMaterialModifierIndex].sections;
            } 

            if(i == selectedMaterialModifierIndex){
                layerPanel.sections[0].elements[i].button.color = layerPanel.sections[0].elements[i].button.color2;
                layerPanel.sections[0].elements[i].button.textColor = ColorPalette::secondColor;
            }
            else{
                layerPanel.sections[0].elements[i].button.color = ColorPalette::secondColor;
                layerPanel.sections[0].elements[i].button.textColor = ColorPalette::oppositeColor;
            }

            if(layerPanel.sections[0].elements[i].button.clickState1){ //If a modifier button is clicked 

                anyPressed = true;

                layerPanel.sections[0].elements[i].button.color.a = 0.0f;
                layerPanel.sections[0].elements[i].button.color2.a = 0.0f;
                layerPanel.sections[0].elements[i].button.textColor.a = 0.0f;
                layerPanel.sections[0].elements[i].button.textColor2.a = 0.0f;

                // Check from bottom to top                
                for (int checkI = layerPanel.sections[0].elements.size() - 1; checkI >= 0; checkI--){
                    float btnPosY = layerPanel.sections[0].elements[checkI].button.resultPos.y - layerPanel.sections[0].elements[checkI].button.resultScale.y;
                    float cursorPosY = Mouse::cursorPos()->y;

                    if(cursorPosY > btnPosY){        
                        if(i < material.materialModifiers.size() && checkI < material.materialModifiers.size()){
                            if(i != checkI){
                                registerMaterialAction("Modifier moved", material);
                                modMoved = true;
                            }

                            selectedMaterialModifierIndex = checkI; //Select the clicked button

                            MaterialModifier topModifier = material.materialModifiers[checkI];
                            MaterialModifier currentModifier = material.materialModifiers[i];

                            material.materialModifiers[i] = topModifier;
                            material.materialModifiers[checkI] = currentModifier;

                            Element btn = layerPanel.sections[0].elements[i];
                            layerPanel.sections[0].elements.erase(layerPanel.sections[0].elements.begin() + i);
                            layerPanel.sections[0].elements.insert(layerPanel.sections[0].elements.begin() + checkI, btn);
                        }

                        break;
                    }
                }

                break;
            }
            else{
                layerPanel.sections[0].elements[i].button.color.a = 1.0f;
                layerPanel.sections[0].elements[i].button.color2.a = 1.0f;
                layerPanel.sections[0].elements[i].button.textColor.a = 1.0f;
                layerPanel.sections[0].elements[i].button.textColor2.a = 1.0f;
            }
        }

        if(!anyPressed && modMoved){
            this->dialogControl.firstFrameActivated = true;
            modMoved = false;
        }
    }
}


void MaterialEditorDialog::checkModifiersPanel(Material &material, TextureSelectionDialog &textureSelectionDialog){
    
    //Clear the modifiers panel as the panel starts
    if(dialogControl.firstFrameActivated){
        for (size_t modI = 0; modI < material.materialModifiers.size(); modI++)
        {
            for (size_t secI = 0; secI < material.materialModifiers[modI].sections.size(); secI++)
            {
                for (size_t elementI = 0; elementI < material.materialModifiers[modI].sections[secI].elements.size(); elementI++)
                {
                    //If button is clicked update the material
                    if(material.materialModifiers[modI].sections[secI].elements[elementI].state == 0)
                        material.materialModifiers[modI].sections[secI].elements[elementI].button.clicked = false;

                    //If range bar's value changed update the material
                    if(material.materialModifiers[modI].sections[secI].elements[elementI].state == 1){
                        material.materialModifiers[modI].sections[secI].elements[elementI].rangeBar.pointerPressed = false;
                        material.materialModifiers[modI].sections[secI].elements[elementI].rangeBar.leftArrowHover = false;
                        material.materialModifiers[modI].sections[secI].elements[elementI].rangeBar.rightArrowHover = false;
                        material.materialModifiers[modI].sections[secI].elements[elementI].rangeBar.defaultPointBtnPressed = false;

                    }

                    //If checkbox clicked update the material
                    if(material.materialModifiers[modI].sections[secI].elements[elementI].state == 2)
                        material.materialModifiers[modI].sections[secI].elements[elementI].checkBox.hover = false;
                }
            }
        }
    }
    
    
    //Update the material if interacted with modifier's panel
    for (size_t secI = 0; secI < modifiersPanel.sections.size(); secI++)
    {
        if(true){
            for (size_t elementI = 0; elementI < modifiersPanel.sections[secI].elements.size(); elementI++)
            {
                //If button is clicked update the material
                if(modifiersPanel.sections[secI].elements[elementI].state == 0)
                    if(modifiersPanel.sections[secI].elements[elementI].button.clicked){

                        registerMaterialAction("Modifier updated - " + modifiersPanel.sections[secI].elements[elementI].button.text, material);

                        if(secI == modifiersPanel.sections.size()-1 && elementI == modifiersPanel.sections[secI].elements.size()-1){
                            modifiersPanel.sections[secI].elements[0].button.filter.shader.ID = 0;
                            glDeleteTextures(1, &modifiersPanel.sections[secI].elements[0].button.texture.ID);
                            modifiersPanel.sections[secI].elements[0].button.texture = Texture();
                            material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[0].button.filter.shader.ID = 0; 
                            material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[0].button.texture = Texture(); 
                        }
                        if(secI == 0 && elementI == 0 && material.materialModifiers[this->selectedMaterialModifierIndex].modifierIndex == TEXTURE_MATERIAL_MODIFIER){
                            Texture* txtr = &modifiersPanel.sections[secI].elements[elementI].button.texture;
                            
                            if(txtr->ID){
                                Texture albedo;
                                Texture roughness;
                                Texture metallic;
                                Texture normalMap;
                                Texture heightMap;
                                Texture ambientOcc;

                                albedo.proceduralProps = txtr->proceduralProps;
                                albedo.generateProceduralDisplayingTexture(256, true);
                                albedo.title = "AutoFilledTexture";

                                roughness.proceduralProps = txtr->proceduralProps;
                                roughness.proceduralProps.proceduralBrightness = 1.f;
                                roughness.proceduralProps.proceduralGrayScale = true;
                                roughness.proceduralProps.proceduralNormalMap = false;
                                roughness.proceduralProps.proceduralNormalGrayScale = false;
                                roughness.proceduralProps.proceduralNormalStrength = 0.f;
                                roughness.proceduralProps.proceduralnverted = false;
                                roughness.generateProceduralDisplayingTexture(256, true);
                                roughness.title = "AutoFilledTexture";

                                metallic.proceduralProps = txtr->proceduralProps;
                                metallic.proceduralProps.proceduralBrightness = 1.f;
                                metallic.proceduralProps.proceduralGrayScale = true;
                                metallic.proceduralProps.proceduralNormalMap = false;
                                metallic.proceduralProps.proceduralNormalGrayScale = false;
                                metallic.proceduralProps.proceduralNormalStrength = 0.f;
                                metallic.proceduralProps.proceduralnverted = true;
                                metallic.generateProceduralDisplayingTexture(256, true);
                                metallic.title = "AutoFilledTexture";
                                
                                normalMap.proceduralProps = txtr->proceduralProps;
                                normalMap.proceduralProps.proceduralBrightness = 1.f;
                                normalMap.proceduralProps.proceduralGrayScale = false;
                                normalMap.proceduralProps.proceduralNormalMap = true;
                                normalMap.proceduralProps.proceduralNormalGrayScale = false;
                                normalMap.proceduralProps.proceduralNormalStrength = 1.f;
                                normalMap.proceduralProps.proceduralnverted = false;
                                normalMap.generateProceduralDisplayingTexture(256, true);
                                normalMap.title = "AutoFilledTexture";
                                
                                heightMap.proceduralProps = txtr->proceduralProps;
                                heightMap.proceduralProps.proceduralBrightness = 1.f;
                                heightMap.proceduralProps.proceduralGrayScale = true;
                                heightMap.proceduralProps.proceduralNormalMap = false;
                                heightMap.proceduralProps.proceduralNormalGrayScale = false;
                                heightMap.proceduralProps.proceduralNormalStrength = 0.f;
                                heightMap.proceduralProps.proceduralnverted = true;
                                heightMap.generateProceduralDisplayingTexture(256, true);
                                heightMap.title = "AutoFilledTexture";
                                
                                ambientOcc.proceduralProps = txtr->proceduralProps;
                                ambientOcc.proceduralProps.proceduralBrightness = 2.f;
                                ambientOcc.proceduralProps.proceduralGrayScale = true;
                                ambientOcc.proceduralProps.proceduralNormalMap = false;
                                ambientOcc.proceduralProps.proceduralNormalGrayScale = false;
                                ambientOcc.proceduralProps.proceduralNormalStrength = 0.f;
                                ambientOcc.proceduralProps.proceduralnverted = false;
                                ambientOcc.generateProceduralDisplayingTexture(256, true);
                                ambientOcc.title = "AutoFilledTexture";
                                
                                material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[1].button.texture = albedo;
                                material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[2].button.texture = roughness;
                                material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[3].button.texture = metallic;
                                material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[4].button.texture = normalMap;
                                material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[5].button.texture = heightMap;
                                material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[6].button.texture = ambientOcc;
                                
                                modifiersPanel.sections[secI].elements[1].button.texture = albedo;
                                modifiersPanel.sections[secI].elements[2].button.texture = roughness;
                                modifiersPanel.sections[secI].elements[3].button.texture = metallic;
                                modifiersPanel.sections[secI].elements[4].button.texture = normalMap;
                                modifiersPanel.sections[secI].elements[5].button.texture = heightMap;
                                modifiersPanel.sections[secI].elements[6].button.texture = ambientOcc;

                                txtr->title = "";
                                txtr->ID = 0;
                            }

                        }

                        this->updateTheMaterial = true;
                        //material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[elementI].button = modifiersPanel.sections[secI].elements[elementI].button; 
                    }

                //If range bar's value changed update the material
                if(modifiersPanel.sections[secI].elements[elementI].state == 1)
                    if(modifiersPanel.sections[secI].elements[elementI].rangeBar.pointerPressed == true || modifiersPanel.sections[secI].elements[elementI].rangeBar.defaultPointBtnPressed || (modifiersPanel.sections[secI].elements[elementI].rangeBar.leftArrowHover && *Mouse::LClick()) || (modifiersPanel.sections[secI].elements[elementI].rangeBar.rightArrowHover && *Mouse::LClick())){
                        if(modifiersPanel.sections[secI].elements[elementI].rangeBar.hover && *Mouse::LClick() || modifiersPanel.sections[secI].elements[elementI].rangeBar.defaultPointBtnPressed || (modifiersPanel.sections[secI].elements[elementI].rangeBar.leftArrowHover && *Mouse::LClick()) || (modifiersPanel.sections[secI].elements[elementI].rangeBar.rightArrowHover && *Mouse::LClick()))
                            registerMaterialAction("Modifier updated - " + modifiersPanel.sections[secI].elements[elementI].rangeBar.text, material);
                        
                        this->updateTheMaterial = true;
                        //material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[elementI].rangeBar = modifiersPanel.sections[secI].elements[elementI].rangeBar; 
                    }

                //If checkbox clicked update the material
                if(modifiersPanel.sections[secI].elements[elementI].state == 2)
                    if(modifiersPanel.sections[secI].elements[elementI].checkBox.hover && *Mouse::LClick() == true){
                        registerMaterialAction("Modifier updated - " + modifiersPanel.sections[secI].elements[elementI].checkBox.text, material);

                        this->updateTheMaterial = true;
                        //material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[elementI].checkBox = modifiersPanel.sections[secI].elements[elementI].checkBox; 
                    }
            }

        }
    }
    /*


    //If pressed to any of the texture modifier's channel button show the texture selection panel    
    if(modifiersPanel.sections.size() && material.materialModifiers.size()){
        if(material.materialModifiers[selectedMaterialModifierIndex].modifierIndex == TEXTURE_MATERIAL_MODIFIER) { //If is a texture modifier
            for (size_t i = 0; i < modifiersPanel.sections[0].elements.size(); i++) //For each channel
            {
                if(modifiersPanel.sections[0].elements[i].button.clickedMixVal){ //If clicked to any channel button in the texture modifier's panel
                    //Show the texture selection dialog
                    textureModifierTextureSelectingButtonIndex = i;
                    textureSelectionDialog.dialogControl.activate();
                }
            }
        }
    }
    */
}

void MaterialEditorDialog::checkTextureSelectionDialog(TextureSelectionDialog &textureSelectionDialog, Material &material){
    //If the texture selection dialog is active and indexing number indicates a channel button 
    if(textureSelectionDialog.dialogControl.isActive() && textureModifierTextureSelectingButtonIndex != 1000){
        if(textureSelectionDialog.selectedTextureIndex != 1000){// If a texture is selected in the texture selection dialog
            
            //Change the texture of the channel button
            //if(library.textures.size())
            //    modifiersPanel.sections[0].elements[textureModifierTextureSelectingButtonIndex].button.texture = library.textures[textureSelectionDialog.selectedTextureIndex];
            
            //Change the texture of the source modifier's texture 
            //if(library.textures.size())
            //    material.materialModifiers[selectedMaterialModifierIndex].sections[0].elements[textureModifierTextureSelectingButtonIndex].button.texture = library.textures[textureSelectionDialog.selectedTextureIndex];
            
            //Return to default after a texture is selected
            textureModifierTextureSelectingButtonIndex = 1000;
            textureSelectionDialog.selectedTextureIndex = 1000;
            textureSelectionDialog.dialogControl.unActivate();
            
            //Update the material after a selection is made
            material.updateMaterialDisplayingTexture((float)Settings::properties()->textureRes, true, this->displayerCamera, this->displayModeComboBox.selectedIndex, true, this->displayingFBO);
        }
    }
}

void MaterialEditorDialog::moveModifierToTop(int index, Material &material){
    if(index != 0){
        registerMaterialAction("Modifier moved to top", material);
        
        MaterialModifier topModifier = material.materialModifiers[index - 1];
        MaterialModifier currentModifier = material.materialModifiers[index];

        material.materialModifiers[index] = topModifier;
        material.materialModifiers[index - 1] = currentModifier;

        selectedMaterialModifierIndex--;
    }
}

void MaterialEditorDialog::moveModifierToBottom(int index, Material &material){
    if(ContextMenus::materialModifier.selectedElement != material.materialModifiers.size()-1){
        registerMaterialAction("Modifier moved to bottom", material);
        
        MaterialModifier bottomModifier = material.materialModifiers[ContextMenus::materialModifier.selectedElement + 1];
        MaterialModifier currentModifier = material.materialModifiers[ContextMenus::materialModifier.selectedElement];

        material.materialModifiers[ContextMenus::materialModifier.selectedElement] = bottomModifier;
        material.materialModifiers[ContextMenus::materialModifier.selectedElement + 1] = currentModifier;

        selectedMaterialModifierIndex++;
    }
}

void MaterialEditorDialog::manageContextMenuActions( Material &material)
{
    if(ContextMenus::materialModifier.dialogControl.isActive()){ //If material modifier context menu is active
        // Delete button pressed
        if(ContextMenus::materialModifier.contextPanel.sections[0].elements[0].button.clicked){
            registerMaterialAction("Texture modifier deleted", material);

            // Deleting the mask texture
            glDeleteTextures(1, &material.materialModifiers[ContextMenus::materialModifier.selectedElement].maskTexture.ID); 

            // Deleting the albedo filter's displaying texture
            glDeleteTextures(1, &modifiersPanel.sections[modifiersPanel.sections.size()-1].elements[0].button.texture.ID);
            glDeleteTextures(1, &modifiersPanel.sections[modifiersPanel.sections.size()-1].elements[1].button.texture.ID);
            
            if(material.materialModifiers[ContextMenus::materialModifier.selectedElement].modifierIndex == TEXTURE_MATERIAL_MODIFIER){
                glDeleteTextures(1, &material.materialModifiers[ContextMenus::materialModifier.selectedElement].sections[0].elements[1].button.texture.ID); 
                glDeleteTextures(1, &material.materialModifiers[ContextMenus::materialModifier.selectedElement].sections[0].elements[2].button.texture.ID); 
                glDeleteTextures(1, &material.materialModifiers[ContextMenus::materialModifier.selectedElement].sections[0].elements[3].button.texture.ID); 
                glDeleteTextures(1, &material.materialModifiers[ContextMenus::materialModifier.selectedElement].sections[0].elements[4].button.texture.ID); 
                glDeleteTextures(1, &material.materialModifiers[ContextMenus::materialModifier.selectedElement].sections[0].elements[5].button.texture.ID); 
                glDeleteTextures(1, &material.materialModifiers[ContextMenus::materialModifier.selectedElement].sections[0].elements[6].button.texture.ID); 
            }

            material.materialModifiers.erase(material.materialModifiers.begin() + ContextMenus::materialModifier.selectedElement);
            dialogControl.firstFrameActivated = true;
            selectedMaterialModifierIndex = 0;
        }
        
        //Move to top button pressed
        else if(ContextMenus::materialModifier.contextPanel.sections[0].elements[1].button.clicked){ 
            this->moveModifierToTop(ContextMenus::materialModifier.selectedElement, material);
            dialogControl.firstFrameActivated = true;

        }

        //Move to bottom button pressed
        else if(ContextMenus::materialModifier.contextPanel.sections[0].elements[2].button.clicked){ 
            this->moveModifierToBottom(ContextMenus::materialModifier.selectedElement, material);
            dialogControl.firstFrameActivated = true;
        }
        
        //Change mask button pressed
        else if(ContextMenus::materialModifier.contextPanel.sections[0].elements[3].button.clicked){ 
            
            registerMaterialAction("Modifier change mask", material);
            
            showTextureSelectionDialog(material.materialModifiers[ContextMenus::materialModifier.selectedElement].maskTexture, 128, false);

            dialogControl.firstFrameActivated = true;
        }
    }

    if(ContextMenus::addMaterialModifier.dialogControl.isActive()){ //If material modifier context menu is active
        
        char whitePixel[] = { 127, 127, 127, 127 }; // 1 pixel, RGBA format (white)
        
        // Texture Modifier
        if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[0].button.clicked){
            registerMaterialAction("Texture modifier added", material);
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.textureModifier);
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
            updateLayerPanel(material);
            selectedMaterialModifierIndex = 0;
        }
        
        // Dust Modifier
        else if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[1].button.clicked){
            registerMaterialAction("Dust modifier added", material);
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.dustModifier);
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
            updateLayerPanel(material);
            selectedMaterialModifierIndex = 0;
        }
        
        // Asphalt Modifier
        else if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[2].button.clicked){
            registerMaterialAction("Asphalt modifier added", material);
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.asphaltModifier);
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
            updateLayerPanel(material);
            selectedMaterialModifierIndex = 0;
        }
        
        // Liquid Modifier
        else if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[3].button.clicked){
            registerMaterialAction("Liquid modifier added", material);
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.liquidModifier);
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
            updateLayerPanel(material);
            selectedMaterialModifierIndex = 0;
        }
        
        // Moss Modifier
        else if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[4].button.clicked){
            registerMaterialAction("Moss modifier added", material);
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.mossModifier);
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
            updateLayerPanel(material);
            selectedMaterialModifierIndex = 0;
        }
        
        // Rust Modifier
        else if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[5].button.clicked){
            registerMaterialAction("Rust modifier added", material);
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.rustModifier);
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
            updateLayerPanel(material);
            selectedMaterialModifierIndex = 0;
        }
        
        // Skin Modifier
        else if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[6].button.clicked){
            registerMaterialAction("Skin modifier added", material);
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.skinModifier);
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
            updateLayerPanel(material);
            selectedMaterialModifierIndex = 0;
        }
        
        // Solid Modifier
        else if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[7].button.clicked){
            registerMaterialAction("Solid modifier added", material);
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.solidModifier);
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
            updateLayerPanel(material);
            selectedMaterialModifierIndex = 0;
        }
        
        // Wooden Modifier
        else if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[8].button.clicked){
            registerMaterialAction("Wooden modifier added", material);
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.woodenModifier);
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
            updateLayerPanel(material);
            selectedMaterialModifierIndex = 0;
        }
    }
}