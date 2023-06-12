/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

Rendering UI Elements

Currently all the UI elements are rendered to the default framebuffer.
Before rendering them bind 2D square vertex buffer.

Example :
    Box box();
    box.init();

    while(){
        box.bindBuffers();

        ..Render UI 

        box.unbindBuffers();
    }

    Define UI Elements inside of the UI class
    Render them inside of the render function of the UI class

    Position of the vertex is determined that way:
        monitor size / 100 * pos val

    *A panel is consists of sections
    *Sections consists of elements and a header
    *Elements consists of buttons, checkboxes, slide bars and more

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


Element::Element(){}

Element::Element(Button button){
    //Init as button
    this->button = button;
    pos = button.pos;
    scale = button.scale;
    panelOffset = button.panelOffset;
    state = 0;
}
Element::Element(RangeBar rangeBar){
    //Init as range bar
    this->rangeBar = rangeBar;
    pos = rangeBar.pos;
    scale = rangeBar.scale;
    panelOffset = rangeBar.panelOffset;
    state = 1;
}
Element::Element(CheckBox checkBox){
    //Init as range bar
    this->checkBox = checkBox;
    pos = checkBox.pos;
    scale = checkBox.scale;
    panelOffset = checkBox.panelOffset;
    state = 2;
}
Element::Element(ComboBox comboBox,GLFWwindow* window){
    //Init as combo box
    this->comboBox = comboBox;
    this->window = window;
    pos = comboBox.pos;
    scale = comboBox.scale;
    panelOffset = comboBox.panelOffset;
    state = 3;
}
Element::Element(TextBox textBox,GLFWwindow* window){
    //Init as text box
    this->textBox = textBox;
    this->window = window;
    pos = textBox.pos;
    scale = textBox.scale;
    panelOffset = textBox.panelOffset;
    state = 4;
}
Element::Element(Button button,int nodeState){
    //Init as button
    this->button = button;
    pos = button.pos;
    scale = button.scale;
    panelOffset = button.panelOffset;
    state = 0;
    this->nodeState = nodeState;
}
Element::Element(RangeBar rangeBar,int nodeState){
    //Init as range bar
    this->rangeBar = rangeBar;
    pos = rangeBar.pos;
    scale = rangeBar.scale;
    panelOffset = rangeBar.panelOffset;
    state = 1;
    this->nodeState = nodeState;
}

void Element::render(glm::vec2 videoScale,Mouse& mouse, Timer &timer,TextRenderer &textRenderer,bool doMouseTracking){
    if(state == 0){ //Render the button
        button.pos = pos;
        button.scale = scale;
        button.panelOffset = panelOffset;
        button.doMouseTracking = doMouseTracking;
        button.render(videoScale,mouse, timer,textRenderer,doMouseTracking);
    }
    if(state == 1){ //Render the rangeBar
        rangeBar.pos = pos;
        rangeBar.scale = scale;
        rangeBar.panelOffset = panelOffset;
        rangeBar.doMouseTracking = doMouseTracking;
        rangeBar.render(videoScale,mouse, timer,textRenderer,doMouseTracking);
    }
    if(state == 2){ //Render the checkBox
        checkBox.pos = pos;
        checkBox.scale = scale;
        checkBox.panelOffset = panelOffset;
        checkBox.doMouseTracking = doMouseTracking;
        checkBox.render(videoScale,mouse, timer,textRenderer,doMouseTracking);
    }
    if(state == 3){ //Render the comboxBox
        comboBox.pos = pos;
        comboBox.scale = scale;
        comboBox.panelOffset = panelOffset;
        comboBox.doMouseTracking = doMouseTracking;
        comboBox.render(videoScale,mouse, timer,textRenderer,doMouseTracking,window);
    }
    if(state == 4){ //Render the textbox
        textBox.pos = pos;
        textBox.scale = scale;
        textBox.panelOffset = panelOffset;
        textBox.doMouseTracking = doMouseTracking;
        textBox.render(videoScale,mouse, timer,textRenderer,doMouseTracking,window);
    }
}

Section::Section(){}
Section::Section(Element header,std::vector<Element> elements){
    this->header = header;
    this->elements = elements;
}

UI::UI(){} 

void UI::render(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer,Context context,Box box,Library &library,std::vector<Node> &appNodes,std::vector<Node> &nodeScene,
            std::vector<ContextMenu> &contextMenus,int &textureRes, Project &project, Painter &painter,bool &VSync,Skybox &skybox,Model &model){
    
    glDepthFunc(GL_LEQUAL);
    //Give projection to the curve shader        
    shaders.singleCurve.use();
    shaders.singleCurve.setMat4("projection",projection); 
    
    //Give projection to the color picker shader 
    //!Color picker shader is not used rn        
    shaders.colorPicker.use();
    shaders.colorPicker.setMat4("projection",projection); 
    //Use the related shader
    shaders.buttonShader.use();
    
    //Projection is changing in the renderer.render()
    //userInterface.projection = glm::mat4(...)
    shaders.buttonShader.setMat4("projection",projection); 
    float screenGap = videoScale.x - context.windowScale.x; //Use that value to keep panels on the right side
    float screenGapPerc = screenGap / videoScale.x * 100.f; 
    //TODO Don't call that everyframe (maybe)
    library.uniqueNameControl();
    //!Render all the UI elements
    
    anyDialogActive = 
                        textureSelectionDialog.active   || 
                        materialEditorDialog.active     || 
                        anyContextMenuActive            || 
                        displayerDialog.active          || 
                        greetingDialog.active           || 
                        settingsDialog.active           || 
                        newProjectDialog.active         || 
                        newTextureDialog.active         || 
                        loadProjectDialog.active; 

    anyPanelHover = 
                    navigationPanel.hover               ||
                    windowPanel.hover                   ||
                    paintingPanel.hover                 ||
                    libraryPanelLeft.hover              ||
                    libraryPanelDisplayer.hover         ||
                    nodeEditorDisplayer.hover           ||
                    selectedTextureDisplayer.hover      || 
                    paintingModesPanel.hover;

    navigationPanel.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    windowPanel.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    paintingPanel.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    libraryPanelLeft.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    libraryPanelDisplayer.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    nodeEditorDisplayer.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    selectedTextureDisplayer.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
    if(!painter.threeDimensionalMode){
        twoDPaintingPanel.render(videoScale,mouse,timer,textRenderer,!anyDialogActive);
        
        //Render the painting texture
        shaders.tdModelShader.use();
        shaders.tdModelShader.setInt("render2D",0);
        shaders.tdModelShader.setInt("useTransformUniforms",1);
        shaders.tdModelShader.setMat4("orthoProjection",projection);
        shaders.tdModelShader.setInt("returnSingleTxtr",1);
        //Bind the selected texture as albedo
        glActiveTexture(GL_TEXTURE2);
        if(library.textures.size())
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
    Util util;
    if(paintingPanel.sections[2].elements[5].button.hover && mouse.LClick){ //If pressed to remove the brush texture button
        painter.brushTexture = Texture(0);
    }
    if(paintingPanel.sections[2].elements[11].button.hover && mouse.LClick){ //If pressed to export brush file button
        Brush exportBrush
                        (    
                            1.f - paintingPanel.sections[2].elements[0].rangeBar.value/100.f,
                            1.f - paintingPanel.sections[2].elements[3].rangeBar.value/100.f,
                            1.f - paintingPanel.sections[2].elements[1].rangeBar.value/100.f,
                            paintingPanel.sections[2].elements[7].rangeBar.value,
                            1.f - paintingPanel.sections[2].elements[8].rangeBar.value/100.f,
                            1.f - paintingPanel.sections[2].elements[9].rangeBar.value/100.f,
                            paintingPanel.sections[2].elements[6].checkBox.clickState1,
                            paintingPanel.sections[2].elements[2].checkBox.clickState1,
                            "brush_1",
                            painter.brushTexture
                        );
        exportBrush.saveFile("");
    }
    //Update the selected texture
    for (size_t i = 0; i < libraryPanelDisplayer.sections[0].elements.size(); i++) //Check all the texture button elements from the library displayer panel
    {
        if(selectedLibraryElementIndex == 0){ //Textures selected
            if(libraryPanelDisplayer.sections[0].elements[i].button.hover && mouse.LClick){
                if(paintingPanel.sections[2].elements[4].button.clickState1){//If brush texture displayer is pressed
                    painter.brushTexture = libraryPanelDisplayer.sections[0].elements[i].button.texture; //Select a brush texture
                    paintingPanel.sections[2].elements[4].button.clickState1 = false;
                } 
                else
                    painter.selectedTexture = library.textures[i]; //Select the texture 
            } //If any texture button element is pressed
        
            if(library.textures[i].ID == painter.selectedTexture.ID) //Highlight the selected texture
                libraryPanelDisplayer.sections[0].elements[i].button.clickState1 = true;
        }
        if(selectedLibraryElementIndex == 3){ //Models selected
            if(libraryPanelDisplayer.sections[0].elements[i].button.hover && mouse.LClick){
                model = library.TDModels[i]; //Select the model 
            } 
        }
    }
    
    //Check all the library element button is they are pressed
    for (size_t i = 0; i < libraryPanelLeft.sections[0].elements.size(); i++) 
    {
        if(libraryPanelLeft.sections[0].elements[i].button.hover && mouse.LClick){//If any button element is pressed
            if(selectedLibraryElementIndex != i){
                libraryPanelLeft.sections[0].elements[selectedLibraryElementIndex].button.clickState1 = false;
                selectedLibraryElementIndex = i;
                break;
            }
        } 
    }
    elementInteraction(painter,mouse,library,contextMenus,appNodes,nodeScene,context,videoScale,textRenderer,timer,textureRes,screenGapPerc);
    
    selectedTextureDisplayer.sections[0].elements[0].scale.y = selectedTextureDisplayer.scale.y;
    if(library.textures.size())
        selectedTextureDisplayer.sections[0].elements[0].button.texture = painter.selectedTexture;
    twoDPaintingPanel.sections[0].elements[0].scale.y = twoDPaintingPanel.scale.y;
    
    //Render the nodes
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
    
    //Dialogs
    
        
    if(newProjectDialog.active)
        newProjectDialog.render(context.window,colorPalette,mouse,timer,textRenderer,videoScale,project,greetingDialog.active,library,shaders,model);
    
    if(loadProjectDialog.active)
        loadProjectDialog.render(context.window,colorPalette,mouse,timer,textRenderer,videoScale,project,greetingDialog.active,library,shaders,model);
    
    if(greetingDialog.active)
        greetingDialog.render(context.window,colorPalette,mouse,timer,textRenderer,videoScale,newProjectDialog,loadProjectDialog);
    //colorPickerDialog.render(context.window,colorPalette,mouse,timer,textRenderer,videoScale,project);
    
    if(displayerDialog.active)
        displayerDialog.render(context.window,colorPalette,mouse,timer,textRenderer,library,videoScale,skybox);
    
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
    if(frameCounter > 1000)
        frameCounter = 0;
    //!Make sure the execute these after rendering everything
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
    
    //If clicked to the dropper button activate the dropper
    if(paintingPanel.sections[0].elements[3].button.hover && mouse.LClick){
        dropper.active = true;
    }
    frameCounter++;
}
    
    
    
    
    
void UI::elementInteraction(Painter &painter,Mouse &mouse, Library &library,std::vector<ContextMenu> &contextMenus,std::vector<Node> &appNodes,std::vector<Node> &nodeScene,
                        Context &context,glm::vec2 &videoScale,TextRenderer &textRenderer, Timer &timer, int &textureRes,float screenGapPerc){
    //!PAINTING PANEL COLOR BUTTONS
    if(paintingPanel.sections[0].elements[0].button.hover && mouse.LDoubleClick){//Pressed to first color button element
        painter.loadColor1();
    }
    if(paintingPanel.sections[0].elements[1].button.hover && mouse.LDoubleClick){//Pressed to second color button element
        painter.loadColor2();
    }
    if(paintingPanel.sections[0].elements[2].button.hover && mouse.LDoubleClick){//Pressed to third color button element
        painter.loadColor3();
    }
    //Prevent multiple selection and update the painter.selectedColorIndex for colors
    for (size_t i = 0; i < paintingPanel.sections[0].elements.size(); i++)
    {
        if(i == 3) 
            break; //Don't bring the dropper button
        if(paintingPanel.sections[0].elements[i].button.clickState1){ //If a color button is clicked
            if(painter.selectedColorIndex != i){ //If the clicked button is not selected 
                paintingPanel.sections[0].elements[painter.selectedColorIndex].button.clickState1 = false; //Unselect the selected one
                painter.selectedColorIndex = i; //Select the clicked color button
                break; 
            }
        }
    }
    //Update the color values of the color buttons
    paintingPanel.sections[0].elements[0].button.color = glm::vec4(painter.color1.RGB/glm::vec3(255.f),1.f);
    paintingPanel.sections[0].elements[1].button.color = glm::vec4(painter.color2.RGB/glm::vec3(255.f),1.f);
    paintingPanel.sections[0].elements[2].button.color = glm::vec4(painter.color3.RGB/glm::vec3(255.f),1.f);
    
    //Update the brush texture displayer's texture
    paintingPanel.sections[2].elements[4].button.texture = painter.brushTexture;
    //!WINDOW PANEL BUTTONS
    if(windowPanel.sections[0].elements[0].button.hover && mouse.LClick){//Pressed to the 3D painting button of the window panel
        painter.threeDimensionalMode = true;
    }
    else if(windowPanel.sections[0].elements[1].button.hover && mouse.LClick){//Pressed to the 2D painting button of the window panel
        painter.threeDimensionalMode = false;
    }
    else if(windowPanel.sections[0].elements[4].button.hover && mouse.LClick){//Pressed to the settings button of the window panel
        settingsDialog.activate();
    }
    else if(windowPanel.sections[0].elements[3].button.hover && mouse.LClick){//Pressed to the displayer button of the window panel
        displayerDialog.activate();
    }
    //!CONTEXT MENU
    anyContextMenuActive = false; 
    for (size_t i = 0; i < contextMenus.size(); i++)//Check all the contextMenus
    {
        if(contextMenus[i].active) //Set anyContextMenuActive UI class variable 
            anyContextMenuActive = true;
        
        //CONTEXT MENU BUTTONS
        if(i == 0 && selectedLibraryElementIndex == 0 && contextMenus[i].active){ //If texture context menu is active
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && mouse.LClick){//Clicked to rename button
                renamingTextBox.active = true;
                renamingTextBox.pos = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.pos;
                renamingTextBox.pos.y += 2.8f;
                lastTitleBeforeRenaming = library.textures[contextMenus[i].selectedElement].title;
                library.textures[contextMenus[i].selectedElement].title = "";
                renamingTextBox.text = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.text;
                renamingTextBox.activeChar = renamingTextBox.text.size();
                renamingTextBox.activeChar2 = renamingTextBox.activeChar;
                renamingIndices.x = 0;
                renamingIndices.y = contextMenus[i].selectedElement;
            }
            if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && mouse.LClick){//Clicked to duplicate button
                Texture duplicatedTexture;
                duplicatedTexture = library.textures[contextMenus[i].selectedElement];
                duplicatedTexture.ID = library.textures[contextMenus[i].selectedElement].duplicateTexture();
                library.textures.push_back(duplicatedTexture);
            }
            if(contextMenus[i].contextPanel.sections[0].elements[3].button.hover && mouse.LClick){//Clicked to delete button
                library.textures.erase(library.textures.begin() + contextMenus[i].selectedElement);
            }
        }
        
        if(i == 1 && selectedLibraryElementIndex == 1 && contextMenus[i].active){ //If material context menu is active
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && mouse.LClick){//Clicked to edit button
                //Select the material that material editor will edit & show the material editor dialog
                selectedMaterialIndex = contextMenus[i].selectedElement;
                materialEditorDialog.activate();
            }
            if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && mouse.LClick){//Clicked to add to scene button
                //Create the node of the materail an add to the node scene
                Node materialNode = appNodes[0];
                materialNode.barButton.text = library.materials[contextMenus[i].selectedElement].title;
                materialNode.materialID = library.materials[contextMenus[i].selectedElement].ID;
                nodeScene.push_back(materialNode); //Add material node
            }
            if(contextMenus[i].contextPanel.sections[0].elements[2].button.hover && mouse.LClick){//Clicked to rename button
                renamingTextBox.active = true;
                renamingTextBox.pos = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.pos;
                renamingTextBox.pos.y += 2.8f;
                lastTitleBeforeRenaming = library.materials[contextMenus[i].selectedElement].title;
                library.materials[contextMenus[i].selectedElement].title = "";
                renamingTextBox.text = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.text;
                renamingTextBox.activeChar = renamingTextBox.text.size();
                renamingTextBox.activeChar2 = renamingTextBox.activeChar;
                renamingIndices.x = 1;
                renamingIndices.y = contextMenus[i].selectedElement;
            }
            if(contextMenus[i].contextPanel.sections[0].elements[3].button.hover && mouse.LClick){//Clicked to duplicate button
                Material duplicatedMaterial;
                duplicatedMaterial = library.materials[contextMenus[i].selectedElement];
                duplicatedMaterial.ID = materialIDCounter;
                library.materials.push_back(duplicatedMaterial);
                
                materialIDCounter++;
            }
            if(contextMenus[i].contextPanel.sections[0].elements[5].button.hover && mouse.LClick){//Clicked to delete button
                library.materials.erase(library.materials.begin() + contextMenus[i].selectedElement);
            }
        }
        if(i == 2 && selectedLibraryElementIndex == 2 && contextMenus[i].active){ //If brush context menu is active
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && mouse.LClick){//Clicked to use brush button
                paintingPanel.sections[2].elements[0].rangeBar.value        =   library.brushes[contextMenus[i].selectedElement].sizeJitter;
                paintingPanel.sections[2].elements[3].rangeBar.value        =   library.brushes[contextMenus[i].selectedElement].scatter;
                paintingPanel.sections[2].elements[1].rangeBar.value        =   library.brushes[contextMenus[i].selectedElement].fade;
                paintingPanel.sections[2].elements[7].rangeBar.value        =   library.brushes[contextMenus[i].selectedElement].rotation;
                paintingPanel.sections[2].elements[8].rangeBar.value        =   library.brushes[contextMenus[i].selectedElement].rotationJitter;
                paintingPanel.sections[2].elements[9].rangeBar.value        =   library.brushes[contextMenus[i].selectedElement].alphaJitter;
                paintingPanel.sections[2].elements[6].checkBox.clickState1  =   library.brushes[contextMenus[i].selectedElement].individualTexture;
                paintingPanel.sections[2].elements[2].checkBox.clickState1  =   library.brushes[contextMenus[i].selectedElement].sinWavePattern;
            }
            if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && mouse.LClick){//Clicked to apply brush settings
                library.brushes[contextMenus[i].selectedElement].sizeJitter         =   paintingPanel.sections[2].elements[0].rangeBar.value;          
                library.brushes[contextMenus[i].selectedElement].scatter            =   paintingPanel.sections[2].elements[3].rangeBar.value;          
                library.brushes[contextMenus[i].selectedElement].fade               =   paintingPanel.sections[2].elements[1].rangeBar.value;          
                library.brushes[contextMenus[i].selectedElement].rotation           =   paintingPanel.sections[2].elements[7].rangeBar.value;          
                library.brushes[contextMenus[i].selectedElement].rotationJitter     =   paintingPanel.sections[2].elements[8].rangeBar.value;          
                library.brushes[contextMenus[i].selectedElement].alphaJitter        =   paintingPanel.sections[2].elements[9].rangeBar.value;          
                library.brushes[contextMenus[i].selectedElement].individualTexture  =   paintingPanel.sections[2].elements[6].checkBox.clickState1;    
                library.brushes[contextMenus[i].selectedElement].sinWavePattern     =   paintingPanel.sections[2].elements[2].checkBox.clickState1;    
            }
            if(contextMenus[i].contextPanel.sections[0].elements[2].button.hover && mouse.LClick){//Clicked to rename button
                renamingTextBox.active = true;
                renamingTextBox.pos = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.pos;
                renamingTextBox.pos.y += 2.8f;
                lastTitleBeforeRenaming = library.brushes[contextMenus[i].selectedElement].title;
                library.brushes[contextMenus[i].selectedElement].title = "";
                renamingTextBox.text = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.text;
                renamingTextBox.activeChar = renamingTextBox.text.size();
                renamingTextBox.activeChar2 = renamingTextBox.activeChar;
                renamingIndices.x = 2;
                renamingIndices.y = contextMenus[i].selectedElement;
            }
            if(contextMenus[i].contextPanel.sections[0].elements[3].button.hover && mouse.LClick){//Clicked to duplicate button
                library.brushes.push_back(library.brushes[contextMenus[i].selectedElement]);
            }
            if(contextMenus[i].contextPanel.sections[0].elements[5].button.hover && mouse.LClick){//Clicked to delete button
                library.brushes.erase(library.brushes.begin() + contextMenus[i].selectedElement);
            }
        }
        if(i == 3 && contextMenus[i].active){ //If project context menu is active
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && mouse.LClick){
                
            }
        }
        if(i == 4 && contextMenus[i].active){ //If painting context menu is active
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && mouse.LClick){
                
            }
        }
        if(i == 5 && contextMenus[i].active){ //If help context menu is active
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && mouse.LClick){
                websites.ligidTools.open();
            }
            if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && mouse.LClick){
                websites.youTube.open();
            }
        }




        if (   //Conditions to turn any context menu off
                mouse.LClick|| //Mouse left click
                mouse.RClick|| //Mouse right click
                mouse.MClick|| //Mouse middle click
                glfwGetKey(context.window,GLFW_KEY_ESCAPE) == GLFW_PRESS|| //Pressed to escape key 
                glfwGetKey(context.window,GLFW_KEY_ENTER) == GLFW_PRESS //Pressed to enter key
            )
        {
           contextMenus[i].active = false; //Turn the context menu offs
        }
        //Render the context menu if active
        if(contextMenus[i].active){
            contextMenus[i].render(videoScale,mouse,timer,textRenderer);
            if(contextMenus[i].contextPanel.hover == false)
                contextMenus[i].active = false; 
        }
        else{
            contextMenus[i].selectedElement = 0;
        }
    }
    
    //*Show context menu
    //For library panel displayer
    for (size_t i = 0; i < libraryPanelDisplayer.sections[0].elements.size(); i++)
    {
        if(libraryPanelDisplayer.sections[0].elements[i].button.hover && mouse.RClick){ //Right clicked to an element
            if(selectedLibraryElementIndex == 0){//To a texture
                //Show the context menu
                contextMenus[0].active = true;
                contextMenus[0].pos.x = mouse.cursorPos.x / videoScale.x * 100.f;
                contextMenus[0].pos.y = mouse.cursorPos.y / videoScale.y * 100.f + contextMenus[0].contextPanel.scale.y;
                contextMenus[0].pos.z = 0.95f;
                contextMenus[0].selectedElement = i;
                
            }
            if(selectedLibraryElementIndex == 1){//To a material
                //Show the context menu
                contextMenus[1].active = true;
                contextMenus[1].pos.x = mouse.cursorPos.x / videoScale.x * 100.f;
                contextMenus[1].pos.y = mouse.cursorPos.y / videoScale.y * 100.f + contextMenus[1].contextPanel.scale.y;
                contextMenus[1].pos.z = 0.95f;
                contextMenus[1].selectedElement = i;
            }
            if(selectedLibraryElementIndex == 2){//To a brush
                //Show the context menu
                contextMenus[2].active = true;
                contextMenus[2].pos.x = mouse.cursorPos.x / videoScale.x * 100.f;
                contextMenus[2].pos.y = mouse.cursorPos.y / videoScale.y * 100.f + contextMenus[2].contextPanel.scale.y;
                contextMenus[2].pos.z = 0.95f;
                contextMenus[2].selectedElement = i;
            }
        }
    }



    if(navigationPanel.sections[0].elements[0].button.hover && mouse.LClick){ //If pressed to "Project" button in the menu bar (navigation panel)
        //Show the context menu
        contextMenus[3].active = true;
        contextMenus[3].pos.x = mouse.cursorPos.x / videoScale.x * 100.f;
        contextMenus[3].pos.y = mouse.cursorPos.y / videoScale.y * 100.f + contextMenus[3].contextPanel.scale.y;
        contextMenus[3].pos.z = 0.95f;
        contextMenus[3].selectedElement = 0;
    }
    if(navigationPanel.sections[0].elements[1].button.hover && mouse.LClick){ //If pressed to "Painting" button in the menu bar (navigation panel)
        //Show the context menu
        contextMenus[4].active = true;
        contextMenus[4].pos.x = mouse.cursorPos.x / videoScale.x * 100.f;
        contextMenus[4].pos.y = mouse.cursorPos.y / videoScale.y * 100.f + contextMenus[4].contextPanel.scale.y;
        contextMenus[4].pos.z = 0.95f;
        contextMenus[4].selectedElement = 0;
    }
    
    if(navigationPanel.sections[0].elements[2].button.hover && mouse.LClick){ //If pressed to "Help" button in the menu bar (navigation panel)
        //Show the context menu
        contextMenus[5].active = true;
        contextMenus[5].pos.x = mouse.cursorPos.x / videoScale.x * 100.f;
        contextMenus[5].pos.y = mouse.cursorPos.y / videoScale.y * 100.f + contextMenus[5].contextPanel.scale.y;
        contextMenus[5].pos.z = 0.95f;
        contextMenus[5].selectedElement = 0;
    }



    //!LIBRARY PANEL DISPLAYER
    //Update the library displayer panel every frame
    //if(frameCounter % 100 == 0){
    if(true){
        libraryPanelDisplayer.sections.clear(); //Remove all the elements of the library panel displayer
        
        //Create a new section
        Section libSection;
        libSection.header = Element(Button()); //Has no section button
        //Fill the elements of the section using the data in the library structure
        if(selectedLibraryElementIndex == 0){//Update textures
            for (size_t i = 0; i < library.textures.size(); i++)
            {
                //Push texture elements into the section
                libSection.elements.push_back(Element(Button(1,glm::vec2(2,4.f),colorPalette,shaders.buttonShader,library.textures[i].title       , library.textures[i], 0.f,false))) ;
            }
        }
        else if(selectedLibraryElementIndex == 1){ //Update materials
            for (size_t i = 0; i < library.materials.size(); i++)
            {
                //Push texture elements into the section
                libSection.elements.push_back(Element(Button(1,glm::vec2(2,4.f),colorPalette,shaders.buttonShader,library.materials[i].title       , Texture(library.materials[i].displayingTexture), 0.f,false))) ;
            }
        }
        else if(selectedLibraryElementIndex == 2){ //Update materials
            for (size_t i = 0; i < library.brushes.size(); i++)
            {
                //Push texture elements into the section
                libSection.elements.push_back(Element(Button(1,glm::vec2(2,4.f),colorPalette,shaders.buttonShader,library.brushes[i].title       , Texture(library.brushes[i].displayingTexture), 0.f,false))) ;
            }
        }
        else if(selectedLibraryElementIndex == 3){ //Update tdmodels
            for (size_t i = 0; i < library.TDModels.size(); i++)
            {
                //Push texture elements into the section
                libSection.elements.push_back(Element(Button(1,glm::vec2(2,4.f),colorPalette,shaders.buttonShader,library.TDModels[i].title       , Texture(), 0.f,false))) ;
            }
        }
        //Give the section
        libraryPanelDisplayer.sections.push_back(Section(Element(Button()),libSection.elements));
    }
    //Add button from the barButtons in the library displayer panel clicked 
    if(libraryPanelDisplayer.barButtons[0].clickedMixVal == 1.f){
        if(selectedLibraryElementIndex == 0){//Textures
            newTextureDialog.active = true;
        }
        if(selectedLibraryElementIndex == 1){ //Materials
            //Add new material to the library & not the panel
            //Will be displayed right after library panel is updated which happens in every 100 frame
            library.materials.push_back(Material(textureRes,"material_0",materialIDCounter));
            materialIDCounter++;
        }
        if(selectedLibraryElementIndex == 2){ //Brushes
            library.brushes.push_back(
                                        Brush
                                            (    
                                                paintingPanel.sections[2].elements[0].rangeBar.value,
                                                paintingPanel.sections[2].elements[3].rangeBar.value,
                                                paintingPanel.sections[2].elements[1].rangeBar.value,
                                                paintingPanel.sections[2].elements[7].rangeBar.value,
                                                paintingPanel.sections[2].elements[8].rangeBar.value,
                                                paintingPanel.sections[2].elements[9].rangeBar.value,
                                                paintingPanel.sections[2].elements[6].checkBox.clickState1,
                                                paintingPanel.sections[2].elements[2].checkBox.clickState1,
                                                "brush_1",
                                                painter.brushTexture
                                            )
                                    );
            
            library.brushes[library.brushes.size()-1].updateDisplayTexture(shaders.twoDPainting,shaders.buttonShader);
            
        }
        if(selectedLibraryElementIndex == 3){ //3D Models
            
            char const* lFilterPatterns[11] = { "*.obj","*.gltf", "*.fbx", "*.stp", "*.max","*.x3d","*.obj","*.vrml","*.3ds","*.stl","*.dae" };

            char* test = tinyfd_openFileDialog("Select a 3D model","",11, lFilterPatterns,"",false);
            if(test){
                Model tdModel;
                tdModel.loadModel(test,true);
                library.TDModels.push_back(tdModel);
            }
            
        }
    }
    if(libraryPanelDisplayer.barButtons[1].clickedMixVal == 1.f){ //Import button
        if(selectedLibraryElementIndex == 0){//Textures
            //Select Texture
            char const* lFilterPatterns[12] = 
                                            { 
                                                "*.png",                     //(Portable Network Graphics)
                                                "*.jpeg",                    //(Joint Photographic Experts Group)
                                                "*.jpg",                    //(Joint Photographic Experts Group but 3 letters)
                                                "*.bmp",                     //(Bitmap)
                                                "*.gif",                     //(Graphics Interchange Format)
                                                "*.tga",                     //(Truevision Targa)
                                                "*.hdr",                     //(High Dynamic Range)
                                                "*.pic",                     //(Softimage PIC)
                                                "*.pnm",                     //(Portable Any Map)
                                                "*.ppm",                     //(Portable Pixel Map)
                                                "*.pgm",                     //(Portable Gray Map)
                                                "*.pbm"                     //(Portable Bitmap) };
                                            };   
            char* test = tinyfd_openFileDialog("Select a texture","", 12, lFilterPatterns,"",false);
            if(test){
                Texture txtr;
                txtr.load(test);
                library.textures.push_back(txtr);
            }
        }
        if(selectedLibraryElementIndex == 1){ //Materials
            //Select material
            char const* lFilterPatterns[12] = 
                                            { 
                                                "*.lgdmaterial",
                                            };   
            char* test = tinyfd_openFileDialog("Select a material","", 12, lFilterPatterns,"",false);
            
            if(test){
                Material material;
                material.readFile(test,colorPalette,shaders.buttonShader,appTextures);
                library.materials.push_back(material);
            }
        }
        if(selectedLibraryElementIndex == 2){ //Brushes
            library.brushes.push_back(
                                        Brush
                                            (    
                                                paintingPanel.sections[2].elements[0].rangeBar.value,
                                                paintingPanel.sections[2].elements[3].rangeBar.value,
                                                paintingPanel.sections[2].elements[1].rangeBar.value,
                                                paintingPanel.sections[2].elements[7].rangeBar.value,
                                                paintingPanel.sections[2].elements[8].rangeBar.value,
                                                paintingPanel.sections[2].elements[9].rangeBar.value,
                                                paintingPanel.sections[2].elements[6].checkBox.clickState1,
                                                paintingPanel.sections[2].elements[2].checkBox.clickState1,
                                                "brush_1",
                                                painter.brushTexture
                                            )
                                    );
            
            library.brushes[library.brushes.size()-1].updateDisplayTexture(shaders.twoDPainting,shaders.buttonShader);
            
        }
        if(selectedLibraryElementIndex == 3){ //3D Models
            
            char const* lFilterPatterns[11] = { "*.obj","*.gltf", "*.fbx", "*.stp", "*.max","*.x3d","*.obj","*.vrml","*.3ds","*.stl","*.dae" };

            char* test = tinyfd_openFileDialog("Select a 3D model","",11, lFilterPatterns,"",false);
            if(test){
                Model tdModel;
                tdModel.loadModel(test,true);
                library.TDModels.push_back(tdModel);
            }
            
        }
        
    }
    
    
    //!PAINTING MODES PANEL
    for (size_t i = 0; i < paintingModesPanel.sections[0].elements.size(); i++)
    {
        if(paintingModesPanel.sections[0].elements[i].button.clickState1){ //Painting mode pressed
            if(painter.selectedPaintingModeIndex != i){
                paintingModesPanel.sections[0].elements[painter.selectedPaintingModeIndex].button.clickState1 = false;
                painter.selectedPaintingModeIndex = i;
                break;
            }
        }
        if(painter.selectedPaintingModeIndex == i){
            paintingModesPanel.sections[0].elements[painter.selectedPaintingModeIndex].button.clickState1 = true;
        }
    }
    
    //!Positioning the panels
    paintingPanel.pos.x = windowPanel.pos.x - windowPanel.scale.x - paintingPanel.scale.x; //Keep on the left side of the window panel 
    windowPanel.pos.x = 100.f - windowPanel.scale.x - screenGapPerc; //Keep on the right side
    libraryPanelDisplayer.pos.x = libraryPanelLeft.pos.x + libraryPanelLeft.scale.x + libraryPanelDisplayer.scale.x; //Keep on the left side of the window panel 
    
    paintingPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + paintingPanel.scale.y; //Keep beneath the navigation bar
    windowPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + windowPanel.scale.y; //Keep beneath the navigation bar
    libraryPanelLeft.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + windowPanel.scale.y; //Keep beneath the navigation bar
    libraryPanelDisplayer.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + libraryPanelDisplayer.scale.y; //Keep beneath the navigation bar
    libraryPanelDisplayer.scale.y = 50 - navigationPanel.scale.y - nodeEditorDisplayer.scale.y;
    libraryPanelLeft.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + libraryPanelDisplayer.scale.y; //Keep beneath the navigation bar
    libraryPanelLeft.scale.y = 50 - navigationPanel.scale.y - nodeEditorDisplayer.scale.y;
    nodeEditorDisplayer.pos.x = paintingPanel.pos.x - paintingPanel.scale.x - nodeEditorDisplayer.scale.x; //Keep on the left side of the window panel 
    selectedTextureDisplayer.pos.x = libraryPanelDisplayer.pos.x - libraryPanelLeft.pos.x;
    selectedTextureDisplayer.pos.y = nodeEditorDisplayer.pos.y;
    selectedTextureDisplayer.scale.y = nodeEditorDisplayer.scale.y;
    selectedTextureDisplayer.scale.x = libraryPanelDisplayer.scale.x + libraryPanelLeft.scale.x;
    
    nodeEditorDisplayer.scale.x = 50 - screenGapPerc/2.f - selectedTextureDisplayer.scale.x - (paintingPanel.scale.x+windowPanel.scale.x);
    
    twoDPaintingPanel.scale.x = 50 - screenGapPerc/2.f - selectedTextureDisplayer.scale.x - (paintingPanel.scale.x+windowPanel.scale.x);
    twoDPaintingPanel.scale.y = 50 - navigationPanel.scale.y - nodeEditorDisplayer.scale.y;
    twoDPaintingPanel.pos.x = paintingPanel.pos.x - paintingPanel.scale.x - twoDPaintingPanel.scale.x; //Keep on the left side of the window panel 
    twoDPaintingPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + twoDPaintingPanel.scale.y; //Keep beneath the navigation bar
    paintingModesPanel.pos.x = libraryPanelDisplayer.pos.x + libraryPanelDisplayer.scale.x + paintingModesPanel.scale.x + 1;
    paintingModesPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + paintingModesPanel.scale.y;
}