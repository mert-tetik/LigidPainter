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

#include <string>
#include <iostream>
#include <vector>


void updateMaterialDisplayerButton(Button &materialDisplayer, Material &material, Panel &bgPanel, Panel &modifiersPanel, Panel &layerPanel){
    //Update material displayer button
    materialDisplayer.texture = Texture(material.displayingTexture);
    materialDisplayer.pos = bgPanel.pos + bgPanel.additionalPos;
    materialDisplayer.scale = glm::vec2(35.f); 
    materialDisplayer.scale.x = (modifiersPanel.pos.x + modifiersPanel.additionalPos.x - modifiersPanel.scale.x) - (layerPanel.pos.x + layerPanel.additionalPos.x + layerPanel.scale.x); 
    materialDisplayer.scale.x /= 2.f;
    materialDisplayer.pos.x = modifiersPanel.pos.x + modifiersPanel.additionalPos.x - modifiersPanel.scale.x - materialDisplayer.scale.x;
}

bool __materialEditorDialogESCPressed = false;
bool __materialEditorDialogESCFirstFramePressed = false;

bool __lastDisplayModeComboBoxPressed = false;

void MaterialEditorDialog::render
                                (
                                    Timer &timer,
                                    TextureSelectionDialog &textureSelectionDialog,
                                    Material &material,
                                    Box box
                                )
{
    
    dialogControl.updateStart();

    //Render the panels & material displayer button
    bgPanel.render(timer,        !(textureSelectionDialog.dialogControl.isActive() || ContextMenus::materialModifier.dialogControl.isActive() || ContextMenus::addMaterialModifier.dialogControl.isActive()));
    layerPanel.render(timer,     !(textureSelectionDialog.dialogControl.isActive() || ContextMenus::materialModifier.dialogControl.isActive() || ContextMenus::addMaterialModifier.dialogControl.isActive()));
    modifiersPanel.render(timer, !(textureSelectionDialog.dialogControl.isActive() || ContextMenus::materialModifier.dialogControl.isActive() || ContextMenus::addMaterialModifier.dialogControl.isActive()));
    barButton.render(timer,      !(textureSelectionDialog.dialogControl.isActive() || ContextMenus::materialModifier.dialogControl.isActive() || ContextMenus::addMaterialModifier.dialogControl.isActive()));

    barButton.pos.x = bgPanel.pos.x + bgPanel.additionalPos.x;
    barButton.pos.y = bgPanel.pos.y + bgPanel.additionalPos.y - bgPanel.scale.y - barButton.scale.y;

    bgPanel.additionalPos = -glm::vec3((*Settings::videoScale() - glm::vec2(getContext()->windowScale)) / *Settings::videoScale() * 50.f ,0);
    layerPanel.additionalPos = -glm::vec3((*Settings::videoScale() - glm::vec2(getContext()->windowScale)) / *Settings::videoScale() * 50.f ,0);
    modifiersPanel.additionalPos = -glm::vec3((*Settings::videoScale() - glm::vec2(getContext()->windowScale)) / *Settings::videoScale() * 50.f ,0);

    //Update the texture, scale & position of the material displayer button
    updateMaterialDisplayerButton(materialDisplayer, material, bgPanel, modifiersPanel, layerPanel);
    
    //If texture selection dialog is not active reset the index values used to navigate textures
    if(textureSelectionDialog.dialogControl.isActive() == false){
        textureModifierTextureSelectingButtonIndex = 1000;
        textureSelectionDialog.selectedTextureIndex = 0;
    }

    //Make sure selectedMaterialModifierIndex won't cause any vector out of range error
    if(selectedMaterialModifierIndex >= material.materialModifiers.size()){
        selectedMaterialModifierIndex = material.materialModifiers.size()-1;
        if(selectedMaterialModifierIndex < 0)
            selectedMaterialModifierIndex = 0;
    }

    //Check layerpanel if any modifier is clicked & change selectedMaterialModifierIndex if clicked
    checkLayerPanel(material);
    
    //Check the modifier's panel and update the material if interacted with any of the GUI element (show the texture selection dialog if pressed to a texture modifier's channel button)
    checkModifiersPanel(material, box, textureSelectionDialog);
    
    //Manage actions of the context menus 
    this->manageContextMenuActions(material, box);

    //Update the layer panel recreate all the modifiers using material.materialModifiers vector & add a new modifier if add is pressed to that vector
    updateLayerPanelElements(material, box);
    
    //If texture selection done
    checkTextureSelectionDialog(textureSelectionDialog,material, box);

    //Render the material displayer
    materialDisplayer.render(timer,false);

    this->displayModeComboBox.pos = layerPanel.pos + layerPanel.additionalPos;
    this->displayModeComboBox.pos.y -= materialDisplayer.scale.y + this->displayModeComboBox.scale.y;
    this->displayModeComboBox.pos.x += layerPanel.scale.x + materialDisplayer.scale.x;
    this->displayModeComboBox.render(timer, true);
    
    dialogControl.updateEnd(timer,0.15f);

    if(!this->updateTheMaterial && this->prevUpdateTheMaterial){
        material.updateMaterialDisplayingTexture((float)Settings::properties()->textureRes, box, true, this->displayerCamera, this->displayModeComboBox.selectedIndex);
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
    if(__materialEditorDialogESCFirstFramePressed || ((!bgPanel.hover && !barButton.hover) && *Mouse::LClick()) || (barButton.hover && *Mouse::LDoubleClick())){
        if(!wasTextureSelectionDialogActive() && !ContextMenus::materialModifier.dialogControl.isActive() && !ContextMenus::addMaterialModifier.dialogControl.isActive()){
            NodeScene::updateNodeResults(Settings::properties()->textureRes, -1);
            this->displayModeComboBox.selectedIndex = 0;
            material.updateMaterialDisplayingTexture((float)Settings::properties()->textureRes, box, false, this->displayerCamera, this->displayModeComboBox.selectedIndex);

            this->deactivate(textureSelectionDialog);
        }

    }

    __materialEditorDialogESCFirstFramePressed = false; 

    if((!ContextMenus::materialModifier.dialogControl.isActive() && !ContextMenus::addMaterialModifier.dialogControl.isActive() && getContext()->window.isMouseButtonPressed(LIGIDGL_MOUSE_BUTTON_RIGHT) == LIGIDGL_PRESS) || __lastDisplayModeComboBoxPressed)
        material.updateMaterialDisplayingTexture((float)Settings::properties()->textureRes, box, false, this->displayerCamera, this->displayModeComboBox.selectedIndex);

    __lastDisplayModeComboBoxPressed = this->displayModeComboBox.pressed;
}




















// -------------------- UTILITY FUNCTIONS --------------------


void MaterialEditorDialog::updateLayerPanel(Material &material, Box &box){
    //Clear the elements of the layerPanel
    layerPanel.sections.clear();
    
    //Create the section
    Section layerPanelSection;
    
    //Give empty header to the new layer panel section
    layerPanelSection.header = Element(Button());
    
    //Push the elements one by one from the materialModifiers of the material
    for (size_t i = 0; i < material.materialModifiers.size(); i++)
    {
        Element btn = Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1.5f), material.materialModifiers[i].title , material.materialModifiers[i].maskTexture, 0.f, true));
        layerPanelSection.elements.push_back(btn);
    }

    //Give the section to the layerPanel
    layerPanel.sections.push_back(layerPanelSection);
    
    //Update the material after updating layerPanel
    material.updateMaterialDisplayingTexture((float)Settings::properties()->textureRes, box, true,this->displayerCamera, this->displayModeComboBox.selectedIndex);
}


void MaterialEditorDialog::checkLayerPanel(Material &material){
    //Update the selected material modifier index (pressed to a modifier)
    if(layerPanel.sections.size()){
        for (size_t i = 0; i < layerPanel.sections[0].elements.size(); i++)
        {
            if(layerPanel.sections[0].elements[i].button.clickState1){ //If a modifier button is clicked 
                if(selectedMaterialModifierIndex != i){ //If the clicked button is not selected 
                    layerPanel.sections[0].elements[selectedMaterialModifierIndex].button.clickState1 = false; //Unselect the selected one
                    selectedMaterialModifierIndex = i; //Select the clicked button
                    if(material.materialModifiers.size()){
                        modifiersPanel.sections = material.materialModifiers[selectedMaterialModifierIndex].sections;
                    }
                    break; 
                }
            }

            //Keep the selected material modifier as selected
            if(i == selectedMaterialModifierIndex){
                layerPanel.sections[0].elements[i].button.clickState1 = true;
            }
        }
    }

}


void MaterialEditorDialog::checkModifiersPanel(Material &material,Box box,TextureSelectionDialog &textureSelectionDialog){
    
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
                    if(material.materialModifiers[modI].sections[secI].elements[elementI].state == 1)
                        material.materialModifiers[modI].sections[secI].elements[elementI].rangeBar.pointerPressed = false;

                    //If checkbox clicked update the material
                    if(material.materialModifiers[modI].sections[secI].elements[elementI].state == 2)
                        material.materialModifiers[modI].sections[secI].elements[elementI].checkBox.hover = false;
                }
            }
        }
        
        if(!material.materialModifiers.size())
            modifiersPanel.sections.clear();
        else 
            modifiersPanel.sections = material.materialModifiers[0].sections;
        
        this->selectedMaterialModifierIndex = 0;

    }
    
    
    //Update the material if interacted with modifier's panel
    for (size_t secI = 0; secI < modifiersPanel.sections.size(); secI++)
    {
        for (size_t elementI = 0; elementI < modifiersPanel.sections[secI].elements.size(); elementI++)
        {
            //If button is clicked update the material
            if(modifiersPanel.sections[secI].elements[elementI].state == 0)
                if(modifiersPanel.sections[secI].elements[elementI].button.clicked){
                    this->updateTheMaterial = true;
                    material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[elementI].button = modifiersPanel.sections[secI].elements[elementI].button; 
                }

            //If range bar's value changed update the material
            if(modifiersPanel.sections[secI].elements[elementI].state == 1)
                if(modifiersPanel.sections[secI].elements[elementI].rangeBar.pointerPressed == true || (modifiersPanel.sections[secI].elements[elementI].rangeBar.leftArrowHover && *Mouse::LClick()) || (modifiersPanel.sections[secI].elements[elementI].rangeBar.rightArrowHover && *Mouse::LClick())){
                    this->updateTheMaterial = true;
                    material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[elementI].rangeBar = modifiersPanel.sections[secI].elements[elementI].rangeBar; 
                }

            //If checkbox clicked update the material
            if(modifiersPanel.sections[secI].elements[elementI].state == 2)
                if(modifiersPanel.sections[secI].elements[elementI].checkBox.hover && *Mouse::LClick() == true){
                    this->updateTheMaterial = true;
                    material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[elementI].checkBox = modifiersPanel.sections[secI].elements[elementI].checkBox; 
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

void MaterialEditorDialog::updateLayerPanelElements(Material &material, Box &box){
    //Update layer panal elements
    if  (
            dialogControl.firstFrameActivated // Or in the first frame this dialog is activated
        )
    { 
        
        ////If clicked to add modifier button show the modifier selection context menu
        ////material.materialModifiers.insert(material.materialModifiers.begin(),appMaterialModifiers.dustModifier);
        
        //Creates layer panel elements from scratch using material.materialModifiers
        updateLayerPanel(material,box);
    }
}

void MaterialEditorDialog::checkTextureSelectionDialog(TextureSelectionDialog &textureSelectionDialog, Material &material,Box box){
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
            material.updateMaterialDisplayingTexture((float)Settings::properties()->textureRes, box, true, this->displayerCamera, this->displayModeComboBox.selectedIndex);
        }
    }
}

void MaterialEditorDialog::manageContextMenuActions( Material &material, Box box)
{
    if(ContextMenus::materialModifier.dialogControl.isActive()){ //If material modifier context menu is active
        //Delete button pressed
        if(ContextMenus::materialModifier.contextPanel.sections[0].elements[0].button.hover && *Mouse::LClick()){
            glDeleteTextures(1, &material.materialModifiers[ContextMenus::materialModifier.selectedElement].maskTexture.ID); 
            material.materialModifiers.erase(material.materialModifiers.begin() + ContextMenus::materialModifier.selectedElement);
            dialogControl.firstFrameActivated = true;
            selectedMaterialModifierIndex = 0;
            
            if(material.materialModifiers.size())
                modifiersPanel.sections = material.materialModifiers[0].sections;
            else
                modifiersPanel.sections.clear();
        }
        
        //Move to top button pressed
        if(ContextMenus::materialModifier.contextPanel.sections[0].elements[1].button.hover && *Mouse::LClick()){ 
            
            if(ContextMenus::materialModifier.selectedElement != 0){
                MaterialModifier topModifier = material.materialModifiers[ContextMenus::materialModifier.selectedElement - 1];
                MaterialModifier currentModifier = material.materialModifiers[ContextMenus::materialModifier.selectedElement];

                material.materialModifiers[ContextMenus::materialModifier.selectedElement] = topModifier;
                material.materialModifiers[ContextMenus::materialModifier.selectedElement - 1] = currentModifier;

                dialogControl.firstFrameActivated = true;
                selectedMaterialModifierIndex--;
            }
        }

        //Move to bottom button pressed
        if(ContextMenus::materialModifier.contextPanel.sections[0].elements[2].button.hover && *Mouse::LClick()){ 
            if(ContextMenus::materialModifier.selectedElement != material.materialModifiers.size()-1){
                MaterialModifier bottomModifier = material.materialModifiers[ContextMenus::materialModifier.selectedElement + 1];
                MaterialModifier currentModifier = material.materialModifiers[ContextMenus::materialModifier.selectedElement];

                material.materialModifiers[ContextMenus::materialModifier.selectedElement] = bottomModifier;
                material.materialModifiers[ContextMenus::materialModifier.selectedElement + 1] = currentModifier;

                dialogControl.firstFrameActivated = true;
                selectedMaterialModifierIndex++;
            }
        }
        
        //Change mask button pressed
        if(ContextMenus::materialModifier.contextPanel.sections[0].elements[3].button.hover && *Mouse::LClick()){ 
            showTextureSelectionDialog(material.materialModifiers[ContextMenus::materialModifier.selectedElement].maskTexture, 512);

            dialogControl.firstFrameActivated = true;
        }
    }

    if(ContextMenus::addMaterialModifier.dialogControl.isActive()){ //If material modifier context menu is active
        
        char whitePixel[] = { 127, 127, 127, 127 }; // 1 pixel, RGBA format (white)
        
        // Texture Modifier
        if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[0].button.hover && *Mouse::LClick()){
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.textureModifier);
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralID = 24;
            updateLayerPanel(material,box);
            modifiersPanel.sections = material.materialModifiers[0].sections;
            selectedMaterialModifierIndex = 0;
        }
        
        // Dust Modifier
        if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[1].button.hover && *Mouse::LClick()){
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.dustModifier);
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralID = 24;
            updateLayerPanel(material,box);
            modifiersPanel.sections = material.materialModifiers[0].sections;
            selectedMaterialModifierIndex = 0;
        }
        
        // Asphalt Modifier
        if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[2].button.hover && *Mouse::LClick()){
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.asphaltModifier);
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralID = 24;
            updateLayerPanel(material,box);
            modifiersPanel.sections = material.materialModifiers[0].sections;
            selectedMaterialModifierIndex = 0;
        }
        
        // Fabric Modifier
        if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[3].button.hover && *Mouse::LClick()){
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.fabricModifier);
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralID = 24;
            updateLayerPanel(material,box);
            modifiersPanel.sections = material.materialModifiers[0].sections;
            selectedMaterialModifierIndex = 0;
        }
        
        // Moss Modifier
        if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[4].button.hover && *Mouse::LClick()){
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.mossModifier);
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralID = 24;
            updateLayerPanel(material,box);
            modifiersPanel.sections = material.materialModifiers[0].sections;
            selectedMaterialModifierIndex = 0;
        }
        
        // Rust Modifier
        if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[5].button.hover && *Mouse::LClick()){
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.rustModifier);
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralID = 24;
            updateLayerPanel(material,box);
            modifiersPanel.sections = material.materialModifiers[0].sections;
            selectedMaterialModifierIndex = 0;
        }
        
        // Skin Modifier
        if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[6].button.hover && *Mouse::LClick()){
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.skinModifier);
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralID = 24;
            updateLayerPanel(material,box);
            modifiersPanel.sections = material.materialModifiers[0].sections;
            selectedMaterialModifierIndex = 0;
        }
        
        // Solid Modifier
        if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[7].button.hover && *Mouse::LClick()){
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.solidModifier);
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralID = 24;
            updateLayerPanel(material,box);
            modifiersPanel.sections = material.materialModifiers[0].sections;
            selectedMaterialModifierIndex = 0;
        }
        
        // Wooden Modifier
        if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[8].button.hover && *Mouse::LClick()){
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.woodenModifier);
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralID = 24;
            updateLayerPanel(material,box);
            modifiersPanel.sections = material.materialModifiers[0].sections;
            selectedMaterialModifierIndex = 0;
        }
    }
}