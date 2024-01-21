/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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

extern bool ligidPainter_DEBUG_MODE;
#define LIGIDPAINTER_DEBUGGING_TEST_MODE false
#define LIGIDPAINTER_DEBUGGING_MEMORY_ONLY false

struct DebugObject{
    double time = 0.;
    size_t memoryUsage = 0;
    std::string name = "";
    DebugObject(){}
    DebugObject(double time, size_t memoryUsage, std::string name){
        this->name = name;
        this->memoryUsage = memoryUsage;
        this->time = time;
    }
};

static std::vector<DebugObject> objects;

void Debugger::block(std::string name){
    if(ligidPainter_DEBUG_MODE){
        bool matched = false;
        
        for (size_t i = 0; i < objects.size(); i++)
        {
            if(objects[i].name == name){
                if(LIGIDPAINTER_DEBUGGING_TEST_MODE){
                    if(name == "TEST"){
                        if((LIGIDPAINTER_DEBUGGING_MEMORY_ONLY && (int)LigidGL::getRamUsage() - (int)objects[i].memoryUsage != 0) || !LIGIDPAINTER_DEBUGGING_MEMORY_ONLY)
                            std::cout << "Block : " << name << " Duration : " << 1 / (LigidGL::getTime() - objects[i].time) << ", Registered Memory : " << (int)LigidGL::getRamUsage() - (int)objects[i].memoryUsage << ", Memory Left : " << LigidGL::getRamLeft() << " Out of " << LigidGL::getTotalRam()  << std::endl; 
                    }
                }
                else
                    if((LIGIDPAINTER_DEBUGGING_MEMORY_ONLY && (int)LigidGL::getRamUsage() - (int)objects[i].memoryUsage != 0) || !LIGIDPAINTER_DEBUGGING_MEMORY_ONLY)
                        std::cout << "Block : " << name << " Duration : " << 1 / (LigidGL::getTime() - objects[i].time) << ", Registered Memory : " << (int)LigidGL::getRamUsage() - (int)objects[i].memoryUsage << ", Memory Left : " << LigidGL::getRamLeft() << " Out of " << LigidGL::getTotalRam() << std::endl; 
                
                matched = true;

                objects.erase(objects.begin() + i);
            }
        }
        
        if(!matched){
            objects.push_back(DebugObject(LigidGL::getTime(), LigidGL::getRamUsage(), name));
        }
    }
}