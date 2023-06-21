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
#include <filesystem>

#include "tinyfiledialogs.h"

void libraryPanelDisplayerInteraction(Panel &libraryPanelDisplayer, int &selectedLibraryElementIndex, Mouse &mouse, Panel &paintingPanel, 
                                      Painter &painter, Library &library, Model &model, ColorPalette& colorPalette, Shaders &shaders, int &textureRes,
                                      NewTextureDialog &newTextureDialog,AppTextures &appTextures,int frameCounter){
    
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
                model.newModelAdded = true; 
            } 
        }
    }

    //Add button from the barButtons in the library displayer panel clicked 
    if(libraryPanelDisplayer.barButtons[0].clicked){
        if(selectedLibraryElementIndex == 0){//Textures
            newTextureDialog.dialogControl.activate();
        }
        if(selectedLibraryElementIndex == 1){ //Materials
            //Add new material to the library & not the panel
            //Will be displayed right after library panel is updated
            library.addMaterial(Material(textureRes, "material_0", 0));
        }
        if(selectedLibraryElementIndex == 2){ //Brushes
            library.addBrush(
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
                library.addModel(tdModel);
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
                library.addTexture(txtr);
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
                library.addMaterial(material);
            }
        }
        if(selectedLibraryElementIndex == 2){ //Brushes
            library.addBrush(
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
                library.addModel(tdModel);
            }
            
        }
        
    }
}

void updateLibraryPanelDisplayerElements(Panel &libraryPanelDisplayer, int &selectedLibraryElementIndex, 
                                      Library &library, ColorPalette& colorPalette, Shaders &shaders,
                                      int frameCounter){
    
    //!LIBRARY PANEL DISPLAYER
    //Update the library displayer panel every frame
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
                libSection.elements.push_back(Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,4.f),colorPalette,shaders.buttonShader,library.textures[i].title       , library.textures[i], 0.f,false))) ;
            }
        }
        else if(selectedLibraryElementIndex == 1){ //Update materials
            for (size_t i = 0; i < library.materials.size(); i++)
            {
                //Push texture elements into the section
                libSection.elements.push_back(Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,4.f),colorPalette,shaders.buttonShader,library.materials[i].title       , Texture(library.materials[i].displayingTexture), 0.f,false))) ;
            }
        }
        else if(selectedLibraryElementIndex == 2){ //Update materials
            for (size_t i = 0; i < library.brushes.size(); i++)
            {
                //Push texture elements into the section
                libSection.elements.push_back(Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,4.f),colorPalette,shaders.buttonShader,library.brushes[i].title       , Texture(library.brushes[i].displayingTexture), 0.f,false))) ;
            }
        }
        else if(selectedLibraryElementIndex == 3){ //Update tdmodels
            for (size_t i = 0; i < library.TDModels.size(); i++)
            {
                //Push texture elements into the section
                libSection.elements.push_back(Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,4.f),colorPalette,shaders.buttonShader,library.TDModels[i].title       , Texture(), 0.f,false))) ;
            }
        }
        //Give the section
        libraryPanelDisplayer.sections.push_back(Section(Element(Button()),libSection.elements));
    }
}

void libraryPanelLeftInteraction(Panel &libraryPanelLeft, int &selectedLibraryElementIndex, Mouse &mouse){
    
    //Check all the library element button if they are pressed
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
}


void paintingPanelInteraction(Panel &paintingPanel, Mouse &mouse, Painter &painter, Dropper &dropper,ColorPalette colorPalette,Shader buttonShader, AppTextures appTextures, Model &model){
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

    //If clicked to the dropper button activate the dropper
    if(paintingPanel.sections[0].elements[3].button.hover && mouse.LClick){
        dropper.active = true;
    }

    if(paintingPanel.sections[2].elements[5].button.hover && mouse.LClick){ //If pressed to remove the brush texture button from brush/more
        painter.brushTexture = Texture(0);
    }
    
    if(paintingPanel.sections[2].elements[11].button.hover && mouse.LClick){ //If pressed to export brush file button from brush/more
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

    //Update the meshes section of the painting panel if a new model is added
    if(model.newModelAdded){
        painter.selectedMeshIndex = 0;
        
        paintingPanel.sections[6].elements.clear();
        
        for (size_t i = 0; i < model.meshes.size(); i++)
        {

            Element meshButton = Element(Button(BUTTON_STYLE_SOLID,glm::vec2(2,2),colorPalette,buttonShader, model.meshes[i].materialName , Texture(), 0.f,true));

            paintingPanel.sections[6].elements.push_back(meshButton);
        }
    }

    //Check all the mesh buttons if they are pressed
    for (size_t i = 0; i < paintingPanel.sections[6].elements.size(); i++) 
    {
        if(paintingPanel.sections[6].elements[i].button.hover && mouse.LClick){//If any button element is pressed
            if(painter.selectedMeshIndex != i){
                paintingPanel.sections[6].elements[painter.selectedMeshIndex].button.clickState1 = false;
                painter.selectedMeshIndex = i;
                break;
            }
        } 
    }

    //Set the selected mesh button as selected 
    for (size_t i = 0; i < paintingPanel.sections[6].elements.size(); i++)
    {
        if(i == painter.selectedMeshIndex)
            paintingPanel.sections[6].elements[i].button.clickState1 = true;
    }
    
}


void windowPanelInteraction(Panel &windowPanel, Mouse &mouse, Painter &painter, SettingsDialog &settingsDialog, DisplayerDialog &displayerDialog,ExportDialog &exportDialog){
    //!WINDOW PANEL BUTTONS
    if(windowPanel.sections[0].elements[0].button.hover && mouse.LClick){//Pressed to the 3D painting button of the window panel
        painter.threeDimensionalMode = true;
    }
    else if(windowPanel.sections[0].elements[1].button.hover && mouse.LClick){//Pressed to the 2D painting button of the window panel
        painter.threeDimensionalMode = false;
    }
    else if(windowPanel.sections[0].elements[4].button.hover && mouse.LClick){//Pressed to the settings button of the window panel
        settingsDialog.dialogControl.activate();
    }
    else if(windowPanel.sections[0].elements[3].button.hover && mouse.LClick){//Pressed to the displayer button of the window panel
        displayerDialog.dialogControl.activate();
    }
    else if(windowPanel.sections[0].elements[2].button.hover && mouse.LClick){//Pressed to the export button of the window panel
        exportDialog.dialogControl.activate();
    }
}

void UI::contextMenuInteraction(std::vector<ContextMenu> &contextMenus, Mouse &mouse , Library &library,std::vector<Node>& appNodes ,
                                std::vector<Node> &nodeScene,Context &context,glm::vec2 videoScale,Timer &timer,TextRenderer &textRenderer,
                                Project& project){
    anyContextMenuActive = false; 
    for (size_t i = 0; i < contextMenus.size(); i++)//Check all the contextMenus
    {
        if(contextMenus[i].dialogControl.isActive()) //Set anyContextMenuActive UI class variable 
            anyContextMenuActive = true;
        
        //CONTEXT MENU BUTTONS
        if(i == 0 && selectedLibraryElementIndex == 0 && contextMenus[i].dialogControl.isActive()){ //If texture context menu is active
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
                library.addTexture(duplicatedTexture);
            }
            if(contextMenus[i].contextPanel.sections[0].elements[3].button.hover && mouse.LClick){//Clicked to delete button
                library.textures.erase(library.textures.begin() + contextMenus[i].selectedElement);
            }
        }
        
        if(i == 1 && selectedLibraryElementIndex == 1 && contextMenus[i].dialogControl.isActive()){ //If material context menu is active
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && mouse.LClick){//Clicked to edit button
                //Select the material that material editor will edit & show the material editor dialog
                selectedMaterialIndex = contextMenus[i].selectedElement;
                materialEditorDialog.activate();
            }
            if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && mouse.LClick){//Clicked to add to scene button
                //Create the node of the materail an add to the node scene
                Node materialNode = appNodes[1];
                materialNode.barButton.text = library.materials[contextMenus[i].selectedElement].title;
                materialNode.materialID = library.materials[contextMenus[i].selectedElement].uniqueID;
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
                library.addMaterial(duplicatedMaterial);
            }
            if(contextMenus[i].contextPanel.sections[0].elements[5].button.hover && mouse.LClick){//Clicked to delete button
                library.materials.erase(library.materials.begin() + contextMenus[i].selectedElement);
            }
        }
        if(i == 2 && selectedLibraryElementIndex == 2 && contextMenus[i].dialogControl.isActive()){ //If brush context menu is active
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
                library.addBrush(library.brushes[contextMenus[i].selectedElement]);
            }
            if(contextMenus[i].contextPanel.sections[0].elements[5].button.hover && mouse.LClick){//Clicked to delete button
                library.brushes.erase(library.brushes.begin() + contextMenus[i].selectedElement);
            }
        }
        if(i == 3 && contextMenus[i].dialogControl.isActive()){ //If project context menu is active
            //Save
            if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && mouse.LClick){
                project.updateProject(library);
            }
            
            //Save as
            if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && mouse.LClick){
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
                Util util;
                util.openInFileExplorer(std::filesystem::absolute(project.folderPath).string().c_str());
            }
        }
        if(i == 4 && contextMenus[i].dialogControl.isActive()){ //If painting context menu is active
            
        }
        if(i == 5 && contextMenus[i].dialogControl.isActive()){ //If help context menu is active
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
            if(selectedLibraryElementIndex == 0){//To a texture
                //Show the context menu
                contextMenus[0].dialogControl.activate();
                contextMenus[0].pos.x = mouse.cursorPos.x / videoScale.x * 100.f;
                contextMenus[0].pos.y = mouse.cursorPos.y / videoScale.y * 100.f + contextMenus[0].contextPanel.scale.y;
                contextMenus[0].pos.z = 0.95f;
                contextMenus[0].selectedElement = i;
                
            }
            if(selectedLibraryElementIndex == 1){//To a material
                //Show the context menu
                contextMenus[1].dialogControl.activate();
                contextMenus[1].pos.x = mouse.cursorPos.x / videoScale.x * 100.f;
                contextMenus[1].pos.y = mouse.cursorPos.y / videoScale.y * 100.f + contextMenus[1].contextPanel.scale.y;
                contextMenus[1].pos.z = 0.95f;
                contextMenus[1].selectedElement = i;
            }
            if(selectedLibraryElementIndex == 2){//To a brush
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
}


void paintingModesPanelInteraction(Panel &paintingModesPanel, Painter &painter){
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
}

void nodeInteraction(Model &model, std::vector<Node> &nodeScene,std::vector<Node> &appNodes,Shaders shaders,ColorPalette colorPalette,glm::vec2 videoScale){
    
    //Update the mesh node if a new model is added
    if(model.newModelAdded){
        Node meshOutputNode;
        std::vector <NodeIO> meshOutputNodeInputElements;
        for (size_t i = 0; i < model.meshes.size(); i++)
        {
            meshOutputNodeInputElements.push_back(NodeIO(model.meshes[i].materialName,Element(Button(BUTTON_STYLE_SOLID,glm::vec2(1,1),colorPalette,shaders.buttonShader,model.meshes[i].materialName,Texture(),2.f,false)),colorPalette.mainColor,colorPalette,shaders.buttonShader,videoScale,0));
        }

        //Create the mesh output node
        meshOutputNode.loadIO
        (
            meshOutputNodeInputElements,
            {

            },
            shaders.buttonShader,
            shaders.singleCurve,
            colorPalette,
            0,
            0
        );

        appNodes[0] = meshOutputNode;
        nodeScene[0] = meshOutputNode;
    }
}


void UI::panelPositioning(float &screenGapPerc, Library &library, Painter &painter){
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

    selectedTextureDisplayer.sections[0].elements[0].scale.y = selectedTextureDisplayer.scale.y;
    if(library.textures.size())
        selectedTextureDisplayer.sections[0].elements[0].button.texture = painter.selectedTexture;
    twoDPaintingPanel.sections[0].elements[0].scale.y = twoDPaintingPanel.scale.y;
}









void UI::elementInteraction(Painter &painter,Mouse &mouse, Library &library,std::vector<ContextMenu> &contextMenus,std::vector<Node> &appNodes,std::vector<Node> &nodeScene,
                        Context &context,glm::vec2 &videoScale,TextRenderer &textRenderer, Timer &timer, int &textureRes,float screenGapPerc,Model &model, Project& project){
    
    contextMenuInteraction(contextMenus,mouse,library,appNodes,nodeScene,context,videoScale,timer,textRenderer,project);
    
    libraryPanelDisplayerInteraction(libraryPanelDisplayer,selectedLibraryElementIndex,mouse,paintingPanel,painter,library,model,colorPalette,shaders,textureRes,newTextureDialog,appTextures,frameCounter);
    
    libraryPanelLeftInteraction(libraryPanelLeft,selectedLibraryElementIndex,mouse);
    
    updateLibraryPanelDisplayerElements(libraryPanelDisplayer,selectedLibraryElementIndex,library,colorPalette,shaders,frameCounter);

    paintingPanelInteraction(paintingPanel,mouse,painter,dropper,colorPalette,shaders.buttonShader,appTextures,model);
    
    windowPanelInteraction(windowPanel, mouse, painter, settingsDialog, displayerDialog,exportDialog);

    paintingModesPanelInteraction(paintingModesPanel,painter);

    nodeInteraction(model,nodeScene,appNodes,shaders,colorPalette,videoScale);
    
    panelPositioning(screenGapPerc,library,painter);

    //!Dialog & panel state
    anyDialogActive = 
                    textureSelectionDialog.dialogControl.isActive() || 
                    materialEditorDialog.dialogControl.isActive()   || 
                    anyContextMenuActive                            || 
                    displayerDialog.dialogControl.isActive()        || 
                    greetingDialog.dialogControl.isActive()         || 
                    settingsDialog.dialogControl.isActive()         || 
                    newProjectDialog.dialogControl.isActive()       || 
                    newTextureDialog.dialogControl.isActive()       || 
                    loadProjectDialog.dialogControl.isActive();   

    anyPanelHover = 
                    navigationPanel.hover               ||
                    windowPanel.hover                   ||
                    paintingPanel.hover                 ||
                    libraryPanelLeft.hover              ||
                    libraryPanelDisplayer.hover         ||
                    nodeEditorDisplayer.hover           ||
                    selectedTextureDisplayer.hover      || 
                    paintingModesPanel.hover;
}