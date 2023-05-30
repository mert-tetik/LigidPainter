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

#ifndef LGDPAINTER_HPP
#define LGDPAINTER_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Shader.hpp"
#include "Box.hpp"
#include "Renderer.h"
#include "Util.hpp"
#include "Timer.hpp"
#include "Texture.hpp"

#include "Mouse.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>
#include <filesystem>

#include "tinyfiledialogs.h"

class Painter
{
public:
    //3 selected colors to paint
    //These colors will be switched between & at the end the color1 will used to paint. 
    glm::vec3 color1; 
    glm::vec3 color2; 
    glm::vec3 color3; 

    Painter(){

    }

    void changeColor(glm::vec3 &color,glm::vec3 value){
        //tinyfd_colorChooser("Select a color","#000000",{},{}) << std::endl;
        color = value;
    }
private:
};


#endif