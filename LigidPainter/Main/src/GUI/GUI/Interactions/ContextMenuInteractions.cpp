/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/


#include<glad/glad.h>

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
#include <filesystem>

void UI::contextMenuInteraction(std::vector<ContextMenu> &contextMenus, Mouse &mouse , Library &library,
                                std::vector<Node> &meshNodeScene,Context &context,glm::vec2 videoScale,Timer &timer,TextRenderer &textRenderer,
                                Project& project, int &textureRes, Painter &painter){
    anyContextMenuActive = false; 
    for (size_t i = 0; i < contextMenus.size(); i++)//Check all the contextMenus
    {
        if(contextMenus[i].dialogControl.isActive()) //Set anyContextMenuActive UI class variable 
            anyContextMenuActive = true;
        
        //CONTEXT MENU BUTTONS
        if(i == 0 && library.selectedElementIndex == 0 && contextMenus[i].dialogControl.isActive() && library.textures.size()){ //If texture context menu is active
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && mouse.LClick){//Clicked to rename button
                renamingTextBox.active = true;
                library.changed = true;
                renamingTextBox.pos = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.pos;
                renamingTextBox.pos.y += 2.8f;
                lastTitleBeforeRenaming = library.textures[contextMenus[i].selectedElement].title;
                library.textures[contextMenus[i].selectedElement].title = "";
                renamingTextBox.text = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.text;
                renamingTextBox.activeChar = renamingTextBox.text.size()-1;
                renamingTextBox.activeChar2 = renamingTextBox.activeChar;
                renamingIndices.x = 0;
                renamingIndices.y = contextMenus[i].selectedElement;
            }
            if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && mouse.LClick){//Clicked to duplicate button
                Texture duplicatedTexture;
                duplicatedTexture = library.textures[contextMenus[i].selectedElement];
                duplicatedTexture.ID = library.textures[contextMenus[i].selectedElement].duplicateTexture();
                library.addTexture(duplicatedTexture);
            }
            if(contextMenus[i].contextPanel.sections[0].elements[3].button.hover && mouse.LClick){//Clicked to delete button
                library.eraseTexture(contextMenus[i].selectedElement);
            }
        }
        
        if(i == 1 && library.selectedElementIndex == 1 && contextMenus[i].dialogControl.isActive()){ //If material context menu is active
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && mouse.LClick){//Clicked to edit button
                //Select the material that material editor will edit & show the material editor dialog
                selectedMaterialIndex = contextMenus[i].selectedElement;
                materialEditorDialog.activate();
            }
            if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && mouse.LClick){//Clicked to add to scene button
                //Create the node of the materail an add to the node scene
                Node materialNode = Node(
                                            MATERIAL_NODE, 
                                            library.materials[contextMenus[i].selectedElement].uniqueID,
                                            shaders.buttonShader,
                                            shaders.connectionCurve,
                                            colorPalette,
                                            appTextures,
                                            videoScale
                                        );
                materialNode.barButton.text = library.materials[contextMenus[i].selectedElement].title;
                meshNodeScene.push_back(materialNode); //Add material node
            }
            if(contextMenus[i].contextPanel.sections[0].elements[2].button.hover && mouse.LClick){//Clicked to rename button
                renamingTextBox.active = true;
                library.changed = true;
                renamingTextBox.pos = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.pos;
                renamingTextBox.pos.y += 2.8f;
                lastTitleBeforeRenaming = library.materials[contextMenus[i].selectedElement].title;
                library.materials[contextMenus[i].selectedElement].title = "";
                renamingTextBox.text = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.text;
                renamingTextBox.activeChar = renamingTextBox.text.size()-1;
                renamingTextBox.activeChar2 = renamingTextBox.activeChar;
                renamingIndices.x = 1;
                renamingIndices.y = contextMenus[i].selectedElement;
            }
            if(contextMenus[i].contextPanel.sections[0].elements[3].button.hover && mouse.LClick){//Clicked to duplicate button
                Material duplicatedMaterial = library.materials[contextMenus[i].selectedElement].duplicateMaterial(textureRes);
                library.addMaterial(duplicatedMaterial);
            }
            if(contextMenus[i].contextPanel.sections[0].elements[5].button.hover && mouse.LClick){//Clicked to delete button
                
                //Delete the nodes using same material
                for (int nodeI = 0; nodeI < meshNodeScene.size(); nodeI++)
                {
                    if(meshNodeScene[nodeI].materialID == library.materials[contextMenus[i].selectedElement].uniqueID && meshNodeScene[nodeI].nodeIndex == MATERIAL_NODE){
                        UTIL::deleteNode(meshNodeScene, nodeI);
                        nodeI--;
                    }
                }
                
                //Delete the material
                library.eraseMaterial(contextMenus[i].selectedElement);
            }
        }
        if(i == 2 && library.selectedElementIndex == 2 && contextMenus[i].dialogControl.isActive()){ //If brush context menu is active
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && mouse.LClick){//Clicked to use brush button
                
                library.brushes[contextMenus[i].selectedElement].useBrush(paintingPanel);

            }
            if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && mouse.LClick){//Clicked to apply brush settings

                library.brushes[contextMenus[i].selectedElement].applyToBrush(paintingPanel, shaders);

            }
            if(contextMenus[i].contextPanel.sections[0].elements[2].button.hover && mouse.LClick){//Clicked to rename button
                renamingTextBox.active = true;
                library.changed = true;
                renamingTextBox.pos = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.pos;
                renamingTextBox.pos.y += 2.8f;
                lastTitleBeforeRenaming = library.brushes[contextMenus[i].selectedElement].title;
                library.brushes[contextMenus[i].selectedElement].title = "";
                renamingTextBox.text = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.text;
                renamingTextBox.activeChar = renamingTextBox.text.size()-1;
                renamingTextBox.activeChar2 = renamingTextBox.activeChar;
                renamingIndices.x = 2;
                renamingIndices.y = contextMenus[i].selectedElement;
            }
            if(contextMenus[i].contextPanel.sections[0].elements[3].button.hover && mouse.LClick){//Clicked to duplicate button
                library.addBrush(library.brushes[contextMenus[i].selectedElement]);
            }
            if(contextMenus[i].contextPanel.sections[0].elements[5].button.hover && mouse.LClick){//Clicked to delete button
                library.eraseBrush(contextMenus[i].selectedElement);
            }
        }
        if(i == 3 && contextMenus[i].dialogControl.isActive()){ //If project context menu is active
            //Save
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && mouse.LClick){
                project.updateProject(library,meshNodeScene,textureRes);
            }
            
            //Save as
            if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && mouse.LClick){
                project.updateProject(library,meshNodeScene,textureRes);
                project.duplicateFolder("");
            }
            
            //Create new
            if(contextMenus[i].contextPanel.sections[0].elements[2].button.hover && mouse.LClick){
                newProjectDialog.dialogControl.activate();
            }
            
            //Load new
            if(contextMenus[i].contextPanel.sections[0].elements[3].button.hover && mouse.LClick){
                loadProjectDialog.dialogControl.activate();
            }
            
            //Copy path
            if(contextMenus[i].contextPanel.sections[0].elements[4].button.hover && mouse.LClick){
                project.copyTheProjectPathToTheClipboard(context.window);
            }

            //Open in file explorer
            if(contextMenus[i].contextPanel.sections[0].elements[5].button.hover && mouse.LClick){
                ;
                UTIL::openInFileExplorer(std::filesystem::absolute(project.folderPath).string().c_str());
            }
        }
        if(i == 4 && contextMenus[i].dialogControl.isActive()){ //If painting context menu is active
            //Undo
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && mouse.LClick){
                painter.selectedTexture.readTMP();
            }
            
            //Redo
            if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && mouse.LClick){

            }
            
        }
        if(i == 5 && contextMenus[i].dialogControl.isActive()){ //If help context menu is active
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && mouse.LClick){
                websites.ligidTools.open();
            }
            if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && mouse.LClick){
                websites.youTube.open();
            }
        }
        
        if(i == 7 && contextMenus[i].dialogControl.isActive()){ //If node context menu is active
            
            //Delete the node
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && mouse.LClick && contextMenus[i].selectedElement){

                ;
                UTIL::deleteNode(meshNodeScene,contextMenus[i].selectedElement);                

            }
        
        }


        if (   //Conditions to turn any context menu off
                mouse.LClick|| //Mouse left click
                mouse.RClick|| //Mouse right click
                mouse.MClick|| //Mouse middle click
                context.window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS|| //Pressed to escape key 
                context.window.isKeyPressed(LIGIDGL_KEY_ENTER) == LIGIDGL_PRESS //Pressed to enter key
            )
        {
           contextMenus[i].dialogControl.unActivate(); //Turn the context menu offs
        }
        //Render the context menu if active
        if(contextMenus[i].dialogControl.isActive()){
            contextMenus[i].render(videoScale,mouse,timer,textRenderer);
            if(contextMenus[i].contextPanel.hover == false)
                contextMenus[i].dialogControl.unActivate(); 
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
            if(library.selectedElementIndex == 0){//To a texture
                //Show the context menu
                contextMenus[0].dialogControl.activate();
                contextMenus[0].pos.x = mouse.cursorPos.x / videoScale.x * 100.f;
                contextMenus[0].pos.y = mouse.cursorPos.y / videoScale.y * 100.f + contextMenus[0].contextPanel.scale.y;
                contextMenus[0].pos.z = 0.95f;
                contextMenus[0].selectedElement = i;
                
            }
            if(library.selectedElementIndex == 1){//To a material
                //Show the context menu
                contextMenus[1].dialogControl.activate();
                contextMenus[1].pos.x = mouse.cursorPos.x / videoScale.x * 100.f;
                contextMenus[1].pos.y = mouse.cursorPos.y / videoScale.y * 100.f + contextMenus[1].contextPanel.scale.y;
                contextMenus[1].pos.z = 0.95f;
                contextMenus[1].selectedElement = i;
            }
            if(library.selectedElementIndex == 2){//To a brush
                //Show the context menu
                contextMenus[2].dialogControl.activate();
                contextMenus[2].pos.x = mouse.cursorPos.x / videoScale.x * 100.f;
                contextMenus[2].pos.y = mouse.cursorPos.y / videoScale.y * 100.f + contextMenus[2].contextPanel.scale.y;
                contextMenus[2].pos.z = 0.95f;
                contextMenus[2].selectedElement = i;
            }
        }
    }
    if(navigationPanel.sections[0].elements[1].button.hover && mouse.LClick){ //If pressed to "Project" button in the menu bar (navigation panel)
        //Show the context menu
        contextMenus[3].dialogControl.activate();
        contextMenus[3].pos.x = mouse.cursorPos.x / videoScale.x * 100.f;
        contextMenus[3].pos.y = mouse.cursorPos.y / videoScale.y * 100.f + contextMenus[3].contextPanel.scale.y;
        contextMenus[3].pos.z = 0.95f;
        contextMenus[3].selectedElement = 0;
    }
    if(navigationPanel.sections[0].elements[2].button.hover && mouse.LClick){ //If pressed to "Painting" button in the menu bar (navigation panel)
        //Show the context menu
        contextMenus[4].dialogControl.activate();
        contextMenus[4].pos.x = mouse.cursorPos.x / videoScale.x * 100.f;
        contextMenus[4].pos.y = mouse.cursorPos.y / videoScale.y * 100.f + contextMenus[4].contextPanel.scale.y;
        contextMenus[4].pos.z = 0.95f;
        contextMenus[4].selectedElement = 0;
    }
    
    if(navigationPanel.sections[0].elements[3].button.hover && mouse.LClick){ //If pressed to "Help" button in the menu bar (navigation panel)
        //Show the context menu
        contextMenus[5].dialogControl.activate();
        contextMenus[5].pos.x = mouse.cursorPos.x / videoScale.x * 100.f;
        contextMenus[5].pos.y = mouse.cursorPos.y / videoScale.y * 100.f + contextMenus[5].contextPanel.scale.y;
        contextMenus[5].pos.z = 0.95f;
        contextMenus[5].selectedElement = 0;
    }
    
    if(materialEditorDialog.layerPanel.sections.size()){
        for (size_t i = 0; i < materialEditorDialog.layerPanel.sections[0].elements.size(); i++)
        {
            if(materialEditorDialog.layerPanel.sections[0].elements[i].button.hover && mouse.RClick && materialEditorDialog.dialogControl.isActive()){

                contextMenus[6].dialogControl.activate();
                contextMenus[6].pos.x = mouse.cursorPos.x / videoScale.x * 100.f;
                contextMenus[6].pos.y = mouse.cursorPos.y / videoScale.y * 100.f + contextMenus[6].contextPanel.scale.y;
                contextMenus[6].pos.z = 0.95f;
                contextMenus[6].selectedElement = i;

            }
        }
    }

    for (size_t i = 0; i < meshNodeScene.size(); i++)
    {
        if((meshNodeScene[i].nodePanel.hover || meshNodeScene[i].barButton.hover) && mouse.RClick && !meshNodeScene[i].cursorOnBarriers){
            contextMenus[7].dialogControl.activate();
            contextMenus[7].pos.x = mouse.cursorPos.x / videoScale.x * 100.f;
            contextMenus[7].pos.y = mouse.cursorPos.y / videoScale.y * 100.f + contextMenus[7].contextPanel.scale.y;
            contextMenus[7].pos.z = 0.95f;
            contextMenus[7].selectedElement = i;
        }
    }
    
}