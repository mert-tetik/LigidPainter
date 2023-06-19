/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"

#include <string>
#include <iostream>
#include <vector>


void updateMaterialDisplayerButton(Button &materialDisplayer, Material &material, Panel &bgPanel, Panel &modifiersPanel, Panel &layerPanel){
    //Update material displayer button
    materialDisplayer.texture = Texture(material.displayingTexture);
    materialDisplayer.pos = bgPanel.pos;
    materialDisplayer.scale = glm::vec2(35.f); 
    materialDisplayer.scale.x = (modifiersPanel.pos.x - modifiersPanel.scale.x) - (layerPanel.pos.x + layerPanel.scale.x); 
    materialDisplayer.scale.x /= 2.f;
    materialDisplayer.pos.x = modifiersPanel.pos.x - modifiersPanel.scale.x - materialDisplayer.scale.x;
}


void MaterialEditorDialog::updateLayerPanel(Material &material,int &textureRes,Box &box,Context &context){
    //Clear the elements of the layerPanel
    layerPanel.sections.clear();
    
    //Create the section
    Section layerPanelSection;
    
    //Give empty header to the new layer panel section
    layerPanelSection.header = Element(Button());
    
    //Push the elements one by one from the materialModifiers of the material
    for (size_t i = 0; i < material.materialModifiers.size(); i++)
    {
        layerPanelSection.elements.push_back(
            Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette,buttonShader,material.materialModifiers[i].title , Texture(), 0.f,true))
        );
    }

    //Give the section to the layerPanel
    layerPanel.sections.push_back(layerPanelSection);
    
    //Update the material after updating layerPanel
    updateMaterial(material,(float)textureRes,box,context);
    
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
        }
    }
}


void MaterialEditorDialog::checkModifiersPanel(Material &material,float textureRes,Box box,Context context,Mouse &mouse, TextureSelectionDialog &textureSelectionDialog){
    
    //Clear the modifiers panel as the panel starts
    if(dialogControl.firstFrameActivated){
        modifiersPanel.sections.clear();
    }
    
    //Update the material if interacted with modifier's panel
    for (size_t secI = 0; secI < modifiersPanel.sections.size(); secI++)
    {
        for (size_t elementI = 0; elementI < modifiersPanel.sections[secI].elements.size(); elementI++)
        {
            //If button is clicked update the material
            if(modifiersPanel.sections[secI].elements[elementI].state == 0)
                if(modifiersPanel.sections[secI].elements[elementI].button.clickedMixVal == 1.f)
                    updateMaterial(material,(float)textureRes,box,context);

            //If range bar's value changed update the material
            if(modifiersPanel.sections[secI].elements[elementI].state == 1)
                if(modifiersPanel.sections[secI].elements[elementI].rangeBar.pointerPressed == true)
                    updateMaterial(material,(float)textureRes,box,context);

            //If checkbox clicked update the material
            if(modifiersPanel.sections[secI].elements[elementI].state == 2)
                if(modifiersPanel.sections[secI].elements[elementI].checkBox.hover && mouse.LClick == true)
                    updateMaterial(material,(float)textureRes,box,context);
        }
    }


    //If pressed to any of the texture modifier's channel button show the texture selection panel    
    if(modifiersPanel.sections.size() && material.materialModifiers.size()){
        if(material.materialModifiers[selectedMaterialModifierIndex].modifierIndex == 0) { //If is a texture modifier
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
}

void MaterialEditorDialog::updateLayerPanelElements(Material &material,int &textureRes,Box &box,Context &context){
    //Update layer panal elements
    if  (
            layerPanel.barButtons[0].clickedMixVal == 1.f || //Pressed to add modifier button in the layer panel
            dialogControl.firstFrameActivated // Or in the first frame this dialog is activated
        )
    { 
        
        //If clicked to add modifier button add the texture modifier to the modifiers of the material
        if(!dialogControl.firstFrameActivated) 
            material.materialModifiers.push_back(appMaterialModifiers.textureModifier);

        //Creates layer panel elements from scratch using material.materialModifiers
        updateLayerPanel(material,textureRes,box,context);
    }
}

void MaterialEditorDialog::checkTextureSelectionDialog(TextureSelectionDialog &textureSelectionDialog, Material &material,Library &library,float textureRes,Box box,Context context){
    //If the texture selection dialog is active and indexing number indicates a channel button 
    if(textureSelectionDialog.dialogControl.isActive() && textureModifierTextureSelectingButtonIndex != 1000){
        if(textureSelectionDialog.selectedTextureIndex != 1000){// If a texture is selected in the texture selection dialog
            
            //Change the texture of the channel button
            modifiersPanel.sections[0].elements[textureModifierTextureSelectingButtonIndex].button.texture = library.textures[textureSelectionDialog.selectedTextureIndex];
            
            //Change the texture of the source modifier's texture 
            material.materialModifiers[selectedMaterialModifierIndex].sections[0].elements[textureModifierTextureSelectingButtonIndex].button.texture = library.textures[textureSelectionDialog.selectedTextureIndex];
            
            //Return to default after a texture is selected
            textureModifierTextureSelectingButtonIndex = 1000;
            textureSelectionDialog.selectedTextureIndex = 1000;
            textureSelectionDialog.dialogControl.unActivate();
            
            //Update the material after a selection is made
            updateMaterial(material,(float)textureRes,box,context);
        }
    }
}



void MaterialEditorDialog::render
                                (
                                    glm::vec2 videoScale,
                                    Mouse& mouse,
                                    Timer &timer,
                                    TextRenderer &textRenderer,
                                    TextureSelectionDialog &textureSelectionDialog,
                                    Library &library,
                                    Material &material,
                                    int textureRes,
                                    Box box,
                                    Context context
                                ){
    
    dialogControl.updateStart(buttonShader);

    //Render the panels & material displayer button
    bgPanel.render(videoScale,mouse,timer,textRenderer,!textureSelectionDialog.dialogControl.isActive());
    layerPanel.render(videoScale,mouse,timer,textRenderer,!textureSelectionDialog.dialogControl.isActive());
    modifiersPanel.render(videoScale,mouse,timer,textRenderer,!textureSelectionDialog.dialogControl.isActive());
    
    //Update the texture, scale & position of the material displayer button
    updateMaterialDisplayerButton(materialDisplayer, material, bgPanel, modifiersPanel, layerPanel);
    
    //If texture selection dialog is not active reset the index values used to navigate textures
    if(textureSelectionDialog.dialogControl.isActive() == false){
        textureModifierTextureSelectingButtonIndex = 1000;
        textureSelectionDialog.selectedTextureIndex = 1000;
    }

    //Make sure selectedMaterialModifierIndex won't cause any vector out of range error
    if(selectedMaterialModifierIndex >= material.materialModifiers.size()){
        selectedMaterialModifierIndex = material.materialModifiers.size()-1;
        if(selectedMaterialModifierIndex < 0)
            selectedMaterialModifierIndex = 0;
    }

    //Check the modifier's panel and update the material if interacted with any of the GUI element (show the texture selection dialog if pressed to a texture modifier's channel button)
    checkModifiersPanel(material,textureRes,box,context,mouse,textureSelectionDialog);

    //Check layerpanel if any modifier is clicked & change selectedMaterialModifierIndex if clicked
    checkLayerPanel(material);

    //Update the layer panel recreate all the modifiers using material.materialModifiers vector & add a new modifier if add is pressed to that vector
    updateLayerPanelElements(material,textureRes,box,context);
    
    //If texture selection done
    checkTextureSelectionDialog(textureSelectionDialog,material,library,textureRes,box,context);

    //Render the material displayer
    materialDisplayer.render(videoScale,mouse,timer,textRenderer,false);
    
    dialogControl.updateEnd(timer,buttonShader,0.3f);
}