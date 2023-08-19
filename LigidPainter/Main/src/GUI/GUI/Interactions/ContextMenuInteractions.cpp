/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

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
#include <filesystem>

void UI::contextMenuInteraction(std::vector<ContextMenu> &contextMenus,  
                                Timer &timer,TextRenderer &textRenderer,
                                Project& project, Painter &painter){

    anyContextMenuActive = false; 
    for (size_t i = 0; i < contextMenus.size(); i++)//Check all the contextMenus
    {
        if(contextMenus[i].dialogControl.isActive()) //Set anyContextMenuActive UI class variable 
            anyContextMenuActive = true;
        
        //CONTEXT MENU BUTTONS
        if(i == 0 && Library::getSelectedElementIndex() == 0 && contextMenus[i].dialogControl.isActive() && Library::getTextureArraySize()){ //If texture context menu is active
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && *Mouse::LClick()){//Clicked to rename button
                renamingTextBox.active = true;
                Library::setChanged(true);
                renamingTextBox.pos = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.pos;
                renamingTextBox.pos.y += 2.8f;
                lastTitleBeforeRenaming = Library::getTexture(contextMenus[i].selectedElement)->title;
                Library::getTexture(contextMenus[i].selectedElement)->title = "";
                renamingTextBox.text = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.text;
                renamingTextBox.activeChar = renamingTextBox.text.size()-1;
                renamingTextBox.activeChar2 = renamingTextBox.activeChar;
                renamingIndices.x = 0;
                renamingIndices.y = contextMenus[i].selectedElement;
            }
            if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && *Mouse::LClick()){//Clicked to duplicate button
                Texture duplicatedTexture;
                duplicatedTexture = *Library::getTexture(contextMenus[i].selectedElement);
                duplicatedTexture.ID = Library::getTexture(contextMenus[i].selectedElement)->duplicateTexture();
                Library::addTexture(duplicatedTexture);
            }
            if(contextMenus[i].contextPanel.sections[0].elements[3].button.hover && *Mouse::LClick()){//Clicked to delete button
                Library::eraseTexture(contextMenus[i].selectedElement);
            }
        }
        
        if(i == 1 && Library::getSelectedElementIndex() == 1 && contextMenus[i].dialogControl.isActive()){ //If material context menu is active
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && *Mouse::LClick()){//Clicked to edit button
                //Select the material that material editor will edit & show the material editor dialog
                selectedMaterialIndex = contextMenus[i].selectedElement;
                materialEditorDialog.activate();
            }
            if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && *Mouse::LClick()){//Clicked to add to scene button
                //Create the node of the materail an add to the node scene
                Node materialNode = Node(
                                            MATERIAL_NODE, 
                                            Library::getMaterial(contextMenus[i].selectedElement)->uniqueID,
                                            colorPalette
                                        );
                materialNode.barButton.text = Library::getMaterial(contextMenus[i].selectedElement)->title;
                NodeScene::addNode(materialNode); //Add material node
            }
            if(contextMenus[i].contextPanel.sections[0].elements[2].button.hover && *Mouse::LClick()){//Clicked to rename button
                renamingTextBox.active = true;
                Library::setChanged(true);
                renamingTextBox.pos = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.pos;
                renamingTextBox.pos.y += 2.8f;
                lastTitleBeforeRenaming = Library::getMaterial(contextMenus[i].selectedElement)->title;
                Library::getMaterial(contextMenus[i].selectedElement)->title = "";
                renamingTextBox.text = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.text;
                renamingTextBox.activeChar = renamingTextBox.text.size()-1;
                renamingTextBox.activeChar2 = renamingTextBox.activeChar;
                renamingIndices.x = 1;
                renamingIndices.y = contextMenus[i].selectedElement;
            }
            if(contextMenus[i].contextPanel.sections[0].elements[3].button.hover && *Mouse::LClick()){//Clicked to duplicate button
                Material duplicatedMaterial = Library::getMaterial(contextMenus[i].selectedElement)->duplicateMaterial();
                Library::addMaterial(duplicatedMaterial);
            }
            if(contextMenus[i].contextPanel.sections[0].elements[5].button.hover && *Mouse::LClick()){//Clicked to delete button
                
                //Delete the nodes using same material
                for (int nodeI = 0; nodeI < NodeScene::getArraySize(); nodeI++)
                {
                    if(NodeScene::getNode(nodeI)->materialID == Library::getMaterial(contextMenus[i].selectedElement)->uniqueID && NodeScene::getNode(nodeI)->nodeIndex == MATERIAL_NODE){
                        NodeScene::deleteNode( nodeI);
                        nodeI--;
                    }
                }
                
                //Delete the material
                Library::eraseMaterial(contextMenus[i].selectedElement);
            }
            if(contextMenus[i].contextPanel.sections[0].elements[6].button.hover && *Mouse::LClick()){//Clicked to export button
                std::string exportingPath = showFileSystemObjectSelectionDialog("Export the lgdmaterial file", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MATERIAL, false,FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_EXPORT_FILE);
                if(exportingPath.size())
                    FileHandler::writeLGDMATERIALFile(exportingPath, *Library::getMaterial(contextMenus[i].selectedElement));
            }
        }
        if(i == 2 && Library::getSelectedElementIndex() == 2 && contextMenus[i].dialogControl.isActive()){ //If brush context menu is active
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && *Mouse::LClick()){//Clicked to use brush button
                
                Library::getBrush(contextMenus[i].selectedElement)->useBrush(paintingPanel);

            }
            if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && *Mouse::LClick()){//Clicked to apply brush settings

                Library::getBrush(contextMenus[i].selectedElement)->applyToBrush(paintingPanel);

            }
            if(contextMenus[i].contextPanel.sections[0].elements[2].button.hover && *Mouse::LClick()){//Clicked to rename button
                renamingTextBox.active = true;
                Library::setChanged(true);
                renamingTextBox.pos = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.pos;
                renamingTextBox.pos.y += 2.8f;
                lastTitleBeforeRenaming = Library::getBrush(contextMenus[i].selectedElement)->title;
                Library::getBrush(contextMenus[i].selectedElement)->title = "";
                renamingTextBox.text = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.text;
                renamingTextBox.activeChar = renamingTextBox.text.size()-1;
                renamingTextBox.activeChar2 = renamingTextBox.activeChar;
                renamingIndices.x = 2;
                renamingIndices.y = contextMenus[i].selectedElement;
            }
            if(contextMenus[i].contextPanel.sections[0].elements[3].button.hover && *Mouse::LClick()){//Clicked to duplicate button
                Library::addBrush(*Library::getBrush(contextMenus[i].selectedElement));
            }
            if(contextMenus[i].contextPanel.sections[0].elements[5].button.hover && *Mouse::LClick()){//Clicked to delete button
                Library::eraseBrush(contextMenus[i].selectedElement);
            }
        }
        if(i == 3 && contextMenus[i].dialogControl.isActive()){ //If project context menu is active
            //Save
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && *Mouse::LClick()){
                project.updateProject();
            }
            
            //Save as
            if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && *Mouse::LClick()){
                project.updateProject();
                project.duplicateFolder("");
            }
            
            //Create new
            if(contextMenus[i].contextPanel.sections[0].elements[2].button.hover && *Mouse::LClick()){
                newProjectDialog.dialogControl.activate();
            }
            
            //Load new
            if(contextMenus[i].contextPanel.sections[0].elements[3].button.hover && *Mouse::LClick()){
                loadProjectDialog.dialogControl.activate();
            }
            
            //Copy path
            if(contextMenus[i].contextPanel.sections[0].elements[4].button.hover && *Mouse::LClick()){
                project.copyTheProjectPathToTheClipboard();
            }

            //Open in file explorer
            if(contextMenus[i].contextPanel.sections[0].elements[5].button.hover && *Mouse::LClick()){
                UTIL::openInFileExplorer(std::filesystem::absolute(project.folderPath).string().c_str());
            }
        }
        if(i == 4 && contextMenus[i].dialogControl.isActive()){ //If painting context menu is active
            //TODO REMOVE THE PAINTING CONTEXT MENU            
        }
        if(i == 5 && contextMenus[i].dialogControl.isActive()){ //If help context menu is active
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && *Mouse::LClick()){
                websites.ligidTools.open();
            }
            if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && *Mouse::LClick()){
                websites.youTube.open();
            }
        }
        
        if(i == 7 && contextMenus[i].dialogControl.isActive()){ //If node context menu is active
            //Delete the node
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && *Mouse::LClick() && contextMenus[i].selectedElement){
                NodeScene::deleteNode(contextMenus[i].selectedElement);                
            }
        }

        if(i == 9 && contextMenus[i].dialogControl.isActive()){//If node scene context menu is active
            //Add material ID node button pressed
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && *Mouse::LClick()){
                NodeScene::addNode(Node(MATERIAL_ID_NODE, 0, colorPalette));
                NodeScene::getNode(NodeScene::getArraySize()-1)->nodePanel.pos = NodeScene::getNode(0)->nodePanel.pos;
            }
            //Add material mask node button pressed
            else if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && *Mouse::LClick()){
                NodeScene::addNode(Node(MATERIAL_MASK_NODE, 0, colorPalette));
                NodeScene::getNode(NodeScene::getArraySize()-1)->nodePanel.pos = NodeScene::getNode(0)->nodePanel.pos;
            }
        } 


        if (   //Conditions to turn any context menu off
                *Mouse::LClick()|| //Mouse left click
                *Mouse::RClick()|| //Mouse right click
                *Mouse::MClick()|| //Mouse middle click
                getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS|| //Pressed to escape key 
                getContext()->window.isKeyPressed(LIGIDGL_KEY_ENTER) == LIGIDGL_PRESS //Pressed to enter key
            )
        {
           contextMenus[i].dialogControl.unActivate(); //Turn the context menu offs
        }
        //Render the context menu if active
        if(contextMenus[i].dialogControl.isActive()){
            contextMenus[i].render(timer,textRenderer);
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
        if(libraryPanelDisplayer.sections[0].elements[i].button.hover && *Mouse::RClick()){ //Right clicked to an element
            if(Library::getSelectedElementIndex() == 0){//To a texture
                //Show the context menu
                contextMenus[0].dialogControl.activate();
                contextMenus[0].pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
                contextMenus[0].pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + contextMenus[0].contextPanel.scale.y;
                contextMenus[0].pos.z = 0.95f;
                contextMenus[0].selectedElement = i;
                
            }
            if(Library::getSelectedElementIndex() == 1){//To a material
                //Show the context menu
                contextMenus[1].dialogControl.activate();
                contextMenus[1].pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
                contextMenus[1].pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + contextMenus[1].contextPanel.scale.y;
                contextMenus[1].pos.z = 0.95f;
                contextMenus[1].selectedElement = i;
            }
            if(Library::getSelectedElementIndex() == 2){//To a brush
                //Show the context menu
                contextMenus[2].dialogControl.activate();
                contextMenus[2].pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
                contextMenus[2].pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + contextMenus[2].contextPanel.scale.y;
                contextMenus[2].pos.z = 0.95f;
                contextMenus[2].selectedElement = i;
            }
        }
    }
    if(navigationPanel.sections[0].elements[1].button.hover && *Mouse::LClick()){ //If pressed to "Project" button in the menu bar (navigation panel)
        //Show the context menu
        contextMenus[3].dialogControl.activate();
        contextMenus[3].pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
        contextMenus[3].pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + contextMenus[3].contextPanel.scale.y;
        contextMenus[3].pos.z = 0.95f;
        contextMenus[3].selectedElement = 0;
    }
    if(navigationPanel.sections[0].elements[2].button.hover && *Mouse::LClick()){ //If pressed to "Painting" button in the menu bar (navigation panel)
        //Show the context menu
        contextMenus[4].dialogControl.activate();
        contextMenus[4].pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
        contextMenus[4].pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + contextMenus[4].contextPanel.scale.y;
        contextMenus[4].pos.z = 0.95f;
        contextMenus[4].selectedElement = 0;
    }
    
    if(navigationPanel.sections[0].elements[3].button.hover && *Mouse::LClick()){ //If pressed to "Help" button in the menu bar (navigation panel)
        //Show the context menu
        contextMenus[5].dialogControl.activate();
        contextMenus[5].pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
        contextMenus[5].pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + contextMenus[5].contextPanel.scale.y;
        contextMenus[5].pos.z = 0.95f;
        contextMenus[5].selectedElement = 0;
    }
    
    if(materialEditorDialog.layerPanel.sections.size()){
        for (size_t i = 0; i < materialEditorDialog.layerPanel.sections[0].elements.size(); i++)
        {
            if(materialEditorDialog.layerPanel.sections[0].elements[i].button.hover && *Mouse::RClick() && materialEditorDialog.dialogControl.isActive()){

                contextMenus[6].dialogControl.activate();
                contextMenus[6].pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
                contextMenus[6].pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + contextMenus[6].contextPanel.scale.y;
                contextMenus[6].pos.z = 0.95f;
                contextMenus[6].selectedElement = i;

            }
        }
    }

    if(materialEditorDialog.layerPanel.barButtons[0].clicked && materialEditorDialog.dialogControl.isActive()){
        contextMenus[8].dialogControl.activate();
        contextMenus[8].pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
        contextMenus[8].pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + contextMenus[6].contextPanel.scale.y;
        contextMenus[8].pos.z = 0.95f;
    }

    bool anyNodeHover = false;
    for (size_t i = 0; i < NodeScene::getArraySize(); i++)
    {
        if((NodeScene::getNode(i)->nodePanel.hover || NodeScene::getNode(i)->barButton.hover))
            anyNodeHover = true;

        if((NodeScene::getNode(i)->nodePanel.hover || NodeScene::getNode(i)->barButton.hover) && *Mouse::RClick() && !NodeScene::getNode(i)->cursorOnBarriers){
            contextMenus[7].dialogControl.activate();
            contextMenus[7].pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
            contextMenus[7].pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + contextMenus[7].contextPanel.scale.y;
            contextMenus[7].pos.z = 0.95f;
            contextMenus[7].selectedElement = i;
        }
    }
    
    if((this->nodeEditorDisplayer.hover && !anyNodeHover) && *Mouse::RClick()){
        contextMenus[9].dialogControl.activate();
        contextMenus[9].pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
        contextMenus[9].pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + contextMenus[7].contextPanel.scale.y;
        contextMenus[9].pos.z = 0.95f;
        contextMenus[9].selectedElement = 0;
    }
}