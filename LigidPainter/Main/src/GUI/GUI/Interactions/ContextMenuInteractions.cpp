/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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
                           ContextMenus::model.dialogControl.isActive() ||
                           ContextMenus::menuBarProject.dialogControl.isActive() ||
                           ContextMenus::menuBarHelp.dialogControl.isActive() ||
                           ContextMenus::materialModifier.dialogControl.isActive() ||
                           ContextMenus::addMaterialModifier.dialogControl.isActive();
    
                              
    if(Library::getSelectedElementIndex() == 0 && ContextMenus::texture.dialogControl.isActive() && ContextMenus::texture.selectedElement < Library::getTextureArraySize()){ //If texture context menu is active
        if(ContextMenus::texture.contextPanel.sections[0].elements[0].button.clicked){//Clicked to rename button
            renamingTextBox.active = true;
            Library::setChanged(true);
            renamingTextBox.pos = libraryPanelDisplayer.sections[0].elements[ContextMenus::texture.selectedElement].button.pos;
            renamingTextBox.pos.y += 2.8f + this->libraryPanelDisplayer.slideVal;
            lastTitleBeforeRenaming = Library::getTexture(ContextMenus::texture.selectedElement)->title;
            Library::getTexture(ContextMenus::texture.selectedElement)->title = "";
            renamingTextBox.text = libraryPanelDisplayer.sections[0].elements[ContextMenus::texture.selectedElement].button.text;
            renamingTextBox.activeChar = renamingTextBox.text.size()-1;
            renamingTextBox.activeChar2 = renamingTextBox.activeChar;
            renamingIndices.x = 0;
            renamingIndices.y = ContextMenus::texture.selectedElement;
        }
        else if(ContextMenus::texture.contextPanel.sections[0].elements[1].button.clicked){//Clicked to duplicate button
            Texture duplicatedTexture = *Library::getTexture(ContextMenus::texture.selectedElement);
            duplicatedTexture.ID = Library::getTexture(ContextMenus::texture.selectedElement)->duplicateTexture();
            duplicatedTexture.title += "_duplicated";
            Library::addTexture(duplicatedTexture, "New texture via duplication");
        }
        else if(ContextMenus::texture.contextPanel.sections[0].elements[2].button.clicked){//Clicked to copy path button
            LigidGL::setClipboardText(project.absoluteProjectPath() + UTIL::folderDistinguisher() + "Textures" + UTIL::folderDistinguisher() + Library::getTexture(ContextMenus::texture.selectedElement)->title + ".png");
        }
        else if(ContextMenus::texture.contextPanel.sections[0].elements[3].button.clicked){//Clicked to edit button
            dialog_textureEditor.show(timer, Skybox(), Library::getTexture(ContextMenus::texture.selectedElement));
        }
        else if(ContextMenus::texture.contextPanel.sections[0].elements[4].button.clicked){//Clicked to delete button
            Library::eraseTexture(ContextMenus::texture.selectedElement);
        }
    }
    
    if(Library::getSelectedElementIndex() == 1 && ContextMenus::material.dialogControl.isActive() && ContextMenus::material.selectedElement < Library::getMaterialArraySize()){ //If material context menu is active
        if(ContextMenus::material.contextPanel.sections[0].elements[0].button.clicked){//Clicked to edit button
            //Select the material that material editor will edit & show the material editor dialog
            dialog_materialEditor.show(timer, Library::getMaterial(ContextMenus::material.selectedElement));
        }
        else if(ContextMenus::material.contextPanel.sections[0].elements[1].button.clicked){//Clicked to rename button
            renamingTextBox.active = true;
            Library::setChanged(true);
            renamingTextBox.pos = libraryPanelDisplayer.sections[0].elements[ContextMenus::material.selectedElement].button.pos;
            renamingTextBox.pos.y += 2.8f + this->libraryPanelDisplayer.slideVal;
            lastTitleBeforeRenaming = Library::getMaterial(ContextMenus::material.selectedElement)->title;
            Library::getMaterial(ContextMenus::material.selectedElement)->title = "";
            renamingTextBox.text = libraryPanelDisplayer.sections[0].elements[ContextMenus::material.selectedElement].button.text;
            renamingTextBox.activeChar = renamingTextBox.text.size()-1;
            renamingTextBox.activeChar2 = renamingTextBox.activeChar;
            renamingIndices.x = 1;
            renamingIndices.y = ContextMenus::material.selectedElement;
        }
        else if(ContextMenus::material.contextPanel.sections[0].elements[2].button.clicked){//Clicked to duplicate button
            Material duplicatedMaterial = Library::getMaterial(ContextMenus::material.selectedElement)->duplicateMaterial();
            Library::addMaterial(duplicatedMaterial, "New material via duplication");
        }
        else if(ContextMenus::material.contextPanel.sections[0].elements[3].button.clicked){//Clicked to coppy path button
            LigidGL::setClipboardText(project.absoluteProjectPath() + UTIL::folderDistinguisher() + "Materials" + UTIL::folderDistinguisher() + Library::getMaterial(ContextMenus::material.selectedElement)->title + ".lgdmaterial");
        }
        else if(ContextMenus::material.contextPanel.sections[0].elements[4].button.clicked){//Clicked to delete button
            //Delete the material
            Library::eraseMaterial(ContextMenus::material.selectedElement);
        }
        else if(ContextMenus::material.contextPanel.sections[0].elements[5].button.clicked){//Clicked to export button
            std::string exportingPath = showFileSystemObjectSelectionDialog("Export the lgdmaterial file", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MATERIAL, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_EXPORT_FILE);
            if(exportingPath.size())
                FileHandler::writeLGDMATERIALFile(exportingPath, *Library::getMaterial(ContextMenus::material.selectedElement));
        }
    }
    if(Library::getSelectedElementIndex() == 2 && ContextMenus::brush.dialogControl.isActive() && ContextMenus::brush.selectedElement < Library::getBrushArraySize()){ //If brush context menu is active
        if(ContextMenus::brush.contextPanel.sections[0].elements[0].button.clicked){//Clicked to edit brush button
            registerBrushChangedAction("Edit brush", Texture(), *Library::getBrush(ContextMenus::brush.selectedElement), ContextMenus::brush.selectedElement);
            dialog_brushModification.show(timer, &Library::getBrush(ContextMenus::brush.selectedElement)->properties);
        }
        else if(ContextMenus::brush.contextPanel.sections[0].elements[1].button.clicked){//Clicked to rename button
            renamingTextBox.active = true;
            Library::setChanged(true);
            renamingTextBox.pos = libraryPanelDisplayer.sections[0].elements[ContextMenus::brush.selectedElement].button.pos;
            renamingTextBox.pos.y += 2.8f + this->libraryPanelDisplayer.slideVal;
            lastTitleBeforeRenaming = Library::getBrush(ContextMenus::brush.selectedElement)->title;
            Library::getBrush(ContextMenus::brush.selectedElement)->title = "";
            renamingTextBox.text = libraryPanelDisplayer.sections[0].elements[ContextMenus::brush.selectedElement].button.text;
            renamingTextBox.activeChar = renamingTextBox.text.size()-1;
            renamingTextBox.activeChar2 = renamingTextBox.activeChar;
            renamingIndices.x = 2;
            renamingIndices.y = ContextMenus::brush.selectedElement;
        }
        else if(ContextMenus::brush.contextPanel.sections[0].elements[2].button.clicked && ContextMenus::brush.selectedElement < Library::getBrushArraySize()){//Clicked to duplicate button
            Brush selectedBrush = *Library::getBrush(ContextMenus::brush.selectedElement);
            Texture dupTxtr = selectedBrush.properties.brushTexture.duplicateTexture();
            dupTxtr.proceduralProps = selectedBrush.properties.brushTexture.proceduralProps;
            Library::addBrush(
                                Brush(
                                        0.1f,
                                        selectedBrush.properties.spacing,
                                        selectedBrush.properties.hardness,
                                        selectedBrush.properties.sizeJitter,
                                        selectedBrush.properties.scatter,
                                        selectedBrush.properties.fade,
                                        selectedBrush.properties.rotation,
                                        selectedBrush.properties.rotationJitter,
                                        selectedBrush.properties.alphaJitter,
                                        selectedBrush.properties.individualTexture,
                                        selectedBrush.properties.sinWavePattern,
                                        selectedBrush.title + "_duplicated",
                                        dupTxtr                     
                                    ),
                                    "New brush via duplication"
                            );
        }
        else if(ContextMenus::brush.contextPanel.sections[0].elements[3].button.clicked){//Clicked to copy path button
            LigidGL::setClipboardText(project.absoluteProjectPath() + UTIL::folderDistinguisher() + "Brushes" + UTIL::folderDistinguisher() + Library::getBrush(ContextMenus::brush.selectedElement)->title + ".lgdfilter");
        }
        else if(ContextMenus::brush.contextPanel.sections[0].elements[4].button.clicked){//Clicked to delete button
            Library::eraseBrush(ContextMenus::brush.selectedElement);
        }
    }
    if(Library::getSelectedElementIndex() == 3 && ContextMenus::model.dialogControl.isActive() && ContextMenus::model.selectedElement < Library::getBrushArraySize()){ //If brush context menu is active
        if(ContextMenus::model.contextPanel.sections[0].elements[0].button.clicked){//Clicked to model info button
            dialog_modelInfo.show(timer, Library::getModel(ContextMenus::model.selectedElement));
        }
        else if(ContextMenus::model.contextPanel.sections[0].elements[1].button.clicked){//Clicked to use the model button
            setModel(Library::getModel(ContextMenus::model.selectedElement)); //Select the model
            getModel()->newModelAdded = true; 
        }
    }

    //Save
    if(ContextMenus::menuBarProject.contextPanel.sections[0].elements[0].button.clicked || shortcuts_CTRL_S()){
        project.updateProject(true, false);
    }
    
    //Save as
    else if(ContextMenus::menuBarProject.contextPanel.sections[0].elements[1].button.clicked || shortcuts_CTRL_SHIFT_S()){
        project.saveAs("");
    }
    
    //Create new
    else if(ContextMenus::menuBarProject.contextPanel.sections[0].elements[2].button.clicked){
        dialog_newProject.dialogControl.activate();
    }
    
    //Load new
    else if(ContextMenus::menuBarProject.contextPanel.sections[0].elements[3].button.clicked){
        dialog_loadProject.dialogControl.activate();
    }
    
    //Copy path
    else if(ContextMenus::menuBarProject.contextPanel.sections[0].elements[4].button.clicked){
        project.copyTheProjectPathToTheClipboard();
    }

    //Open in file explorer
    else if(ContextMenus::menuBarProject.contextPanel.sections[0].elements[5].button.clicked){
        UTIL::openInFileExplorer(project.absoluteProjectPath().c_str());
    }

    //Recover
    else if(ContextMenus::menuBarProject.contextPanel.sections[0].elements[6].button.clicked){
        dialog_projectRecover.show(timer, project);
    }
    
    if(ContextMenus::menuBarHelp.dialogControl.isActive()){ //If help context menu is active
        if(ContextMenus::menuBarHelp.contextPanel.sections[0].elements[0].button.clicked){
            websites.ligidTools.open();
        }
        if(ContextMenus::menuBarHelp.contextPanel.sections[0].elements[1].button.clicked){
            websites.youTube.open();
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
    
    if(ContextMenus::model.dialogControl.isActive())
        ContextMenus::model.render(timer);
    else
        ContextMenus::model.selectedElement = 0;

    if(ContextMenus::menuBarProject.dialogControl.isActive())
        ContextMenus::menuBarProject.render(timer);
    else
        ContextMenus::menuBarProject.selectedElement = 0;

    if(ContextMenus::menuBarHelp.dialogControl.isActive())
        ContextMenus::menuBarHelp.render(timer);
    else
        ContextMenus::menuBarHelp.selectedElement = 0;

    if(ContextMenus::materialModifier.dialogControl.isActive())
        ContextMenus::materialModifier.render(timer);
    else
        ContextMenus::materialModifier.selectedElement = 0;

    if(ContextMenus::addMaterialModifier.dialogControl.isActive())
        ContextMenus::addMaterialModifier.render(timer);
    else
        ContextMenus::addMaterialModifier.selectedElement = 0;



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
            if(Library::getSelectedElementIndex() == 3){//To a model
                //Show the context menu
                ContextMenus::model.dialogControl.activate();
                ContextMenus::model.pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
                ContextMenus::model.pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + ContextMenus::model.contextPanel.scale.y;
                ContextMenus::model.pos.z = 0.95f;
                ContextMenus::model.selectedElement = i;
            }
        }
    }
    if(navigationPanel.sections[0].elements[1].button.clicked){ //If pressed to "Project" button in the menu bar (navigation panel)
        //Show the context menu
        ContextMenus::menuBarProject.dialogControl.activate();
        ContextMenus::menuBarProject.pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
        ContextMenus::menuBarProject.pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + ContextMenus::menuBarProject.contextPanel.scale.y;
        ContextMenus::menuBarProject.pos.z = 0.95f;
        ContextMenus::menuBarProject.selectedElement = 0;
    }
    
    if(navigationPanel.sections[0].elements[2].button.clicked){ //If pressed to "Help" button in the menu bar (navigation panel)
        //Show the context menu
        ContextMenus::menuBarHelp.dialogControl.activate();
        ContextMenus::menuBarHelp.pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
        ContextMenus::menuBarHelp.pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + ContextMenus::menuBarHelp.contextPanel.scale.y;
        ContextMenus::menuBarHelp.pos.z = 0.95f;
        ContextMenus::menuBarHelp.selectedElement = 0;
    }
    
    if(dialog_materialEditor.layerPanel.sections.size()){
        for (size_t i = 0; i < dialog_materialEditor.layerPanel.sections[0].elements.size(); i++)
        {
            if(dialog_materialEditor.layerPanel.sections[0].elements[i].button.hover && *Mouse::RClick() && dialog_materialEditor.dialogControl.isActive()){

                ContextMenus::materialModifier.dialogControl.activate();
                ContextMenus::materialModifier.pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
                ContextMenus::materialModifier.pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + ContextMenus::materialModifier.contextPanel.scale.y;
                ContextMenus::materialModifier.pos.z = 0.95f;
                ContextMenus::materialModifier.selectedElement = i;

            }
        }
    }

    if(dialog_materialEditor.layerPanel.barButtons[0].hover && *Mouse::LClick() && dialog_materialEditor.dialogControl.isActive()){
        ContextMenus::addMaterialModifier.dialogControl.activate();
        ContextMenus::addMaterialModifier.pos.x = Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f;
        ContextMenus::addMaterialModifier.pos.y = Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + ContextMenus::materialModifier.contextPanel.scale.y;
        ContextMenus::addMaterialModifier.pos.z = 0.95f;
    }

}