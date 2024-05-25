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
#include "UTIL/Threads/Threads.hpp"

#include <string>
#include <iostream>
#include <vector>

// Defined in the RenderPanel.cpp
extern bool updateThePreRenderedPanels;

bool __materialEditorDialogESCPressed = false;
bool __materialEditorDialogESCFirstFramePressed = false;

void MaterialEditorDialog::show(Timer &timer, Material* material)
{
    this->updateTheMaterial = true;

    dialogControl.activate();

    while (!getContext()->window.shouldClose())
    {
        dialogControl.updateStart(true);

        this->displayerCamera.interaction(*Mouse::mouseScroll(), *Mouse::mouseOffset(), true);

        // Update the shortcut elements of the material
        for (size_t i = 0; i < material->materialShortcuts.size(); i++)
        {
            material->materialShortcuts[i].updateElement(*material, material->materialShortcuts[i].modI);
        }

        bool mouseTrackingFlag = true;
        
        // Changing / Updating the positions of the panels & scaling them
        this->positioningPanels();

        // ------- Rendering the panels -------
        this->bgPanel.render(timer, mouseTrackingFlag);

        // Render the layer panel and handle it's interactions
        this->renderLayerPanel(timer, mouseTrackingFlag, material);
        
        // Render the modifier's panel panel and handle it's interactions
        this->renderModifiersPanel(timer, mouseTrackingFlag, material);

        this->barButton.render(timer, mouseTrackingFlag);

        this->renderShortcutPanel(timer, mouseTrackingFlag, material);

        if(material_thread.active){
            ShaderSystem::buttonShader().use();
            ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 0.4f);
        }

        this->renderSkyboxTxtr();
        this->materialDisplayer.texture = this->displayingTexture;
        this->materialDisplayer.render(timer, false);

        if(material_thread.active){
            ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);

            glDisable(GL_DEPTH_TEST);
            appVideos.loading.render(timer, this->materialDisplayer.pos, glm::vec2(this->materialDisplayer.scale.y / (Settings::videoScale()->x / Settings::videoScale()->y), this->materialDisplayer.scale.y) / 6.f, 1.f, 1, true);
            glEnable(GL_DEPTH_TEST);
        }
        
        this->renderZoomIndicator(timer, mouseTrackingFlag);

        this->renderNavPanel(timer, mouseTrackingFlag);

        //If texture selection dialog is not active reset the index values used to navigate textures
        if(!dialog_textureSelection.dialogControl.isActive()){
            textureModifierTextureSelectingButtonIndex = 1000;
            dialog_textureSelection.selectedTextureIndex = 0;
        }
        

        // Update the material if needed
        if((!this->updateTheMaterial && this->prevUpdateTheMaterial) || displayTxtrResComboBox.selectionDone){
            // Updating the material textures
            int specificUpdateI = -1;
            if(this->selectedResultModeIndex == 1)
                specificUpdateI = this->selectedMaterialModifierIndex;

            material_thread.update_material_displaying_texture(material, this->getDisplayModel(), &this->getDisplayModel()->meshes[0], &getMaterialDisplayerModel()->meshes[0].material_channels, std::stoi(this->displayTxtrResComboBox.texts[this->displayTxtrResComboBox.selectedIndex]));
            
            material->updateMaterialDisplayingTexture(std::stoi(this->displayTxtrResComboBox.texts[this->displayTxtrResComboBox.selectedIndex]), false, this->displayerCamera, this->displayModeComboBox.selectedIndex, true, this->getDisplayModel()->meshes[0], getMaterialDisplayerModel()->meshes[0].material_channels, this->displayingTexture, specificUpdateI);
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
        if(__materialEditorDialogESCFirstFramePressed || ((!bgPanel.hover && !barButton.hover && !dialog_log.isHovered()) && *Mouse::LClick()) || (barButton.hover && *Mouse::LDoubleClick())){
            this->displayModeComboBox.selectedIndex = 0;

            // Update the material displaying texture one more time before closing the dialog
            material_thread.update_material_displaying_texture(material, getMaterialDisplayerModel(), &getMaterialDisplayerModel()->meshes[0], &getMaterialDisplayerModel()->meshes[0].material_channels, std::stoi(this->displayTxtrResComboBox.texts[this->displayTxtrResComboBox.selectedIndex]));

            updateThePreRenderedPanels = true;

            this->deactivate();
        }

        __materialEditorDialogESCFirstFramePressed = false; 

        // Update the displaying texture
        material->updateMaterialDisplayingTexture(std::stoi(this->displayTxtrResComboBox.texts[this->displayTxtrResComboBox.selectedIndex]), false, this->displayerCamera, this->displayModeComboBox.selectedIndex, true, this->getDisplayModel()->meshes[0], getMaterialDisplayerModel()->meshes[0].material_channels, this->displayingTexture, -1);

        glClear(GL_DEPTH_BUFFER_BIT);
        
        dialogControl.updateEnd(timer,0.15f);
        if(dialogControl.mixVal == 0.f){
            break;
        }
    }
}