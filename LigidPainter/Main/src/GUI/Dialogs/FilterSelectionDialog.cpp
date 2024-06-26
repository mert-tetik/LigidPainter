/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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

#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/GL/GL.hpp"

#include <string>
#include <iostream>
#include <vector>

FilterSelectionDialog::FilterSelectionDialog(int){
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

static void updateTextureSelectingPanelElements(Panel& textureSelectingPanel);

void FilterSelectionDialog::show(Timer &timer, Filter& receivedFilter, int displayingTextureRes){
    
    this->selectedFilterIndex = 0;

    for (size_t i = 0; i < Library::getFilterArraySize(); i++)
    {
        if(receivedFilter.shader.ID == Library::getFilter(i)->shader.ID)
            this->selectedFilterIndex = i;
    }
    
    this->subPanel.sections[0].elements[0].rangeBar.value = receivedFilter.strength;

    this->dialogControl.activate();

    while (!getContext()->window.shouldClose())
    {
        dialogControl.updateStart(true);

        updateTextureSelectingPanelElements(this->textureSelectingPanel);

        //Render the panel
        this->bgPanel.render(timer, !dialog_log.isHovered());

        //Render the texture mode selection panel
        this->subPanel.render(timer, !dialog_log.isHovered());

        selectedTextureDisplayingPanel.sections[0].elements[0].button.texture = displayingTexture;

        this->textureSelectingPanel.render(timer, !dialog_log.isHovered());

        this->selectedTextureDisplayingPanel.render(timer, false);

        ShaderSystem::splitTexturesShader().use();
        ShaderSystem::splitTexturesShader().setMat4("projection"  ,   getContext()->ortho_projection);
        ShaderSystem::splitTexturesShader().setVec3("pos"         ,   selectedTextureDisplayingPanel.sections[0].elements[0].button.resultPos);
        ShaderSystem::splitTexturesShader().setVec2("scale"       ,   glm::vec2(std::min(selectedTextureDisplayingPanel.sections[0].elements[0].button.resultScale.x, selectedTextureDisplayingPanel.sections[0].elements[0].button.resultScale.y)));

        ShaderSystem::splitTexturesShader().setInt("texture1", GL::bindTexture_2D(appTextures.filterDisplayerImage.ID, "FilterSelectionDialog::show"));
        ShaderSystem::splitTexturesShader().setInt("texture2", GL::bindTexture_2D(this->selectedFilter.displayingTxtr.ID, "FilterSelectionDialog::show"));

        getBox()->draw("Filter selection dialog : Split filters");

        GL::releaseBoundTextures("FilterSelectionDialog::show");
        ShaderUTIL::release_bound_shader();

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
        if(dialogControl.mixVal == 0.f)
            break;
    }
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