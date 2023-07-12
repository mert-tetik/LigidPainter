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
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"

#include <string>
#include <iostream>
#include <vector>

TextureSelectionDialog::TextureSelectionDialog(){}

TextureSelectionDialog::TextureSelectionDialog(Shader buttonShader,ColorPalette colorPalette){
    this->colorPalette = colorPalette;
    this->buttonShader = buttonShader;
    
    texturePanel = Panel(buttonShader,colorPalette,{},scale,pos,colorPalette.mainColor,colorPalette.thirdColor,true,true,false,true,true,1.f,15.f,{},20.f,true);
}

void TextureSelectionDialog::render(glm::vec2 videoScale,Mouse &mouse,Timer &timer,TextRenderer &textRenderer,Library library){
    
    dialogControl.updateStart(buttonShader);

    //Update the panel elements
    texturePanel.sections.clear();
    std::vector<Element> sectionElements;
    for (size_t i = 0; i < library.textures.size(); i++)
    {
        sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),colorPalette,buttonShader,"texture_0"       , library.textures[i], 0.f,false)));
    }
    
    //Push the section elements to the texture panel
    texturePanel.sections.push_back(
        Section
                (
                    Element(Button()),
                    sectionElements
                )
    );

    //Render the panel
    texturePanel.render(videoScale,mouse,timer,textRenderer,true);
    
    //If pressed any of the texture select the texture
    for (size_t i = 0; i < texturePanel.sections[0].elements.size(); i++)
    {
        if(texturePanel.sections[0].elements[i].button.hover && mouse.LClick){
            selectedTextureIndex = i;
        }
    }

    //End the dialog
    if((textRenderer.key == LIGIDGL_KEY_ESCAPE && textRenderer.keyInput) || (!texturePanel.hover && mouse.LClick)){
        dialogControl.unActivate();
        selectedTextureIndex = 1000;
    }

    dialogControl.updateEnd(timer,buttonShader,0.15f);
}