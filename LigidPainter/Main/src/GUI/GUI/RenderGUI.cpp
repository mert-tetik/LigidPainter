/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Render all the GUI elements (Panels, Dialogs & stuff)

*/


#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "ShaderSystem/Shader.hpp"
#include "LibrarySystem/Library.hpp"
#include "NodeSystem/Node/Node.hpp"
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

// Defined in the RenderPanel.cpp
extern bool updateThePreRenderedPanels;

TextureSelectionDialog __texture_selection_dialog;
FilterSelectionDialog __filter_selection_dialog;
MeshSelectionDialog __mesh_selection_dialog;
TexturePackEditorDialog __texture_Pack_Editor_Dialog;
ProjectRecoverDialog __project_recover_dialog;
glm::mat4 __projection;
Timer __timer; 
bool __wasTextureSelectionDialogActive = false;

void showTextureSelectionDialog(Texture& txtr, int displayingTextureRes, bool twoDMode){
    __texture_selection_dialog.show(__timer, __projection, txtr, displayingTextureRes, twoDMode);
    __wasTextureSelectionDialogActive = true;
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
}

void showFilterSelectionDialog(Filter& filter, int displayingTextureRes){
    __filter_selection_dialog.show(__timer, __projection, filter, displayingTextureRes);
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
}

void showMeshSelectionDialog(int& selectedMeshI){
    __mesh_selection_dialog.show(__timer, __projection, selectedMeshI);
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
}

void showTexturePackEditorDialog(TexturePack& texturePack){
    __texture_Pack_Editor_Dialog.show(__timer, __projection, texturePack);
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
}

void showProjectRecoverDialog(Project &project, AppMaterialModifiers appMaterialModifiers){
    __project_recover_dialog.render(__timer, project, appMaterialModifiers);
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
}

bool wasTextureSelectionDialogActive(){
    return __wasTextureSelectionDialogActive;
}

/* -- Forward declerations -- */

static void renderBrushCursor(float radius, glm::mat4 guiProjection);

void UI::render(Timer &timer,Project &project, Painter &painter, Skybox &skybox){
    
    Debugger::block("GUI : Start"); // Start
    
    __texture_selection_dialog = this->textureSelectionDialog;
    __filter_selection_dialog = this->filterSelectionDialog;
    __texture_Pack_Editor_Dialog = this->texturePackEditorDialog;
    __project_recover_dialog = this->projectRecoverDialog;
    __projection = this->projection;
    __timer = timer; 

    //Set pass less or equal
    glDepthFunc(GL_LEQUAL);

    //Give projection to the curve shader        
    ShaderSystem::vectoralCurve().use();
    ShaderSystem::vectoralCurve().setMat4("projection",this->projection); 
    
    //Give projection to the curve shader        
    ShaderSystem::nodeConnectionCurve().use();
    ShaderSystem::nodeConnectionCurve().setMat4("projection",this->projection); 
    

    //Give projection to the color picker shader !IS NOT USED RN 
    ShaderSystem::colorPicker().use();
    ShaderSystem::colorPicker().setMat4("projection",this->projection); 
    
    //Use the related shader 
    ShaderSystem::buttonShader().use();

    ShaderSystem::buttonShader().setVec2("properties.txtrScale", glm::vec2(1.f));
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);

    //Set the ortho projection     
    ShaderSystem::buttonShader().setMat4("projection",this->projection); 

    //Calculate the total screen gap
    float screenGap = Settings::videoScale()->x - getContext()->windowScale.x; //Use that value to keep panels on the right side
    
    //Calculate the screen gap in 0 - 100 range
    float screenGapPerc = screenGap / Settings::videoScale()->x * 100.f; 

    Debugger::block("GUI : Start"); // End
    
    Debugger::block("GUI : Complete Panels"); // Start

    //Render the panels
    renderPanels(timer, painter, screenGapPerc);

    updateThePreRenderedPanels = false;

    Debugger::block("GUI : Complete Panels"); // End

    //Render renaming textbox
    renderRenamingTextbox(timer, painter);
    
    Debugger::block("GUI : Dialogs"); // Start
    
    //Render the dialogs
    renderDialogs(timer, project, skybox, painter);
    
    Debugger::block("GUI : Dialogs"); // End

    Debugger::block("GUI : Element Interaction"); // Start
    //Interactions of the UI elements
    elementInteraction(painter, timer, screenGapPerc, project, this->materialEditorDialog.appMaterialModifiers);

    Debugger::block("GUI : Element Interaction"); // End

    
    Debugger::block("GUI : Dropper"); // Start
    
    //Render the dropper & pick color if mouse left button clicked
    renderDropper(painter);

    if(dropper.active){
        Mouse::setCursor(*Mouse::dropperCursor());
    }
    
    Debugger::block("GUI : Dropper"); // End

    //Render the brush cursor
    if(
            !this->anyPanelHover && 
            !this->anyDialogActive && 
            !this->anyContextMenuActive && 
            (painter.selectedDisplayingModeIndex == 1 || painter.selectedDisplayingModeIndex == 2) && painter.selectedPaintingModeIndex != 5 && painter.selectedPaintingModeIndex != 6 &&
            !painter.paintingoverTextureEditorMode &&
            !painter.faceSelection.editMode &&
            !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) &&
            !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_ALT) && 
            !dropper.active
        )
    {
        renderBrushCursor(painter.brushProperties.radius, this->projection);
    }
    else if(painter.faceSelection.editMode && painter.faceSelection.selectionModeIndex == 0 && !anyDialogActive && !anyPanelHover)
        renderBrushCursor(painter.faceSelection.radius / Settings::videoScale()->x, this->projection);
    else
        getContext()->window.setCursorVisibility(true);


    frameCounter++;

    if(frameCounter > 1000)
        frameCounter = 0;

    __wasTextureSelectionDialogActive = false;

}



//UTILITY FUNCTIONS

static void renderBrushCursor(float radius, glm::mat4 guiProjection){
    /* Use the circle shader */
    ShaderSystem::circleShader().use();

    /* Set the transform data & the projection */
    ShaderSystem::circleShader().setMat4("projection", guiProjection);
    ShaderSystem::circleShader().setVec3("pos", Mouse::cursorPos()->x, Mouse::cursorPos()->y, 1);
    ShaderSystem::circleShader().setVec2("scale", glm::vec2(radius * (Settings::videoScale()->x)));

    /* Hide the cursor */
    getContext()->window.setCursorVisibility(false);

    /* Render the circle s*/
    glDrawArrays(GL_TRIANGLES, 0, 6);

    /* Clear the depth buffer of the current framebuffers*/
    glClear(GL_DEPTH_BUFFER_BIT);

    /* Use the button shader back */
    ShaderSystem::buttonShader().use();
}

std::string __faceSelectionActiveMesh = "";
int __faceSelectionActiveObjIndex = 0;

void UI::renderPanels(Timer &timer, Painter &painter,  float screenGapPerc){
    
    Debugger::block("GUI : Panels : Panel rendering"); // Start
    
    navigationPanel.render(timer,!anyDialogActive);
    if(navigationPanel.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }
    windowPanel.render(timer,!anyDialogActive);
    if(windowPanel.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }
    
    paintingPanelModePanel.render(timer,!anyDialogActive);
    if(paintingPanelModePanel.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }

    Section* paintingPanelActiveSection = nullptr;

    if(selectedPaintingPanelMode == 0 && painter.selectedDisplayingModeIndex != 0)
        paintingPanelActiveSection = &this->colorSection;
    if(selectedPaintingPanelMode == 2)
        paintingPanelActiveSection = &this->brushSection;
    if(selectedPaintingPanelMode == 3 && painter.selectedDisplayingModeIndex != 0)
        paintingPanelActiveSection = &this->meshSection;
    if(selectedPaintingPanelMode == 4 && painter.selectedDisplayingModeIndex != 0)
        paintingPanelActiveSection = &this->mirrorSection;
    if(selectedPaintingPanelMode == 5 && painter.selectedDisplayingModeIndex != 0)
        paintingPanelActiveSection = &this->paintingOverSection;
    
    if(selectedPaintingPanelMode == 1)
        paintingPanel.sections = this->paintingChannelsSection;
    else{
        if(paintingPanel.sections.size() > 1)
            paintingPanel.sections.erase(paintingPanel.sections.begin()+1, paintingPanel.sections.end());
        
        if(paintingPanelActiveSection != nullptr)
            paintingPanel.sections[0] = *paintingPanelActiveSection;
        else
            paintingPanel.sections[0] = this->cantBeDisplayedSection;
    }
    
    paintingPanel.render(timer,!anyDialogActive);
    if(paintingPanel.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }
    if(selectedPaintingPanelMode == 1)
        this->paintingChannelsSection = paintingPanel.sections; 
    else if(paintingPanelActiveSection != nullptr)
        *paintingPanelActiveSection = paintingPanel.sections[0];
    
    paintingPanelModeDisplayer.text = paintingPanelModePanel.sections[0].elements[selectedPaintingPanelMode].button.text; 
    paintingPanelModeDisplayer.texture = paintingPanelModePanel.sections[0].elements[selectedPaintingPanelMode].button.texture; 

    paintingPanelModeDisplayer.render(timer, false);
    
    libraryPanelLeft.render(timer,!anyDialogActive);
    if(libraryPanelLeft.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }
    libraryPanelDisplayer.render(timer,!anyDialogActive);
    if(libraryPanelDisplayer.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }
    //nodeEditorDisplayer.render(timer,!anyDialogActive);
    if(nodeEditorDisplayer.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }
    selectedTextureDisplayer.render(timer, true);
    if(selectedTextureDisplayer.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }
    if(this->colorUseCustomMatSelectionPanelActive){
        colorUseCustomMatSelectionPanel.sections[0].elements.clear();

        for (size_t i = 0; i < Library::getMaterialArraySize(); i++)
        {
            Button btn = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), Library::getMaterialObj(i).title, Library::getMaterialObj(i).displayingTexture, 0.f, false);
            btn.textureSizeScale = 1.2f;
            colorUseCustomMatSelectionPanel.sections[0].elements.push_back(btn);
        }  

        this->colorUseCustomMatSelectionPanel.render(timer, true);

        for (size_t i = 0; i < colorUseCustomMatSelectionPanel.sections[0].elements.size(); i++)
        {
            if(colorUseCustomMatSelectionPanel.sections[0].elements[i].button.hover && *Mouse::LClick()){
                painter.customMaterialID = Library::getMaterialObj(i).uniqueID;
            }
        }

        if(!paintingChannelsTextureSelectionPanel.hover && *Mouse::LClick() || getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)){
            this->colorUseCustomMatSelectionPanelActive = false;
        }
    }
    
    if(paintingChannelsTextureSelectionPanelActive){
        paintingChannelsTextureSelectionPanel.sections[0].elements.clear();

        for (size_t i = 0; i < Library::getTextureArraySize(); i++)
        {
            Button btn = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), Library::getTextureObj(i).title, Library::getTextureObj(i), 0.f, false);
            btn.textureSizeScale = 1.2f;
            paintingChannelsTextureSelectionPanel.sections[0].elements.push_back(btn);
        }  

        paintingChannelsTextureSelectionPanel.render(timer, true);

        for (size_t i = 0; i < paintingChannelsTextureSelectionPanel.sections[0].elements.size(); i++)
        {
            if(paintingChannelsTextureSelectionPanel.sections[0].elements[i].button.hover && *Mouse::LClick()){
                for (size_t secI = 1; secI < paintingChannelsSection.size(); secI++)
                {
                    for (size_t elI = 0; elI < paintingChannelsSection[secI].elements.size(); elI++){
                        
                        
                        if(paintingChannelsSection[secI].elements[elI].button.clickState1){
                            for (size_t elICheck = 0; elICheck < paintingChannelsSection[secI].elements.size(); elICheck++){
                                if(paintingChannelsSection[secI].elements[elICheck].button.texture.ID == Library::getTexture(i)->ID && elI != elICheck){
                                    paintingChannelsSection[secI].elements[elICheck].button.texture = Texture();
                                    LGDLOG::start << "WARNING! Same textures in a material. " << paintingChannelsSection[secI].elements[elICheck].button.text << " is replaced." << LGDLOG::end;
                                }
                            }

                            paintingChannelsSection[secI].elements[elI].button.texture = *Library::getTexture(i);
                            paintingChannelsSection[secI].elements[elI].button.clickState1 = false;
                        }
                    }
                }
                
                paintingChannelsTextureSelectionPanelActive = false;
            }
        }
        
        if(!paintingChannelsTextureSelectionPanel.hover && *Mouse::LClick() || getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)){
            for (size_t secI = 1; secI < paintingChannelsSection.size(); secI++)
            {
                for (size_t elI = 0; elI < paintingChannelsSection[secI].elements.size(); elI++){
                    paintingChannelsSection[secI].elements[elI].button.clickState1 = false;
                }
            }
           
            paintingChannelsTextureSelectionPanelActive = false;
        }

    }
    
    if(nodeEditorDisplayer.hover){
        if(*Mouse::MPressed()){
            this->nodePanel.position += *Mouse::mouseOffset()/4.f;
            
            /*Restrict movement*/
            if(this->nodePanel.position.x > 40){
                this->nodePanel.boundaryState = 0;
                this->nodePanel.hitBoundaries = true;
                this->nodePanel.position.x = 40;
            }
            if(this->nodePanel.position.x < -40){
                this->nodePanel.boundaryState = 1;
                this->nodePanel.hitBoundaries = true;
                this->nodePanel.position.x = -40;
            }
            
            if(this->nodePanel.position.y > 40){
                this->nodePanel.boundaryState = 2;
                this->nodePanel.hitBoundaries = true;
                this->nodePanel.position.y = 40;
            }
            if(this->nodePanel.position.y < -40){
                this->nodePanel.boundaryState = 3;
                this->nodePanel.hitBoundaries = true;
                this->nodePanel.position.y = -40;
            }

        }
        else
            this->nodePanel.hitBoundaries = false;

        /* Calculate boundary animation */        
        if(this->nodePanel.hitBoundaries){

            glm::vec2 transPower = 1.f - abs(nodePanel.mixVal);
            
            if(transPower.x < 0)
                transPower.x = 0;
            if(transPower.y < 0)
                transPower.y = 0;


            this->nodePanel.mixVal += (*Mouse::mouseOffset() / 100.f * transPower);
        
        }
        else{
            timer.transition(false, this->nodePanel.mixVal.x, 0.5f);
            timer.transition(false, this->nodePanel.mixVal.y, 0.5f);
            
            this->nodePanel.mixVal.x = sin(this->nodePanel.mixVal.x);
            this->nodePanel.mixVal.y = sin(this->nodePanel.mixVal.y);
        }

    }

    //Prep rendering dots
    ShaderSystem::dotsShader().use();

    ShaderSystem::dotsShader().setMat4("projection", this->projection);
    ShaderSystem::dotsShader().setVec3("pos", nodeEditorDisplayer.resultPos);
    ShaderSystem::dotsShader().setVec2("scale", nodeEditorDisplayer.resultScale);
    
    glm::vec2 dotPos = this->nodePanel.position * glm::vec2(2.f) + this->nodePanel.mixVal*10.f;
    ShaderSystem::dotsShader().setVec2("dotPos", dotPos);
    ShaderSystem::dotsShader().setFloat("scroll", this->nodePanel.scroll);
    
    /* Render the dots */
    glDrawArrays(GL_TRIANGLES, 0, 6);

    /* Clear the depth buffer of the current framebuffers*/
    glClear(GL_DEPTH_BUFFER_BIT);

    ShaderSystem::buttonShader().use();
    //


    if(!painter.threeDimensionalMode && painter.selectedDisplayingModeIndex == 2){
        glm::vec2 destScale = glm::vec2(glm::vec2(painter.selectedTexture.getResolution()));
        glm::vec2 prevScale = destScale * this->twoDPaintingSceneScroll;
        float scrVal = *Mouse::mouseScroll() / Settings::videoScale()->y * 4.f;
        if(!this->twoDPaintingPanel.hover)
            scrVal = 0.f;

        //Calculate the scroll value
        if(this->twoDPaintingSceneScroll > 1.f)
            this->twoDPaintingSceneScroll += scrVal;
        else{
            if(scrVal < 0)
                this->twoDPaintingSceneScroll -= twoDPaintingSceneScroll / 4.f;
            if(scrVal > 0)
                this->twoDPaintingSceneScroll += twoDPaintingSceneScroll / 4.f;
        } 
        if(this->twoDPaintingSceneScroll < 0.02)
            this->twoDPaintingSceneScroll = 0.02;

        bool posChanged = false;
        //Calculate the position value        
        if(*Mouse::MPressed() && this->twoDPaintingPanel.hover){
            this->twoDPaintingScenePos += *Mouse::mouseOffset();
            posChanged = true;
        }

        //(Zoom into the cursor)
        glm::vec2 scaleGap = prevScale - destScale * this->twoDPaintingSceneScroll;
        this->twoDPaintingScenePos += (scaleGap) * (((*Mouse::cursorPos() - glm::vec2(twoDPaintingPanel.sections[0].elements[0].button.resultPos.x + this->twoDPaintingScenePos.x, twoDPaintingPanel.sections[0].elements[0].button.resultPos.y + this->twoDPaintingScenePos.y)) + 0.00001f) / (prevScale + 0.00001f));

        if(this->twoDPaintingScenePos.x > destScale.x * this->twoDPaintingSceneScroll)
            this->twoDPaintingScenePos.x = destScale.x * this->twoDPaintingSceneScroll;
        if(this->twoDPaintingScenePos.x < -destScale.x * this->twoDPaintingSceneScroll)
            this->twoDPaintingScenePos.x = -destScale.x * this->twoDPaintingSceneScroll;
        
        if(this->twoDPaintingScenePos.y > destScale.y * this->twoDPaintingSceneScroll)
            this->twoDPaintingScenePos.y = destScale.y * this->twoDPaintingSceneScroll;
        if(this->twoDPaintingScenePos.y < -destScale.y * this->twoDPaintingSceneScroll)
            this->twoDPaintingScenePos.y = -destScale.y * this->twoDPaintingSceneScroll;


        //Render the 2D painting panel
        twoDPaintingPanel.sections[0].elements[0].button.text = "";
        twoDPaintingPanel.render(timer,false);
        if(twoDPaintingPanel.resizingDone){
            for (size_t i = 0; i < 5; i++)
                this->panelPositioning(screenGapPerc, painter);
        }

        //Bottom Barrier
        ShaderSystem::buttonShader().setVec3("pos", glm::vec3(twoDPaintingPanel.resultPos.x,   twoDPaintingPanel.resultPos.y + twoDPaintingPanel.resultScale.y + 5000,   1.f)); 
        ShaderSystem::buttonShader().setVec2("scale", glm::vec2(5000));
        ShaderSystem::buttonShader().setFloat("properties.radius", 0.f); 
        ShaderSystem::buttonShader().setInt("outlineExtra" , false); 
        ShaderSystem::buttonShader().setVec4("properties.color", glm::vec4(0)); //Invisible
        ShaderSystem::buttonShader().setVec3("properties.outline.color", glm::vec4(0)); //Invisible
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //Top Barrier
        ShaderSystem::buttonShader().setVec3("pos", glm::vec3(twoDPaintingPanel.resultPos.x,   twoDPaintingPanel.resultPos.y - twoDPaintingPanel.resultScale.y - 5000,   1.f));
        ShaderSystem::buttonShader().setVec2("scale", glm::vec2(5000));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //Left Barrier
        ShaderSystem::buttonShader().setVec3("pos", glm::vec3(twoDPaintingPanel.resultPos.x  - twoDPaintingPanel.resultScale.x  - 5000,   twoDPaintingPanel.resultPos.y,   1.f));
        ShaderSystem::buttonShader().setVec2("scale", glm::vec2(5000));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //Right Barrier
        ShaderSystem::buttonShader().setVec3("pos", glm::vec3(twoDPaintingPanel.resultPos.x  + twoDPaintingPanel.resultScale.x  + 5000,   twoDPaintingPanel.resultPos.y,   1.f));
        ShaderSystem::buttonShader().setVec2("scale", glm::vec2(5000));
        glDrawArrays(GL_TRIANGLES, 0, 6);


        ShaderSystem::twoDPaintingModeAreaShader().use();

        //*Fragment
        ShaderSystem::twoDPaintingModeAreaShader().setInt("txtr", 5);
        ShaderSystem::twoDPaintingModeAreaShader().setInt("paintingTexture", 6);
        ShaderSystem::twoDPaintingModeAreaShader().setInt("brushModeState", painter.selectedPaintingModeIndex);
        ShaderSystem::twoDPaintingModeAreaShader().setInt("usePaintingOver", painter.usePaintingOver);
        ShaderSystem::twoDPaintingModeAreaShader().setFloat("smearTransformStrength", painter.smearTransformStrength);
        ShaderSystem::twoDPaintingModeAreaShader().setFloat("smearBlurStrength", painter.smearBlurStrength);
        ShaderSystem::twoDPaintingModeAreaShader().setInt("multiChannelsPaintingMod", false);
        ShaderSystem::twoDPaintingModeAreaShader().setInt("channelI", 0);
        ShaderSystem::twoDPaintingModeAreaShader().setFloat("channelStrength", 1.f);

        //* Bind the textures
        //painted texture
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, painter.selectedTexture.ID);

        // Render the texture as it's pixels can be seen
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        //paintingTexture 
        glActiveTexture(GL_TEXTURE6);
        if(painter.selectedDisplayingModeIndex == 0)
            glBindTexture(GL_TEXTURE_2D, 0);
        else
            glBindTexture(GL_TEXTURE_2D, painter.projectedPaintingTexture.ID);

        //*Vertex
        ShaderSystem::twoDPaintingModeAreaShader().setMat4("projection", this->projection);
        ShaderSystem::twoDPaintingModeAreaShader().setMat4("view", glm::mat4(1.));
        ShaderSystem::twoDPaintingModeAreaShader().setMat4("modelMatrix", glm::mat4(1.));

        // If scene transformed update the 2D square buffers
        if(posChanged || *Mouse::mouseScroll() || *Mouse::LClick()){
            glm::vec3 boxPos = glm::vec3(
                                            twoDPaintingPanel.sections[0].elements[0].button.resultPos.x + this->twoDPaintingScenePos.x,
                                            twoDPaintingPanel.sections[0].elements[0].button.resultPos.y + this->twoDPaintingScenePos.y,
                                            twoDPaintingPanel.sections[0].elements[0].button.resultPos.z
                                        );
            
            glm::vec2 boxScale = glm::vec2(destScale * this->twoDPaintingSceneScroll);

            this->twoDPaintingBox.customMeshUpdate(boxPos, boxScale);
        }
        
        twoDPaintingBox.bindBuffers();
        glDrawArrays(GL_TRIANGLES, 0 ,6);
        getBox()->bindBuffers();

        ShaderSystem::buttonShader().use();
    }
    else if(!painter.threeDimensionalMode){
        //Render the 2D painting panel
        twoDPaintingPanel.sections[0].elements[0].button.text = "2D Painting can't be displayed in the current displaying mode";
        twoDPaintingPanel.render(timer, false);
        if(twoDPaintingPanel.resizingDone){
            for (size_t i = 0; i < 5; i++)
                this->panelPositioning(screenGapPerc, painter);
        }
    }

    glClear(GL_DEPTH_BUFFER_BIT);

    if(painter.selectedDisplayingModeIndex == 1 || painter.selectedDisplayingModeIndex == 2){
        paintingModesPanel.render(timer,!anyDialogActive);
        if(paintingModesPanel.resizingDone){
            for (size_t i = 0; i < 5; i++)
                this->panelPositioning(screenGapPerc, painter);
        }

        if(painter.selectedPaintingModeIndex == 2)
            smearPaintingModePropertyPanel.render(timer, !anyDialogActive); 
        
        if(painter.selectedPaintingModeIndex == 4)
            this->filterPaintingModeFilterBtn.render(timer, !anyDialogActive);

        if(painter.selectedPaintingModeIndex == 5)
            vectorPaintingModePropertyPanel.render(timer, !anyDialogActive); 
    }
    else{
        this->textureSelectedObjectsButton.render(timer, !anyDialogActive);

        if(this->textureSelectedObjectsButton.clicked){
            bool anyObjSelected = false;
            for (size_t i = 0; i < getModel()->meshes.size(); i++)
            {
                if(getModel()->meshes[i].selectedObjectIndices.size())
                    anyObjSelected = true;
            }
            
            if(!anyObjSelected)
                LGDLOG::start << "WARNING! No object was selected" << LGDLOG::end;

            this->objectTexturingDialog.dialogControl.activate();
        }

        for (size_t secI = 0; secI < objectsPanel.sections.size(); secI++)
        {
            for (size_t elI = 0; elI < objectsPanel.sections[secI].elements.size(); elI++){
                bool match = false;
                if(secI < getModel()->meshes.size()){
                    for (size_t i = 0; i < getModel()->meshes[secI].selectedObjectIndices.size(); i++)
                    {
                        if(getModel()->meshes[secI].selectedObjectIndices[i] == elI)
                            match = true;
                    }
                }

                if(match)
                    objectsPanel.sections[secI].elements[elI].button.color = ColorPalette::themeColor;
                else
                    objectsPanel.sections[secI].elements[elI].button.color = ColorPalette::secondColor;
            }
        }
        

        this->objectsPanel.render(timer, !anyDialogActive);

        if(getModel()->newModelAdded){
            this->objectsPanel.sections.clear();
            
            for (size_t meshI = 0; meshI < getModel()->meshes.size(); meshI++)
            {
                Section section;
                section.header = SectionHolder(ColorPalette::secondColor,0.f,getModel()->meshes[meshI].materialName);
                section.header.sectionHolder.active = true;

                for (size_t objI = 0; objI < getModel()->meshes[meshI].objects.size(); objI++){
                    Element btn = Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1.f), getModel()->meshes[meshI].objects[objI].title, Texture(), 0.f, false));
                    btn.button.color = ColorPalette::mainColor;
                    section.elements.push_back(btn);
                }

                objectsPanel.sections.push_back(section);
            }
        }
    }

    displayingModesPanel.render(timer,!anyDialogActive);
    if(displayingModesPanel.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }

    Debugger::block("GUI : Panels : Panel rendering"); // End

    Debugger::block("GUI : Panels : Rest"); // Start

    bool anyVectorPointHover = false;
    for (size_t i = 0; i < painter.vectorStrokes.size(); i++){
        if(painter.vectorStrokes[i].endPointHover || painter.vectorStrokes[i].startPointHover || painter.vectorStrokes[i].offsetPointHover)
            anyVectorPointHover = true;
    }


    if(*Mouse::LClick() && painter.selectedPaintingModeIndex == 5 && !anyVectorPointHover && !anyDialogActive && !anyContextMenuActive && !anyPanelHover){
        VectorStroke vecStroke;
        if(!painter.vectorStrokes.size()){
            registerVectorAction("First point created", painter.vectorStrokes);
            vecStroke.startPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f; 
            vecStroke.endPos = vecStroke.startPos;
            vecStroke.offsetPos = vecStroke.startPos;
            painter.vectorStrokes.push_back(vecStroke);
        }
        else{
            if(painter.vectorStrokes[painter.vectorStrokes.size() - 1].endPos == painter.vectorStrokes[painter.vectorStrokes.size() - 1].startPos){
                registerVectorAction("New point", painter.vectorStrokes);
                painter.vectorStrokes[painter.vectorStrokes.size() - 1].endPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
                painter.vectorStrokes[painter.vectorStrokes.size() - 1].offsetPos = painter.vectorStrokes[painter.vectorStrokes.size() - 1].startPos - (painter.vectorStrokes[painter.vectorStrokes.size() - 1].startPos - painter.vectorStrokes[painter.vectorStrokes.size() - 1].endPos) / 2.f;
            }
            else{
                registerVectorAction("New point", painter.vectorStrokes);
                vecStroke.startPos = painter.vectorStrokes[painter.vectorStrokes.size() - 1].endPos; 
                vecStroke.endPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
                vecStroke.offsetPos = vecStroke.startPos - (vecStroke.startPos - vecStroke.endPos) /2.f;
                painter.vectorStrokes.push_back(vecStroke);
            }
        }
    }
    else{
        for (size_t i = 0; i < painter.vectorStrokes.size(); i++)
        {
            if(painter.vectorStrokes[i].endPointHover && *Mouse::LClick()){
                registerVectorAction("Point transformed", painter.vectorStrokes);
                break;
            }
            else if(painter.vectorStrokes[i].startPointHover && *Mouse::LClick()){
                registerVectorAction("Point transformed", painter.vectorStrokes);
                break;
            }
            else if(painter.vectorStrokes[i].offsetPointHover && *Mouse::LClick()){
                registerVectorAction("Offset transformed", painter.vectorStrokes);
                break;
            }
        }
    }

    if(painter.selectedPaintingModeIndex == 5 && painter.selectedDisplayingModeIndex != 0){
        if(!anyContextMenuActive && !anyDialogActive && !anyPanelHover && Mouse::activeCursor()->cursorType == Mouse::defaultCursor()->cursorType)
            Mouse::setCursor(*Mouse::inkPenCursor());

        for (int i = painter.vectorStrokes.size() - 1; i >= 0; i--)
        {
            painter.vectorStrokes[i].draw(timer, 0.0005f, anyContextMenuActive || anyDialogActive || anyPanelHover, painter.vectorStrokes, i);

            VectorStroke offsetStrokeEnd = VectorStroke(painter.vectorStrokes[i].endPos, painter.vectorStrokes[i].offsetPos, painter.vectorStrokes[i].offsetPos); 
            VectorStroke offsetStrokeStart = VectorStroke(painter.vectorStrokes[i].startPos, painter.vectorStrokes[i].offsetPos, painter.vectorStrokes[i].offsetPos); 
        
            offsetStrokeEnd.draw(timer, 0.0001f, anyContextMenuActive || anyDialogActive || anyPanelHover, painter.vectorStrokes, i);
            offsetStrokeStart.draw(timer, 0.0001f, anyContextMenuActive || anyDialogActive || anyPanelHover, painter.vectorStrokes, i);
        }

        glClear(GL_DEPTH_BUFFER_BIT);
    }


    //---------------- Updating the painting over texture ----------------- 
    
    painter.usePaintingOver = this->paintingOverSection.elements[0].checkBox.clickState1;
    painter.paintingoverTextureEditorMode = this->paintingOverSection.elements[1].checkBox.clickState1;
    painter.paintingOverGrayScale = this->paintingOverSection.elements[4].checkBox.clickState1;
    painter.paintingOverWraping = this->paintingOverSection.elements[5].checkBox.clickState1;
    
    if(this->paintingOverSection.elements[2].button.clicked){
        Texture texture;
        showTextureSelectionDialog(texture, 512, true);
        
        if(texture.ID){
            registerTextureFieldAction("New texture field via texture selection dialog", this->paintingOverTextureFields);
            this->paintingOverTextureFields.push_back(TextureField(texture));
        }
    }
    else if(this->paintingOverSection.elements[3].button.clicked){
        std::string test = showFileSystemObjectSelectionDialog("Select a texture file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_TEXTURE, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);

        if(test.size()){
            Texture texture;
            texture.load(test.c_str());
            registerTextureFieldAction("New texture field via path", this->paintingOverTextureFields);
            this->paintingOverTextureFields.push_back(TextureField(texture));
        }

    }
    
    bool updatePaintingOverTexture = false;
    
    getBox()->bindBuffers();
    glClear(GL_DEPTH_BUFFER_BIT);
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
    ShaderSystem::buttonShader().use();
    ShaderSystem::buttonShader().setMat4("projection", this->projection);

    // Rendering all the painting over texture fields
    if(painter.usePaintingOver){
        for (int i = 0; i < this->paintingOverTextureFields.size(); i++)
        {
            if(this->paintingOverTextureFields[i].transformedFlag)
                updatePaintingOverTexture = true;

            bool anyHover = false;
            for (int ii = 0; ii < this->paintingOverTextureFields.size(); ii++){
                if(ii > i){
                    if(this->paintingOverTextureFields[ii].isHover()){
                        anyHover = true;
                    }    
                }
            }

            this->paintingOverTextureFields[i].render(timer, painter.paintingoverTextureEditorMode && !anyDialogActive && !anyHover && !painter.faceSelection.editMode, false, this->paintingOverTextureFields, i, !painter.paintingOverWraping);
        }    
    }

    if(painter.paintingoverTextureEditorMode && !painter.faceSelection.editMode){
        ShaderSystem::dotsShader().use();

        ShaderSystem::dotsShader().setMat4("projection", this->projection);
        ShaderSystem::dotsShader().setVec3("pos", glm::vec3(getContext()->windowScale / glm::ivec2(2), 0.5f));
        ShaderSystem::dotsShader().setVec2("scale", getContext()->windowScale / glm::ivec2(2));

        ShaderSystem::dotsShader().setVec2("dotPos", glm::vec2(0.f));
        ShaderSystem::dotsShader().setFloat("scroll", 1.f);

        /* Render the dots */
        glDrawArrays(GL_TRIANGLES, 0, 6);

        /* Clear the depth buffer of the current framebuffers*/
        glClear(GL_DEPTH_BUFFER_BIT);

        ShaderSystem::buttonShader().use();   
    }

    if(updatePaintingOverTexture){
        glm::ivec2 paintingRes = glm::ivec2(*Settings::videoScale() / Settings::properties()->paintingResolutionDivier);

        Framebuffer FBO = Framebuffer(painter.paintingOverTexture, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, paintingRes));
        FBO.bind();

        glViewport(0, 0, paintingRes.x, paintingRes.y);

        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Rendering all the painting over texture fields
        for (int i = 0; i < this->paintingOverTextureFields.size(); i++)
        {
            this->paintingOverTextureFields[i].render(timer, false, true, this->paintingOverTextureFields, i, true);
        }    

        // Finish
        Settings::defaultFramebuffer()->FBO.bind();
        Settings::defaultFramebuffer()->setViewport();

        // Deleting the OpenGL framebuffer object & the renderbuffer object
        FBO.deleteBuffers(false, true);
    }

    bool straightLinePaintingCondition = painter.selectedDisplayingModeIndex != 0 && painter.selectedPaintingModeIndex != 5 && !anyDialogActive && (getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) || getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_ALT)) && *Mouse::LPressed(); 
    if(straightLinePaintingCondition && !painter.faceSelection.editMode){

        if(!prevStraightLinePaintingCondition){
            straightLinePaintingStartPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
            glm::vec2 cursorOffset = *Mouse::mouseOffset();
            if(cursorOffset.x || cursorOffset.y)
                cursorOffset = glm::normalize(cursorOffset);
            straightLinePaintingDirectionPos = straightLinePaintingStartPos + cursorOffset * 5.f; 
        }

        glm::vec2 destPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
        
        if(glm::distance(straightLinePaintingDirectionPos, straightLinePaintingStartPos) < 10.f){
            straightLinePaintingDirectionPos = destPos; 
        }

        //ABS
        ShaderSystem::vectoralCurve().use();
        
        ShaderSystem::vectoralCurve().setVec3("pos", glm::vec3(Settings::videoScale()->x/2.f, Settings::videoScale()->y/2.f, 0.9f));
        ShaderSystem::vectoralCurve().setVec2("scale", glm::vec2(Settings::videoScale()->x/2.f, Settings::videoScale()->y/2.f));
        
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_ALT))
            ShaderSystem::vectoralCurve().setVec2("direction", straightLinePaintingDirectionPos);
        else
            ShaderSystem::vectoralCurve().setVec2("direction", destPos);
        
        ShaderSystem::vectoralCurve().setVec2("startPos", straightLinePaintingStartPos);
        ShaderSystem::vectoralCurve().setVec2("destPos", destPos);
        
        ShaderSystem::vectoralCurve().setVec2("percScale", *Settings::videoScale());
        ShaderSystem::vectoralCurve().setInt("lineCapturingMode", 0);
        
        ShaderSystem::vectoralCurve().setFloat("EDGE", 0.0005f);

        glDrawArrays(GL_TRIANGLES, 0 , 6);
        
        ShaderSystem::buttonShader().use();
    }
    else if(prevStraightLinePaintingCondition && !painter.faceSelection.editMode){
        std::vector<VectorStroke> strokeArray;
        strokeArray.push_back(VectorStroke(straightLinePaintingStartPos, *Mouse::cursorPos() / *Settings::videoScale() * 100.f, straightLinePaintingDirectionPos));
        painter.applyVectorStrokes(strokeArray, this->twoDPaintingPanel, this->projection, painter.selectedPaintingModeIndex, this->filterPaintingModeFilterBtn.filter, this->twoDPaintingBox);
    }
    
    prevStraightLinePaintingCondition = straightLinePaintingCondition;


    // ------------------------- Face Selection ----------------------------
    painter.faceSelection.activated = meshSection.elements[1].checkBox.clickState1;
    painter.faceSelection.editMode = meshSection.elements[2].checkBox.clickState1;
    painter.faceSelection.selectionModeIndex = meshSection.elements[3].comboBox.selectedIndex;
    painter.faceSelection.radius = meshSection.elements[4].rangeBar.value;
    if(meshSection.elements[6].button.clicked && painter.selectedMeshIndex < getModel()->meshes.size()){
        glDeleteTextures(1, &painter.faceSelection.meshMask.ID);
        painter.faceSelection.meshMask = meshSection.elements[6].button.texture.generateProceduralTexture(getModel()->meshes[painter.selectedMeshIndex], 1024); 
    }
    if(meshSection.elements[7].button.clicked){
        glDeleteTextures(1, &painter.faceSelection.meshMask.ID);
        painter.faceSelection.meshMask.ID = 0;
        glDeleteTextures(1, &meshSection.elements[6].button.texture.ID);
        meshSection.elements[6].button.texture.ID = 0;
        meshSection.elements[6].button.texture.title = "";
    }

    painter.faceSelection.hideUnselected = meshSection.elements[8].checkBox.clickState1;

    if(painter.selectedMeshIndex < getModel()->meshes.size()){
        if(getModel()->newModelAdded || getModel()->meshes[painter.selectedMeshIndex].materialName != __faceSelectionActiveMesh){
            meshSection.elements[5].comboBox.texts.clear();
            meshSection.elements[5].comboBox.hover.clear();
            meshSection.elements[5].comboBox.hoverMixVal.clear();
            meshSection.elements[5].comboBox.clickedMixVal.clear();

            meshSection.elements[5].comboBox.texts.push_back("Custom");

            __faceSelectionActiveMesh = getModel()->meshes[painter.selectedMeshIndex].materialName;

            for (size_t ii = 0; ii < getModel()->meshes[painter.selectedMeshIndex].objects.size(); ii++)
            {
                meshSection.elements[5].comboBox.texts.push_back(getModel()->meshes[painter.selectedMeshIndex].objects[ii].title);
            }

            for (size_t i = 0; i < meshSection.elements[5].comboBox.texts.size(); i++)
            {
                meshSection.elements[5].comboBox.hover.push_back(0);
                meshSection.elements[5].comboBox.hoverMixVal.push_back(0);
                meshSection.elements[5].comboBox.clickedMixVal.push_back(0);
            }

            meshSection.elements[5].comboBox.selectedIndex = 0;
        }
    }
    
    if(meshSection.elements[5].comboBox.selectedIndex != 0){
        painter.faceSelection.changedIndices.clear();
        for (size_t i = 0; i < painter.faceSelection.selectedPrimitiveIDs.size(); i++)
        {
            painter.faceSelection.changedIndices.push_back(i);
        }
        
        for (size_t i = 0; i < painter.faceSelection.selectedPrimitiveIDs.size(); i++)
        {
            painter.faceSelection.selectedPrimitiveIDs[i] = false;
        }
        

        if(painter.selectedMeshIndex < getModel()->meshes.size()){
            int objI = 0;
            for (size_t ii = 0; ii < getModel()->meshes[painter.selectedMeshIndex].objects.size(); ii++)
            {
                if(getModel()->meshes[painter.selectedMeshIndex].objects[ii].title == meshSection.elements[5].comboBox.texts[meshSection.elements[5].comboBox.selectedIndex]){
                    objI = ii;
                }
            }

            for (size_t i = getModel()->meshes[painter.selectedMeshIndex].objects[objI].vertIndices.x / 3; i < getModel()->meshes[painter.selectedMeshIndex].objects[objI].vertIndices.y / 3; i++)
            {
                if(i < painter.faceSelection.selectedPrimitiveIDs.size())
                    painter.faceSelection.selectedPrimitiveIDs[i] = true;
            }
        }
    }
    

    bool applyBoxSelection = false;
    if(!anyDialogActive && painter.faceSelection.editMode && painter.faceSelection.selectionModeIndex == 1)
        applyBoxSelection = painter.faceSelection.boxSelectionInteraction(timer);

    if(
            (painter.selectedMeshIndex < getModel()->meshes.size() && 
            (((*Mouse::LPressed() || Shortcuts::CTRL_A()) && painter.faceSelection.selectionModeIndex == 0) || (applyBoxSelection && painter.faceSelection.selectionModeIndex == 1)) && 
            !anyPanelHover && 
            !anyDialogActive && 
            painter.faceSelection.editMode) ||
            __faceSelectionActiveObjIndex != meshSection.elements[5].comboBox.selectedIndex

        )
    {
        if(painter.faceSelection.interaction(getModel()->meshes[painter.selectedMeshIndex], painter.selectedMeshIndex, !anyPanelHover, getScene()->viewMatrix, getScene()->projectionMatrix, getScene()->transformMatrix, *Mouse::cursorPos(), false, true)){
            if(__faceSelectionActiveObjIndex == meshSection.elements[5].comboBox.selectedIndex)
                meshSection.elements[5].comboBox.selectedIndex = 0;
        }
        __faceSelectionActiveObjIndex = meshSection.elements[5].comboBox.selectedIndex;
    }

    Debugger::block("GUI : Panels : Rest"); // End
}

void UI::renderRenamingTextbox(Timer &timer, Painter &painter){
    
    if(renamingTextBox.active){
        renamingTextBox.render(timer,false);
        renamingTextBoxClosed = false;
    }
    else{
        //The first frame renamingTextBox is closed 
        if(!renamingTextBoxClosed){
            if(getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_RELEASE || *Mouse::mouseScroll()){
                if(renamingIndices.x == 0 && renamingIndices.y < Library::getTextureArraySize()){
                    registerLibraryElementRenamingAction("Texture renamed", Texture(), renamingIndices.y, "TEXTURE", lastTitleBeforeRenaming);
                    Library::getTexture(renamingIndices.y)->title = renamingTextBox.text;
                }
                else if(renamingIndices.x == 1 && renamingIndices.y < Library::getMaterialArraySize()){
                    registerLibraryElementRenamingAction("Material renamed", Texture(), renamingIndices.y, "MATERIAL", lastTitleBeforeRenaming);
                    Library::getMaterial(renamingIndices.y)->title = renamingTextBox.text;
                }
                else if(renamingIndices.x == 2 && renamingIndices.y < Library::getBrushArraySize()){
                    registerLibraryElementRenamingAction("Brush renamed", Texture(), renamingIndices.y, "BRUSH", lastTitleBeforeRenaming);
                    Library::getBrush(renamingIndices.y)->title = renamingTextBox.text;
                }
            }
            else{
                if(renamingIndices.x == 0 && renamingIndices.y < Library::getTextureArraySize())
                    Library::getTexture(renamingIndices.y)->title = lastTitleBeforeRenaming;
                else if(renamingIndices.x == 1 && renamingIndices.y < Library::getMaterialArraySize())
                    Library::getMaterial(renamingIndices.y)->title = lastTitleBeforeRenaming;
                else if(renamingIndices.x == 2 && renamingIndices.y < Library::getBrushArraySize())
                    Library::getBrush(renamingIndices.y)->title = lastTitleBeforeRenaming;
            }

            Library::nameControl();

            //Update the library panel
            Library::setChanged(true);
        }
        renamingTextBoxClosed = true;
    }
}

void UI::renderDialogs(Timer &timer,  Project &project, Skybox &skybox, Painter& painter){
    if(newProjectDialog.dialogControl.isActive())
        newProjectDialog.render(timer,project,greetingDialog.dialogControl.active,greetingDialog.startScreen, logDialog);
    
    if(loadProjectDialog.dialogControl.isActive())
        loadProjectDialog.render(timer,project,greetingDialog.dialogControl.active,greetingDialog.startScreen, logDialog);    

    if(greetingDialog.dialogControl.isActive())
        greetingDialog.render(timer,newProjectDialog,loadProjectDialog);

    if(displayerDialog.dialogControl.isActive())
        displayerDialog.render(timer,skybox, logDialog);
    
    if(textureEditorDialog.dialogControl.isActive())
        textureEditorDialog.render(timer,skybox,this->projection, this->textureEditorSelectedTxtr, logDialog);
    
    if(exportDialog.dialogControl.isActive())
        exportDialog.render(timer,project,greetingDialog.dialogControl.active,materialEditorDialog, logDialog);
    
    if(newTextureDialog.dialogControl.isActive())
        newTextureDialog.render(timer);
    
    if(paintingChannelsAutoCreateTexturesDialog.dialogControl.isActive())
        paintingChannelsAutoCreateTexturesDialog.render(timer, this->paintingChannelsSection);
    
    if(settingsDialog.dialogControl.isActive())
        settingsDialog.render(timer, painter, logDialog);
    
    if(materialDisplayerDialog.dialogControl.isActive())
        materialDisplayerDialog.render(timer);
    
    if(filterDisplayerDialog.dialogControl.isActive())
        filterDisplayerDialog.render(timer, projection);
    
    if(bakingDialog.dialogControl.isActive())
        bakingDialog.render(timer, skybox, logDialog);
    
    if(objectTexturingDialog.dialogControl.isActive() && !materialEditorDialog.dialogControl.active)
        objectTexturingDialog.render(timer, this->projection, materialEditorDialog, logDialog);
    
    if(materialEditorDialog.dialogControl.isActive())
        materialEditorDialog.render(timer, textureSelectionDialog, logDialog);
    
    if(!Settings::properties()->cat_hide){
        logDialog.render(
                            timer, painter, greetingDialog, newProjectDialog, exportDialog, materialDisplayerDialog, filterDisplayerDialog,
                            loadProjectDialog, materialEditorDialog, textureSelectionDialog, bakingDialog, filterSelectionDialog, newTextureDialog, 
                            paintingChannelsAutoCreateTexturesDialog, settingsDialog, displayerDialog, textureEditorDialog, texturePackEditorDialog, 
                            projectRecoverDialog, objectTexturingDialog, paintingOverTextureFields, project
                        );
    }
    else
        logDialog.cryCounter = 5;
}

static Color prevClr;

void UI::renderDropper(Painter &painter){
    if(*Mouse::LClick() || getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)){
        
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)){
            if(painter.selectedColorIndex == 0)
                painter.color1 = prevClr;
            if(painter.selectedColorIndex == 1)
                painter.color2 = prevClr;
            if(painter.selectedColorIndex == 2)
                painter.color3 = prevClr;
        }

        if(painter.selectedColorIndex == 0)
            prevClr = painter.color1;
        if(painter.selectedColorIndex == 1)
            prevClr = painter.color2;
        if(painter.selectedColorIndex == 2)
            prevClr = painter.color3;
    
        if(!colorSection.elements[5].button.clicked){
            dropper.active = false;
        }
    }

    if(dropper.active){
        Settings::defaultFramebuffer()->FBO.bind();
        Settings::defaultFramebuffer()->setViewport();
        
        //Dropper active pick color
        glm::vec4 cursorHoverPixelRGBData;
        //Read the cursor position from the default frame buffer
        glReadPixels(Mouse::cursorPos()->x, getContext()->windowScale.y - Mouse::cursorPos()->y, 1, 1, GL_RGBA, GL_FLOAT, &cursorHoverPixelRGBData);
        
        dropper.value = cursorHoverPixelRGBData;

        //std::cout << glm::to_string(*Mouse::cursorPos()) << std::endl; 
    
        //dropper.active = false;
        if(painter.selectedColorIndex == 0)
            painter.color1.loadRGB(dropper.value*glm::vec3(255));
        if(painter.selectedColorIndex == 1)
            painter.color2.loadRGB(dropper.value*glm::vec3(255));
        if(painter.selectedColorIndex == 2)
            painter.color3.loadRGB(dropper.value*glm::vec3(255));
        if(painter.selectedColorIndex == 0)
            painter.color1.loadRGB(dropper.value*glm::vec3(255));
        if(painter.selectedColorIndex == 1)
            painter.color2.loadRGB(dropper.value*glm::vec3(255));
        if(painter.selectedColorIndex == 2)
            painter.color3.loadRGB(dropper.value*glm::vec3(255));
    }
} 