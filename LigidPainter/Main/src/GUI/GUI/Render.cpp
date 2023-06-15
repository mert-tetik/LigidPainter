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
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

#include "tinyfiledialogs.h"


void UI::render(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer,Context context,Box box,Library &library,std::vector<Node> &appNodes,std::vector<Node> &nodeScene,
            std::vector<ContextMenu> &contextMenus,int &textureRes, Project &project, Painter &painter,bool &VSync,Skybox &skybox,Model &model){
    
    //Set pass less or equal
    glDepthFunc(GL_LEQUAL);


    //Give projection to the curve shader        
    shaders.singleCurve.use();
    shaders.singleCurve.setMat4("projection",projection); 
    
    //Give projection to the color picker shader !IS NOT USED RN 
    shaders.colorPicker.use();
    shaders.colorPicker.setMat4("projection",projection); 
    
    //Use the related shader 
    shaders.buttonShader.use();

    shaders.buttonShader.setFloat("groupOpacity", 1.f);


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
    renderPanels(videoScale,mouse,timer,textRenderer,painter,library);

    //Render renaming textbox
    renderRenamingTextbox(videoScale,mouse,timer,textRenderer,painter,library,context);

    //Interactions of the UI elements
    elementInteraction(painter,mouse,library,contextMenus,appNodes,nodeScene,context,videoScale,textRenderer,timer,textureRes,screenGapPerc,model);
    
    //Render the nodes
    renderNodes(videoScale,mouse,timer,textRenderer,library,nodeScene);
    
    //Render the dialogs
    renderDialogs(videoScale,mouse,timer,textRenderer,library,nodeScene,context,project,model,skybox,textureRes,VSync,box);

    //Render the dropper & pick color if mouse left button clicked
    renderDropper(mouse,painter);

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
        shaders.tdModelShader.use();
        shaders.tdModelShader.setInt("render2D",0);
        shaders.tdModelShader.setInt("useTransformUniforms",1);
        shaders.tdModelShader.setMat4("orthoProjection",projection);
        shaders.tdModelShader.setInt("returnSingleTxtr",1);
        //Bind the selected texture as albedo
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D,painter.selectedTexture.ID);
        shaders.tdModelShader.setVec2("scale",glm::vec2(glm::min(twoDPaintingPanel.sections[0].elements[0].button.resultScale.x,twoDPaintingPanel.sections[0].elements[0].button.resultScale.y)));
        shaders.tdModelShader.setVec3("pos",twoDPaintingPanel.sections[0].elements[0].button.resultPos);
        glDrawArrays(GL_TRIANGLES,0,6);
        painter.windowProjection = projection;
        painter.scale2D = glm::vec2(std::min(twoDPaintingPanel.sections[0].elements[0].button.resultScale.x,twoDPaintingPanel.sections[0].elements[0].button.resultScale.y));
        painter.pos2D = twoDPaintingPanel.sections[0].elements[0].button.resultPos;
        shaders.tdModelShader.setInt("returnSingleTxtr",0);
        shaders.tdModelShader.setInt("useTransformUniforms",0);
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
            if(glfwGetKey(context.window,GLFW_KEY_ESCAPE) == GLFW_RELEASE){
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
        }
        renamingTextBoxClosed = true;
    }
}

void UI::renderNodes(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer, Library &library,std::vector<Node> &nodeScene){
    for (size_t i = 0; i < nodeScene.size(); i++)
    {
        //Update the display texture of the material node
        if(nodeScene[i].nodeIndex == 1){//Is a material node
            for (size_t matI = 0; matI < library.materials.size(); matI++)
            {
                if(nodeScene[i].materialID == library.materials[matI].ID)
                    nodeScene[i].nodePanel.sections[0].elements[0].button.texture = Texture(library.materials[matI].displayingTexture);
            }
        }
        nodeScene[i].render(videoScale,mouse,timer,textRenderer,nodeEditorDisplayer,nodeScene,i);
    }
}

void UI::renderDialogs(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer, Library &library,std::vector<Node> &nodeScene, Context &context, Project &project, Model& model, Skybox &skybox, int &textureRes, bool &VSync, Box &box){
    if(newProjectDialog.dialogControl.isActive())
        newProjectDialog.render(context.window,colorPalette,mouse,timer,textRenderer,videoScale,project,greetingDialog.dialogControl.active,library,shaders,model);
    
    if(loadProjectDialog.dialogControl.isActive())
        loadProjectDialog.render(context.window,colorPalette,mouse,timer,textRenderer,videoScale,project,greetingDialog.dialogControl.active,library,shaders,model);
    
    if(greetingDialog.dialogControl.isActive())
        greetingDialog.render(context.window,colorPalette,mouse,timer,textRenderer,videoScale,newProjectDialog,loadProjectDialog);

    if(displayerDialog.active)
        displayerDialog.render(context.window,colorPalette,mouse,timer,textRenderer,library,videoScale,skybox);
    
    if(exportDialog.active)
        exportDialog.render(context.window,colorPalette,mouse,timer,textRenderer,videoScale,project,greetingDialog.dialogControl.active,library,shaders,model,materialEditorDialog,nodeScene);
    
    if(newTextureDialog.active)
        newTextureDialog.render(context.window,colorPalette,mouse,timer,textRenderer,library,videoScale,textureRes);
    
    if(settingsDialog.active)
        settingsDialog.render(context.window,colorPalette,mouse,timer,textRenderer,library,videoScale,textureRes,VSync);
    
    if(materialEditorDialog.active && library.materials.size()){
        if(glfwGetKey(context.window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
            materialEditorDialog.deactivate(textureSelectionDialog);
        materialEditorDialog.render(videoScale,mouse,timer,textRenderer,textureSelectionDialog,library,library.materials[selectedMaterialIndex],textureRes,box,context);
    }
    if(textureSelectionDialog.active)
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