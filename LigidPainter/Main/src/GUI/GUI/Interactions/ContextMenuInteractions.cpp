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

void UI::contextMenuInteraction(Timer &timer, Project& project, Painter &painter){

    anyContextMenuActive = ContextMenus::texture.dialogControl.isActive() ||
                           ContextMenus::material.dialogControl.isActive() ||
                           ContextMenus::brush.dialogControl.isActive() ||
                           ContextMenus::menuBarProject.dialogControl.isActive() ||
                           ContextMenus::menuBarPainting.dialogControl.isActive() ||
                           ContextMenus::menuBarHelp.dialogControl.isActive() ||
                           ContextMenus::materialModifier.dialogControl.isActive() ||
                           ContextMenus::node.dialogControl.isActive() ||
                           ContextMenus::addMaterialModifier.dialogControl.isActive() ||
                           ContextMenus::nodeScenePanel.dialogControl.isActive(); 

                              
    if(Library::getSelectedElementIndex() == 0 && ContextMenus::texture.dialogControl.isActive() && Library::getTextureArraySize()){ //If texture context menu is active
        if(ContextMenus::texture.contextPanel.sections[0].elements[0].button.hover && *Mouse::LClick()){//Clicked to rename button
            renamingTextBox.active = true;
            Library::setChanged(true);
            renamingTextBox.pos = libraryPanelDisplayer.sections[0].elements[ContextMenus::texture.selectedElement].button.pos;
            renamingTextBox.pos.y += 2.8f;
            lastTitleBeforeRenaming = Library::getTexture(ContextMenus::texture.selectedElement)->title;
            Library::getTexture(ContextMenus::texture.selectedElement)->title = "";
            renamingTextBox.text = libraryPanelDisplayer.sections[0].elements[ContextMenus::texture.selectedElement].button.text;
            renamingTextBox.activeChar = renamingTextBox.text.size()-1;
            renamingTextBox.activeChar2 = renamingTextBox.activeChar;
            renamingIndices.x = 0;
            renamingIndices.y = ContextMenus::texture.selectedElement;
        }
        if(ContextMenus::texture.contextPanel.sections[0].elements[1].button.hover && *Mouse::LClick()){//Clicked to duplicate button
            Texture duplicatedTexture;
            duplicatedTexture = *Library::getTexture(ContextMenus::texture.selectedElement);
            duplicatedTexture.ID = Library::getTexture(ContextMenus::texture.selectedElement)->duplicateTexture();
            Library::addTexture(duplicatedTexture);
        }
        if(ContextMenus::texture.contextPanel.sections[0].elements[3].button.hover && *Mouse::LClick()){//Clicked to edit button
            textureEditorSelectedTxtr = *Library::getTexture(ContextMenus::texture.selectedElement);
            this->textureEditorDialog.dialogControl.activate();
        }
        if(ContextMenus::texture.contextPanel.sections[0].elements[4].button.hover && *Mouse::LClick()){//Clicked to delete button
            Library::eraseTexture(ContextMenus::texture.selectedElement);
        }
    }
    
    if(Library::getSelectedElementIndex() == 1 && ContextMenus::material.dialogControl.isActive()){ //If material context menu is active
        if(ContextMenus::material.contextPanel.sections[0].elements[0].button.hover && *Mouse::LClick()){//Clicked to edit button
            //Select the material that material editor will edit & show the material editor dialog
            selectedMaterialIndex = ContextMenus::material.selectedElement;
            materialEditorDialog.activate();
        }
        if(ContextMenus::material.contextPanel.sections[0].elements[1].button.hover && *Mouse::LClick()){//Clicked to add to scene button
            //Create the node of the materail an add to the node scene
            Node materialNode = Node(
                                        MATERIAL_NODE, 
                                        Library::getMaterial(ContextMenus::material.selectedElement)->uniqueID
                                    );
            materialNode.barButton.text = Library::getMaterial(ContextMenus::material.selectedElement)->title;
            NodeScene::addNode(materialNode); //Add material node
        }
        if(ContextMenus::material.contextPanel.sections[0].elements[2].button.hover && *Mouse::LClick()){//Clicked to rename button
            renamingTextBox.active = true;
            Library::setChanged(true);
            renamingTextBox.pos = libraryPanelDisplayer.sections[0].elements[ContextMenus::material.selectedElement].button.pos;
            renamingTextBox.pos.y += 2.8f;
            lastTitleBeforeRenaming = Library::getMaterial(ContextMenus::material.selectedElement)->title;
            Library::getMaterial(ContextMenus::material.selectedElement)->title = "";
            renamingTextBox.text = libraryPanelDisplayer.sections[0].elements[ContextMenus::material.selectedElement].button.text;
            renamingTextBox.activeChar = renamingTextBox.text.size()-1;
            renamingTextBox.activeChar2 = renamingTextBox.activeChar;
            renamingIndices.x = 1;
            renamingIndices.y = ContextMenus::material.selectedElement;
        }
        if(ContextMenus::material.contextPanel.sections[0].elements[3].button.hover && *Mouse::LClick()){//Clicked to duplicate button
            Material duplicatedMaterial = Library::getMaterial(ContextMenus::material.selectedElement)->duplicateMaterial();
            Library::addMaterial(duplicatedMaterial);
        }
        if(ContextMenus::material.contextPanel.sections[0].elements[5].button.hover && *Mouse::LClick()){//Clicked to delete button
            
            //Delete the nodes using same material
            for (int nodeI = 0; nodeI < NodeScene::getArraySize(); nodeI++)
            {
                if(NodeScene::getNode(nodeI)->materialID == Library::getMaterial(ContextMenus::material.selectedElement)->uniqueID && NodeScene::getNode(nodeI)->nodeIndex == MATERIAL_NODE){
                    NodeScene::deleteNode( nodeI);
                    nodeI--;
                }
            }
            
            //Delete the material
            Library::eraseMaterial(ContextMenus::material.selectedElement);
        }
        if(ContextMenus::material.contextPanel.sections[0].elements[6].button.hover && *Mouse::LClick()){//Clicked to export button
            std::string exportingPath = showFileSystemObjectSelectionDialog("Export the lgdmaterial file", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MATERIAL, false,FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_EXPORT_FILE);
            if(exportingPath.size())
                FileHandler::writeLGDMATERIALFile(exportingPath, *Library::getMaterial(ContextMenus::material.selectedElement));
        }
    }
    if(Library::getSelectedElementIndex() == 2 && ContextMenus::brush.dialogControl.isActive()){ //If brush context menu is active
        if(ContextMenus::brush.contextPanel.sections[0].elements[0].button.hover && *Mouse::LClick()){//Clicked to use brush button
            
            Library::getBrush(ContextMenus::brush.selectedElement)->useBrush(paintingPanel);

        }
        if(ContextMenus::brush.contextPanel.sections[0].elements[1].button.hover && *Mouse::LClick()){//Clicked to apply brush settings

            Library::getBrush(ContextMenus::brush.selectedElement)->applyToBrush(paintingPanel);

        }
        if(ContextMenus::brush.contextPanel.sections[0].elements[2].button.hover && *Mouse::LClick()){//Clicked to rename button
            renamingTextBox.active = true;
            Library::setChanged(true);
            renamingTextBox.pos = libraryPanelDisplayer.sections[0].elements[ContextMenus::brush.selectedElement].button.pos;
            renamingTextBox.pos.y += 2.8f;
            lastTitleBeforeRenaming = Library::getBrush(ContextMenus::brush.selectedElement)->title;
            Library::getBrush(ContextMenus::brush.selectedElement)->title = "";
            renamingTextBox.text = libraryPanelDisplayer.sections[0].elements[ContextMenus::brush.selectedElement].button.text;
            renamingTextBox.activeChar = renamingTextBox.text.size()-1;
            renamingTextBox.activeChar2 = renamingTextBox.activeChar;
            renamingIndices.x = 2;
            renamingIndices.y = ContextMenus::brush.selectedElement;
        }
        if(ContextMenus::brush.contextPanel.sections[0].elements[3].button.hover && *Mouse::LClick()){//Clicked to duplicate button
            Library::addBrush(*Library::getBrush(ContextMenus::brush.selectedElement));
        }
        if(ContextMenus::brush.contextPanel.sections[0].elements[5].button.hover && *Mouse::LClick()){//Clicked to delete button
            Library::eraseBrush(ContextMenus::brush.selectedElement);
        }
    }
    if(ContextMenus::menuBarProject.dialogControl.isActive()){ //If project context menu is active
        //Save
        if(ContextMenus::menuBarProject.contextPanel.sections[0].elements[0].button.hover && *Mouse::LClick()){
            project.updateProject(true);
        }
        
        //Save as
        if(ContextMenus::menuBarProject.contextPanel.sections[0].elements[1].button.hover && *Mouse::LClick()){
            project.updateProject(true);
            project.duplicateFolder("");
        }
        
        //Create new
        if(ContextMenus::menuBarProject.contextPanel.sections[0].elements[2].button.hover && *Mouse::LClick()){
            newProjectDialog.dialogControl.activate();
        }
        
        //Load new
        if(ContextMenus::menuBarProject.contextPanel.sections[0].elements[3].button.hover && *Mouse::LClick()){
            loadProjectDialog.dialogControl.activate();
        }
        
        //Copy path
        if(ContextMenus::menuBarProject.contextPanel.sections[0].elements[4].button.hover && *Mouse::LClick()){
            project.copyTheProjectPathToTheClipboard();
        }

        //Open in file explorer
        if(ContextMenus::menuBarProject.contextPanel.sections[0].elements[5].button.hover && *Mouse::LClick()){
            UTIL::openInFileExplorer(std::filesystem::absolute(project.folderPath).string().c_str());
        }
    }
    if(ContextMenus::menuBarPainting.dialogControl.isActive()){ //If painting context menu is active
        //TODO REMOVE THE PAINTING CONTEXT MENU            
    }
    if(ContextMenus::menuBarHelp.dialogControl.isActive()){ //If help context menu is active
        if(ContextMenus::menuBarHelp.contextPanel.sections[0].elements[0].button.hover && *Mouse::LClick()){
            websites.ligidTools.open();
        }
        if(ContextMenus::menuBarHelp.contextPanel.sections[0].elements[1].button.hover && *Mouse::LClick()){
            websites.youTube.open();
        }
    }
    
    if(ContextMenus::node.dialogControl.isActive()){ //If node context menu is active
        //Delete the node
        if(ContextMenus::node.contextPanel.sections[0].elements[0].button.hover && *Mouse::LClick() && ContextMenus::node.selectedElement){
            NodeScene::deleteNode(ContextMenus::node.selectedElement);                
        }
    }

    if(ContextMenus::nodeScenePanel.dialogControl.isActive()){//If node scene context menu is active
        //Add material ID node button pressed
        if(ContextMenus::nodeScenePanel.contextPanel.sections[0].elements[0].button.hover && *Mouse::LClick()){
            NodeScene::addNode(Node(MATERIAL_ID_NODE, 0));
            NodeScene::getNode(NodeScene::getArraySize()-1)->nodePanel.pos = NodeScene::getNode(0)->nodePanel.pos;
        }
        //Add material mask node button pressed
        else if(ContextMenus::nodeScenePanel.contextPanel.sections[0].elements[1].button.hover && *Mouse::LClick()){
            NodeScene::addNode(Node(MATERIAL_MASK_NODE, 0));
            NodeScene::getNode(NodeScene::getArraySize()-1)->nodePanel.pos = NodeScene::getNode(0)->nodePanel.pos;
        }
    } 


    if(ContextMenus::texture.dialogControl.isActive())
        ContextMenus::texture.render(timer);
    else
        ContextMenus::texture.selectedElement = 0;

    if(ContextMenus::material.dialogControl.isActive())
        ContextMenus::material.render(timer);
    else
        ContextMenus::material.selectedElement = 0;

    if(ContextMenus::brush.dialogControl.isActive())
        ContextMenus::brush.render(timer);
    else
        ContextMenus::brush.selectedElement = 0;

    if(ContextMenus::menuBarProject.dialogControl.isActive())
        ContextMenus::menuBarProject.render(timer);
    else
        ContextMenus::menuBarProject.selectedElement = 0;

    if(ContextMenus::menuBarPainting.dialogControl.isActive())
        ContextMenus::menuBarPainting.render(timer);
    else
        ContextMenus::menuBarPainting.selectedElement = 0;

    if(ContextMenus::menuBarHelp.dialogControl.isActive())
        ContextMenus::menuBarHelp.render(timer);
    else
        ContextMenus::menuBarHelp.selectedElement = 0;

    if(ContextMenus::materialModifier.dialogControl.isActive())
        ContextMenus::materialModifier.render(timer);
    else
        ContextMenus::materialModifier.selectedElement = 0;

    if(ContextMenus::node.dialogControl.isActive())
        ContextMenus::node.render(timer);
    else
        ContextMenus::node.selectedElement = 0;

    if(ContextMenus::addMaterialModifier.dialogControl.isActive())
        ContextMenus::addMaterialModifier.render(timer);
    else
        ContextMenus::addMaterialModifier.selectedElement = 0;

    if(ContextMenus::nodeScenePanel.dialogControl.isActive())
        ContextMenus::nodeScenePanel.render(timer);
    else
        ContextMenus::nodeScenePanel.selectedElement = 0;



    //*Show context menu
    //For library panel displayer
    for (size_t i = 0; i < libraryPanelDisplayer.sections[0].elements.size(); i++)
    {
        if(libraryPanelDisplayer.sections[0].elements[i].button.hover && *Mouse::RClick()){ //Right clicked to an element
            if(Library::getSelectedElementIndex() == 0){//To a texture
                //Show the context menu
                ContextMenus::texture.dialogControl.activate();
                ContextMenus::texture.pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
                ContextMenus::texture.pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + ContextMenus::texture.contextPanel.scale.y;
                ContextMenus::texture.pos.z = 0.95f;
                ContextMenus::texture.selectedElement = i;
                
            }
            if(Library::getSelectedElementIndex() == 1){//To a material
                //Show the context menu
                ContextMenus::material.dialogControl.activate();
                ContextMenus::material.pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
                ContextMenus::material.pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + ContextMenus::material.contextPanel.scale.y;
                ContextMenus::material.pos.z = 0.95f;
                ContextMenus::material.selectedElement = i;
            }
            if(Library::getSelectedElementIndex() == 2){//To a brush
                //Show the context menu
                ContextMenus::brush.dialogControl.activate();
                ContextMenus::brush.pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
                ContextMenus::brush.pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + ContextMenus::brush.contextPanel.scale.y;
                ContextMenus::brush.pos.z = 0.95f;
                ContextMenus::brush.selectedElement = i;
            }
        }
    }
    if(navigationPanel.sections[0].elements[1].button.hover && *Mouse::LClick()){ //If pressed to "Project" button in the menu bar (navigation panel)
        //Show the context menu
        ContextMenus::menuBarProject.dialogControl.activate();
        ContextMenus::menuBarProject.pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
        ContextMenus::menuBarProject.pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + ContextMenus::menuBarProject.contextPanel.scale.y;
        ContextMenus::menuBarProject.pos.z = 0.95f;
        ContextMenus::menuBarProject.selectedElement = 0;
    }
    if(navigationPanel.sections[0].elements[2].button.hover && *Mouse::LClick()){ //If pressed to "Painting" button in the menu bar (navigation panel)
        //Show the context menu
        ContextMenus::menuBarPainting.dialogControl.activate();
        ContextMenus::menuBarPainting.pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
        ContextMenus::menuBarPainting.pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + ContextMenus::menuBarPainting.contextPanel.scale.y;
        ContextMenus::menuBarPainting.pos.z = 0.95f;
        ContextMenus::menuBarPainting.selectedElement = 0;
    }
    
    if(navigationPanel.sections[0].elements[3].button.hover && *Mouse::LClick()){ //If pressed to "Help" button in the menu bar (navigation panel)
        //Show the context menu
        ContextMenus::menuBarHelp.dialogControl.activate();
        ContextMenus::menuBarHelp.pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
        ContextMenus::menuBarHelp.pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + ContextMenus::menuBarHelp.contextPanel.scale.y;
        ContextMenus::menuBarHelp.pos.z = 0.95f;
        ContextMenus::menuBarHelp.selectedElement = 0;
    }
    
    if(materialEditorDialog.layerPanel.sections.size()){
        for (size_t i = 0; i < materialEditorDialog.layerPanel.sections[0].elements.size(); i++)
        {
            if(materialEditorDialog.layerPanel.sections[0].elements[i].button.hover && *Mouse::RClick() && materialEditorDialog.dialogControl.isActive()){

                ContextMenus::materialModifier.dialogControl.activate();
                ContextMenus::materialModifier.pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
                ContextMenus::materialModifier.pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + ContextMenus::materialModifier.contextPanel.scale.y;
                ContextMenus::materialModifier.pos.z = 0.95f;
                ContextMenus::materialModifier.selectedElement = i;

            }
        }
    }

    if(materialEditorDialog.layerPanel.barButtons[0].clicked && materialEditorDialog.dialogControl.isActive()){
        ContextMenus::addMaterialModifier.dialogControl.activate();
        ContextMenus::addMaterialModifier.pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
        ContextMenus::addMaterialModifier.pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + ContextMenus::materialModifier.contextPanel.scale.y;
        ContextMenus::addMaterialModifier.pos.z = 0.95f;
    }

    bool anyNodeHover = false;
    for (size_t i = 0; i < NodeScene::getArraySize(); i++)
    {
        if((NodeScene::getNode(i)->nodePanel.hover || NodeScene::getNode(i)->barButton.hover))
            anyNodeHover = true;

        if((NodeScene::getNode(i)->nodePanel.hover || NodeScene::getNode(i)->barButton.hover) && *Mouse::RClick() && !NodeScene::getNode(i)->cursorOnBarriers){
            ContextMenus::node.dialogControl.activate();
            ContextMenus::node.pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
            ContextMenus::node.pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + ContextMenus::node.contextPanel.scale.y;
            ContextMenus::node.pos.z = 0.95f;
            ContextMenus::node.selectedElement = i;
        }
    }
    
    if((this->nodeEditorDisplayer.hover && !anyNodeHover) && *Mouse::RClick()){
        ContextMenus::nodeScenePanel.dialogControl.activate();
        ContextMenus::nodeScenePanel.pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
        ContextMenus::nodeScenePanel.pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + ContextMenus::nodeScenePanel.contextPanel.scale.y;
        ContextMenus::nodeScenePanel.pos.z = 0.95f;
        ContextMenus::nodeScenePanel.selectedElement = 0;
    }
}