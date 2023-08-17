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

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

TextureSelectionDialog __texture_selection_dialog;
TextRenderer __render_gui_textRenderer;
glm::mat4 __projection;
LigidWindow __window;
glm::vec2 __videoScale; 
Timer __timer; 
bool __wasTextureSelectionDialogActive = false;
glm::ivec2 __windowSize;

void showTextureSelectionDialog(Texture& txtr, int displayingTextureRes){
    __texture_selection_dialog.show(__videoScale, __timer, __projection, txtr, __window, __render_gui_textRenderer, displayingTextureRes);
    __wasTextureSelectionDialogActive = true;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,__windowSize.x,__windowSize.y);
}

bool wasTextureSelectionDialogActive(){
    return __wasTextureSelectionDialogActive;
}

/* -- Forward declerations -- */

static void renderBrushCursor(Painter& painter, glm::mat4 guiProjection, Context& context);

void UI::render(glm::vec2 videoScale, Timer &timer, TextRenderer &textRenderer,Context context,Box box,
            std::vector<ContextMenu> &contextMenus, AppSettings& settings, Project &project, Painter &painter, Skybox &skybox,Model &model, Scene& scene){
    
    __texture_selection_dialog = this->textureSelectionDialog;
    __projection = this->projection;
    __window = context.window;
    __videoScale = videoScale; 
    __timer = timer; 
    __render_gui_textRenderer = textRenderer; 
    __windowSize = context.windowScale;


    //Set pass less or equal
    glDepthFunc(GL_LEQUAL);


    //Give projection to the curve shader        
    ShaderSystem::connectionCurve().use();
    ShaderSystem::connectionCurve().setMat4("projection",this->projection); 
    
    //Give projection to the color picker shader !IS NOT USED RN 
    ShaderSystem::colorPicker().use();
    ShaderSystem::colorPicker().setMat4("projection",this->projection); 
    
    //Use the related shader 
    ShaderSystem::buttonShader().use();

    ShaderSystem::buttonShader().setInt("properties.invertTheTexture", false);
    ShaderSystem::buttonShader().setVec2("properties.txtrScale", glm::vec2(1.f));
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);

    //Set the ortho projection     
    ShaderSystem::buttonShader().setMat4("projection",this->projection); 

    //Calculate the total screen gap
    float screenGap = videoScale.x - context.windowScale.x; //Use that value to keep panels on the right side
    
    //Calculate the screen gap in 0 - 100 range
    float screenGapPerc = screenGap / videoScale.x * 100.f; 
    
    //TODO Don't call that everyframe (maybe)
    //Rename every library element if their name is doubled 
    Library::uniqueNameControl();

    //Render the panels
    renderPanels(videoScale, timer, textRenderer, painter, model, screenGapPerc);

    //Render renaming textbox
    renderRenamingTextbox(videoScale, timer, textRenderer, painter, context);
    
    //Render the nodes
    NodeScene::render(videoScale,timer,textRenderer,model, settings.textureRes, scene, nodeEditorDisplayer, nodePanel);
    
    //Render the dialogs
    renderDialogs(videoScale, timer, textRenderer, context, project, model, skybox, settings, box, contextMenus, scene);
    
    //Render the dropper & pick color if mouse left button clicked
    renderDropper(painter);

    //Render the brush cursor
    if(!this->anyPanelHover && !this->anyDialogActive && !this->anyContextMenuActive && (painter.selectedDisplayingModeIndex == 1 || painter.selectedDisplayingModeIndex == 2))
        renderBrushCursor(painter, this->projection, context);
    else
        context.window.setCursorVisibility(true);

    //Interactions of the UI elements
    elementInteraction(painter, contextMenus, context, videoScale, textRenderer, timer, settings.textureRes, screenGapPerc, model, project, scene, this->materialEditorDialog.appMaterialModifiers);

    frameCounter++;

    if(frameCounter > 1000)
        frameCounter = 0;

    __wasTextureSelectionDialogActive = false;

    
}



//UTILITY FUNCTIONS

static void renderBrushCursor(Painter& painter, glm::mat4 guiProjection, Context& context){
    /* Use the circle shader */
    ShaderSystem::circleShader().use();

    /* Set the transform data & the projection */
    ShaderSystem::circleShader().setMat4("projection", guiProjection);
    ShaderSystem::circleShader().setVec3("pos", Mouse::cursorPos()->x, Mouse::cursorPos()->y, 1);
    ShaderSystem::circleShader().setVec2("scale", glm::vec2(painter.brushProperties.radius));

    /* Hide the cursor */
    context.window.setCursorVisibility(false);

    /* Render the circle s*/
    glDrawArrays(GL_TRIANGLES, 0, 6);

    /* Clear the depth buffer of the current framebuffers*/
    glClear(GL_DEPTH_BUFFER_BIT);

    /* Use the button shader back */
    ShaderSystem::buttonShader().use();
}

void UI::renderPanels(glm::vec2 videoScale, Timer &timer, TextRenderer &textRenderer, Painter &painter,  Model& model, float screenGapPerc){
    navigationPanel.render(videoScale,timer,textRenderer,!anyDialogActive);
    if(navigationPanel.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }
    windowPanel.render(videoScale,timer,textRenderer,!anyDialogActive);
    if(windowPanel.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }
    paintingPanel.render(videoScale,timer,textRenderer,!anyDialogActive);
    if(paintingPanel.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }
    
    libraryPanelLeft.render(videoScale,timer,textRenderer,!anyDialogActive);
    if(libraryPanelLeft.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }
    libraryPanelDisplayer.render(videoScale,timer,textRenderer,!anyDialogActive);
    if(libraryPanelDisplayer.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }
    nodeEditorDisplayer.render(videoScale,timer,textRenderer,!anyDialogActive);
    if(nodeEditorDisplayer.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }
    selectedTextureDisplayer.render(videoScale,timer,textRenderer,false);
    if(selectedTextureDisplayer.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }
    
    if(nodeEditorDisplayer.hover){
        
        if(*Mouse::MPressed()){
            this->nodePanel.position += *Mouse::mouseOffset();
            
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
        float scrVal = *Mouse::mouseScroll() / videoScale.y * 4.f;
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

        //Calculate the position value        
        if(*Mouse::MPressed() && this->twoDPaintingPanel.hover)
            this->twoDPaintingScenePos += *Mouse::mouseOffset();
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
        twoDPaintingPanel.render(videoScale,timer,textRenderer,false);
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

        //Render the painting texture
        ShaderSystem::twoDPaintingModeAreaShader().use();

        //*Fragment
        ShaderSystem::twoDPaintingModeAreaShader().setInt("txtr", 5);
        ShaderSystem::twoDPaintingModeAreaShader().setInt("paintingTexture", 6);
        ShaderSystem::twoDPaintingModeAreaShader().setInt("depthTexture", 7);
        ShaderSystem::twoDPaintingModeAreaShader().setInt("brushModeState", painter.selectedPaintingModeIndex);
        ShaderSystem::twoDPaintingModeAreaShader().setFloat("paintingOpacity", painter.brushProperties.opacity / 100.f);
        ShaderSystem::twoDPaintingModeAreaShader().setVec3("paintingColor", painter.getSelectedColor().getRGB_normalized());

        //*Vertex
        ShaderSystem::twoDPaintingModeAreaShader().setMat4("projectedPosProjection", this->projection);
        ShaderSystem::twoDPaintingModeAreaShader().setMat4("projection", this->projection);
        ShaderSystem::twoDPaintingModeAreaShader().setVec3("pos", glm::vec3(
                                                                        twoDPaintingPanel.sections[0].elements[0].button.resultPos.x + this->twoDPaintingScenePos.x,
                                                                        twoDPaintingPanel.sections[0].elements[0].button.resultPos.y + this->twoDPaintingScenePos.y,
                                                                        twoDPaintingPanel.sections[0].elements[0].button.resultPos.z
                                                                    )
                                                    );
        ShaderSystem::twoDPaintingModeAreaShader().setVec2("scale", destScale * this->twoDPaintingSceneScroll);

        //* Bind the textures
        //painted texture
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, painter.selectedTexture.ID);
        
        //paintingTexture 
        glActiveTexture(GL_TEXTURE6);
        if(painter.selectedDisplayingModeIndex == 0)
            glBindTexture(GL_TEXTURE_2D, 0);
        else
            glBindTexture(GL_TEXTURE_2D, painter.paintingTexture);

        //depthTexture 
        glActiveTexture(GL_TEXTURE7);
        if(painter.selectedDisplayingModeIndex == 0)
            glBindTexture(GL_TEXTURE_2D, 0);
        else
            glBindTexture(GL_TEXTURE_2D, painter.depthTexture);

        glDrawArrays(GL_TRIANGLES,0,6);

        ShaderSystem::buttonShader().use();
    }

    paintingModesPanel.render(videoScale,timer,textRenderer,!anyDialogActive);
    if(paintingModesPanel.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }
    displayingModesPanel.render(videoScale,timer,textRenderer,!anyDialogActive);
    if(displayingModesPanel.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, painter);
    }
}

void UI::renderRenamingTextbox(glm::vec2 videoScale, Timer &timer, TextRenderer &textRenderer, Painter &painter,  Context &context){
    
    if(renamingTextBox.active){
        renamingTextBox.render(videoScale,timer,textRenderer,false,context.window);
        renamingTextBoxClosed = false;
    }
    else{
        //The first frame renamingTextBox is closed 
        if(!renamingTextBoxClosed){
            if(context.window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_RELEASE){
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

void UI::renderDialogs(glm::vec2 videoScale, Timer &timer, TextRenderer &textRenderer,  Context &context, Project &project, Model& model, Skybox &skybox, AppSettings& settings, Box &box,  std::vector<ContextMenu> &contextMenus, Scene scene){
    if(newProjectDialog.dialogControl.isActive())
        newProjectDialog.render(context.window,colorPalette,timer,textRenderer,videoScale,project,greetingDialog.dialogControl.active,greetingDialog.startScreen,model,settings.textureRes);
    
    if(loadProjectDialog.dialogControl.isActive())
        loadProjectDialog.render(context.window,colorPalette,timer,textRenderer,videoScale,project,greetingDialog.dialogControl.active,greetingDialog.startScreen,model,settings.textureRes);
    
    if(greetingDialog.dialogControl.isActive())
        greetingDialog.render(context.window,colorPalette,timer,textRenderer,videoScale,newProjectDialog,loadProjectDialog);

    if(displayerDialog.dialogControl.isActive())
        displayerDialog.render(context.window,colorPalette,timer,textRenderer,videoScale,skybox);
    
    if(exportDialog.dialogControl.isActive())
        exportDialog.render(context.window,colorPalette,timer,textRenderer,videoScale,project,greetingDialog.dialogControl.active,model,materialEditorDialog,sphereModel, scene);
    
    if(newTextureDialog.dialogControl.isActive())
        newTextureDialog.render(context.window,colorPalette,timer,textRenderer,videoScale,settings.textureRes);
    
    if(settingsDialog.dialogControl.isActive())
        settingsDialog.render(context.window, colorPalette, timer, textRenderer, videoScale, settings);
    
    if(materialEditorDialog.dialogControl.isActive() && Library::getMaterialArraySize())
        materialEditorDialog.render(videoScale,timer,textRenderer,textureSelectionDialog,*Library::getMaterial(selectedMaterialIndex),settings.textureRes,box,context,contextMenus,model, scene);
    
    logDialog.render(context.window, colorPalette, timer, textRenderer, videoScale);
}

void UI::renderDropper(Painter &painter){
    if(*Mouse::LClick() && dropper.active){
        //Dropper active pick color
        glm::vec3 cursorHoverPixelRGBData;
        //Read the cursor position from the default frame buffer
        glReadPixels(Mouse::cursorPos()->x,Mouse::cursorPos()->y,1,1,GL_RGB,GL_FLOAT,&cursorHoverPixelRGBData);
        
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