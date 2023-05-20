/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

Timer takes care of time related things
Like getting fps or frame rendering speed
And helps you manage the time
*/

#ifndef LGDTIMER_HPP
#define LGDTIMER_HPP

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

#include "GUI/Panel.hpp"
#include "GUI/Button.hpp"
#include "Mouse.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

class Timer
{
private:
    double lastTimeT = glfwGetTime();
    int nbFramesT = 0;
public:
    Timer(/* args */){
        
    }

    double FPS = 1;
    double renderingSpeed = 0;

    
    bool runTimer(){ 
        //Returns true every second

        double currentTime = glfwGetTime();
        nbFramesT++;
        if (currentTime - lastTimeT >= 1.0) {
        	if(nbFramesT > 5 && nbFramesT < 800)
                FPS = (double)nbFramesT;
            renderingSpeed = 1000./FPS;
            nbFramesT = 0;
        	lastTimeT += 1.0;
        	return true;
        }
        return false;
    }
};


#endif