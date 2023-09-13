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
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>

void paintingPanelInteraction(
                                Panel &paintingPanel, 
                                Painter &painter, 
                                Dropper &dropper
                            )
{

    //Get the brush data from GUI to the painter class
    painter.brushProperties.radius = paintingPanel.sections[1].elements[0].rangeBar.value;
    painter.brushProperties.opacity = paintingPanel.sections[1].elements[1].rangeBar.value;
    painter.brushProperties.hardness = paintingPanel.sections[1].elements[2].rangeBar.value;
    painter.brushProperties.spacing = paintingPanel.sections[1].elements[3].rangeBar.value;
    painter.brushProperties.sizeJitter = paintingPanel.sections[2].elements[0].rangeBar.value;
    painter.brushProperties.fade = paintingPanel.sections[2].elements[1].rangeBar.value;
    painter.brushProperties.sinWavePattern = paintingPanel.sections[2].elements[2].checkBox.clickState1;
    painter.brushProperties.scatter = paintingPanel.sections[2].elements[3].rangeBar.value;
    painter.brushProperties.brushTexture = paintingPanel.sections[2].elements[4].button.texture;
    painter.brushProperties.individualTexture = paintingPanel.sections[2].elements[6].checkBox.clickState1;
    painter.brushProperties.rotation = paintingPanel.sections[2].elements[7].rangeBar.value;
    painter.brushProperties.rotationJitter = paintingPanel.sections[2].elements[8].rangeBar.value;
    painter.brushProperties.alphaJitter = paintingPanel.sections[2].elements[9].rangeBar.value;

    
    if(paintingPanel.sections[0].elements[0].button.hover && *Mouse::LDoubleClick()){//Pressed to first color button element
        painter.loadColor1();
    }
    if(paintingPanel.sections[0].elements[1].button.hover && *Mouse::LDoubleClick()){//Pressed to second color button element
        painter.loadColor2();
    }
    if(paintingPanel.sections[0].elements[2].button.hover && *Mouse::LDoubleClick()){//Pressed to third color button element
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

    //Keep the selected color button pressed
    for (size_t i = 0; i < paintingPanel.sections[0].elements.size(); i++){
        if(i == painter.selectedColorIndex){
            paintingPanel.sections[0].elements[i].button.clickState1 = true;           
        }
    }
    
    //Update the color values of the color buttons
    paintingPanel.sections[0].elements[0].button.color = glm::vec4(painter.color1.getRGB_normalized(), 1.f);
    paintingPanel.sections[0].elements[1].button.color = glm::vec4(painter.color2.getRGB_normalized(), 1.f);
    paintingPanel.sections[0].elements[2].button.color = glm::vec4(painter.color3.getRGB_normalized(), 1.f);
    

    //If clicked to the dropper button activate the dropper
    if(paintingPanel.sections[0].elements[3].button.hover && *Mouse::LClick()){
        dropper.active = true;
    }

    if(paintingPanel.sections[2].elements[5].button.hover && *Mouse::LClick()){ //If pressed to remove the brush texture button from brush/more
        char whitePixel[] = { 127, 127, 127, 127 };
        paintingPanel.sections[2].elements[4].button.textureSelection = true;
        paintingPanel.sections[2].elements[4].button.texture = Texture(whitePixel, 1, 1, GL_NEAREST);
        paintingPanel.sections[2].elements[4].button.texture.proceduralID = 24; //Solid white
        paintingPanel.sections[2].elements[4].button.texture.proceduralnverted = 0;
        paintingPanel.sections[2].elements[4].button.texture.proceduralScale = 1.f;
        paintingPanel.sections[2].elements[4].button.texture.title = "AutoGeneratedMask";        
    }
    
    if(paintingPanel.sections[2].elements[11].button.hover && *Mouse::LClick()){ //If pressed to export brush file button from brush/more
        Brush exportBrush
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
                        );
        FileHandler::writeLGDBRUSHFile("", exportBrush);
    }

    //Update the meshes section of the painting panel if a new model is added
    if(getModel()->newModelAdded){
        paintingPanel.sections[3].elements[0].button.selectedMeshI = 0;
    }

    painter.selectedMeshIndex = paintingPanel.sections[3].elements[0].button.selectedMeshI;

    //Check all the mesh buttons if they are pressed
    for (size_t i = 0; i < paintingPanel.sections[4].elements.size(); i++) 
    {
        if(paintingPanel.sections[4].elements[i].button.hover && *Mouse::LClick()){//If any button element is pressed
            if(painter.selectedPaintingChannelIndex != i){
                paintingPanel.sections[4].elements[painter.selectedPaintingChannelIndex].button.clickState1 = false;
                painter.selectedPaintingChannelIndex = i;
                painter.selectedTexture = paintingPanel.sections[4].elements[painter.selectedPaintingChannelIndex].button.texture;
                break;
            }
        } 
    }

    //Set the selected mesh button as selected 
    for (size_t i = 0; i < paintingPanel.sections[4].elements.size(); i++)
    {
        if(i == painter.selectedPaintingChannelIndex)
            paintingPanel.sections[4].elements[i].button.clickState1 = true;
    }
    
}