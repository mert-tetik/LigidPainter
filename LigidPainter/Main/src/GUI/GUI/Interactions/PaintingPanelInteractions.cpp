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

void paintingPanelInteraction(
                                Panel &paintingPanel, 
                                Mouse &mouse, 
                                Painter &painter, 
                                Dropper &dropper,
                                ColorPalette colorPalette,
                                Shader buttonShader, 
                                AppTextures appTextures, 
                                Model &model, 
                                std::vector<Node> &meshNodeScene
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
    if(paintingPanel.sections[0].elements[3].button.hover && mouse.LClick){
        dropper.active = true;
    }

    if(paintingPanel.sections[2].elements[5].button.hover && mouse.LClick){ //If pressed to remove the brush texture button from brush/more
        paintingPanel.sections[2].elements[4].button.texture = Texture(0);
    }
    
    if(paintingPanel.sections[2].elements[11].button.hover && mouse.LClick){ //If pressed to export brush file button from brush/more
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
        exportBrush.saveFile("");
    }

    //Update the meshes section of the painting panel if a new model is added
    if(model.newModelAdded){
        painter.selectedMeshIndex = 0;
        
        paintingPanel.sections[3].elements.clear();
        
        for (size_t i = 0; i < model.meshes.size(); i++)
        {

            Element meshButton = Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2),colorPalette,buttonShader, model.meshes[i].materialName , Texture(), 0.f,true));

            paintingPanel.sections[3].elements.push_back(meshButton);
        
        }
    }

    //Check all the mesh buttons if they are pressed
    for (size_t i = 0; i < paintingPanel.sections[3].elements.size(); i++) 
    {
        if(paintingPanel.sections[3].elements[i].button.hover && mouse.LClick){//If any button element is pressed
            if(painter.selectedMeshIndex != i){
                paintingPanel.sections[3].elements[painter.selectedMeshIndex].button.clickState1 = false;
                painter.selectedMeshIndex = i;
                break;
            }
        } 
    }

    //Set the selected mesh button as selected 
    for (size_t i = 0; i < paintingPanel.sections[3].elements.size(); i++)
    {
        if(i == painter.selectedMeshIndex)
            paintingPanel.sections[3].elements[i].button.clickState1 = true;
    }
    
}