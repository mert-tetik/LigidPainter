/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"

Filter::Filter(){

}

int Filter::load(std::string path){
    std::ifstream rf(path);

    if (!rf.is_open()) {
        LGDLOG::start << "ERROR : Reading filter file : Can't open the file : " << path << LGDLOG::end;
        return 0;
    }

    std::string firstLine;
    if (!std::getline(rf, firstLine)) {
        LGDLOG::start << "ERROR : Reading filter file : Can't read the header : " << path << LGDLOG::end;
        return 0;
    }
    
    if(firstLine != "#ligidFilter"){
        LGDLOG::start << "ERROR : Reading filter file : Can't locate the header : " << path << LGDLOG::end;
        return 0;
    }

    std::string code;
    
    std::string line;
    while (std::getline(rf, line))
    {
        code += line;
    }
    
    this->shader.loadShaderPS("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", code);

    return 1;
}