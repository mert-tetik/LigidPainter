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

void libraryPanelDisplayerInteraction(
                                        Panel &libraryPanelDisplayer, 
                                        Mouse &mouse, 
                                        Panel &paintingPanel, 
                                        Painter &painter, 
                                        Library &library, 
                                        Model &model, 
                                        ColorPalette& colorPalette, 
                                        Shaders &shaders, 
                                        int &textureRes,
                                        NewTextureDialog &newTextureDialog,
                                        AppTextures &appTextures,
                                        int frameCounter
                                    )
{
    
    //Update the selected texture
    for (size_t i = 0; i < libraryPanelDisplayer.sections[0].elements.size(); i++) //Check all the texture button elements from the library displayer panel
    {
        if(library.selectedElementIndex == 0){ //Textures selected
            if(libraryPanelDisplayer.sections[0].elements[i].button.hover && mouse.LClick){
                if(paintingPanel.sections[2].elements[4].button.clickState1){//If brush texture displayer is pressed
                    paintingPanel.sections[2].elements[4].button.texture = libraryPanelDisplayer.sections[0].elements[i].button.texture; //Select a brush texture
                    paintingPanel.sections[2].elements[4].button.clickState1 = false;
                } 
                else
                    painter.selectedTexture = library.textures[i]; //Select the texture 
            } //If any texture button element is pressed
        
            if(library.textures[i].ID == painter.selectedTexture.ID) //Highlight the selected texture
                libraryPanelDisplayer.sections[0].elements[i].button.clickState1 = true;
        }
        if(library.selectedElementIndex == 3){ //Models selected
            if(libraryPanelDisplayer.sections[0].elements[i].button.hover && mouse.LClick){
                model = library.TDModels[i]; //Select the model
                model.newModelAdded = true; 
            } 
        }
    }

    //Add button from the barButtons in the library displayer panel clicked 
    if(libraryPanelDisplayer.barButtons[0].clicked){
        if(library.selectedElementIndex == 0){//Textures
            newTextureDialog.dialogControl.activate();
        }
        if(library.selectedElementIndex == 1){ //Materials
            //Add new material to the library & not the panel
            //Will be displayed right after library panel is updated
            library.addMaterial(Material(textureRes, "material", 0));
        }
        if(library.selectedElementIndex == 2){ //Brushes
            library.addBrush(
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
            
            library.brushes[library.brushes.size()-1].updateDisplayTexture(shaders.twoDPainting,shaders.buttonShader);
            
        }
        if(library.selectedElementIndex == 3){ //3D Models
            
            std::string test = showFileSystemObjectSelectionDialog("Select a 3D model file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MODEL, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);
            if(test.size()){
                Model tdModel;
                tdModel.loadModel(test, true);
                library.addModel(tdModel);
            }
            
        }
    }
    if(libraryPanelDisplayer.barButtons[1].clicked){ //Import button
        if(library.selectedElementIndex == 0){//Textures  
            std::string test = showFileSystemObjectSelectionDialog("Select a texture file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_TEXTURE, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);
            if(test.size()){
                Texture importedTxtr;
                importedTxtr.load(test.c_str());
                library.addTexture(importedTxtr);
            }
        }
        if(library.selectedElementIndex == 1){ //Materials
            //Select material
            std::string test = showFileSystemObjectSelectionDialog("Select a material file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MATERIAL, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);

            if(test.size()){
                
                Material importedMaterial(textureRes, "", 0);
                importedMaterial.readFile(test,colorPalette,shaders.buttonShader,appTextures,library.materials);
                library.addMaterial(importedMaterial);
            }
        }
        if(library.selectedElementIndex == 2){ //Brushes
            
            std::string test = showFileSystemObjectSelectionDialog("Select a brush file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_BRUSH, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);

            if(test.size()){
                Brush importedBrush;
                importedBrush.readFile(test);
                importedBrush.updateDisplayTexture(shaders.twoDPainting,shaders.buttonShader);
                library.addBrush(importedBrush);
            }
            
        }
        if(library.selectedElementIndex == 3){ //3D Models
            
            std::string test = showFileSystemObjectSelectionDialog("Select a 3D model file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MODEL, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);
            
            if(test.size()){
                Model tdModel;
                tdModel.loadModel(test,true);
                library.addModel(tdModel);
            }
        }
    }
}