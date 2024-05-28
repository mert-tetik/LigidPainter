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

void modifiersPanelInteractions(Panel& modifiersPanel, Material* material, int& selectedMaterialModifierIndex, bool& updateTheMaterial, bool firstFrameActivated);

/* Forward Declared Utility Functions */
void MaterialEditorDialog::renderModifiersPanel(Timer& timer, bool mouseTrackingFlag, Material* material){
        
    // Update the sections of the panel
    if(this->selectedMaterialModifierIndex < material->materialModifiers.size())
        modifiersPanel.sections = material->materialModifiers[this->selectedMaterialModifierIndex].sections;
    else
        modifiersPanel.sections.clear();
    
    // Render the panel
    modifiersPanel.render(timer, mouseTrackingFlag);
    
    // Handle interaction with the modifier's panel
    modifiersPanelInteractions(this->modifiersPanel, material, this->selectedMaterialModifierIndex, this->updateTheMaterial, this->dialogControl.firstFrameActivated);

    // Update the sections of the modifier
    if(this->selectedMaterialModifierIndex < material->materialModifiers.size() && !shortcutPanel.hover)
        material->materialModifiers[this->selectedMaterialModifierIndex].sections = modifiersPanel.sections;

}













void modifiersPanelInteractions(Panel& modifiersPanel, Material* material, int& selectedMaterialModifierIndex, bool& updateTheMaterial, bool firstFrameActivated){    
    //Clear the modifiers panel as the panel starts
    if(firstFrameActivated){
        for (size_t modI = 0; modI < material->materialModifiers.size(); modI++)
        {
            for (size_t secI = 0; secI < material->materialModifiers[modI].sections.size(); secI++)
            {
                for (size_t elementI = 0; elementI < material->materialModifiers[modI].sections[secI].elements.size(); elementI++)
                {
                    //If button is clicked update the material
                    if(material->materialModifiers[modI].sections[secI].elements[elementI].state == 0)
                        material->materialModifiers[modI].sections[secI].elements[elementI].button.clicked = false;

                    //If range bar's value changed update the material
                    if(material->materialModifiers[modI].sections[secI].elements[elementI].state == 1){
                        material->materialModifiers[modI].sections[secI].elements[elementI].rangeBar.pointerPressed = false;
                        material->materialModifiers[modI].sections[secI].elements[elementI].rangeBar.leftArrowHover = false;
                        material->materialModifiers[modI].sections[secI].elements[elementI].rangeBar.rightArrowHover = false;
                        material->materialModifiers[modI].sections[secI].elements[elementI].rangeBar.defaultPointBtnPressed = false;

                    }

                    //If checkbox clicked update the material
                    if(material->materialModifiers[modI].sections[secI].elements[elementI].state == 2)
                        material->materialModifiers[modI].sections[secI].elements[elementI].checkBox.hover = false;
                }
            }
        }
    }
    
    if(selectedMaterialModifierIndex >= material->materialModifiers.size())
        return;

    //Update the material if interacted with modifier's panel
    for (size_t secI = 0; secI < modifiersPanel.sections.size(); secI++)
    {
        if(true){
            for (size_t elementI = 0; elementI < modifiersPanel.sections[secI].elements.size(); elementI++)
            {
                //If button is clicked update the material
                if(modifiersPanel.sections[secI].elements[elementI].state == 0){

                    if(modifiersPanel.sections[secI].elements[elementI].button.clicked){

                        dialog_log.registerMaterialEditorAction("Modifier updated - " + modifiersPanel.sections[secI].elements[elementI].button.text, material);

                        if(secI == modifiersPanel.sections.size()-1 && elementI == modifiersPanel.sections[secI].elements.size()-1){
                            modifiersPanel.sections[secI].elements[0].button.filter.shader.ID = 0;
                            glDeleteTextures(1, &modifiersPanel.sections[secI].elements[0].button.texture.ID);
                            modifiersPanel.sections[secI].elements[0].button.texture = Texture();
                            if(secI < material->materialModifiers[selectedMaterialModifierIndex].sections.size()){
                                material->materialModifiers[selectedMaterialModifierIndex].sections[secI].elements[0].button.filter.shader.ID = 0; 
                                material->materialModifiers[selectedMaterialModifierIndex].sections[secI].elements[0].button.texture = Texture(); 
                            }
                        }
                        if(secI == 0 && elementI == 0 && material->materialModifiers[selectedMaterialModifierIndex].modifierIndex == TEXTURE_MATERIAL_MODIFIER){
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
                                
                                material->materialModifiers[selectedMaterialModifierIndex].sections[secI].elements[1].button.texture = albedo;
                                material->materialModifiers[selectedMaterialModifierIndex].sections[secI].elements[2].button.texture = roughness;
                                material->materialModifiers[selectedMaterialModifierIndex].sections[secI].elements[3].button.texture = metallic;
                                material->materialModifiers[selectedMaterialModifierIndex].sections[secI].elements[4].button.texture = normalMap;
                                material->materialModifiers[selectedMaterialModifierIndex].sections[secI].elements[5].button.texture = heightMap;
                                material->materialModifiers[selectedMaterialModifierIndex].sections[secI].elements[6].button.texture = ambientOcc;
                                
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

                        updateTheMaterial = true;
                    }
                }

                //If range bar's value changed update the material
                if(modifiersPanel.sections[secI].elements[elementI].state == 1){
                    if(modifiersPanel.sections[secI].elements[elementI].rangeBar.pointerPressed == true || modifiersPanel.sections[secI].elements[elementI].rangeBar.defaultPointBtnPressed || (modifiersPanel.sections[secI].elements[elementI].rangeBar.leftArrowHover && *Mouse::LClick()) || (modifiersPanel.sections[secI].elements[elementI].rangeBar.rightArrowHover && *Mouse::LClick())){
                        if(modifiersPanel.sections[secI].elements[elementI].rangeBar.hover && *Mouse::LClick() || modifiersPanel.sections[secI].elements[elementI].rangeBar.defaultPointBtnPressed || (modifiersPanel.sections[secI].elements[elementI].rangeBar.leftArrowHover && *Mouse::LClick()) || (modifiersPanel.sections[secI].elements[elementI].rangeBar.rightArrowHover && *Mouse::LClick()))
                            dialog_log.registerMaterialEditorAction("Modifier updated - " + modifiersPanel.sections[secI].elements[elementI].rangeBar.text, material);
                        
                        updateTheMaterial = true;
                    }
                }

                //If checkbox clicked update the material
                if(modifiersPanel.sections[secI].elements[elementI].state == 2){
                    if(modifiersPanel.sections[secI].elements[elementI].checkBox.hover && *Mouse::LClick() == true){
                        dialog_log.registerMaterialEditorAction("Modifier updated - " + modifiersPanel.sections[secI].elements[elementI].checkBox.text, material);

                        updateTheMaterial = true;
                    }
                }

                //If checkbox clicked update the material
                if(modifiersPanel.sections[secI].elements[elementI].state == 3){
                    if(modifiersPanel.sections[secI].elements[elementI].comboBox.selectionDone){
                        dialog_log.registerMaterialEditorAction("Modifier updated - " + modifiersPanel.sections[secI].elements[elementI].checkBox.text, material);

                        updateTheMaterial = true;
                    }
                }
            }
        }
    }

    for (size_t secI = 0; secI < material->materialModifiers[selectedMaterialModifierIndex].sections.size(); secI++){
        for (size_t elementI = 0; elementI < material->materialModifiers[selectedMaterialModifierIndex].sections[secI].elements.size(); elementI++){
            bool ctrlShiftWCondition = getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) && getContext()->window.isKeyPressed(LIGIDGL_KEY_W);
            if((material->materialModifiers[selectedMaterialModifierIndex].sections[secI].elements[elementI].button.hover || material->materialModifiers[selectedMaterialModifierIndex].sections[secI].elements[elementI].rangeBar.hover || material->materialModifiers[selectedMaterialModifierIndex].sections[secI].elements[elementI].checkBox.hover) && *Mouse::RClick() && ctrlShiftWCondition){
                dialog_log.registerMaterialEditorAction("New material shortcut", material);
                material->materialShortcuts.push_back(MaterialShortcut("New_Shortcut", &material->materialModifiers[selectedMaterialModifierIndex].sections[secI].elements[elementI], nullptr, selectedMaterialModifierIndex, secI, elementI));
            }
        }
    }
}