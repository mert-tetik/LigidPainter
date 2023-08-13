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

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>

void libraryPanelDisplayerInteraction(
                                        Panel &libraryPanelDisplayer, 
                                        Mouse &mouse, 
                                        Panel &paintingPanel, 
                                        Painter &painter, 
                                        Model &model, 
                                        ColorPalette& colorPalette, 
                                        int &textureRes,
                                        NewTextureDialog &newTextureDialog,
                                        AppTextures &appTextures,
                                        AppMaterialModifiers &appMaterialModifiers,
                                        int frameCounter
                                    )
{
    
    //Update the selected texture
    for (size_t i = 0; i < libraryPanelDisplayer.sections[0].elements.size(); i++) //Check all the texture button elements from the library displayer panel
    {
        if(Library::getSelectedElementIndex() == 0){ //Textures selected
            if(libraryPanelDisplayer.sections[0].elements[i].button.hover && mouse.LClick){
                painter.selectedTexture = *Library::getTexture(i); //Select the texture 
                paintingPanel.sections[4].elements[painter.selectedPaintingChannelIndex].button.texture = painter.selectedTexture;
            } //If any texture button element is pressed
        
            if(Library::getTexture(i)->ID == painter.selectedTexture.ID) //Highlight the selected texture
                libraryPanelDisplayer.sections[0].elements[i].button.clickState1 = true;
        }
        if(Library::getSelectedElementIndex() == 3){ //Models selected
            if(libraryPanelDisplayer.sections[0].elements[i].button.hover && mouse.LClick){
                model = *Library::getModel(i); //Select the model
                model.newModelAdded = true; 
            } 
        }
    }

    //Add button from the barButtons in the library displayer panel clicked 
    if(libraryPanelDisplayer.barButtons[0].clicked){
        if(Library::getSelectedElementIndex() == 0){//Textures
            newTextureDialog.dialogControl.activate();
        }
        if(Library::getSelectedElementIndex() == 1){ //Materials
            //Add new material to the library & not the panel
            //Will be displayed right after library panel is updated
            Library::addMaterial(Material(textureRes, "material", 0));
        }
        if(Library::getSelectedElementIndex() == 2){ //Brushes
            Library::addBrush(
                                        Brush
                                            (    
                                                painter.brushProperties.sizeJitter,
                                                painter.brushProperties.scatter,
                                                painter.brushProperties.fade,
                                                painter.brushProperties.rotation,
                                                painter.brushProperties.rotationJitter,
                                                painter.brushProperties.alphaJitter,
                                                painter.brushProperties.individualTexture,
                                                painter.brushProperties.sinWavePattern,
                                                "brush_1",
                                                painter.brushProperties.brushTexture
                                            )
                                    );
            
            Library::getBrush(Library::getBrushArraySize() - 1)->updateDisplayTexture();
            
        }
        if(Library::getSelectedElementIndex() == 3){ //3D Models
            
            std::string test = showFileSystemObjectSelectionDialog("Select a 3D model file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MODEL, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);
            if(test.size()){
                Model tdModel;
                tdModel.loadModel(test, true);
                if(tdModel.meshes.size())
                    Library::addModel(tdModel);
                else
                    std::cout << "ERROR : Can't add the 3D model to the library. Mesh size is 0!" << std::endl;
            }
            
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
                Material importedMaterial(textureRes, "", 0);
                if(FileHandler::readLGDMATERIALFile(test, importedMaterial, colorPalette, appTextures, appMaterialModifiers))
                    Library::addMaterial(importedMaterial);
            }
        }
        if(Library::getSelectedElementIndex() == 2){ //Brushes
            
            std::string test = showFileSystemObjectSelectionDialog("Select a brush file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_BRUSH, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);

            if(test.size()){
                Brush importedBrush;
                if(FileHandler::readLGDBRUSHFile(test,importedBrush)){
                    importedBrush.updateDisplayTexture();
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
                    std::cout << "ERROR : Can't add the 3D model to the library. Mesh size is 0!" << std::endl;
            }
        }
    }
}