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
#include "MouseSystem/Mouse.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>

size_t __libraryPanelDisplayerInteractionHoverStartSecond = 0.f;
bool __libraryPanelDisplayerInteractionFirstHover = false;
int __libraryPanelDisplayerInteractionFocusedElementIndex = 0;

void libraryPanelDisplayerInteraction(
                                        Panel &libraryPanelDisplayer, 
                                        Panel &paintingPanel, 
                                        Painter &painter, 
                                        NewTextureDialog &newTextureDialog,
                                        AppMaterialModifiers &appMaterialModifiers,
                                        MaterialDisplayerDialog &materialDisplayerDialog,
                                        FilterDisplayerDialog &filterDisplayerDialog,
                                        Button& zoomingDisplayingButton,
                                        Timer& timer
                                    )
{
    bool anyElementHover = false;
    //Update the selected texture
    for (size_t i = 0; i < libraryPanelDisplayer.sections[0].elements.size(); i++) //Check all the texture button elements from the library displayer panel
    {
        anyElementHover = true;
        if(libraryPanelDisplayer.sections[0].elements[i].button.hover){
            
            zoomingDisplayingButton.pos = libraryPanelDisplayer.sections[0].elements[i].button.pos;
            zoomingDisplayingButton.texture = libraryPanelDisplayer.sections[0].elements[i].button.texture;
            
            if(i !=__libraryPanelDisplayerInteractionFocusedElementIndex){
                __libraryPanelDisplayerInteractionFocusedElementIndex = i;
                __libraryPanelDisplayerInteractionFirstHover = false;
            }

            if(!__libraryPanelDisplayerInteractionFirstHover)
                __libraryPanelDisplayerInteractionHoverStartSecond = timer.seconds;
            
            __libraryPanelDisplayerInteractionFirstHover = true;

            if(timer.seconds - __libraryPanelDisplayerInteractionHoverStartSecond > 1){
                __libraryPanelDisplayerInteractionFocusedElementIndex = i;
                zoomingDisplayingButton.render(timer, false);
            }
        }

        if(Library::getSelectedElementIndex() == 0){ //Textures selected
            if(libraryPanelDisplayer.sections[0].elements[i].button.clicked){
                painter.selectedTexture = *Library::getTexture(i); //Select the texture 
                paintingPanel.sections[3].elements[painter.selectedPaintingChannelIndex].button.texture = painter.selectedTexture;
            } //If any texture button element is pressed
        
            if(Library::getTexture(i)->ID == painter.selectedTexture.ID) //Highlight the selected texture
                libraryPanelDisplayer.sections[0].elements[i].button.clickState1 = true;
        }
        if(Library::getSelectedElementIndex() == 1){ //Materials selected
            if(libraryPanelDisplayer.sections[0].elements[i].button.hover && *Mouse::LDoubleClick()){
                materialDisplayerDialog.dialogControl.activate();
                materialDisplayerDialog.material = *Library::getMaterial(i);
            } 
        }
        if(Library::getSelectedElementIndex() == 3){ //Models selected
            if(libraryPanelDisplayer.sections[0].elements[i].button.clicked){
                *getModel() = *Library::getModel(i); //Select the model
                getModel()->newModelAdded = true; 
            } 
        }
        if(Library::getSelectedElementIndex() == 6){ //Filters selected
            if(libraryPanelDisplayer.sections[0].elements[i].button.hover && *Mouse::LDoubleClick()){
                filterDisplayerDialog.dialogControl.activate();
                filterDisplayerDialog.filter = *Library::getFilter(i);
            } 
        }
        if(Library::getSelectedElementIndex() == 8){ //Texture Packs selected
            if(libraryPanelDisplayer.sections[0].elements[i].button.clicked){
                showTexturePackEditorDialog(*Library::getTexturePack(i));
            } 
        }
    }

    if(!anyElementHover){

        __libraryPanelDisplayerInteractionFirstHover = false;
    }


    
    //Add button from the barButtons in the library displayer panel clicked 
    if(libraryPanelDisplayer.barButtons[0].clicked){
        if(Library::getSelectedElementIndex() == 0){//Textures
            newTextureDialog.dialogControl.activate();
        }
        if(Library::getSelectedElementIndex() == 1){ //Materials
            //Add new material to the library & not the panel
            //Will be displayed right after library panel is updated
            Library::addMaterial(Material("NewMaterial", 0));
        }
        if(Library::getSelectedElementIndex() == 2){ //Brushes
            Library::addBrush(
                                        Brush
                                            (    
                                                0.1f,
                                                painter.brushProperties.spacing,
                                                painter.brushProperties.hardness,
                                                painter.brushProperties.sizeJitter,
                                                painter.brushProperties.scatter,
                                                painter.brushProperties.fade,
                                                painter.brushProperties.rotation,
                                                painter.brushProperties.rotationJitter,
                                                painter.brushProperties.alphaJitter,
                                                painter.brushProperties.individualTexture,
                                                painter.brushProperties.sinWavePattern,
                                                "NewBrush",
                                                painter.brushProperties.brushTexture
                                            )
                                    );
            
        }
        if(Library::getSelectedElementIndex() == 3){ //3D Models
            
            std::string test = showFileSystemObjectSelectionDialog("Select a 3D model file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MODEL, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);
            if(test.size()){
                Model tdModel;
                tdModel.loadModel(test, true);
                if(tdModel.meshes.size())
                    Library::addModel(tdModel);
                else
                    LGDLOG::start<< "ERROR : Can't add the 3D model to the library. Mesh size is 0!" << LGDLOG::end;
            }
        }
        if(Library::getSelectedElementIndex() == 4){ //Fonts
            
        }
        if(Library::getSelectedElementIndex() == 5){ //Scripts
            
        }
        if(Library::getSelectedElementIndex() == 6){ //Filters
            
            std::string test = showFileSystemObjectSelectionDialog("Select a filter file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_FILTER, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);
            
            if(test.size()){
                Filter filter;
                filter.load(test);
                Library::addFilter(filter);
            }
        }
        if(Library::getSelectedElementIndex() == 8){ //Texture Packs
            TexturePack texturePack;
            texturePack.title = "NewTexturePack";
            Library::addTexturePack(texturePack);
        }
    }
    if(libraryPanelDisplayer.barButtons[1].clicked){ //Import button
        if(Library::getSelectedElementIndex() == 0){//Textures  
            std::string test = showFileSystemObjectSelectionDialog("Select a texture file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_TEXTURE, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);
            if(test.size()){
                Texture importedTxtr;
                importedTxtr.load(test.c_str());
                Library::addTexture(importedTxtr);
            }
        }
        if(Library::getSelectedElementIndex() == 1){ //Materials
            //Select material
            std::string test = showFileSystemObjectSelectionDialog("Select a material file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MATERIAL, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);

            if(test.size()){
                Material importedMaterial("", 0);
                if(FileHandler::readLGDMATERIALFile(test, importedMaterial, appMaterialModifiers))
                    Library::addMaterial(importedMaterial);
            }
        }
        if(Library::getSelectedElementIndex() == 2){ //Brushes
            
            std::string test = showFileSystemObjectSelectionDialog("Select a brush file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_BRUSH, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);

            if(test.size()){
                Brush importedBrush;
                if(FileHandler::readLGDBRUSHFile(test,importedBrush)){
                    Library::addBrush(importedBrush);
                }
            }
            
        }
        if(Library::getSelectedElementIndex() == 3){ //3D Models
            
            std::string test = showFileSystemObjectSelectionDialog("Select a 3D model file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MODEL, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);
            
            if(test.size()){
                Model tdModel;
                tdModel.loadModel(test,true);
                if(tdModel.meshes.size())
                    Library::addModel(tdModel);
                else
                    LGDLOG::start<< "ERROR : Can't add the 3D model to the library. Mesh size is 0!" << LGDLOG::end;
            }
        }

        if(Library::getSelectedElementIndex() == 4){ //Fonts
            
        }
        if(Library::getSelectedElementIndex() == 5){ //Scripts
            
        }
        if(Library::getSelectedElementIndex() == 6){ //Filters
            
            std::string test = showFileSystemObjectSelectionDialog("Select a filter file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_FILTER, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);
            
            if(test.size()){
                Filter filter;
                filter.load(test);
                Library::addFilter(filter);
            }
        }
        if(Library::getSelectedElementIndex() == 8){ //Texture Packs
            std::string test = showFileSystemObjectSelectionDialog("Select a texture pack folder.", "", 0, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER);

            if(test.size()){
                TexturePack texturePack;
                texturePack.load(test);
                Library::addTexturePack(texturePack);
            }
        }
    }
}