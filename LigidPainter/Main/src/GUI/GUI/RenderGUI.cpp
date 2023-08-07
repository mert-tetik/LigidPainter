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

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

TextureSelectionDialog __texture_selection_dialog;
Library __library;
TextRenderer __render_gui_textRenderer;
glm::mat4 __projection;
LigidWindow __window;
glm::vec2 __videoScale; 
Timer __timer; 
bool __wasTextureSelectionDialogActive = false;
glm::ivec2 __windowSize;

void showTextureSelectionDialog(Texture& txtr){
    __texture_selection_dialog.show(__videoScale, __timer, __library, __projection, txtr, __window, __render_gui_textRenderer);
    __wasTextureSelectionDialogActive = true;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,__windowSize.x,__windowSize.y);
}

bool wasTextureSelectionDialogActive(){
    return __wasTextureSelectionDialogActive;
}

/* -- Forward declerations -- */

static void renderBrushCursor(Shader& circleShader, Shader& buttonShader, Painter& painter, Mouse& mouse, glm::mat4 guiProjection, Context& context);

void UI::render(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer,Context context,Box box,Library &library,std::vector<Node> &meshNodeScene,
            std::vector<ContextMenu> &contextMenus, AppSettings& settings, Project &project, Painter &painter, Skybox &skybox,Model &model, Scene& scene){
    
    __texture_selection_dialog = this->textureSelectionDialog;
    __library = library;
    __projection = this->projection;
    __window = context.window;
    __videoScale = videoScale; 
    __timer = timer; 
    __render_gui_textRenderer = textRenderer; 
    __windowSize = context.windowScale;


    //Set pass less or equal
    glDepthFunc(GL_LEQUAL);


    //Give projection to the curve shader        
    shaders.connectionCurve.use();
    shaders.connectionCurve.setMat4("projection",this->projection); 
    
    //Give projection to the color picker shader !IS NOT USED RN 
    shaders.colorPicker.use();
    shaders.colorPicker.setMat4("projection",this->projection); 
    
    //Use the related shader 
    shaders.buttonShader.use();

    shaders.buttonShader.setInt("properties.invertTheTexture", false);
    shaders.buttonShader.setVec2("properties.txtrScale", glm::vec2(1.f));
    shaders.buttonShader.setFloat("properties.groupOpacity", 1.f);

    //Set the ortho projection     
    shaders.buttonShader.setMat4("projection",this->projection); 

    //Calculate the total screen gap
    float screenGap = videoScale.x - context.windowScale.x; //Use that value to keep panels on the right side
    
    //Calculate the screen gap in 0 - 100 range
    float screenGapPerc = screenGap / videoScale.x * 100.f; 
    
    //TODO Don't call that everyframe (maybe)
    //Rename every library element if their name is doubled 
    library.uniqueNameControl();

    //Render the panels
    renderPanels(videoScale, mouse, timer, textRenderer, painter, library, model, screenGapPerc);

    //Render renaming textbox
    renderRenamingTextbox(videoScale, mouse, timer, textRenderer, painter, library, context);
    
    //Render the nodes
    renderNodes(videoScale,mouse,timer,textRenderer,library,meshNodeScene, model, settings.textureRes, scene);
    
    //Render the dialogs
    renderDialogs(videoScale, mouse, timer, textRenderer, library, meshNodeScene, context, project, model, skybox, settings, box, contextMenus, scene);
    
    //Render the dropper & pick color if mouse left button clicked
    renderDropper(mouse,painter);

    //Render the brush cursor
    if(!this->anyPanelHover && !this->anyDialogActive && !this->anyContextMenuActive && painter.selectedDisplayingModeIndex == 1)
        renderBrushCursor(shaders.circleShader, shaders.buttonShader, painter, mouse, this->projection, context);
    else
        context.window.setCursorVisibility(true);

    //Interactions of the UI elements
    elementInteraction(painter, mouse, library, contextMenus, meshNodeScene, context, videoScale, textRenderer, timer, settings.textureRes, screenGapPerc, model, project, scene, this->materialEditorDialog.appMaterialModifiers);

    frameCounter++;

    if(frameCounter > 1000)
        frameCounter = 0;

    __wasTextureSelectionDialogActive = false;

    
}



//UTILITY FUNCTIONS

static void renderBrushCursor(Shader& circleShader, Shader& buttonShader, Painter& painter, Mouse& mouse, glm::mat4 guiProjection, Context& context){
    /* Use the circle shader */
    circleShader.use();

    /* Set the transform data & the projection */
    circleShader.setMat4("projection", guiProjection);
    circleShader.setVec3("pos", mouse.cursorPos.x, mouse.cursorPos.y, 1);
    circleShader.setVec2("scale", glm::vec2(painter.brushProperties.radius));

    /* Hide the cursor */
    context.window.setCursorVisibility(false);

    /* Render the circle s*/
    glDrawArrays(GL_TRIANGLES, 0, 6);

    /* Clear the depth buffer of the current framebuffers*/
    glClear(GL_DEPTH_BUFFER_BIT);

    /* Use the button shader back */
    buttonShader.use();
}

void UI::renderPanels(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer, Painter &painter, Library &library, Model& model, float screenGapPerc){
    navigationPanel.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    if(navigationPanel.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, library, painter);
    }
    windowPanel.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    if(windowPanel.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, library, painter);
    }
    paintingPanel.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    if(paintingPanel.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, library, painter);
    }
    
    libraryPanelLeft.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    if(libraryPanelLeft.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, library, painter);
    }
    libraryPanelDisplayer.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    if(libraryPanelDisplayer.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, library, painter);
    }
    nodeEditorDisplayer.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    if(nodeEditorDisplayer.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, library, painter);
    }
    selectedTextureDisplayer.render(videoScale,mouse,timer,textRenderer,false);
    if(selectedTextureDisplayer.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, library, painter);
    }
    
    if(nodeEditorDisplayer.hover){
        
        if(mouse.MPressed){
            this->nodePanel.position += mouse.mouseOffset;
            

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


            this->nodePanel.mixVal += (mouse.mouseOffset / 100.f * transPower);
        
        }
        else{
            timer.transition(false, this->nodePanel.mixVal.x, 0.5f);
            timer.transition(false, this->nodePanel.mixVal.y, 0.5f);
            
            this->nodePanel.mixVal.x = sin(this->nodePanel.mixVal.x);
            this->nodePanel.mixVal.y = sin(this->nodePanel.mixVal.y);
        }

    }

    //Prep rendering dots
    shaders.dotsShader.use();

    shaders.dotsShader.setMat4("projection", this->projection);
    shaders.dotsShader.setVec3("pos", nodeEditorDisplayer.resultPos);
    shaders.dotsShader.setVec2("scale", nodeEditorDisplayer.resultScale);
    
    glm::vec2 dotPos = this->nodePanel.position * glm::vec2(2.f) + this->nodePanel.mixVal*10.f;
    shaders.dotsShader.setVec2("dotPos", dotPos);
    shaders.dotsShader.setFloat("scroll", this->nodePanel.scroll);
    
    /* Render the dots */
    glDrawArrays(GL_TRIANGLES, 0, 6);

    /* Clear the depth buffer of the current framebuffers*/
    glClear(GL_DEPTH_BUFFER_BIT);

    shaders.buttonShader.use();
    //

    if(!painter.threeDimensionalMode){
        
        glm::vec2 destScale = glm::vec2(glm::vec2(painter.selectedTexture.getResolution()));
        glm::vec2 prevScale = destScale * this->twoDPaintingSceneScroll;
        float scrVal = mouse.mouseScroll / videoScale.y * 4.f;

        //Calculate the scroll value
        if(this->twoDPaintingSceneScroll > 1.f)
            this->twoDPaintingSceneScroll += scrVal;
        else{
            if(scrVal < 0)
                this->twoDPaintingSceneScroll -= twoDPaintingSceneScroll / 4.f;
            if(scrVal > 0)
                this->twoDPaintingSceneScroll += twoDPaintingSceneScroll / 4.f;
        } 
        if(this->twoDPaintingSceneScroll < 0)
            this->twoDPaintingSceneScroll = 0;

        //Calculate the position value        
        if(mouse.MPressed)
            this->twoDPaintingScenePos += mouse.mouseOffset;
        //(Zoom into the cursor)
        glm::vec2 scaleGap = prevScale - destScale * this->twoDPaintingSceneScroll;
        this->twoDPaintingScenePos += (scaleGap) * (((mouse.cursorPos - glm::vec2(twoDPaintingPanel.sections[0].elements[0].button.resultPos.x + this->twoDPaintingScenePos.x, twoDPaintingPanel.sections[0].elements[0].button.resultPos.y + this->twoDPaintingScenePos.y)) + 0.00001f) / (prevScale + 0.00001f));

        //Render the 2D painting panel
        twoDPaintingPanel.render(videoScale,mouse,timer,textRenderer,false);
        if(twoDPaintingPanel.resizingDone){
            for (size_t i = 0; i < 5; i++)
                this->panelPositioning(screenGapPerc, library, painter);
        }

        //Bottom Barrier
        shaders.buttonShader.setVec3("pos", glm::vec3(twoDPaintingPanel.resultPos.x,   twoDPaintingPanel.resultPos.y + twoDPaintingPanel.resultScale.y + 5000,   1.f)); 
        shaders.buttonShader.setVec2("scale", glm::vec2(5000));
        shaders.buttonShader.setFloat("properties.radius", 0.f); 
        shaders.buttonShader.setInt("outlineExtra" , false); 
        shaders.buttonShader.setVec4("properties.color", glm::vec4(0)); //Invisible
        shaders.buttonShader.setVec3("properties.outline.color", glm::vec4(0)); //Invisible
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //Top Barrier
        shaders.buttonShader.setVec3("pos", glm::vec3(twoDPaintingPanel.resultPos.x,   twoDPaintingPanel.resultPos.y - twoDPaintingPanel.resultScale.y - 5000,   1.f));
        shaders.buttonShader.setVec2("scale", glm::vec2(5000));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //Left Barrier
        shaders.buttonShader.setVec3("pos", glm::vec3(twoDPaintingPanel.resultPos.x  - twoDPaintingPanel.resultScale.x  - 5000,   twoDPaintingPanel.resultPos.y,   1.f));
        shaders.buttonShader.setVec2("scale", glm::vec2(5000));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //Right Barrier
        shaders.buttonShader.setVec3("pos", glm::vec3(twoDPaintingPanel.resultPos.x  + twoDPaintingPanel.resultScale.x  + 5000,   twoDPaintingPanel.resultPos.y,   1.f));
        shaders.buttonShader.setVec2("scale", glm::vec2(5000));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //Render the painting texture
        shaders.twoDPaintingModeAreaShader.use();

        //*Fragment
        shaders.twoDPaintingModeAreaShader.setInt("txtr", 5);
        shaders.twoDPaintingModeAreaShader.setInt("paintingTexture", 6);
        shaders.twoDPaintingModeAreaShader.setInt("depthTexture", 7);
        shaders.twoDPaintingModeAreaShader.setInt("brushModeState", painter.selectedPaintingModeIndex);
        shaders.twoDPaintingModeAreaShader.setFloat("paintingOpacity", painter.brushProperties.opacity / 100.f);
        shaders.twoDPaintingModeAreaShader.setVec3("paintingColor", painter.getSelectedColor().getRGB_normalized());

        //*Vertex
        shaders.twoDPaintingModeAreaShader.setMat4("projectedPosProjection", this->projection);
        shaders.twoDPaintingModeAreaShader.setMat4("projection", this->projection);
        shaders.twoDPaintingModeAreaShader.setVec3("pos", glm::vec3(
                                                                        twoDPaintingPanel.sections[0].elements[0].button.resultPos.x + this->twoDPaintingScenePos.x,
                                                                        twoDPaintingPanel.sections[0].elements[0].button.resultPos.y + this->twoDPaintingScenePos.y,
                                                                        twoDPaintingPanel.sections[0].elements[0].button.resultPos.z
                                                                    )
                                                    );
        shaders.twoDPaintingModeAreaShader.setVec2("scale", destScale * this->twoDPaintingSceneScroll);

        //* Bind the textures
        //painted texture
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, painter.selectedTexture.ID);
        
        //paintingTexture 
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, painter.paintingTexture);
        
        //depthTexture 
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, painter.depthTexture);

        glDrawArrays(GL_TRIANGLES,0,6);

        shaders.buttonShader.use();
    }

    paintingModesPanel.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    if(paintingModesPanel.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, library, painter);
    }
    displayingModesPanel.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    if(displayingModesPanel.resizingDone){
        for (size_t i = 0; i < 5; i++)
            this->panelPositioning(screenGapPerc, library, painter);
    }
}

void UI::renderRenamingTextbox(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer, Painter &painter, Library &library, Context &context){
    
    if(renamingTextBox.active){
        renamingTextBox.render(videoScale,mouse,timer,textRenderer,false,context.window);
        renamingTextBoxClosed = false;
    }
    else{
        //The first frame renamingTextBox is closed 
        if(!renamingTextBoxClosed){
            if(context.window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_RELEASE){
                if(renamingIndices.x == 0 && renamingIndices.y < library.textures.size())
                    library.textures[renamingIndices.y].title = renamingTextBox.text;
                else if(renamingIndices.x == 1 && renamingIndices.y < library.materials.size())
                    library.materials[renamingIndices.y].title = renamingTextBox.text;
                else if(renamingIndices.x == 2 && renamingIndices.y < library.brushes.size())
                    library.brushes[renamingIndices.y].title = renamingTextBox.text;
            }
            else{
                if(renamingIndices.x == 0 && renamingIndices.y < library.textures.size())
                    library.textures[renamingIndices.y].title = lastTitleBeforeRenaming;
                else if(renamingIndices.x == 1 && renamingIndices.y < library.materials.size())
                    library.materials[renamingIndices.y].title = lastTitleBeforeRenaming;
                else if(renamingIndices.x == 2 && renamingIndices.y < library.brushes.size())
                    library.brushes[renamingIndices.y].title = lastTitleBeforeRenaming;
            }

            //Update the library panel
            library.changed = true;
        }
        renamingTextBoxClosed = true;
    }
}

void UI::renderNodes(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer, Library &library,std::vector<Node> &meshNodeScene, Model &model, int textureRes, Scene scene){
    
    for (size_t i = 0; i < meshNodeScene.size(); i++)
    {

        //Update the display texture of the material node
        if(meshNodeScene[i].nodeIndex == MATERIAL_NODE){//Is a material node
            for (size_t matI = 0; matI < library.materials.size(); matI++)
            {
                if(meshNodeScene[i].materialID == library.materials[matI].uniqueID)
                    meshNodeScene[i].nodePanel.sections[0].elements[0].button.texture = Texture(library.materials[matI].displayingTexture);
            }
        }

        meshNodeScene[i].render(videoScale,mouse,timer,textRenderer,nodeEditorDisplayer,meshNodeScene,i,this->nodePanel, library, model, textureRes, scene);
    }
}

void UI::renderDialogs(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer, Library &library,std::vector<Node> &meshNodeScene, Context &context, Project &project, Model& model, Skybox &skybox, AppSettings& settings, Box &box,  std::vector<ContextMenu> &contextMenus, Scene scene){
    if(newProjectDialog.dialogControl.isActive())
        newProjectDialog.render(context.window,colorPalette,mouse,timer,textRenderer,videoScale,project,greetingDialog.dialogControl.active,greetingDialog.startScreen,library,shaders,model,settings.textureRes,meshNodeScene);
    
    if(loadProjectDialog.dialogControl.isActive())
        loadProjectDialog.render(context.window,colorPalette,mouse,timer,textRenderer,videoScale,project,greetingDialog.dialogControl.active,greetingDialog.startScreen,library,shaders,model,settings.textureRes,meshNodeScene);
    
    if(greetingDialog.dialogControl.isActive())
        greetingDialog.render(context.window,colorPalette,mouse,timer,textRenderer,videoScale,newProjectDialog,loadProjectDialog);

    if(displayerDialog.dialogControl.isActive())
        displayerDialog.render(context.window,colorPalette,mouse,timer,textRenderer,library,videoScale,skybox);
    
    if(exportDialog.dialogControl.isActive())
        exportDialog.render(context.window,colorPalette,mouse,timer,textRenderer,videoScale,project,greetingDialog.dialogControl.active,library,shaders,model,materialEditorDialog,meshNodeScene, sphereModel, scene);
    
    if(newTextureDialog.dialogControl.isActive())
        newTextureDialog.render(context.window,colorPalette,mouse,timer,textRenderer,library,videoScale,settings.textureRes);
    
    if(settingsDialog.dialogControl.isActive())
        settingsDialog.render(context.window, colorPalette, mouse, timer, textRenderer, library, videoScale, settings);
    
    if(materialEditorDialog.dialogControl.isActive() && library.materials.size())
        materialEditorDialog.render(videoScale,mouse,timer,textRenderer,textureSelectionDialog,library,library.materials[selectedMaterialIndex],settings.textureRes,box,context,contextMenus,meshNodeScene, model, scene);
    
}

void UI::renderDropper(Mouse &mouse, Painter &painter){
    if(mouse.LClick && dropper.active){
        //Dropper active pick color
        glm::vec3 cursorHoverPixelRGBData;
        //Read the cursor position from the default frame buffer
        glReadPixels(mouse.cursorPos.x,mouse.cursorPos.y,1,1,GL_RGB,GL_FLOAT,&cursorHoverPixelRGBData);
        
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