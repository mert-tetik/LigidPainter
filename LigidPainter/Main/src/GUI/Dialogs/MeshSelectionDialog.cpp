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
                                                Button(ELEMENT_STYLE_STYLIZED, glm::vec2(2.f), "Select", Texture(), 0.f, false)
                                            }
                                        )
                            }, glm::vec2(scale.x, 2.f), glm::vec3(pos.x,pos.y + this->bgPanel.scale.y + 2.f ,pos.z), ColorPalette::secondColor, ColorPalette::thirdColor, true, true, false, true, true, 1.f, 5, {}, 20.f, true);
}

//Forward declarations for the utility functions
static void initMeshSelectionDialog(int &selectedTextureMode, unsigned int& bgTexture, glm::ivec2& windowSize, Panel& subPanel, int& selectedMeshIndex);
static void drawBG(unsigned int bgTexture, glm::ivec2 windowSize);

void MeshSelectionDialog::show(Timer &timer, glm::mat4 guiProjection, int& selectedMeshI){
    
    this->dialogControl.activate();

    this->selectedMeshIndex = selectedMeshI;
        
    unsigned int bgTexture;  
    glm::ivec2 windowSize;
    initMeshSelectionDialog(this->selectedTextureMode, bgTexture, windowSize, this->bgPanel, this->selectedMeshIndex);

    while (!getContext()->window.shouldClose())
    {
        getContext()->window.pollEvents();
        
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawBG(bgTexture, windowSize);

        dialogControl.updateStart();

        //Render the panel
        this->bgPanel.render(timer, true);
        this->subPanel.render(timer, true);

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
    }
}





// ---------- UTILITY FUNCTIONS -----------
static void initMeshSelectionDialog(
                                        int &selectedTextureMode, 
                                        unsigned int& bgTexture, 
                                        glm::ivec2& windowSize, 
                                        Panel& bgPanel, 
                                        int& selectedMeshIndex
                                    )
{
    // Get the viewport size
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int viewportWidth = viewport[2];
    int viewportHeight = viewport[3];

    windowSize = glm::ivec2(viewportWidth, viewportHeight);

    GLfloat* pixels = new GLfloat[windowSize.x * windowSize.y * 4];
    glReadPixels(0, 0, windowSize.x, windowSize.y, GL_RGBA, GL_FLOAT, pixels);

    glGenTextures(1, &bgTexture);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bgTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowSize.x, windowSize.y, 0, GL_RGBA, GL_FLOAT, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    
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
                    unsigned int bgTexture, 
                    glm::ivec2 windowSize
                )
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bgTexture);
    ShaderSystem::buttonShader().setVec3("pos", glm::vec3(windowSize / glm::ivec2(2), 0.1));
    ShaderSystem::buttonShader().setVec2("scale", windowSize / glm::ivec2(2));
    ShaderSystem::buttonShader().setFloat("properties.colorMixVal", 0.f);
    ShaderSystem::buttonShader().setInt("states.renderTexture",     1    );
    ShaderSystem::buttonShader().setInt("properties.txtr",     0    );
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    ShaderSystem::buttonShader().setInt("states.renderTexture"  ,     0    );
}