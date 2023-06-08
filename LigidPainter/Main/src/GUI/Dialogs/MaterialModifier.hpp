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

#ifndef LGDUIMATERIALMODIFIER_HPP
#define LGDUIMATERIALMODIFIER_HPP

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

class MaterialModifier
{
private:
    /* data */
public:
    //0 = albedo
    //1 = roughness
    //2 = metallic 
    //3 = normal map
    //4 = height map
    //5 = ambient Occlusion
    Shader shader;  

    int modifierIndex;//0 for the texture modifier

    //Sections of the modifier panel
    std::vector<Section> sections;   

    std::string title;

    MaterialModifier(){}
    MaterialModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures,int modifierIndex){
        if(modifierIndex == 0){
            this->sections = createTextureModifier(colorPalette,buttonShader,appTextures);
            this->title = "Texture Modifier";    
            shader = Shader("LigidPainter/Resources/Shaders/UI/2DBox.vert","LigidPainter/Resources/Shaders/MaterialModifiers/TextureModifier.frag");
        }

        this->modifierIndex = modifierIndex;
    }

    std::vector<Section> createTextureModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
            
            return {
                Section(
                    Element(Button(1,glm::vec2(1,2.f),colorPalette,buttonShader,"Channels",Texture(),0.f,true)),
                    {
                        Element(Button(1,glm::vec2(1,1.5f),colorPalette,buttonShader,"Albedo",              appTextures.greetingDialogImage,0.f,false)),
                        Element(Button(1,glm::vec2(1,1.5f),colorPalette,buttonShader,"Roughness",           appTextures.greetingDialogImage,0.f,false)),
                        Element(Button(1,glm::vec2(1,1.5f),colorPalette,buttonShader,"Metallic",            appTextures.greetingDialogImage,0.f,false)),
                        Element(Button(1,glm::vec2(1,1.5f),colorPalette,buttonShader,"Normal map",          appTextures.greetingDialogImage,0.f,false)),
                        Element(Button(1,glm::vec2(1,1.5f),colorPalette,buttonShader,"Height map",          appTextures.greetingDialogImage,0.f,false)),
                        Element(Button(1,glm::vec2(1,1.5f),colorPalette,buttonShader,"Ambient Occlusion",   appTextures.greetingDialogImage,0.f,false))
                    }
                )
            };
    }
};

#endif