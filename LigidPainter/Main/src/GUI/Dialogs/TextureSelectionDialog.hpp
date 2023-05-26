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

#ifndef LGDUITEXTURESELECTIONDIALOG_HPP
#define LGDUITEXTURESELECTIONDIALOG_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.hpp"
#include "Box.hpp"
#include "Renderer.h"
#include "Util.hpp"

#include "GUI/UI.hpp"
#include "GUI/Button.hpp"
#include "Mouse.hpp"
#include "Timer.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

class TextureSelectionDialog
{
private:
    ColorPalette colorPalette;
    Shader buttonShader;
public:
    Panel texturePanel; //That panel contains the textures
    bool active = false;
    //Selected index is set to 1000 if nothing is selected
    int selectedTextureIndex = 1000; 
    bool clicked = false; //Get the result if clicked to a texture

    glm::vec3 pos = glm::vec3(50,50,0.95f); //Position of the dialog
    glm::vec2 scale = glm::vec2(10); //Scale of the dialog
    
    TextureSelectionDialog(){}
    TextureSelectionDialog(Shader buttonShader,ColorPalette colorPalette){
        this->colorPalette = colorPalette;
        this->buttonShader = buttonShader;

        texturePanel = Panel(buttonShader,colorPalette,{},scale,pos,colorPalette.mainColor,colorPalette.thirdColor,true,true,false,true,true,1.f,15.f,{});
    }

    void render(glm::vec2 videoScale,Mouse &mouse,Timer &timer,TextRenderer &textRenderer,Library library){
        texturePanel.sections.clear();
        std::vector<Element> sectionElements;
        for (size_t i = 0; i < library.textures.size(); i++)
        {
            sectionElements.push_back(Element(Button(1,glm::vec2(2,2.f),colorPalette,buttonShader,"texture_0"       , library.textures[i], 0.f,false)));
        }
        
        texturePanel.sections.push_back(
            Section
                    (
                        Element(Button()),
                        sectionElements
                    )
        );
        texturePanel.render(videoScale,mouse,timer,textRenderer);

        for (size_t i = 0; i < texturePanel.sections[0].elements.size(); i++)
        {
            if(texturePanel.sections[0].elements[i].button.hover && mouse.LClick){
                selectedTextureIndex = i;
            }
        }
        
    }
    
};

#endif