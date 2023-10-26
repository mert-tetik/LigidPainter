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
    
    this->subPanel = Panel(
                                
                                
                                {
                                    Section(
                                        Button(),
                                        {
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(6.f,2.f),"Strength", Texture(), 40.f, 0.f, 1.f, 1.f),
                                            Button(ELEMENT_STYLE_STYLIZED,glm::vec2(6.f,2.f),"Select", Texture(), 1.f, false)
                                        }
                                    )
                                },
                                glm::vec2(scale.x, 2.f),
                                glm::vec3(pos.x , pos.y + bgPanel.scale.y - 2.f, pos.z),
                                ColorPalette::mainColor,
                                ColorPalette::thirdColor,
                                false,
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
                        glm::vec2(8.f, bgPanel.scale.y),
                        glm::vec3(pos.x - bgPanel.scale.x + 8.f, pos.y, pos.z),
                        ColorPalette::mainColor,
                        ColorPalette::thirdColor,
                        true,
                        true,
                        false,
                        true,
                        true,
                        1.f,
                        2.f,
                        {},
                        20.f,
                        true
                    );

    this->selectedTextureDisplayingPanel = Panel(
                                            
                                            
                                            {
                                                Section(
                                                    Button(),
                                                    {
                                                        Button(ELEMENT_STYLE_SOLID,glm::vec2(bgPanel.scale.y - subPanel.scale.y),"", Texture(), 0.f, true),
                                                    }
                                                )
                                            },
                                            glm::vec2(scale.x - textureSelectingPanel.scale.x, bgPanel.scale.y - subPanel.scale.y),
                                            glm::vec3(pos.x + bgPanel.scale.x - (scale.x - textureSelectingPanel.scale.x), pos.y - subPanel.scale.y, pos.z),
                                            ColorPalette::secondColor,
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

    this->selectedTextureDisplayingPanel.sections[0].elements[0].button.color = ColorPalette::secondColor;
    this->textureSelectingPanel.color.a = 1.f;

    this->subPanel.solidStyle = true;
}

//Forward declarations for the utility functions
static void drawBG(unsigned int bgTexture);
static void updateTextureSelectingPanelElements(Panel& textureSelectingPanel);

void FilterSelectionDialog::show(Timer &timer, glm::mat4 guiProjection, Filter& receivedFilter, int displayingTextureRes){
    
    this->dialogControl.activate();
        
    this->selectedFilterIndex = 0;

    for (size_t i = 0; i < Library::getFilterArraySize(); i++)
    {
        if(receivedFilter.shader.ID == Library::getFilter(i)->shader.ID)
            this->selectedFilterIndex = i;
    }
    
    this->subPanel.sections[0].elements[0].rangeBar.value = receivedFilter.strength;

    while (!getContext()->window.shouldClose())
    {
        getContext()->window.pollEvents();
        // Prevent rendering the application if the window is minimized
        while (getContext()->window.isMinimized()){
            getContext()->window.pollEvents();
        }
        
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawBG(Settings::defaultFramebuffer()->bgTxtr.ID);

        dialogControl.updateStart();

        updateTextureSelectingPanelElements(this->textureSelectingPanel);

        //Render the panel
        this->bgPanel.render(timer, true);

        //Render the texture mode selection panel
        this->subPanel.render(timer, true);

        selectedTextureDisplayingPanel.sections[0].elements[0].button.texture = displayingTexture;

        this->textureSelectingPanel.render(timer, true);

        this->selectedTextureDisplayingPanel.render(timer, false);

        ShaderSystem::splitTexturesShader().use();
        ShaderSystem::splitTexturesShader().setMat4("projection"  ,   guiProjection);
        ShaderSystem::splitTexturesShader().setVec3("pos"         ,   selectedTextureDisplayingPanel.sections[0].elements[0].button.resultPos);
        ShaderSystem::splitTexturesShader().setVec2("scale"       ,   glm::vec2(std::min(selectedTextureDisplayingPanel.sections[0].elements[0].button.resultScale.x, selectedTextureDisplayingPanel.sections[0].elements[0].button.resultScale.y)));

        ShaderSystem::splitTexturesShader().setInt("texture1", 0);
        ShaderSystem::splitTexturesShader().setInt("texture2", 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Settings::appTextures().filterDisplayerImage.ID);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->selectedFilter.displayingTxtr.ID);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        dialogControl.updateEnd(timer,0.15f);

        ShaderSystem::buttonShader().use();


        //If pressed any of the texture select the texture
        for (size_t i = 0; i < this->textureSelectingPanel.sections[0].elements.size(); i++)
        {
            if(this->textureSelectingPanel.sections[0].elements[i].button.hover && *Mouse::LClick()){
                selectedFilterIndex = i;
            }
        }

        if(this->selectedFilterIndex < Library::getFilterArraySize())
            this->selectedFilter.shader = Library::getFilter(this->selectedFilterIndex)->shader;
       
        this->selectedFilter.strength = this->subPanel.sections[0].elements[0].rangeBar.value; 
       
        if(this->selectedFilterIndex < Library::getFilterArraySize())
            this->selectedFilter.srcCode = Library::getFilter(this->selectedFilterIndex)->srcCode;
        
        this->selectedFilter.generateDisplayingTexture(glm::vec2(1024));


        // Pressed to the select button
        if(this->subPanel.sections[0].elements[1].button.clicked){

            if(this->selectedFilterIndex < Library::getFilterArraySize()){
                receivedFilter.shader = this->selectedFilter.shader;
                receivedFilter.strength = this->selectedFilter.strength;
                receivedFilter.srcCode = this->selectedFilter.srcCode;
                receivedFilter.generateDisplayingTexture(glm::vec2(256));
            }

            break;
        }

        //End the dialog
        if((getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)) || (!this->bgPanel.hover && *Mouse::LClick())){
            selectedFilterIndex = 0;
            break;
        }

            
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
        Mouse::updateCursor();  


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
    
    ShaderSystem::defaultFramebufferShader().setVec2("resolution", Settings::defaultFramebuffer()->resolution);
    ShaderSystem::defaultFramebufferShader().setInt("txtr", 0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bgTexture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    ShaderSystem::buttonShader().use();
}

static void updateTextureSelectingPanelElements(Panel& textureSelectingPanel){
    textureSelectingPanel.sections.clear();
    std::vector<Element> sectionElements;
    
    for (size_t i = 0; i < Library::getFilterArraySize(); i++)
    {
        Element btn = Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2, 3.f),"", Library::getFilter(i)->displayingTxtr, 0.f,false));
        btn.button.radius = 0.5f;
        btn.button.color = ColorPalette::mainColor;

        sectionElements.push_back(btn);
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