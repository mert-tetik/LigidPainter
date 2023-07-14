/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

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

void UI::render(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer,Context context,Box box,Library &library,std::vector<Node> &meshNodeScene,
            std::vector<ContextMenu> &contextMenus, AppSettings& settings, Project &project, Painter &painter, Skybox &skybox,Model &model){
    
    
    //Set pass less or equal
    glDepthFunc(GL_LEQUAL);


    //Give projection to the curve shader        
    shaders.connectionCurve.use();
    shaders.connectionCurve.setMat4("projection",projection); 
    
    //Give projection to the color picker shader !IS NOT USED RN 
    shaders.colorPicker.use();
    shaders.colorPicker.setMat4("projection",projection); 
    
    //Use the related shader 
    shaders.buttonShader.use();

    shaders.buttonShader.setFloat("properties.groupOpacity", 1.f);


    //Set the ortho projection     
    shaders.buttonShader.setMat4("projection",projection); 

    //Calculate the total screen gap
    float screenGap = videoScale.x - context.windowScale.x; //Use that value to keep panels on the right side
    
    //Calculate the screen gap in 0 - 100 range
    float screenGapPerc = screenGap / videoScale.x * 100.f; 
    
    //TODO Don't call that everyframe (maybe)
    //Rename every library element if their name is doubled 
    library.uniqueNameControl();

    //Render the panels
    renderPanels(videoScale, mouse, timer, textRenderer, painter, library);

    //Render renaming textbox
    renderRenamingTextbox(videoScale, mouse, timer, textRenderer, painter, library, context);
    
    //Render the nodes
    renderNodes(videoScale,mouse,timer,textRenderer,library,meshNodeScene);
    
    //Render the dialogs
    renderDialogs(videoScale, mouse, timer, textRenderer, library, meshNodeScene, context, project, model, skybox, settings, box, contextMenus);
    
    //Render the dropper & pick color if mouse left button clicked
    renderDropper(mouse,painter);

    //Interactions of the UI elements
    elementInteraction(painter, mouse, library, contextMenus, meshNodeScene, context, videoScale, textRenderer, timer, settings.textureRes, screenGapPerc, model, project);

    frameCounter++;

    if(frameCounter > 1000)
        frameCounter = 0;

    
}



//UTILITY FUNCTIONS


void UI::renderPanels(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer, Painter &painter, Library &library){
    navigationPanel.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    windowPanel.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    paintingPanel.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    libraryPanelLeft.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    libraryPanelDisplayer.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    nodeEditorDisplayer.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    selectedTextureDisplayer.render(videoScale,mouse,timer,textRenderer,false);
    
    if(!painter.threeDimensionalMode){
        twoDPaintingPanel.render(videoScale,mouse,timer,textRenderer,false);

        //Render the painting texture
        shaders.twoDPaintingModeAreaShader.use();

        //*Fragment
        shaders.twoDPaintingModeAreaShader.setInt("txtr", 5);
        shaders.twoDPaintingModeAreaShader.setInt("paintingTexture", 6);
        shaders.twoDPaintingModeAreaShader.setInt("depthTexture", 7);
        shaders.twoDPaintingModeAreaShader.setInt("brushModeState", painter.selectedPaintingModeIndex);
        shaders.twoDPaintingModeAreaShader.setFloat("paintingOpacity", painter.brushProperties.opacity / 100.f);
        shaders.twoDPaintingModeAreaShader.setVec3("paintingColor", painter.getSelectedColor().getRGB_normalized());

        glm::vec2 destScale = glm::vec2(glm::min(twoDPaintingPanel.sections[0].elements[0].button.resultScale.x,twoDPaintingPanel.sections[0].elements[0].button.resultScale.y));

        //*Vertex
        shaders.twoDPaintingModeAreaShader.setMat4("projectedPosProjection", this->projection);
        shaders.twoDPaintingModeAreaShader.setMat4("projection", this->projection);
        shaders.twoDPaintingModeAreaShader.setVec3("pos", twoDPaintingPanel.sections[0].elements[0].button.resultPos);
        shaders.twoDPaintingModeAreaShader.setVec2("scale", destScale);

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
                if(renamingIndices.x == 0)
                    library.textures[renamingIndices.y].title = renamingTextBox.text;
                else if(renamingIndices.x == 1)
                    library.materials[renamingIndices.y].title = renamingTextBox.text;
                else if(renamingIndices.x == 2)
                    library.brushes[renamingIndices.y].title = renamingTextBox.text;
            }
            else{
                if(renamingIndices.x == 0)
                    library.textures[renamingIndices.y].title = lastTitleBeforeRenaming;
                else if(renamingIndices.x == 1)
                    library.materials[renamingIndices.y].title = lastTitleBeforeRenaming;
                else if(renamingIndices.x == 2)
                    library.brushes[renamingIndices.y].title = lastTitleBeforeRenaming;
            }

            //Update the library panel
            library.changed = true;
        }
        renamingTextBoxClosed = true;
    }
}

void UI::renderNodes(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer, Library &library,std::vector<Node> &meshNodeScene){
    
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


        meshNodeScene[i].render(videoScale,mouse,timer,textRenderer,nodeEditorDisplayer,meshNodeScene,i);
    }
}

void UI::renderDialogs(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer, Library &library,std::vector<Node> &meshNodeScene, Context &context, Project &project, Model& model, Skybox &skybox, AppSettings& settings, Box &box,  std::vector<ContextMenu> &contextMenus){
    if(newProjectDialog.dialogControl.isActive())
        newProjectDialog.render(context.window,colorPalette,mouse,timer,textRenderer,videoScale,project,greetingDialog.dialogControl.active,greetingDialog.startScreen,library,shaders,model,settings.textureRes,meshNodeScene);
    
    if(loadProjectDialog.dialogControl.isActive())
        loadProjectDialog.render(context.window,colorPalette,mouse,timer,textRenderer,videoScale,project,greetingDialog.dialogControl.active,greetingDialog.startScreen,library,shaders,model,settings.textureRes,meshNodeScene);
    
    if(greetingDialog.dialogControl.isActive())
        greetingDialog.render(context.window,colorPalette,mouse,timer,textRenderer,videoScale,newProjectDialog,loadProjectDialog);

    if(displayerDialog.dialogControl.isActive())
        displayerDialog.render(context.window,colorPalette,mouse,timer,textRenderer,library,videoScale,skybox);
    
    if(exportDialog.dialogControl.isActive())
        exportDialog.render(context.window,colorPalette,mouse,timer,textRenderer,videoScale,project,greetingDialog.dialogControl.active,library,shaders,model,materialEditorDialog,meshNodeScene, sphereModel);
    
    if(newTextureDialog.dialogControl.isActive())
        newTextureDialog.render(context.window,colorPalette,mouse,timer,textRenderer,library,videoScale,settings.textureRes);
    
    if(settingsDialog.dialogControl.isActive())
        settingsDialog.render(context.window, colorPalette, mouse, timer, textRenderer, library, videoScale, settings);
    
    if(materialEditorDialog.dialogControl.isActive() && library.materials.size())
        materialEditorDialog.render(videoScale,mouse,timer,textRenderer,textureSelectionDialog,library,library.materials[selectedMaterialIndex],settings.textureRes,box,context,contextMenus);
    
    if(textureSelectionDialog.dialogControl.isActive())
        textureSelectionDialog.render(videoScale,mouse,timer,textRenderer,library);
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