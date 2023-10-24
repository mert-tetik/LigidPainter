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

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"

#define LIGIDPAINTER_DEBUGGING_MODE false

struct DebugObject{
    double time = 0.;
    std::string name = "";
    DebugObject(){}
    DebugObject(double time, std::string name){
        this->name = name;
        this->time = time;
    }
};

static std::vector<DebugObject> objects;

void Debugger::block(std::string name){
    if(LIGIDPAINTER_DEBUGGING_MODE){
        bool matched = false;
        
        for (size_t i = 0; i < objects.size(); i++)
        {
            if(objects[i].name == name){
                std::cout << "Block : " << name << " Duration : " << 1 / (LigidGL::getTime() - objects[i].time) << std::endl; 
                matched = true;

                objects.erase(objects.begin() + i);
            }
        }
        
        if(!matched){
            objects.push_back(DebugObject(LigidGL::getTime(), name));
        }
    }
}