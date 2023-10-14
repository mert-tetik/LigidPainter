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

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

TextureSelectionDialog __texture_selection_dialog;
FilterSelectionDialog __filter_selection_dialog;
MeshSelectionDialog __mesh_selection_dialog;
TexturePackEditorDialog __texture_Pack_Editor_Dialog;
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

bool wasTextureSelectionDialogActive(){
    return __wasTextureSelectionDialogActive;
}

/* -- Forward declerations -- */

static void renderBrushCursor(Painter& painter, glm::mat4 guiProjection);

void UI::render(Timer &timer,Project &project, Painter &painter, Skybox &skybox){
    
    __texture_selection_dialog = this->textureSelectionDialog;
    __filter_selection_dialog = this->filterSelectionDialog;
    __texture_Pack_Editor_Dialog = this->texturePackEditorDialog;
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
    
    //TODO Don't call that everyframe (maybe)
    //Rename every library element if their name is doubled 
    if(!this->renamingTextBox.active)
        Library::nameControl();

    //Render the panels
    renderPanels(timer, painter, screenGapPerc);

    //Render renaming textbox
    renderRenamingTextbox(timer, painter);
    
    //Render the nodes
    NodeScene::render(timer,nodeEditorDisplayer, nodePanel, !this->anyDialogActive);
    
    //Render the dialogs
    renderDialogs(timer, project, skybox, painter);
    
    //Render the dropper & pick color if mouse left button clicked
    renderDropper(painter);

    //Render the brush cursor
    if(
            !this->anyPanelHover && 
            !this->anyDialogActive && 
            !this->anyContextMenuActive && 
            (painter.selectedDisplayingModeIndex == 1 || painter.selectedDisplayingModeIndex == 2) && painter.selectedPaintingModeIndex != 5 &&
            !painter.paintingoverTextureEditorMode
        )
    {
        renderBrushCursor(painter, this->projection);
    }
    else
        getContext()->window.setCursorVisibility(true);

    //Interactions of the UI elements
    elementInteraction(painter, timer, screenGapPerc, project, this->materialEditorDialog.appMaterialModifiers);

    frameCounter++;

    if(frameCounter > 1000)
        frameCounter = 0;

    __wasTextureSelectionDialogActive = false;

    
}



//UTILITY FUNCTIONS

static void renderBrushCursor(Painter& painter, glm::mat4 guiProjection){
    /* Use the circle shader */
    ShaderSystem::circleShader().use();

    /* Set the transform data & the projection */
    ShaderSystem::circleShader().setMat4("projection", guiProjection);
    ShaderSystem::circleShader().setVec3("pos", Mouse::cursorPos()->x, Mouse::cursorPos()->y, 1);
    ShaderSystem::circleShader().setVec2("scale", glm::vec2(painter.brushProperties.radius * (Settings::videoScale()->x)));

    /* Hide the cursor */
    getContext()->window.setCursorVisibility(false);

    /* Render the circle s*/
    glDrawArrays(GL_TRIANGLES, 0, 6);

    /* Clear the depth buffer of the current framebuffers*/
    glClear(GL_DEPTH_BUFFER_BIT);

    /* Use the button shader back */
    ShaderSystem::buttonShader().use();
}

void UI::renderPanels(Timer &timer, Painter &painter,  float screenGapPerc){
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
    paintingPanel.render(timer,!anyDialogActive);
    if(paintingPanel.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }
    
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
    nodeEditorDisplayer.render(timer,!anyDialogActive);
    if(nodeEditorDisplayer.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }
    selectedTextureDisplayer.render(timer,false);
    if(selectedTextureDisplayer.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
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

    if(!painter.threeDimensionalMode){
        
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
        ShaderSystem::twoDPaintingModeAreaShader().setInt("paintingOverTexture", 10);
        ShaderSystem::twoDPaintingModeAreaShader().setInt("brushModeState", painter.selectedPaintingModeIndex);
        ShaderSystem::twoDPaintingModeAreaShader().setInt("usePaintingOver", painter.usePaintingOver);
        ShaderSystem::twoDPaintingModeAreaShader().setInt("paintingOverGrayScale", painter.paintingOverGrayScale);
        ShaderSystem::twoDPaintingModeAreaShader().setInt("paintingOverWraping", painter.paintingOverWraping);
        ShaderSystem::twoDPaintingModeAreaShader().setVec3("paintingColor", painter.getSelectedColor().getRGB_normalized());
        ShaderSystem::twoDPaintingModeAreaShader().setFloat("smearTransformStrength", painter.smearTransformStrength);
        ShaderSystem::twoDPaintingModeAreaShader().setFloat("smearBlurStrength", painter.smearBlurStrength);


        //* Bind the textures
        //painted texture
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, painter.selectedTexture.ID);

        // Render the texture as it's pixels can be seen
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        // Painting over texture
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, painter.paintingOverTexture);
        
        //paintingTexture 
        glActiveTexture(GL_TEXTURE6);
        if(painter.selectedDisplayingModeIndex == 0)
            glBindTexture(GL_TEXTURE_2D, 0);
        else
            glBindTexture(GL_TEXTURE_2D, painter.projectedPaintingTexture.ID);

        //*Fragment
        ShaderSystem::twoDPaintingModeAreaShader().setInt("doDepthTest", 0);

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

    paintingModesPanel.render(timer,!anyDialogActive);

    if(painter.selectedPaintingModeIndex == 2)
        smearPaintingModePropertyPanel.render(timer, !anyDialogActive); 
    
    if(painter.selectedPaintingModeIndex == 4)
        this->filterPaintingModeFilterBtn.render(timer, !anyDialogActive);

    if(painter.selectedPaintingModeIndex == 5)
        vectorPaintingModePropertyPanel.render(timer, !anyDialogActive); 

    if(paintingModesPanel.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }
    displayingModesPanel.render(timer,!anyDialogActive);
    if(displayingModesPanel.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }

    
    
    bool anyVectorPointHover = false;
    for (size_t i = 0; i < painter.vectorStrokes.size(); i++){
        if(painter.vectorStrokes[i].endPointHover || painter.vectorStrokes[i].startPointHover || painter.vectorStrokes[i].offsetPointHover)
            anyVectorPointHover = true;
    }


    if(*Mouse::LClick() && painter.selectedPaintingModeIndex == 5 && !anyVectorPointHover && !anyDialogActive && !anyContextMenuActive && !anyPanelHover){
        VectorStroke vecStroke;
        if(!painter.vectorStrokes.size()){
            vecStroke.startPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f; 
            vecStroke.endPos = vecStroke.startPos;
            vecStroke.offsetPos = vecStroke.startPos;
            painter.vectorStrokes.push_back(vecStroke);
        }
        else{
            if(painter.vectorStrokes[painter.vectorStrokes.size() - 1].endPos == painter.vectorStrokes[painter.vectorStrokes.size() - 1].startPos){
                painter.vectorStrokes[painter.vectorStrokes.size() - 1].endPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
                painter.vectorStrokes[painter.vectorStrokes.size() - 1].offsetPos = painter.vectorStrokes[painter.vectorStrokes.size() - 1].startPos - (painter.vectorStrokes[painter.vectorStrokes.size() - 1].startPos - painter.vectorStrokes[painter.vectorStrokes.size() - 1].endPos) / 2.f;
            }
            else{
                vecStroke.startPos = painter.vectorStrokes[painter.vectorStrokes.size() - 1].endPos; 
                vecStroke.endPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
                vecStroke.offsetPos = vecStroke.startPos - (vecStroke.startPos - vecStroke.endPos) /2.f;
                painter.vectorStrokes.push_back(vecStroke);
            }
        }
    }

    if(painter.selectedPaintingModeIndex == 5){
        if(!anyContextMenuActive && !anyDialogActive && !anyPanelHover && Mouse::activeCursor()->cursorType == Mouse::defaultCursor()->cursorType)
            Mouse::setCursor(*Mouse::inkPenCursor());

        for (int i = painter.vectorStrokes.size() - 1; i >= 0; i--)
        {
            painter.vectorStrokes[i].draw(0.0005, anyContextMenuActive || anyDialogActive || anyPanelHover, painter.vectorStrokes, i);

            VectorStroke offsetStrokeEnd = VectorStroke(painter.vectorStrokes[i].endPos, painter.vectorStrokes[i].offsetPos, painter.vectorStrokes[i].offsetPos); 
            VectorStroke offsetStrokeStart = VectorStroke(painter.vectorStrokes[i].startPos, painter.vectorStrokes[i].offsetPos, painter.vectorStrokes[i].offsetPos); 
        
            offsetStrokeEnd.draw(0.0001, anyContextMenuActive || anyDialogActive || anyPanelHover, painter.vectorStrokes, i);
            offsetStrokeStart.draw(0.0001, anyContextMenuActive || anyDialogActive || anyPanelHover, painter.vectorStrokes, i);
        }

        glClear(GL_DEPTH_BUFFER_BIT);
    }


    //---------------- Updating the painting over texture ----------------- 
    
    painter.usePaintingOver = this->paintingPanel.sections[4].elements[0].checkBox.clickState1;
    painter.paintingoverTextureEditorMode = this->paintingPanel.sections[4].elements[1].checkBox.clickState1;
    painter.paintingOverGrayScale = this->paintingPanel.sections[4].elements[4].checkBox.clickState1;
    painter.paintingOverWraping = this->paintingPanel.sections[4].elements[5].checkBox.clickState1;
    
    if(this->paintingPanel.sections[4].elements[2].button.clicked){
        Texture texture;
        showTextureSelectionDialog(texture, 512, true);
        
        if(texture.ID)
            this->paintingOverTextureFields.push_back(TextureField(texture));
    }
    else if(this->paintingPanel.sections[4].elements[3].button.clicked){
        std::string test = showFileSystemObjectSelectionDialog("Select a texture file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_TEXTURE, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);

        if(test.size()){
            Texture texture;
            texture.load(test.c_str());
            this->paintingOverTextureFields.push_back(TextureField(texture));
        }

    }
    
    bool updatePaintingOverTexture = false;
    

    // Rendering all the painting over texture fields
    if(painter.usePaintingOver){
        for (int i = 0; i < this->paintingOverTextureFields.size(); i++)
        {
            if(this->paintingOverTextureFields[i].transformedFlag)
                updatePaintingOverTexture = true;
            
            this->paintingOverTextureFields[i].render(timer, painter.paintingoverTextureEditorMode && !anyDialogActive, false, this->paintingOverTextureFields, i);
        }    
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
            this->paintingOverTextureFields[i].render(timer, false, true, this->paintingOverTextureFields, i);
        }    

        // Finish
        Settings::defaultFramebuffer()->FBO.bind();
        Settings::defaultFramebuffer()->setViewport();

        // Deleting the OpenGL framebuffer object & the renderbuffer object
        FBO.deleteBuffers(false, true);
    }
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
                if(renamingIndices.x == 0 && renamingIndices.y < Library::getTextureArraySize())
                    Library::getTexture(renamingIndices.y)->title = renamingTextBox.text;
                else if(renamingIndices.x == 1 && renamingIndices.y < Library::getMaterialArraySize())
                    Library::getMaterial(renamingIndices.y)->title = renamingTextBox.text;
                else if(renamingIndices.x == 2 && renamingIndices.y < Library::getBrushArraySize())
                    Library::getBrush(renamingIndices.y)->title = renamingTextBox.text;
            }
            else{
                if(renamingIndices.x == 0 && renamingIndices.y < Library::getTextureArraySize())
                    Library::getTexture(renamingIndices.y)->title = lastTitleBeforeRenaming;
                else if(renamingIndices.x == 1 && renamingIndices.y < Library::getMaterialArraySize())
                    Library::getMaterial(renamingIndices.y)->title = lastTitleBeforeRenaming;
                else if(renamingIndices.x == 2 && renamingIndices.y < Library::getBrushArraySize())
                    Library::getBrush(renamingIndices.y)->title = lastTitleBeforeRenaming;
            }

            //Update the library panel
            Library::setChanged(true);
        }
        renamingTextBoxClosed = true;
    }
}

void UI::renderDialogs(Timer &timer,  Project &project, Skybox &skybox, Painter& painter){
    if(newProjectDialog.dialogControl.isActive())
        newProjectDialog.render(timer,project,greetingDialog.dialogControl.active,greetingDialog.startScreen);
    
    if(loadProjectDialog.dialogControl.isActive())
        loadProjectDialog.render(timer,project,greetingDialog.dialogControl.active,greetingDialog.startScreen);
    
    if(greetingDialog.dialogControl.isActive())
        greetingDialog.render(timer,newProjectDialog,loadProjectDialog);

    if(displayerDialog.dialogControl.isActive())
        displayerDialog.render(timer,skybox);
    
    if(textureEditorDialog.dialogControl.isActive())
        textureEditorDialog.render(timer,skybox,this->projection, this->textureEditorSelectedTxtr);
    
    if(exportDialog.dialogControl.isActive())
        exportDialog.render(timer,project,greetingDialog.dialogControl.active,materialEditorDialog);
    
    if(newTextureDialog.dialogControl.isActive())
        newTextureDialog.render(timer);
    
    if(settingsDialog.dialogControl.isActive())
        settingsDialog.render(timer, painter);
    
    if(materialDisplayerDialog.dialogControl.isActive())
        materialDisplayerDialog.render(timer);
    
    if(filterDisplayerDialog.dialogControl.isActive())
        filterDisplayerDialog.render(timer, projection);
    
    if(bakingDialog.dialogControl.isActive())
        bakingDialog.render(timer, skybox);
    
    if(materialEditorDialog.dialogControl.isActive() && Library::getMaterialArraySize())
        materialEditorDialog.render(timer,textureSelectionDialog,*Library::getMaterial(selectedMaterialIndex));
    
    logDialog.render(timer);
}

void UI::renderDropper(Painter &painter){
    if(*Mouse::LClick() && dropper.active){
        //Dropper active pick color
        glm::vec3 cursorHoverPixelRGBData;
        //Read the cursor position from the default frame buffer
        glReadPixels(Mouse::cursorPos()->x, Mouse::cursorPos()->y,1,1,GL_RGB,GL_FLOAT,&cursorHoverPixelRGBData);
        
        dropper.value = cursorHoverPixelRGBData; 
    
        dropper.active = false;
        if(painter.selectedColorIndex == 0)
            painter.color1.loadRGB(dropper.value*glm::vec3(255));
        if(painter.selectedColorIndex == 1)
            painter.color2.loadRGB(dropper.value*glm::vec3(255));
        if(painter.selectedColorIndex == 2)
            painter.color3.loadRGB(dropper.value*glm::vec3(255));
    }
}