/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Texture selection dialog is responsible of modifying the procedural variables inside of a Texture class & generating displaying textures

*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "LibrarySystem/Library.hpp"
#include "MouseSystem/Mouse.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

FilterSelectionDialog::FilterSelectionDialog(){
    this->bgPanel = Panel({}, scale, pos, ColorPalette::secondColor, ColorPalette::thirdColor, true, true, false, true, true, 1.f, 15.f, {}, 20.f, true);
    this->textureSelectingPanel = Panel(
                                                    
                                                    {
                                                        /*
                                                        Section(
                                                            Button(),
                                                            {
                                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2, scale.y / 2.f),"", Texture(), 0.f, true),
                                                            }
                                                        )
                                                        */
                                                    },
                                                    glm::vec2(scale.x / 6.f, scale.y / 2.f),
                                                    glm::vec3(pos.x, pos.y + scale.y - scale.y / 2.f, pos.z),
                                                    ColorPalette::mainColor,
                                                    ColorPalette::thirdColor,
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
                                                    
                                                    
                                                    {
                                                        Section(
                                                            Button(),
                                                            {
                                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2, scale.y / 2.f),"", Texture(), 0.f, true),
                                                            }
                                                        )
                                                    },
                                                    glm::vec2(scale.x / 6.f, scale.y / 2.f),
                                                    glm::vec3(pos.x, pos.y - scale.y + scale.y / 2.f, pos.z),
                                                    ColorPalette::mainColor,
                                                    ColorPalette::thirdColor,
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
                                
                                
                                {
                                    Section(
                                        Button(),
                                        {
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Strength", Texture(), 65.f, 0.f, 1.f, 1.f),
                                            Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,2.f),"Select", Texture(), 2.f, false)
                                        }
                                    )
                                },
                                glm::vec2(scale.x / 6.f, scale.y),
                                glm::vec3(pos.x - scale.x + (scale.x / 6.f) , pos.y, pos.z),
                                ColorPalette::mainColor,
                                ColorPalette::thirdColor,
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

}

//Forward declarations for the utility functions
static void drawBG(unsigned int bgTexture);
static void updateTextureSelectingPanelElements(Panel& textureSelectingPanel, int selectedTextureMode);

void FilterSelectionDialog::show(Timer &timer, glm::mat4 guiProjection, Filter& receivedFilter, int displayingTextureRes){
    
    this->dialogControl.activate();
        
    this->selectedTextureMode = 0;
    this->selectedTextureIndex = 0;

    while (!getContext()->window.shouldClose())
    {
        getContext()->window.pollEvents();
        
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawBG(Settings::defaultFramebuffer()->bgTxtr);

        dialogControl.updateStart();

        updateTextureSelectingPanelElements(this->textureSelectingPanel, this->selectedTextureMode);

        //Render the panel
        this->bgPanel.render(timer, true);

        //Render the texture mode selection panel
        this->subPanel.render(timer, true);

        selectedTextureDisplayingPanel.scale.x = this->scale.x - subPanel.scale.x;
        selectedTextureDisplayingPanel.pos.x = this->pos.x + subPanel.scale.x;

        textureSelectingPanel.pos.x = selectedTextureDisplayingPanel.pos.x;
        textureSelectingPanel.scale.x = selectedTextureDisplayingPanel.scale.x;

        selectedTextureDisplayingPanel.sections[0].elements[0].button.texture = displayingTexture;

        this->textureSelectingPanel.render(timer, true);

        this->selectedTextureDisplayingPanel.render(timer, true);

        ShaderSystem::splitTexturesShader().use();
        ShaderSystem::splitTexturesShader().setMat4("projection"  ,   guiProjection);
        ShaderSystem::splitTexturesShader().setVec3("pos"         ,   selectedTextureDisplayingPanel.sections[0].elements[0].button.resultPos);
        ShaderSystem::splitTexturesShader().setVec2("scale"       ,   glm::vec2(std::min(selectedTextureDisplayingPanel.sections[0].elements[0].button.resultScale.x, selectedTextureDisplayingPanel.sections[0].elements[0].button.resultScale.y)));

        ShaderSystem::splitTexturesShader().setInt("texture1", 0);
        ShaderSystem::splitTexturesShader().setInt("texture2", 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Settings::appTextures().filterDisplayerImage.ID);

        glActiveTexture(GL_TEXTURE1);
        if(this->selectedTextureIndex < Library::getFilterArraySize())
            glBindTexture(GL_TEXTURE_2D, Library::getFilter(this->selectedTextureIndex)->displayingTxtr);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        dialogControl.updateEnd(timer,0.15f);

        ShaderSystem::buttonShader().use();


        //If pressed any of the texture select the texture
        for (size_t i = 0; i < this->textureSelectingPanel.sections[0].elements.size(); i++)
        {
            if(this->textureSelectingPanel.sections[0].elements[i].button.hover && *Mouse::LClick()){
                selectedTextureIndex = i;
            }
        }

        // Pressed to the select button
        if(this->subPanel.sections[0].elements[1].button.clicked){

            if(this->selectedTextureIndex < Library::getFilterArraySize())
                receivedFilter = *Library::getFilter(this->selectedTextureIndex);

            dialogControl.unActivate();
        }

        //End the dialog
        if((getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)) || (!this->bgPanel.hover && *Mouse::LClick())){
            dialogControl.unActivate();
            selectedTextureIndex = 0;
        }

        if(!this->dialogControl.isActive())
            break;
            
        dialogControl.updateEnd(timer,0.15f);

        getContext()->window.swapBuffers();

        //Set mouse states to default
        *Mouse::LClick() = false;
        *Mouse::RClick() = false;
        *Mouse::MClick() = false;
        *Mouse::LDoubleClick() = false;
        *Mouse::mouseOffset() = glm::vec2(0);
        *Mouse::mods() = 0;
        *Mouse::mouseScroll() = 0;
        *Mouse::action() = 0;

        //Set keyboard states to default
        textRenderer.keyInput = false;
        textRenderer.mods = 0;

        Settings::defaultFramebuffer()->render();    
        Settings::defaultFramebuffer()->setViewport();   
    }
}

static void drawBG(
                    unsigned int bgTexture
                )
{
    ShaderSystem::defaultFramebufferShader().use();
    ShaderSystem::defaultFramebufferShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    ShaderSystem::defaultFramebufferShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
    ShaderSystem::defaultFramebufferShader().setVec2("scale", glm::vec2(0.5f));
    
    ShaderSystem::defaultFramebufferShader().setInt("txtr", 0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, bgTexture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    ShaderSystem::buttonShader().use();
}

static void updateTextureSelectingPanelElements(Panel& textureSelectingPanel, int selectedTextureMode){
    textureSelectingPanel.sections.clear();
    std::vector<Element> sectionElements;
    
    for (size_t i = 0; i < Library::getFilterArraySize(); i++)
    {
        sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2, 3.f),"", Library::getFilter(i)->displayingTxtr, 0.f,false)));
    }

    textureSelectingPanel.sections.push_back
                                            (
                                                Section
                                                        (
                                                            Element(Button()),
                                                            sectionElements
                                                        )
                                            );  
}