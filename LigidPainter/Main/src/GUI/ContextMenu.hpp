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

#ifndef LGDUICONTEXTMENU_HPP
#define LGDUICONTEXTMENU_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "GUI/UI.hpp"

#include "Shader.hpp"
#include "Renderer.h"
#include "Util.hpp"
#include "Timer.hpp"
#include "Texture.hpp"
#include "Mouse.hpp"
#include "Util.hpp"

#include "ColorPalette.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>


class ContextMenu
{
private:
    /* data */
public:
    Panel contextPanel;
    glm::vec3 pos;
    int selectedElement = 0;
    bool active = false; 
    ContextMenu(){}
    ContextMenu(Shader buttonShader,ColorPalette colorPalette,std::vector<std::string> elements){
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

        Section section;
        for (size_t i = 0; i < elements.size(); i++)
        {
            section.elements.push_back(Element(Button(1,glm::vec2(1,1),colorPalette,buttonShader,elements[i],Texture(),0.f,false)));
        }
        contextPanel.sections.push_back(section);
    }

    void render(glm::vec2 videoScale,Mouse& mouse,Timer &timer,TextRenderer &textRenderer){
        contextPanel.pos = pos;
        contextPanel.render(videoScale,mouse,timer,textRenderer,true);
    }
};


#endif