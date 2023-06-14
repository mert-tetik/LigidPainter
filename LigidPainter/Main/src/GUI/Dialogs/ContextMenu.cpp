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
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"

#include <string>
#include <iostream>
#include <vector>

ContextMenu::ContextMenu(){}

ContextMenu::ContextMenu(Shader buttonShader,ColorPalette colorPalette,std::vector<std::string> elements){
    
    //Create the context panel
    contextPanel = Panel(
        buttonShader,
        colorPalette,
        {
            //Sections
        },
        glm::vec2(5.f,5.f), //Scale
        glm::vec3(50.f,50.f,0.95f), //Pos
        colorPalette.mainColor,
        colorPalette.secondColor,
        true,
        true,
        true,
        true,
        true,
        1.f,
        1,
        {},
        20.f
    );

    //Push the elements taken from parameter into the context panel
    Section section;
    for (size_t i = 0; i < elements.size(); i++)
    {
        section.elements.push_back(Element(Button(1,glm::vec2(1,1),colorPalette,buttonShader,elements[i],Texture(),0.f,false)));
    }
    
    contextPanel.sections.push_back(section);
    contextPanel.scale.y = elements.size();
}

void ContextMenu::render(glm::vec2 videoScale,Mouse& mouse,Timer &timer,TextRenderer &textRenderer){
    //Update the position of the context panel
    contextPanel.pos = pos;

    //Render the context panel
    contextPanel.render(videoScale,mouse,timer,textRenderer,true);
}