
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 (c) 2024 Mert Tetik. All rights reserved.

 Official GitHub Link : https://github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------

    Display settings dialog.
    (Mostly manipulates the skybox )

 */
#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"

#include "3D/ThreeD.hpp"

#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/Threads/Threads.hpp"
#include "UTIL/GL/GL.hpp"

#include <string>
#include <iostream>
#include <vector>

// Defined in the RenderPanel.cpp
extern bool updateThePreRenderedPanels;

void TextureEditorDialog::show(Timer& timer, Skybox &skybox, Texture* receivedTexture){
    
    dialogControl.activate();
   
    while (!getContext()->window.shouldClose())
    {
        dialogControl.updateStart(true);

        this->sectionPanel.pos = this->bgPanel.pos;
        this->sectionPanel.pos.z += 0.001f;
        this->sectionPanel.pos.x -= this->bgPanel.scale.x - this->sectionPanel.scale.x;

        //Render the panel
        this->bgPanel.render(timer, !dialog_log.isHovered());
        this->sectionPanel.render(timer, !dialog_log.isHovered());
        
        if(dialogControl.firstFrameActivated){
            resizeElements[5].textBox.text = std::to_string(receivedTexture->getResolution().x); 
            resizeElements[5].textBox.activeChar = resizeElements[5].textBox.text.size()-1; 
            resizeElements[5].textBox.activeChar2 = resizeElements[5].textBox.text.size()-1;
            
            resizeElements[6].textBox.text = std::to_string(receivedTexture->getResolution().y);
            resizeElements[6].textBox.activeChar = resizeElements[6].textBox.text.size()-1; 
            resizeElements[6].textBox.activeChar2 = resizeElements[6].textBox.text.size()-1;
            this->generate_result(receivedTexture, this->displayingTexture);
        }

        for (size_t i = 0; i < this->sectionPanel.sections[0].elements.size(); i++)
        {
            if(this->sectionPanel.sections[0].elements[i].button.clickState1 && this->selectedSection != i){
                this->selectedSection = i;
                this->generate_result(receivedTexture, this->displayingTexture);
                break;
            }
        }
        
        for (size_t i = 0; i < this->sectionPanel.sections[0].elements.size(); i++){
            if(this->selectedSection != i){
                this->sectionPanel.sections[0].elements[i].button.clickState1 = false;
            }
        }
        
        for (size_t i = 0; i < this->sectionPanel.sections[0].elements.size(); i++){
            if(this->selectedSection == i){
                this->sectionPanel.sections[0].elements[i].button.clickState1 = true;
            }
        }

        this->displayerBtn.scale.y = this->displayerBtn.scale.x;
        this->displayerBtn.scale.y *= Settings::videoScale()->x / Settings::videoScale()->y; 

        this->displayerBtn.pos = this->bgPanel.pos;
        this->displayerBtn.pos.x = this->sectionPanel.pos.x + this->sectionPanel.scale.x + this->displayerBtn.scale.x * 1.1f;
        this->displayerBtn.pos.y -= this->bgPanel.scale.y - this->displayerBtn.scale.y * 1.1f;
        this->displayerBtn.pos.z += 0.001f;
        
        this->displayerBtn.render(timer, false);

        bool anyInteraction = false;

        if(this->selectedSection == 0){
            int eCnt = 0;
            
            //Top Left
            if(resizeElements[2].comboBox.selectedIndex == 0){
                resizeElements[3].rangeBar.value = 0.f;
                resizeElements[4].rangeBar.value = 0.f;
            }

            //Top Right
            if(resizeElements[2].comboBox.selectedIndex == 1){
                resizeElements[3].rangeBar.value = 1.f;
                resizeElements[4].rangeBar.value = 0.f;
            }

            //Bottom Left
            if(resizeElements[2].comboBox.selectedIndex == 2){
                resizeElements[3].rangeBar.value = 0.f;
                resizeElements[4].rangeBar.value = 1.f;
            }

            //Bottom Right
            if(resizeElements[2].comboBox.selectedIndex == 3){
                resizeElements[3].rangeBar.value = 1.f;
                resizeElements[4].rangeBar.value = 1.f;
            }

            //Center
            if(resizeElements[2].comboBox.selectedIndex == 4){
                resizeElements[3].rangeBar.value = 0.5f;
                resizeElements[4].rangeBar.value = 0.5f;
            }

            for (size_t i = 0; i < resizeElements.size(); i++)
            {
                if(i == 1 && resizeElements[0].comboBox.selectedIndex != 2)
                    i++;

                if(resizeElements[i].isInteracted())
                    anyInteraction = true;
                
                resizeElements[i].pos = displayerBtn.pos;
                resizeElements[i].pos.x += displayerBtn.scale.x * 2.f;
                resizeElements[i].pos.y -= displayerBtn.scale.y * .5f;
                resizeElements[i].pos.y += 8.f * eCnt;
                resizeElements[i].render(timer, !dialog_log.isHovered());

                
                eCnt++;
            }
        }
        
        if(this->selectedSection == 1){
            int eCnt = 0;
            
            for (size_t i = 0; i < bluringElement.size(); i++)
            {
                blurinElementsLoopReturn:

                if(i == 1 && bluringElement[0].comboBox.selectedIndex != 2)
                    i++;
                if((i == 2 || i == 3) && bluringElement[0].comboBox.selectedIndex != 3){
                    i++;
                    goto blurinElementsLoopReturn;
                }

                bluringElement[i].pos = displayerBtn.pos;
                bluringElement[i].pos.x += displayerBtn.scale.x * 2.f;
                bluringElement[i].pos.y -= displayerBtn.scale.y * .5f;
                bluringElement[i].pos.y += 8.f * eCnt;
                bluringElement[i].render(timer, !dialog_log.isHovered());
                eCnt++;

                if(bluringElement[i].isInteracted())
                    anyInteraction = true;
            }

        }

        if(this->selectedSection == 2){
            int eCnt = 0;
            
            for (size_t i = 0; i < coloringElements.size(); i++)
            {
                coloringElementsLoopReturn:

                if((i == 1 || i == 2 || i == 3) && coloringElements[0].comboBox.selectedIndex != 0){
                    i++;
                    goto coloringElementsLoopReturn;
                }
                if((i == 4 || i == 5) && coloringElements[0].comboBox.selectedIndex != 1){
                    i++;
                    goto coloringElementsLoopReturn;
                }
                
                if(i == 6 && coloringElements[0].comboBox.selectedIndex != 2)
                    break;

                coloringElements[i].pos = displayerBtn.pos;
                coloringElements[i].pos.x += displayerBtn.scale.x * 2.f;
                coloringElements[i].pos.y -= displayerBtn.scale.y * .5f;
                coloringElements[i].pos.y += 8.f * eCnt;
                coloringElements[i].render(timer, !dialog_log.isHovered());
                eCnt++;
                
                if(coloringElements[i].isInteracted())
                    anyInteraction = true;
            }
        }

        if(this->selectedSection == 3){
            int eCnt = 0;
            
            for (size_t i = 0; i < normalMapElements.size(); i++)
            {
                normalMapElements[i].pos = displayerBtn.pos;
                normalMapElements[i].pos.x += displayerBtn.scale.x * 2.f;
                normalMapElements[i].pos.y -= displayerBtn.scale.y * .5f;
                normalMapElements[i].pos.y += 8.f * eCnt;
                normalMapElements[i].render(timer, !dialog_log.isHovered());
                eCnt++;
                if(normalMapElements[i].isInteracted())
                    anyInteraction = true;
            }
        }

        if(this->selectedSection == 4){
            int eCnt = 0;
            
            for (size_t i = 0; i < distortionElements.size(); i++)
            {
                distortionElementsLoopReturn:

                if((i == 1 || i == 2) && distortionElements[0].comboBox.selectedIndex != 0){
                    i++;
                    goto distortionElementsLoopReturn;
                }

                if((i == 3 || i == 4) && distortionElements[0].comboBox.selectedIndex != 1){
                    i++;
                    goto distortionElementsLoopReturn;
                }
                
                if((i == 5 || i == 6) && distortionElements[0].comboBox.selectedIndex != 2){
                    i++;
                    goto distortionElementsLoopReturn;
                }
                
                if(i == 7 && distortionElements[0].comboBox.selectedIndex != 3){
                    i++;
                }
                
                if(i == 8 && distortionElements[0].comboBox.selectedIndex != 4){
                    i++;
                }

                distortionElements[i].pos = displayerBtn.pos;
                distortionElements[i].pos.x += displayerBtn.scale.x * 2.f;
                distortionElements[i].pos.y -= displayerBtn.scale.y * .5f;
                distortionElements[i].pos.y += 8.f * eCnt;
                distortionElements[i].render(timer, !dialog_log.isHovered());
                eCnt++;
                
                if(distortionElements[i].isInteracted())
                    anyInteraction = true;
            }

        }
        
        if(this->selectedSection == 5){

            filterBtn.pos = displayerBtn.pos;
            filterBtn.pos.x += displayerBtn.scale.x * 2.f;
            filterBtn.pos.y -= displayerBtn.scale.y * .5f;
            filterBtn.pos.y += 8.f;
            filterBtn.render(timer, !dialog_log.isHovered());

            if(filterBtn.clicked){
                anyInteraction = true;
            }        
        }
        
        if(this->selectedSection == 6){

            textureBtn.pos = displayerBtn.pos;
            textureBtn.pos.x += displayerBtn.scale.x * 2.f;
            textureBtn.pos.y -= displayerBtn.scale.y * .5f;
            textureBtn.pos.y += 8.f;
            textureBtn.render(timer, !dialog_log.isHovered());

            if(textureBtn.clicked){
                anyInteraction = true;
            }        
        }

        this->maskTextureButton.pos = displayerBtn.pos;
        this->maskTextureButton.pos.y += displayerBtn.scale.y * 1.15;
        this->maskTextureButton.render(timer, !dialog_log.isHovered());
        
        this->saveButton.pos = displayerBtn.pos;
        this->saveButton.pos.y += displayerBtn.scale.y * 1.2 + maskTextureButton.scale.y * 3.f;
        this->saveButton.render(timer, !dialog_log.isHovered());
        
        this->saveAsButton.pos = saveButton.pos;
        this->saveAsButton.pos.y += saveButton.scale.y * 3.f;
        this->saveAsButton.render(timer, !dialog_log.isHovered());

        if(this->maskTextureButton.clicked)
        anyInteraction = true; 

        if(anyInteraction)
            this->generate_result(receivedTexture, this->displayingTexture);

        if(this->saveButton.clicked){
            dialog_log.registerTextureModificationAction("Texture manipulated via texture editor dialog", *receivedTexture);

            Texture txtr = receivedTexture->duplicateTexture("TextureEditorDialog::show");
            this->generate_result(&txtr, *receivedTexture);
            
            for (size_t i = 0; i < Library::getTextureArraySize(); i++)
            {
                if(receivedTexture->ID == Library::getTexture(i)->ID){
                    projectUpdatingThreadElements.updateTextures = true;
                }
            }

            glDeleteTextures(1, &txtr.ID);
        
            if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL))
                this->dialogControl.unActivate();
        
            updateThePreRenderedPanels = true;
        }

        if(this->saveAsButton.clicked){
            Texture newTxtr;
            newTxtr = Texture((char*)nullptr, receivedTexture->getResolution().x, receivedTexture->getResolution().y);
            this->generate_result(receivedTexture, newTxtr.ID);
            Library::addTexture(newTxtr, "New texture via image editor");
            
            if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL))
                this->dialogControl.unActivate();

            updateThePreRenderedPanels = true;
        }

        //End the dialog
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!bgPanel.hover && !dialog_log.isHovered() && *Mouse::LClick())){
            if(!dialogControl.firstFrameActivated){
                dialogControl.unActivate();
            }
        }

        dialogControl.updateEnd(timer,0.15f);
        if(dialogControl.mixVal == 0.f)
            break;
    }
}