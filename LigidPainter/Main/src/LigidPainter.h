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

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../thirdparty/include/glm/glm.hpp"

//#include "Renderer.h"

#include "LigidGL/LigidGL.hpp"

void mouseBtnCallback(int button, int action, int mods){
    std::cout << button << ' ' << action << ' ' << mods << std::endl;
}

class LigidPainter{
public:
    int run(){
        //glfwInit();//Init the glfw before initializing renderer
        //Renderer renderer(glm::vec2(glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height));
        
        LigidWindow ligidWindow;
        ligidWindow.createWindow(800, 800, "ASDFSAFD"); 

        ligidWindow.setMouseButtonCallback(mouseBtnCallback);

        ligidWindow.setWindowPos(1000,500);

        //int monitorWidth = 0;
        //int monitorHeight = 0;
        //int monitorRefreshRate = 0;
//
        //LigidGL::getPrimaryMonitorData(monitorWidth, monitorHeight, monitorRefreshRate);


        while (!ligidWindow.shouldClose())
        {
            ligidWindow.pollEvents();

            ligidWindow.swapBuffers();
        }
        
        //while(!glfwWindowShouldClose(renderer.context.window))
        //{   
        //    //renderer.render();
        //}
        
        glfwTerminate();
        return 1;
    }
};