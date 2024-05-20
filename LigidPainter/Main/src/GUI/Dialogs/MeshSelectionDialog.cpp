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
#include <glm/gtx/string_cast.hpp>

#include "GUI/GUI.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

MeshSelectionDialog::MeshSelectionDialog(int){
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

void MeshSelectionDialog::show(Timer &timer, int& selectedMeshI){
    
    Settings::defaultFramebuffer()->setViewport(); 

    this->dialogControl.activate();

    this->selectedMeshIndex = selectedMeshI;
        
    initMeshSelectionDialog(this->selectedTextureMode, this->bgPanel, this->selectedMeshIndex);

    while (!getContext()->window.shouldClose())
    {
        dialogControl.updateStart(true);

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

        //End the dialog
        if((getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)) || (!this->bgPanel.hover && !dialog_log.isHovered() && !this->subPanel.hover && *Mouse::LClick())){
            dialogControl.unActivate();
            this->selectedMeshIndex = 0;
        }

        dialogControl.updateEnd(timer,0.15f); 

        if(dialogControl.mixVal == 0.f)
            break;
    }

    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport(); 
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

    for (size_t i = 0; i < getScene()->model->meshes.size(); i++)
    {
        meshButtons.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(10.f), getScene()->model->meshes[i].materialName, getScene()->model->meshes[i].displayingTxtr, 0.f, true));
    }
    

    bgPanel.sections.push_back(
        Section(
            Button(),
            meshButtons
        )
    );
}