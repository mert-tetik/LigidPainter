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

MeshSelectionDialog::MeshSelectionDialog(){
    this->bgPanel = Panel({}, scale, pos, ColorPalette::secondColor, ColorPalette::thirdColor, true, true, false, true, true, 1.f, 5, {}, 20.f, true);
    this->subPanel = Panel(
                            {
                                Section(
                                            Button(),
                                            {
                                                Button(ELEMENT_STYLE_STYLIZED, glm::vec2(8.f, 2.f), "Select", Texture(), 4.f, false)
                                            }
                                        )
                            }, glm::vec2(scale.x, 2.f), glm::vec3(pos.x,pos.y + this->bgPanel.scale.y + 2.f ,pos.z), ColorPalette::secondColor, ColorPalette::thirdColor, false, true, false, true, true, 1.f, 1, {}, 20.f, true);
}

//Forward declarations for the utility functions
static void initMeshSelectionDialog(int &selectedTextureMode, Panel& subPanel, int& selectedMeshIndex);
static void drawBG(unsigned int bgTexture);

void MeshSelectionDialog::show(Timer &timer, glm::mat4 guiProjection, int& selectedMeshI){
    
    this->dialogControl.activate();

    this->selectedMeshIndex = selectedMeshI;
        
    initMeshSelectionDialog(this->selectedTextureMode, this->bgPanel, this->selectedMeshIndex);

    while (!getContext()->window.shouldClose())
    {
        getContext()->window.pollEvents();
        
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawBG(Settings::defaultFramebuffer()->bgTxtr);

        dialogControl.updateStart();

        //Render the panel
        this->bgPanel.render(timer, true);
        this->subPanel.render(timer, true);

        for (size_t i = 0; i < this->bgPanel.sections[0].elements.size(); i++)
        {
            Button txtBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(this->bgPanel.sections[0].elements[i].button.scale.x, 1.f), this->bgPanel.sections[0].elements[i].button.text, Texture(), 0.f, false);
            txtBtn.pos = this->bgPanel.sections[0].elements[i].button.pos;
            txtBtn.pos.z += 0.0001f;
            txtBtn.pos.y += this->bgPanel.sections[0].elements[i].button.scale.y - txtBtn.scale.y;
            txtBtn.color.a /= 2.f;
            txtBtn.render(timer, false);
        }
        

        if(this->subPanel.sections[0].elements[0].button.clicked){
            selectedMeshI = this->selectedMeshIndex;
            dialogControl.unActivate();
            this->selectedMeshIndex = 0;
        }

        for (size_t i = 0; i < this->bgPanel.sections[0].elements.size(); i++){
            if(this->bgPanel.sections[0].elements[i].button.clickState1 && i != this->selectedMeshIndex){
                this->selectedMeshIndex = i;
                for (size_t j = 0; j < this->bgPanel.sections[0].elements.size(); j++)
                {
                    this->bgPanel.sections[0].elements[j].button.clickState1 = false;
                }
            }
        }

        for (size_t i = 0; i < this->bgPanel.sections[0].elements.size(); i++)
        {
            if(this->bgPanel.sections[0].elements[i].button.clickState1 == false && i == this->selectedMeshIndex)
                this->bgPanel.sections[0].elements[i].button.clickState1 = true;
        }

        dialogControl.updateEnd(timer,0.15f);

        //End the dialog
        if((getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)) || (!this->bgPanel.hover && !this->subPanel.hover && *Mouse::LClick())){
            dialogControl.unActivate();
            this->selectedMeshIndex = 0;
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





// ---------- UTILITY FUNCTIONS -----------
static void initMeshSelectionDialog(
                                        int &selectedTextureMode, 
                                        Panel& bgPanel, 
                                        int& selectedMeshIndex
                                    )
{
    selectedTextureMode = 0;

    bgPanel.sections.clear();

    std::vector<Element> meshButtons;

    for (size_t i = 0; i < getModel()->meshes.size(); i++)
    {
        meshButtons.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(10.f), getModel()->meshes[i].materialName, getModel()->meshes[i].displayingTxtr, 0.f, true));
    }
    

    bgPanel.sections.push_back(
        Section(
            Button(),
            meshButtons
        )
    );
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