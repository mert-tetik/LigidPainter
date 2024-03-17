/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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
#include "ColorPaletteSystem/ColorPalette.hpp"
#include "MouseSystem/Mouse.hpp"

#include <string>
#include <iostream>
#include <vector>

ContextMenu::ContextMenu(){}

ContextMenu::ContextMenu(std::vector<std::string> elements){
    
    //Create the context panel
    contextPanel = Panel(
        {
            //Sections
        },
        glm::vec2(5.f,5.f), //Scale
        glm::vec3(50.f,50.f,0.95f), //Pos
        ColorPalette::mainColor,
        glm::vec4(0.f),
        true,
        true,
        true,
        true,
        true,
        1.f,
        1,
        {},
        20.f,
        false
    );

    contextPanel.solidStyle = true;

    //Push the elements taken from parameter into the context panel
    Section section;
    for (size_t i = 0; i < elements.size(); i++)
    {
        Element btn = Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1,1), elements[i], Texture(), 0.f, false));
        btn.button.color = ColorPalette::themeColor;
        btn.button.color.a /= 2.f;
        btn.button.color2 = glm::vec4(0.f);
        section.elements.push_back(btn);
    }
    
    contextPanel.sections.push_back(section);
    contextPanel.scale.y = elements.size();
}

int ContextMenu::show(Timer &timer){
    
    dialogControl.activate();

    //Update the position of the context panel
    contextPanel.pos = glm::vec3(
                                    Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f,
                                    Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f + ContextMenus::texture.contextPanel.scale.y,
                                    0.95f
                                );

    while (!getContext()->window.shouldClose())
    {
        dialogControl.updateStart();
        
        //Render the context panel
        contextPanel.render(timer, true);
    
        bool anyBtnClicked = false;
        for (size_t i = 0; i < this->contextPanel.sections[0].elements.size(); i++)
        {
            if(this->contextPanel.sections[0].elements[i].button.clicked){
                this->dialogControl.unActivate();
                this->dialogControl.mixVal = 0.f;
                return i;
            }
        }

        if (   // Conditions to turn any context menu off
            anyBtnClicked  || // Mouse left click
            *Mouse::mouseScroll() || //Mouse middle click
            getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || //Pressed to escape key 
            getContext()->window.isKeyPressed(LIGIDGL_KEY_ENTER) == LIGIDGL_PRESS || //Pressed to enter key
            !this->contextPanel.hover
        )
        {
            this->dialogControl.unActivate(); //Turn the context menu offs
        }
        
        dialogControl.updateEnd(timer, 0.15f);

        if(dialogControl.mixVal == 0.f)
            break;
    }

    return -1;
}