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
#include "SettingsSystem/Settings.hpp"
#include "MouseSystem/Mouse.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

/* Forward Declared Utility Functions */
static void refreshShortcutPanelSections(Panel& shortcutPanel, Material* material, int shortcutRenamingIndex);
static void shortcutRenamingPart(Timer& timer, Panel& shortcutPanel, int& shortcutRenamingIndex, TextBox& shortcutRenamingTextbox, Material* material);
static void shortcutPanelInteractions(Panel& shortcutPanel, Material* material, bool& updateTheMaterial, int& shortcutRenamingIndex, TextBox& shortcutRenamingTextbox);

void MaterialEditorDialog::renderShortcutPanel(Timer& timer, bool mouseTrackingFlag){    
    
    // Refresh the sections of the shortcut panel (remove & re-create the shortcut elements)
    refreshShortcutPanelSections(this->shortcutPanel, this->material, this->shortcutRenamingIndex);
    
    // Rendering the shortcut panel
    shortcutPanel.render(timer, mouseTrackingFlag);
    
    // Render the textbox if needed & rename the shortcut
    shortcutRenamingPart(timer, this->shortcutPanel, this->shortcutRenamingIndex, this->shortcutRenamingTextbox, this->material);

    // Mouse interactions with the shortcut panel (if buttons were pressed)
    shortcutPanelInteractions(this->shortcutPanel, this->material, this->updateTheMaterial, this->shortcutRenamingIndex, this->shortcutRenamingTextbox);
}








/* UTILITY FUNCTIONS */

static void refreshShortcutPanelSections(Panel& shortcutPanel, Material* material, int shortcutRenamingIndex){
    
    // Update the shortcut elements of the material
    for (size_t i = 0; i < material->materialShortcuts.size(); i++)
    {
        material->materialShortcuts[i].updateElement(*material, material->materialShortcuts[i].modI);
    }
    
    // Remove all the sections related to the shortcuts (first section has the info text & stuff)
    shortcutPanel.sections.erase(shortcutPanel.sections.begin() + 1, shortcutPanel.sections.end());  

    // Create the shortcut sections into the shortcutPanel's sections
    for (size_t i = 0; i < material->materialShortcuts.size(); i++)
    {
        Section section;

        // If the shortcut is an element shortcut
        if(material->materialShortcuts[i].element != nullptr){
            section = Section(
                                        SectionHolder(ColorPalette::secondColor, ColorPalette::thirdColor, 3.f, material->materialShortcuts[i].title),
                                        {
                                            *material->materialShortcuts[i].element,
                                            Button(ELEMENT_STYLE_STYLIZED, glm::vec2(1.f), "Rename", Texture(), 2.f, false),
                                            Button(ELEMENT_STYLE_STYLIZED, glm::vec2(1.f), "Delete", Texture(), 1.f, false),
                                        }
                                    );
            section.elements[0].panelOffset = 1.f;
        }
        
        // If the shortcut is a mask texture shortcut
        if(material->materialShortcuts[i].maskTxtr != nullptr){
            section = Section(
                                        SectionHolder(ColorPalette::secondColor, ColorPalette::thirdColor, 3.f, material->materialShortcuts[i].title),
                                        {
                                            Button(ELEMENT_STYLE_BASIC, glm::vec2(5.f), "Modifier Mask Texture", *material->materialShortcuts[i].maskTxtr, 1.f, false),
                                            CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Use UV", 0.f),
                                            Button(ELEMENT_STYLE_STYLIZED, glm::vec2(1.f), "Rename", Texture(), 1.f, false),
                                            Button(ELEMENT_STYLE_STYLIZED, glm::vec2(1.f), "Delete", Texture(), 1.f, false),
                                        }
                                    );

            section.elements[0].button.textureSizeScale = 0.95f;
            section.elements[1].checkBox.clickState1 = material->materialShortcuts[i].maskTxtr->proceduralProps.proceduralUseTexCoords;
            section.elements[1].checkBox.clickedMixVal = material->materialShortcuts[i].maskTxtr->proceduralProps.proceduralUseTexCoords;
        }

        section.elements[section.elements.size() - 1].button.color = glm::vec4(1.f, 0.f, 0.f, 0.5f);

        // Create the header's text as invisible while renaming this shortcut (there's a textbox rendering on top of it)        
        if(shortcutRenamingIndex == i){
            section.header.sectionHolder.textColor = ColorPalette::secondColor;
            section.header.sectionHolder.textColor.a = 1.f;
        }
        
        section.header.sectionHolder.sectionColor.a = 1.f;
        section.header.sectionHolder.active = true;
        section.header.sectionHolder.fullActive = true;

        // Push the section into the shortcut panel's sections
        shortcutPanel.sections.push_back(section);    
    }
}

static void shortcutRenamingPart(Timer& timer, Panel& shortcutPanel, int& shortcutRenamingIndex, TextBox& shortcutRenamingTextbox, Material* material){
    if(shortcutRenamingIndex != -1 && shortcutRenamingIndex + 1 < shortcutPanel.sections.size() && shortcutRenamingIndex < material->materialShortcuts.size()){
        // Prep & render the textbox
        shortcutRenamingTextbox.active = true;
        shortcutRenamingTextbox.pos = shortcutPanel.sections[shortcutRenamingIndex + 1].header.pos;
        shortcutRenamingTextbox.scale = shortcutPanel.sections[shortcutRenamingIndex + 1].header.scale;
        shortcutRenamingTextbox.scale.y += 0.2f;
        shortcutRenamingTextbox.render(timer, false);

        // Apply the text of the textbox to the renamed shortcut
        material->materialShortcuts[shortcutRenamingIndex].title = shortcutRenamingTextbox.text; 

        // Quit renaming the shortcut
        if(*Mouse::LClick() && !shortcutRenamingTextbox.hover || getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) || getContext()->window.isKeyPressed(LIGIDGL_KEY_ENTER)){
            shortcutRenamingIndex = -1;
        }
    }
}

static void shortcutPanelInteractions(Panel& shortcutPanel, Material* material, bool& updateTheMaterial, int& shortcutRenamingIndex, TextBox& shortcutRenamingTextbox){
    
    // Check all the shortcuts
    for (size_t i = 0; i < material->materialShortcuts.size(); i++){
    
        // Rename button of the shortcut is clicked
        if(shortcutPanel.sections[i + 1].elements[shortcutPanel.sections[i + 1].elements.size() - 2].button.hover && *Mouse::LClick()){
            registerMaterialAction("Material shortcut renamed", *material);
            shortcutRenamingIndex = i;
            shortcutRenamingTextbox.text = material->materialShortcuts[i].title; 
            shortcutRenamingTextbox.activeChar = shortcutRenamingTextbox.text.size()-1; 
            shortcutRenamingTextbox.activeChar2 = shortcutRenamingTextbox.text.size()-1;
        }
        
        // Delete button of the shortcut is clicked
        if(shortcutPanel.sections[i + 1].elements[shortcutPanel.sections[i + 1].elements.size() - 1].button.hover && *Mouse::LClick()){
            registerMaterialAction("Material shortcut removed", *material);
            material->materialShortcuts.erase(material->materialShortcuts.begin() + i);    
            break;
        }
        
        // Apply the changes to the original element if the shortcut is an element shortcut 
        if(material->materialShortcuts[i].element != nullptr){
            if(material->materialShortcuts[i].element->isInteracted())
                updateTheMaterial = true;
            *material->materialShortcuts[i].element = shortcutPanel.sections[i + 1].elements[0];
        }

        // Apply the changes to the original mask texture if the shortcut is an mask texture shortcut 
        else if(material->materialShortcuts[i].maskTxtr != nullptr){
            // Show the texture selection dialog if clicked to the texture button
            if(shortcutPanel.sections[i + 1].elements[0].button.hover && *Mouse::LClick()){
                showTextureSelectionDialog(*material->materialShortcuts[i].maskTxtr, 400, false);
                updateTheMaterial = true;
            }

            // Apply the "use uv" to the texture
            material->materialShortcuts[i].maskTxtr->proceduralProps.proceduralUseTexCoords = shortcutPanel.sections[i + 1].elements[1].checkBox.clickState1;
        }

        // Update the material if interacted with the "use uv" checkbox
        if(shortcutPanel.sections[i + 1].elements[1].checkBox.hover && *Mouse::LClick())
            updateTheMaterial = true;
    }

    // "Use UV or model pos for all the textures" button pressed
    bool useUVBtnClicked = shortcutPanel.sections[0].elements[2].button.clicked; 
    bool usePosBtnClicked = shortcutPanel.sections[0].elements[3].button.clicked; 
    if(useUVBtnClicked || usePosBtnClicked){
        for (size_t i = 0; i < material->materialModifiers.size(); i++)
        {
            // Modifier's mask texture
            if(useUVBtnClicked)
                material->materialModifiers[i].maskTexture.proceduralProps.proceduralUseTexCoords = true;
            else if(usePosBtnClicked)
                material->materialModifiers[i].maskTexture.proceduralProps.proceduralUseTexCoords = false;
        
            // Modifier's section textures
            for (size_t secI = 0; secI < material->materialModifiers[i].sections.size(); secI++)
            {
                for (size_t elementI = 0; elementI < material->materialModifiers[i].sections[secI].elements.size(); elementI++)
                {
                    if(useUVBtnClicked)
                        material->materialModifiers[i].sections[secI].elements[elementI].button.texture.proceduralProps.proceduralUseTexCoords = true;
                    else if(usePosBtnClicked)
                        material->materialModifiers[i].sections[secI].elements[elementI].button.texture.proceduralProps.proceduralUseTexCoords = false;
                }
            }
        }
        
        updateTheMaterial = true;
    }
}