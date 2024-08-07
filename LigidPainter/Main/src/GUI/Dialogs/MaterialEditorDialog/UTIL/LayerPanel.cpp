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

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

/// @brief If a layer hide-unhide button hovered (disable layer panel mouse tracking if so) (to prevent pressing layer panel and the eye button at the same time)
bool anyEyeHover = false;

/* Forward Declared Utility Functions */
static void renderEyes(Timer& timer, Panel &layerPanel, Material* material, bool& updateTheMaterial);
static void checkShortcutInteraction(Panel& layerPanel, Material* material);
static void updateLayerPanel(Panel& layerPanel, Material* material);
static void layerPanelInteractions(Panel& layerPanel, Panel& modifiersPanel, Material* material, int& selectedMaterialModifierIndex, int selectedResultModeIndex, bool& updateTheMaterial);
static void contextMenuInteractions(Material* material, Panel& modifiersPanel, Panel& layerPanel, bool& updateTheMaterial, int& selectedMaterialModifierIndex, Timer& timer);

void MaterialEditorDialog::renderLayerPanel(Timer& timer, bool mouseTrackingFlag, Material* material){
    // Update the elements of the layer panel according to the material's modifiers (if they doesn't match)
    updateLayerPanel(this->layerPanel, material);

    // Rendering the layer panel
    layerPanel.render(timer, mouseTrackingFlag && !anyEyeHover);

    // Render the hide - unhide buttons on top of the layer buttons
    renderEyes(timer, this->layerPanel, material, this->updateTheMaterial);
    
    // Check if a layer button got "ctrl shift w r-clicked" (add mask shortcut to the material if so)
    checkShortcutInteraction(this->layerPanel, material);

    // Check if a layer button got clicked or pressed (select the modifier if clicked) (move the modifier if pressed)
    layerPanelInteractions(this->layerPanel, this->modifiersPanel, material, this->selectedMaterialModifierIndex, this->selectedResultModeIndex, this->updateTheMaterial);

    // Layer panel's context menu interactions (add new modifier & modifier actions)
    contextMenuInteractions(material, this->modifiersPanel, this->layerPanel, this->updateTheMaterial, this->selectedMaterialModifierIndex, timer);
}












/* UTILITY FUNCTIONS */

static void renderEyes(Timer& timer, Panel &layerPanel, Material* material, bool& updateTheMaterial){
    
    anyEyeHover = false;
    
    if(layerPanel.sections.size()){
        for (size_t i = 0; i < layerPanel.sections[0].elements.size(); i++)
        {
            /// Modifier's layer button
            Button btn = layerPanel.sections[0].elements[i].button;
            
            /// The eye button (hide - unhide)
            Button eyeBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(0.6f), "", appTextures.eyeOpenedIcon, 0.f, false);
            eyeBtn.color.a = 0.f; 
            eyeBtn.outline = false; 
            eyeBtn.scale.y = btn.scale.y;
            eyeBtn.pos = btn.pos;
            eyeBtn.pos.x += btn.scale.x - eyeBtn.scale.x * 2.f;

            // Calculate the eye button's transform values on the window scale
            glm::vec3 resultPos = glm::vec3(UTIL::getPercent(*Settings::videoScale(), glm::vec2(eyeBtn.pos.x,eyeBtn.pos.y)), eyeBtn.pos.z); 
            glm::vec2 resultScale = UTIL::getPercent(*Settings::videoScale(), eyeBtn.scale);

            // Mouse hover animation
            if(Mouse::isMouseHover(resultScale, resultPos)){
                anyEyeHover = true;
                eyeBtn.pos.y -= 0.2f;    
                eyeBtn.scale += 0.05f;    
            }

            // Render & handle the interactions regarding the eye
            if(i < material->materialModifiers.size() && btn.scale.x > 3.f){ // If needs to be rendered & prevent rendering if the modifier button is small enough
                
                // If the eye is closed (modifier is hidden)
                if(material->materialModifiers[i].hide)
                    eyeBtn.texture = appTextures.eyeClosedIcon;

                // Render the eye
                eyeBtn.render(timer, !dialog_log.isHovered());

                // If clicked to the eye update the material & unhide or hide 
                if(eyeBtn.hover && *Mouse::LClick()){
                    updateTheMaterial = true;
                    material->materialModifiers[i].hide = !material->materialModifiers[i].hide;
                }
            }
        }
    }
}

static void checkShortcutInteraction(Panel& layerPanel, Material* material){
    if(layerPanel.sections.size()){
        // Check all the layers
        for (size_t elementI = 0; elementI < layerPanel.sections[0].elements.size(); elementI++){
            
            // If pressed to the ctrl shift w
            bool ctrlShiftWCondition = getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) && getContext()->window.isKeyPressed(LIGIDGL_KEY_W);
            
            // If a layer button got ctrl shift w r-clicked
            if((layerPanel.sections[0].elements[elementI].button.hover) && *Mouse::RClick() && ctrlShiftWCondition && elementI < material->materialModifiers.size()){
                
                // Create a new mask texture shortcut
                dialog_log.registerMaterialEditorAction("New material shortcut", material);
                material->materialShortcuts.push_back(MaterialShortcut("New_Shortcut", nullptr, &material->materialModifiers[elementI].maskTexture, elementI, -1, -1));
            }
        }
    }
}

static void updateLayerPanel(Panel& layerPanel, Material* material){
    
    bool needsUpdate = false;

    // Count doesn't match
    if(material->materialModifiers.size() != layerPanel.sections[0].elements.size())
        needsUpdate = true;

    // If the count matches
    if(!needsUpdate){
        // Content doesn't match
        for (size_t i = 0; i < material->materialModifiers.size(); i++)
        {
            if(layerPanel.sections[0].elements[i].button.text != material->materialModifiers[i].title)
                needsUpdate = true;
        }
    }
    
    // Update the elements of the layer panel according to the material's modifiers
    if(needsUpdate){
        layerPanel.sections[0].elements.clear();
        
        //Push the elements one by one from the materialModifiers
        for (size_t i = 0; i < material->materialModifiers.size(); i++)
        {
            Element btn = Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1.5f), material->materialModifiers[i].title , material->materialModifiers[i].maskTexture, 0.f, false));
            layerPanel.sections[0].elements.push_back(btn);
        }
    }
}

static bool modMoved = false; 

static void layerPanelInteractions(Panel& layerPanel, Panel& modifiersPanel, Material* material, int& selectedMaterialModifierIndex, int selectedResultModeIndex, bool& updateTheMaterial){
    
    if(layerPanel.sections.size()){
        bool anyPressed = false;
        for (size_t i = 0; i < layerPanel.sections[0].elements.size(); i++)
        {
            // Select the clicked modifier
            if(layerPanel.sections[0].elements[i].button.clicked){
                selectedMaterialModifierIndex = i; //Select the clicked button
                if(selectedResultModeIndex == 1)
                    updateTheMaterial = true;
                
                // Apply the selected modifier's sections to the modifier's panel
                if(selectedMaterialModifierIndex < material->materialModifiers.size() && selectedMaterialModifierIndex >= 0)
                    modifiersPanel.sections = material->materialModifiers[selectedMaterialModifierIndex].sections;
            } 

            // Coloring the selected modifier
            if(i == selectedMaterialModifierIndex){
                layerPanel.sections[0].elements[i].button.color = layerPanel.sections[0].elements[i].button.color2;
                layerPanel.sections[0].elements[i].button.textColor = ColorPalette::secondColor;
            }
            else{
                layerPanel.sections[0].elements[i].button.color = ColorPalette::secondColor;
                layerPanel.sections[0].elements[i].button.textColor = ColorPalette::oppositeColor;
            }

            // If pressed to a material modifier button (move the material modifier)
            if(layerPanel.sections[0].elements[i].button.clickState1){ 

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
                        if(i < material->materialModifiers.size() && checkI < material->materialModifiers.size()){
                            if(i != checkI){
                                dialog_log.registerMaterialEditorAction("Modifier moved", material);
                                modMoved = true;
                            }

                            selectedMaterialModifierIndex = checkI; //Select the clicked button

                            MaterialModifier topModifier = material->materialModifiers[checkI];
                            MaterialModifier currentModifier = material->materialModifiers[i];

                            material->materialModifiers[i] = topModifier;
                            material->materialModifiers[checkI] = currentModifier;

                            Element btn = layerPanel.sections[0].elements[i];
                            layerPanel.sections[0].elements.erase(layerPanel.sections[0].elements.begin() + i);
                            layerPanel.sections[0].elements.insert(layerPanel.sections[0].elements.begin() + checkI, btn);
 
                            if(i != checkI){
                                // Update the material shortcuts
                                for (size_t ii = 0; ii < material->materialShortcuts.size(); ii++)
                                {
                                    if(material->materialShortcuts[ii].modI == checkI)
                                        material->materialShortcuts[ii].updateElement(*material, i);
                                    else if(material->materialShortcuts[ii].modI == i)
                                        material->materialShortcuts[ii].updateElement(*material, checkI);
                                }
                            }
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
            updateTheMaterial = true;
            modMoved = false;
        }
    }
}


static void moveModifierToTop(int index, Material* material, int& selectedMaterialModifierIndex){
    if(index != 0){
        dialog_log.registerMaterialEditorAction("Modifier moved to top", material);

        MaterialModifier topModifier = material->materialModifiers[index - 1];
        MaterialModifier currentModifier = material->materialModifiers[index];

        material->materialModifiers[index] = topModifier;
        material->materialModifiers[index - 1] = currentModifier;
        
        // Update the material shortcuts
        for (size_t i = 0; i < material->materialShortcuts.size(); i++)
        {
            if(material->materialShortcuts[i].modI == index)
                material->materialShortcuts[i].updateElement(*material, index - 1);
            else if(material->materialShortcuts[i].modI == index - 1)
                material->materialShortcuts[i].updateElement(*material, index);
        }

        selectedMaterialModifierIndex--;
    }
}

static void moveModifierToBottom(int index, Material* material, int& selectedMaterialModifierIndex){
    if(index != material->materialModifiers.size()-1){
        dialog_log.registerMaterialEditorAction("Modifier moved to bottom", material);
        
        MaterialModifier bottomModifier = material->materialModifiers[index + 1];
        MaterialModifier currentModifier = material->materialModifiers[index];

        material->materialModifiers[index] = bottomModifier;
        material->materialModifiers[index + 1] = currentModifier;

        // Update the material shortcuts
        for (size_t i = 0; i < material->materialShortcuts.size(); i++)
        {
            if(material->materialShortcuts[i].modI == index)
                material->materialShortcuts[i].updateElement(*material, index + 1);
            else if(material->materialShortcuts[i].modI == index + 1)
                material->materialShortcuts[i].updateElement(*material, index);
        }

        selectedMaterialModifierIndex++;
    }
}

static void contextMenuInteractions(Material* material, Panel& modifiersPanel, Panel& layerPanel, bool& updateTheMaterial, int& selectedMaterialModifierIndex, Timer& timer)
{
    if(dialog_materialEditor.layerPanel.sections.size()){
        for (size_t elementI = 0; elementI < dialog_materialEditor.layerPanel.sections[0].elements.size(); elementI++)
        {
            if(dialog_materialEditor.layerPanel.sections[0].elements[elementI].button.hover && *Mouse::RClick()){
                int res = show_context_menu(timer, {"Delete", "Move to top", "Move to bottom", "Change mask"});   
               
                if( elementI < material->materialModifiers.size()){ //If material modifier context menu is active
                    
                    // Delete the material modifier button pressed
                    if(res == 0){            
                        dialog_log.registerMaterialEditorAction("Modifier deleted", material);

                        // Delete the related shortcut
                        for (size_t i = 0; i < material->materialShortcuts.size(); i++)
                        {
                            if(material->materialShortcuts[i].modI == elementI){
                                material->materialShortcuts.erase(material->materialShortcuts.begin() + i);
                                i--;
                            }
                        }
                        
                        for (size_t i = 0; i < material->materialShortcuts.size(); i++)
                        {
                            if(material->materialShortcuts[i].modI > elementI)
                                material->materialShortcuts[i].updateElement(*material, material->materialShortcuts[i].modI - 1);
                        }
                        
                        // Deleting the mask texture
                        glDeleteTextures(1, &material->materialModifiers[elementI].maskTexture.ID); 

                        // Deleting the albedo filter's displaying texture
                        // glDeleteTextures(1, &modifiersPanel.sections[modifiersPanel.sections.size()-1].elements[0].button.texture.ID);
                        // glDeleteTextures(1, &modifiersPanel.sections[modifiersPanel.sections.size()-1].elements[1].button.texture.ID);
                        
                        if(material->materialModifiers[elementI].modifierIndex == TEXTURE_MATERIAL_MODIFIER){
                            glDeleteTextures(1, &material->materialModifiers[elementI].sections[0].elements[1].button.texture.ID); 
                            glDeleteTextures(1, &material->materialModifiers[elementI].sections[0].elements[2].button.texture.ID); 
                            glDeleteTextures(1, &material->materialModifiers[elementI].sections[0].elements[3].button.texture.ID); 
                            glDeleteTextures(1, &material->materialModifiers[elementI].sections[0].elements[4].button.texture.ID); 
                            glDeleteTextures(1, &material->materialModifiers[elementI].sections[0].elements[5].button.texture.ID); 
                            glDeleteTextures(1, &material->materialModifiers[elementI].sections[0].elements[6].button.texture.ID); 
                        }

                        material->materialModifiers.erase(material->materialModifiers.begin() + elementI);
                        updateTheMaterial = true;
                        selectedMaterialModifierIndex = 0;
                    }
                    
                    //Move to top button pressed
                    else if(res == 1){ 
                        moveModifierToTop(elementI, material, selectedMaterialModifierIndex);
                        updateTheMaterial = true;

                    }

                    //Move to bottom button pressed
                    else if(res == 2){ 
                        moveModifierToBottom(elementI, material, selectedMaterialModifierIndex);
                        updateTheMaterial = true;
                    }
                    
                    //Change mask button pressed
                    else if(res == 3){ 
                        dialog_log.registerMaterialEditorAction("Modifier change mask", material);
                        
                        dialog_textureSelection.show(timer, material->materialModifiers[elementI].maskTexture, 128, false);

                        updateTheMaterial = true;
                    }
                }
            }
        }
    }


    // If clicked to add material modifier button
    if(dialog_materialEditor.layerPanel.barButtons[0].clicked){
        
        char whitePixel[] = { 127, 127, 127, 127 }; // 1 pixel, RGBA format (white)
        
        bool newModifierAdded = false;

        int res = show_context_menu(timer, {"Texture modifier", "Dust modifier", "Asphalt modifier", "Liquid modifier", "Moss modifier", "Rust modifier", "Skin modifier", "Solid modifier", "Wooden modifier", "Math modifier"});

        // Texture Modifier
        if(res == 0){
            dialog_log.registerMaterialEditorAction("Texture modifier added", material);
            material->materialModifiers.insert(material->materialModifiers.begin(), MaterialModifier(TEXTURE_MATERIAL_MODIFIER));
            newModifierAdded = true;
        }
        
        // Dust Modifier
        else if(res == 1){
            dialog_log.registerMaterialEditorAction("Dust modifier added", material);
            material->materialModifiers.insert(material->materialModifiers.begin(), MaterialModifier(DUST_MATERIAL_MODIFIER));
            newModifierAdded = true;
        }
        
        // Asphalt Modifier
        else if(res == 2){
            dialog_log.registerMaterialEditorAction("Asphalt modifier added", material);
            material->materialModifiers.insert(material->materialModifiers.begin(), MaterialModifier(ASPHALT_MATERIAL_MODIFIER));
            newModifierAdded = true;
        }
        
        // Liquid Modifier
        else if(res == 3){
            dialog_log.registerMaterialEditorAction("Liquid modifier added", material);
            material->materialModifiers.insert(material->materialModifiers.begin(), MaterialModifier(LIQUID_MATERIAL_MODIFIER));
            newModifierAdded = true;
        }
        
        // Moss Modifier
        else if(res == 4){
            dialog_log.registerMaterialEditorAction("Moss modifier added", material);
            material->materialModifiers.insert(material->materialModifiers.begin(), MaterialModifier(MOSS_MATERIAL_MODIFIER));
            newModifierAdded = true;
        }
        
        // Rust Modifier
        else if(res == 5){
            dialog_log.registerMaterialEditorAction("Rust modifier added", material);
            material->materialModifiers.insert(material->materialModifiers.begin(), MaterialModifier(RUST_MATERIAL_MODIFIER));
            newModifierAdded = true;
        }
        
        // Skin Modifier
        else if(res == 6){
            dialog_log.registerMaterialEditorAction("Skin modifier added", material);
            material->materialModifiers.insert(material->materialModifiers.begin(), MaterialModifier(SKIN_MATERIAL_MODIFIER));
            newModifierAdded = true;
        }
        
        // Solid Modifier
        else if(res == 7){
            dialog_log.registerMaterialEditorAction("Solid modifier added", material);
            material->materialModifiers.insert(material->materialModifiers.begin(), MaterialModifier(SOLID_MATERIAL_MODIFIER));
            newModifierAdded = true;
        }
        
        // Wooden Modifier
        else if(res == 8){
            dialog_log.registerMaterialEditorAction("Wooden modifier added", material);
            material->materialModifiers.insert(material->materialModifiers.begin(), MaterialModifier(WOODEN_MATERIAL_MODIFIER));
            newModifierAdded = true;
        }

        // Math Modifier
        else if(res == 9){
            dialog_log.registerMaterialEditorAction("Math modifier added", material);
            material->materialModifiers.insert(material->materialModifiers.begin(), MaterialModifier(MATH_MATERIAL_MODIFIER));
            newModifierAdded = true;
        }

        if(newModifierAdded){
            material->materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material->materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
            updateTheMaterial = true;
            selectedMaterialModifierIndex = 0;
        
            for (size_t i = 0; i < material->materialShortcuts.size(); i++)
            {
                material->materialShortcuts[i].updateElement(*material, material->materialShortcuts[i].modI + 1);
            }
        }
    }
}