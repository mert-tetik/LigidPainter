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

bool __materialEditorDialogESCPressed = false;
bool __materialEditorDialogESCFirstFramePressed = false;

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
                                    Context context,
                                    std::vector<ContextMenu> &contextMenus
                                )
{
    
    dialogControl.updateStart(buttonShader);



    //Render the panels & material displayer button
    bgPanel.render(videoScale,mouse,timer,textRenderer,         !(textureSelectionDialog.dialogControl.isActive() || contextMenus[6].dialogControl.isActive() || contextMenus[8].dialogControl.isActive()));
    layerPanel.render(videoScale,mouse,timer,textRenderer,      !(textureSelectionDialog.dialogControl.isActive() || contextMenus[6].dialogControl.isActive() || contextMenus[8].dialogControl.isActive()));
    modifiersPanel.render(videoScale,mouse,timer,textRenderer,  !(textureSelectionDialog.dialogControl.isActive() || contextMenus[6].dialogControl.isActive() || contextMenus[8].dialogControl.isActive()));
    barButton.render(videoScale,mouse,timer,textRenderer,       !(textureSelectionDialog.dialogControl.isActive() || contextMenus[6].dialogControl.isActive() || contextMenus[8].dialogControl.isActive()));

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
    checkLayerPanel(material,mouse,contextMenus,videoScale );
    
    //Check the modifier's panel and update the material if interacted with any of the GUI element (show the texture selection dialog if pressed to a texture modifier's channel button)
    checkModifiersPanel(material,textureRes,box,context,mouse,textureSelectionDialog);
    
    //Manage actions of the context menus 
    this->manageContextMenuActions(library, mouse, material, textureRes, box, context, contextMenus);

    //Update the layer panel recreate all the modifiers using material.materialModifiers vector & add a new modifier if add is pressed to that vector
    updateLayerPanelElements(material, textureRes, box, context, mouse, videoScale, contextMenus);
    
    //If texture selection done
    checkTextureSelectionDialog(textureSelectionDialog,material,library,textureRes,box,context);

    //Render the material displayer
    materialDisplayer.render(videoScale,mouse,timer,textRenderer,false);
    
    dialogControl.updateEnd(timer,buttonShader,0.15f);

    if(!this->updateTheMaterial && this->prevUpdateTheMaterial){
        material.updateMaterialDisplayingTexture((float)textureRes, box, context, buttonShader, tdModelShader,sphereModel, heightToNormalShader);
    }
    
    this->prevUpdateTheMaterial = this->updateTheMaterial;
    this->updateTheMaterial = false;


    if(context.window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS){
        if(!__materialEditorDialogESCPressed)
            __materialEditorDialogESCFirstFramePressed = true;
        __materialEditorDialogESCPressed = true;
    }
    else{
        __materialEditorDialogESCPressed = false;
    } 


    //Close the dialog
    if(__materialEditorDialogESCFirstFramePressed || ((!bgPanel.hover && !barButton.hover) && mouse.LClick) || (barButton.hover && mouse.LDoubleClick)){
        if(!wasTextureSelectionDialogActive() && !contextMenus[6].dialogControl.isActive() && !contextMenus[8].dialogControl.isActive())
            this->deactivate(textureSelectionDialog);

    }

    __materialEditorDialogESCFirstFramePressed = false; 
}




















// -------------------- UTILITY FUNCTIONS --------------------


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
        Element btn = Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1.5f), colorPalette,buttonShader, material.materialModifiers[i].title , material.materialModifiers[i].maskTexture, 0.f, true));
        layerPanelSection.elements.push_back(btn);
    }

    //Give the section to the layerPanel
    layerPanel.sections.push_back(layerPanelSection);
    
    //Update the material after updating layerPanel
    material.updateMaterialDisplayingTexture((float)textureRes, box, context, buttonShader,tdModelShader, sphereModel, heightToNormalShader);
}


void MaterialEditorDialog::checkLayerPanel(Material &material, Mouse &mouse, std::vector<ContextMenu> &contextMenus, glm::vec2 videoScale){
    //Update the selected material modifier index (pressed to a modifier)
    if(layerPanel.sections.size()){
        for (size_t i = 0; i < layerPanel.sections[0].elements.size(); i++)
        {
            if(layerPanel.sections[0].elements[i].button.clickState1){ //If a modifier button is clicked 
                if(selectedMaterialModifierIndex != i){ //If the clicked button is not selected 
                    layerPanel.sections[0].elements[selectedMaterialModifierIndex].button.clickState1 = false; //Unselect the selected one
                    selectedMaterialModifierIndex = i; //Select the clicked button
                    break; 
                }
            }

            //If clicked to a mofifier button
            if(layerPanel.sections[0].elements[i].button.hover && mouse.LClick){
                if(material.materialModifiers.size()){
                    modifiersPanel.sections = material.materialModifiers[selectedMaterialModifierIndex].sections;
                }
            }

            //Keep the selected material modifier as selected
            if(i == selectedMaterialModifierIndex){
                layerPanel.sections[0].elements[i].button.clickState1 = true;
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
                if(modifiersPanel.sections[secI].elements[elementI].button.clicked){
                    this->updateTheMaterial = true;
                    material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[elementI].button = modifiersPanel.sections[secI].elements[elementI].button; 
                }

            //If range bar's value changed update the material
            if(modifiersPanel.sections[secI].elements[elementI].state == 1)
                if(modifiersPanel.sections[secI].elements[elementI].rangeBar.pointerPressed == true){
                    this->updateTheMaterial = true;
                    material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[elementI].rangeBar = modifiersPanel.sections[secI].elements[elementI].rangeBar; 
                }

            //If checkbox clicked update the material
            if(modifiersPanel.sections[secI].elements[elementI].state == 2)
                if(modifiersPanel.sections[secI].elements[elementI].checkBox.hover && mouse.LClick == true){
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

void MaterialEditorDialog::updateLayerPanelElements(Material &material,int &textureRes,Box &box,Context &context, Mouse& mouse, glm::vec2 videoScale, std::vector<ContextMenu> contextMenus){
    //Update layer panal elements
    if  (
            dialogControl.firstFrameActivated // Or in the first frame this dialog is activated
        )
    { 
        
        ////If clicked to add modifier button show the modifier selection context menu
        ////material.materialModifiers.insert(material.materialModifiers.begin(),appMaterialModifiers.dustModifier);
        
        //Creates layer panel elements from scratch using material.materialModifiers
        updateLayerPanel(material,textureRes,box,context);
    }
}

void MaterialEditorDialog::checkTextureSelectionDialog(TextureSelectionDialog &textureSelectionDialog, Material &material,Library &library,float textureRes,Box box,Context context){
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
            material.updateMaterialDisplayingTexture((float)textureRes, box, context, buttonShader,tdModelShader , sphereModel, heightToNormalShader);

        }
    }
}

void MaterialEditorDialog::manageContextMenuActions(Library &library, Mouse &mouse, Material &material, int textureRes, Box box, Context context, std::vector<ContextMenu> &contextMenus)
{
    if(contextMenus[6].dialogControl.isActive()){ //If material modifier context menu is active
        
        //Delete button pressed
        if(contextMenus[6].contextPanel.sections[0].elements[0].button.hover && mouse.LClick){ 
            material.materialModifiers.erase(material.materialModifiers.begin() + contextMenus[6].selectedElement);
            dialogControl.firstFrameActivated = true;
            selectedMaterialModifierIndex = 0;
        }
        
        //Move to top button pressed
        if(contextMenus[6].contextPanel.sections[0].elements[1].button.hover && mouse.LClick){ 
            
            if(contextMenus[6].selectedElement != 0){
                MaterialModifier topModifier = material.materialModifiers[contextMenus[6].selectedElement - 1];
                MaterialModifier currentModifier = material.materialModifiers[contextMenus[6].selectedElement];

                material.materialModifiers[contextMenus[6].selectedElement] = topModifier;
                material.materialModifiers[contextMenus[6].selectedElement - 1] = currentModifier;

                dialogControl.firstFrameActivated = true;
                selectedMaterialModifierIndex--;
            }
        }

        //Move to bottom button pressed
        if(contextMenus[6].contextPanel.sections[0].elements[2].button.hover && mouse.LClick){ 
            if(contextMenus[6].selectedElement != material.materialModifiers.size()-1){
                MaterialModifier bottomModifier = material.materialModifiers[contextMenus[6].selectedElement + 1];
                MaterialModifier currentModifier = material.materialModifiers[contextMenus[6].selectedElement];

                material.materialModifiers[contextMenus[6].selectedElement] = bottomModifier;
                material.materialModifiers[contextMenus[6].selectedElement + 1] = currentModifier;

                dialogControl.firstFrameActivated = true;
                selectedMaterialModifierIndex++;
            }
        }
        
        //Change mask button pressed
        if(contextMenus[6].contextPanel.sections[0].elements[3].button.hover && mouse.LClick){ 
            showTextureSelectionDialog(material.materialModifiers[contextMenus[6].selectedElement].maskTexture);

            dialogControl.firstFrameActivated = true;
        }
    }

    if(contextMenus[8].dialogControl.isActive()){ //If material modifier context menu is active
        
        //Delete button pressed
        
        // Texture Modifier
        if(contextMenus[8].contextPanel.sections[0].elements[0].button.hover && mouse.LClick){
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.textureModifier);
            updateLayerPanel(material,textureRes,box,context);
        }
        
        // Dust Modifier
        if(contextMenus[8].contextPanel.sections[0].elements[1].button.hover && mouse.LClick){
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.dustModifier);
            updateLayerPanel(material,textureRes,box,context);
        }
        
        // Asphalt Modifier
        if(contextMenus[8].contextPanel.sections[0].elements[2].button.hover && mouse.LClick){
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.asphaltModifier);
            updateLayerPanel(material,textureRes,box,context);
        }
        
        // Fabric Modifier
        if(contextMenus[8].contextPanel.sections[0].elements[3].button.hover && mouse.LClick){
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.fabricModifier);
            updateLayerPanel(material,textureRes,box,context);
        }
        
        // Marble Modifier
        if(contextMenus[8].contextPanel.sections[0].elements[4].button.hover && mouse.LClick){
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.marbleModifier);
            updateLayerPanel(material,textureRes,box,context);
        }
        
        // Moss Modifier
        if(contextMenus[8].contextPanel.sections[0].elements[5].button.hover && mouse.LClick){
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.mossModifier);
            updateLayerPanel(material,textureRes,box,context);
        }
        
        // Rust Modifier
        if(contextMenus[8].contextPanel.sections[0].elements[6].button.hover && mouse.LClick){
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.rustModifier);
            updateLayerPanel(material,textureRes,box,context);
        }
        
        // Skin Modifier
        if(contextMenus[8].contextPanel.sections[0].elements[7].button.hover && mouse.LClick){
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.skinModifier);
            updateLayerPanel(material,textureRes,box,context);
        }
        
        // Solid Modifier
        if(contextMenus[8].contextPanel.sections[0].elements[8].button.hover && mouse.LClick){
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.solidModifier);
            updateLayerPanel(material,textureRes,box,context);
        }
        
        // Wooden Modifier
        if(contextMenus[8].contextPanel.sections[0].elements[9].button.hover && mouse.LClick){
            material.materialModifiers.insert(material.materialModifiers.begin(), appMaterialModifiers.woodenModifier);
            updateLayerPanel(material,textureRes,box,context);
        }
    }
}