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
    std::vector<Shader> channelShaders;  

    int modifierIndex;//0 for the texture modifier

    //Sections of the modifier panel
    std::vector<Section> sections;   

    std::string title;

    MaterialModifier(){}
    MaterialModifier(std::string title,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures, std::vector<Section> sections,int modifierIndex,
                    std::vector<std::string>shaderPaths){

        for (size_t i = 0; i < shaderPaths.size(); i++)
        {
            channelShaders.push_back(Shader("LigidPainter/Resources/Shaders/UI/2DBox.vert",shaderPaths[i].c_str()));
        }

        this->title = title;    
        this->sections = sections;
        this->modifierIndex = modifierIndex;
    }
};

#endif