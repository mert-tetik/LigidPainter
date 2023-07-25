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
    
    this->proceduralDisplayerShader = Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/TextureSelectionProceduralDisplayer.frag" ,nullptr, nullptr, nullptr);

    this->bgPanel = Panel(buttonShader, colorPalette, {}, scale, pos, colorPalette.secondColor, colorPalette.thirdColor, true, true, false, true, true, 1.f, 15.f, {}, 20.f, true);
    this->textureSelectingPanel = Panel(
                                                    buttonShader,
                                                    colorPalette,
                                                    {
                                                        /*
                                                        Section(
                                                            Button(),
                                                            {
                                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2, scale.y / 2.f),colorPalette,buttonShader,"", Texture(), 0.f, true),
                                                            }
                                                        )
                                                        */
                                                    },
                                                    glm::vec2(scale.x / 6.f, scale.y / 2.f),
                                                    glm::vec3(pos.x, pos.y + scale.y - scale.y / 2.f, pos.z),
                                                    colorPalette.mainColor,
                                                    colorPalette.thirdColor,
                                                    true,
                                                    true,
                                                    false,
                                                    true,
                                                    true,
                                                    1.f,
                                                    12.f,
                                                    {},
                                                    20.f,
                                                    true
                                                );
    
    this->selectedTextureDisplayingPanel = Panel(
                                                    buttonShader,
                                                    colorPalette,
                                                    {
                                                        Section(
                                                            Button(),
                                                            {
                                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2, scale.y / 2.f),colorPalette,buttonShader,"", Texture(), 0.f, true),
                                                            }
                                                        )
                                                    },
                                                    glm::vec2(scale.x / 6.f, scale.y / 2.f),
                                                    glm::vec3(pos.x, pos.y - scale.y + scale.y / 2.f, pos.z),
                                                    colorPalette.mainColor,
                                                    colorPalette.thirdColor,
                                                    true,
                                                    true,
                                                    false,
                                                    true,
                                                    true,
                                                    1.f,
                                                    1.f,
                                                    {},
                                                    20.f,
                                                    true
                                                );
    this->subPanel = Panel(
                                buttonShader,
                                colorPalette,
                                {
                                    Section(
                                        Button(),
                                        {
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),colorPalette,buttonShader,"Library Textures", Texture(), 5.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),colorPalette,buttonShader,"Procedural Noise Textures", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),colorPalette,buttonShader,"Procedural Pattern Textures", Texture(), 2.f, true),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),colorPalette,buttonShader,"Invert", 40.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),colorPalette,buttonShader,"Scale", Texture(), 2.f, 0.f, 10.f, 1.f),
                                            Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,2.f),colorPalette,buttonShader,"Select", Texture(), 2.f, false),
                                        }
                                    )
                                },
                                glm::vec2(scale.x / 6.f, scale.y),
                                glm::vec3(pos.x - scale.x + (scale.x / 6.f) , pos.y, pos.z),
                                colorPalette.mainColor,
                                colorPalette.thirdColor,
                                true,
                                true,
                                false,
                                true,
                                true,
                                1.f,
                                1.f,
                                {},
                                20.f,
                                true
                            );

    for (size_t i = 0; i < 3; i++)
    {
        this->subPanel.sections[0].elements[i].button.color = glm::vec4(0);
    }
}

void TextureSelectionDialog::render(glm::vec2 videoScale,Mouse &mouse,Timer &timer,TextRenderer &textRenderer,Library library, glm::mat4 guiProjection){
    dialogControl.updateStart(buttonShader);

    //Update the panel elements
    this->textureSelectingPanel.sections.clear();
    std::vector<Element> sectionElements;
    if(this->selectedTextureMode == 0){
        for (size_t i = 0; i < library.textures.size(); i++)
        {
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),colorPalette,buttonShader,"texture"       , library.textures[i], 0.f,false)));
        }
    }
    else if(this->selectedTextureMode == 1){
        for (size_t i = 0; i < 24; i++)
        {
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,3.f),colorPalette,buttonShader,""       , Texture(), 0.f,false)));
        }
    }
    
    //Push the section elements to the texture panel
    this->textureSelectingPanel.sections.push_back(
        Section
                (
                    Element(Button()),
                    sectionElements
                )
    );

    for (size_t i = 0; i < 3; i++)
    {
        if(this->subPanel.sections[0].elements[i].button.clickState1 && this->selectedTextureMode != i){
            this->selectedTextureMode = i;
            this->selectedTextureIndex = 0;
            for (size_t i = 0; i < this->subPanel.sections[0].elements.size(); i++)
                this->subPanel.sections[0].elements[i].button.clickState1 = false;
        }

        if(this->selectedTextureMode == i)
            this->subPanel.sections[0].elements[i].button.clickState1 = true;
        else
            this->subPanel.sections[0].elements[i].button.clickState1 = false;
    }
    
    //Render the panel
    this->bgPanel.render(videoScale,mouse,timer,textRenderer,true);
    
    //Render the texture mode selection panel
    this->subPanel.render(videoScale,mouse,timer,textRenderer,true);
    
    selectedTextureDisplayingPanel.scale.x = this->scale.x - subPanel.scale.x;
    selectedTextureDisplayingPanel.pos.x = this->pos.x + subPanel.scale.x;

    textureSelectingPanel.pos.x = selectedTextureDisplayingPanel.pos.x;
    textureSelectingPanel.scale.x = selectedTextureDisplayingPanel.scale.x;

    if(library.textures.size())
        selectedTextureDisplayingPanel.sections[0].elements[0].button.texture = library.textures[selectedTextureIndex];

    this->textureSelectingPanel.render(videoScale,mouse,timer,textRenderer,true);
    
    if(this->selectedTextureMode == 0){
        
        buttonShader.setInt("properties.invertTheTexture", this->subPanel.sections[0].elements[3].checkBox.clickState1);
        buttonShader.setVec2("properties.txtrScale", glm::vec2(this->subPanel.sections[0].elements[4].rangeBar.value));
        this->selectedTextureDisplayingPanel.render(videoScale,mouse,timer,textRenderer,true);
        buttonShader.setInt("properties.invertTheTexture", false);
        buttonShader.setVec2("properties.txtrScale", glm::vec2(1.f));
        
    }
    else{
        proceduralDisplayerShader.use();
        proceduralDisplayerShader.setMat4("projection"  ,       guiProjection);
        proceduralDisplayerShader.setVec3("pos"         ,       selectedTextureDisplayingPanel.resultPos);
        proceduralDisplayerShader.setVec2("scale"       ,       glm::vec2(std::min(selectedTextureDisplayingPanel.resultScale.x,selectedTextureDisplayingPanel.resultScale.y)));
        proceduralDisplayerShader.setInt("proceduralID", selectedTextureIndex);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    
        for (size_t i = 0; i < this->textureSelectingPanel.sections[0].elements.size(); i++)
        {
            proceduralDisplayerShader.use();
            proceduralDisplayerShader.setVec3("pos"         ,       this->textureSelectingPanel.sections[0].elements[i].button.resultPos);
            proceduralDisplayerShader.setVec2("scale"       ,       glm::vec2(std::min(this->textureSelectingPanel.sections[0].elements[i].button.resultScale.x, this->textureSelectingPanel.sections[0].elements[i].button.resultScale.y)));
            proceduralDisplayerShader.setInt("proceduralID", i);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        
        buttonShader.use();
        
    }

    //If pressed any of the texture select the texture
    for (size_t i = 0; i < this->textureSelectingPanel.sections[0].elements.size(); i++)
    {
        if(this->textureSelectingPanel.sections[0].elements[i].button.hover && mouse.LClick){
            selectedTextureIndex = i;
        }
    }

    //End the dialog
    if((textRenderer.key == LIGIDGL_KEY_ESCAPE && textRenderer.keyInput) || (!this->bgPanel.hover && mouse.LClick)){
        dialogControl.unActivate();
        selectedTextureIndex = 0;
    }

    dialogControl.updateEnd(timer,buttonShader,0.15f);
}