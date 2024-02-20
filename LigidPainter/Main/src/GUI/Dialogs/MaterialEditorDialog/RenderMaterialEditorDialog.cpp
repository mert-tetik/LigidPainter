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

// Defined in the RenderPanel.cpp
extern bool updateThePreRenderedPanels;

bool __materialEditorDialogESCPressed = false;
bool __materialEditorDialogESCFirstFramePressed = false;

bool __lastDisplayModeComboBoxPressed = false;

unsigned int tempTextureResolution = 1024;

void MaterialEditorDialog::render
                                (
                                    Timer &timer,
                                    TextureSelectionDialog &textureSelectionDialog,
                                    LogDialog& logDialog,
                                    glm::mat4 projection
                                )
{
    dialogControl.updateStart();

    if(dialogControl.firstFrameActivated)
        this->updateTheMaterial = true;

    // Update the shortcut elements of the material
    for (size_t i = 0; i < this->material->materialShortcuts.size(); i++)
    {
        this->material->materialShortcuts[i].updateElement(*this->material, this->material->materialShortcuts[i].modI);
    }

    bool mouseTrackingFlag = !(textureSelectionDialog.dialogControl.isActive() || ContextMenus::materialModifier.dialogControl.isActive() || ContextMenus::addMaterialModifier.dialogControl.isActive());
    
    // Changing / Updating the positions of the panels & scaling them
    this->positioningPanels();

    // ------- Rendering the panels -------
    this->bgPanel.render(timer, mouseTrackingFlag);

    // Render the layer panel and handle it's interactions
    this->renderLayerPanel(timer, mouseTrackingFlag);
    
    // Render the modifier's panel panel and handle it's interactions
    this->renderModifiersPanel(timer, mouseTrackingFlag);

    this->barButton.render(timer, mouseTrackingFlag);

    this->renderShortcutPanel(timer, mouseTrackingFlag);

    this->renderSkyboxTxtr(projection);
    
    this->materialDisplayer.texture = this->displayingFBO.colorBuffer;
    this->materialDisplayer.render(timer, false);
    
    this->renderZoomIndicator(timer, mouseTrackingFlag);

    this->renderNavPanel(timer, mouseTrackingFlag);

    //If texture selection dialog is not active reset the index values used to navigate textures
    if(!textureSelectionDialog.dialogControl.isActive()){
        textureModifierTextureSelectingButtonIndex = 1000;
        textureSelectionDialog.selectedTextureIndex = 0;
    }
    
    dialogControl.updateEnd(timer,0.15f);

    // Update the material if needed
    if(!this->updateTheMaterial && this->prevUpdateTheMaterial){
        // Updating the material textures
        int specificUpdateI = -1;
        if(this->selectedResultModeIndex == 1)
            specificUpdateI = this->selectedMaterialModifierIndex;
        material->updateMaterialDisplayingTexture((float)tempTextureResolution, true, this->displayerCamera, this->displayModeComboBox.selectedIndex, true, this->displayingFBO, *this->getDisplayModel(), specificUpdateI);
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
            // Update the material displaying texture one more time before closing the dialog
            material->updateMaterialDisplayingTexture((float)tempTextureResolution, false, Camera(), 0, false);

            updateThePreRenderedPanels = true;

            this->deactivate(textureSelectionDialog);
        }
    }

    __materialEditorDialogESCFirstFramePressed = false; 


    // Update the displaying texture
    material->updateMaterialDisplayingTexture((float)tempTextureResolution, false, this->displayerCamera, this->displayModeComboBox.selectedIndex, true, this->displayingFBO, *this->getDisplayModel(), -1);

    __lastDisplayModeComboBoxPressed = this->displayModeComboBox.pressed;

    glClear(GL_DEPTH_BUFFER_BIT);
}