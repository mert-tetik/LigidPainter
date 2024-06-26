/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Texture selection dialog is responsible of modifying the procedural variables inside of a Texture class & generating displaying textures

*/

// OpenGL
#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

// GL math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// LigidPainter 
#include "GUI/GUI.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/Settings/Settings.hpp"

// System
#include <string>
#include <iostream>
#include <vector>

// Texture selection dialog
/* Defining the constructor and the index macros by including this header file */
#include "GUI/Dialogs/TextureSelectionDialog/Init.hpp" 
/* Defining the utility functions by including this header file */
#include "GUI/Dialogs/TextureSelectionDialog/Util.hpp" 

void TextureSelectionDialog::show(Timer &timer, Texture& receivedTexture, int displayingTextureRes, bool twoDMode){
    
    // Assign the default values
    initTextureSelectionDialog(receivedTexture, twoDMode);

    this->dialogControl.activate();

    // Rendering loop
    while (!getContext()->window.shouldClose())
    {
        dialogControl.updateStart(true);

        // Updating the displaying texture
        this->selectTheTexture(this->displayingTexture, 512, this->activeSelectedTextureDisplayingMode, false);

        // Recreating the texture selection panel's elements every frame
        updateTextureSelectingPanelElements(this->textureSelectingPanel, this->selectedTextureMode, this->proceduralDisplayingTextures);

        // Rendering all the panels
        this->renderPanels(timer);
        
        // Updating the texture modes panel
        updateTextureModesPanel(twoDMode);
        
        if(this->textureSelectingPanel.sections.size()){

            //If pressed any of the texture select the texture
            for (size_t i = 0; i < this->textureSelectingPanel.sections[0].elements.size(); i++)
            {
                if(this->textureSelectingPanel.sections[0].elements[i].button.hover && *Mouse::LClick()){
                    selectedTextureIndex = i;
                    if(this->selectedTextureMode == 4){
                        if(selectedTextureIndex == 0 || selectedTextureIndex == 1 || selectedTextureIndex == 2){
                            smartPositionTexturePanelActive = true;
                            smartPositionTexturePanel.pos = this->textureSelectingPanel.sections[0].elements[i].button.pos;
                            smartPositionTexturePanel.pos.y += this->textureSelectingPanel.sections[0].elements[i].button.scale.y + smartPositionTexturePanel.scale.y;
                        }
                        if(selectedTextureIndex == 3 || selectedTextureIndex == 4){
                            smartStripesTexturePanelActive = true;
                            smartStripesTexturePanel.pos = this->textureSelectingPanel.sections[0].elements[i].button.pos;
                            smartStripesTexturePanel.pos.y += this->textureSelectingPanel.sections[0].elements[i].button.scale.y + smartStripesTexturePanel.scale.y;
                        }
                        if(selectedTextureIndex == 5){
                            edgeWearTexturePanelActive = true;
                            edgeWearTexturePanel.pos = this->textureSelectingPanel.sections[0].elements[i].button.pos;
                            edgeWearTexturePanel.pos.y += this->textureSelectingPanel.sections[0].elements[i].button.scale.y + edgeWearTexturePanel.scale.y;
                        }
                        if(selectedTextureIndex == 6){
                            smartDistanceTexturePanelActive = true;
                            smartDistanceTexturePanel.pos = this->textureSelectingPanel.sections[0].elements[i].button.pos;
                            smartDistanceTexturePanel.pos.y += this->textureSelectingPanel.sections[0].elements[i].button.scale.y + smartDistanceTexturePanel.scale.y;
                        }
                    }
                }
            }
        }
        else{
            LGDLOG::start << "WARNING! : Invalid texture selecting panel section size!" << LGDLOG::end;
        }

        // Pressed to the select button
        if  (
                (this->subPanel.sections[0].elements[subPanel_Select_INDEX].button.clicked && this->selectedTextureMode != 3 && this->selectedTextureMode != 4) || 
                (this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Select_INDEX].button.clicked && this->selectedTextureMode == 3) || 
                (this->subPanelSmartTextures.sections[0].elements[subPanelSmartTextures_Select_INDEX].button.clicked && this->selectedTextureMode == 4)
            )
        {
            // If a 3D texture is selected while the 2D selection mode is active
            if(twoDMode && this->selectedTextureMode == 4){
                LGDLOG::start << "ERROR : Can't select 3D smart textures in 2D texture selection mode" << LGDLOG::end;
            }

            // Assigning the procedural values to the received texture then generating the displaying texture
            else
                this->selectTheTexture(receivedTexture, displayingTextureRes, !twoDMode, false);

            // Close the dialog
            dialogControl.unActivate();
        }

        //End the dialog
        if((getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)) || (!this->bgPanel.hover && !dialog_log.isHovered() && *Mouse::LClick())){
            dialogControl.unActivate();
            selectedTextureIndex = 0;
        }

        // Update the dialog control
        dialogControl.updateEnd(timer,0.15f);
        if(dialogControl.mixVal == 0.f)
            break;
    }
}