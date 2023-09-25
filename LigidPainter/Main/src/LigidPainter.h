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

#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include "../../thirdparty/include/glm/glm.hpp"

#include "SettingsSystem/Settings.hpp"
#include "Renderer.h"
#include "UTIL/Util.hpp"

Renderer::Renderer(){

}

ThreadElements projectUpdatingThreadElements;

class LigidPainter{
public:
    int run(){

        // //Create the copy context for another threads    
        getCopyContext()->window.createWindow(1, 1, L"Copy Thread");
        //Init GLAD
        if (!gladLoadGLLoader((GLADloadproc)LigidGL::getProcAddress))
        {
            LGDLOG::start<< "Failed to initialize GLAD" << LGDLOG::end;
        }    
        
        // .x : width | .y : height | .z : refresh rate  
        glm::ivec3 primaryMonitorData;
        LigidGL::getPrimaryMonitorData(primaryMonitorData.x, primaryMonitorData.y, primaryMonitorData.z);

        *Settings::videoScale() = glm::vec2(primaryMonitorData.x, primaryMonitorData.y);

        //Create the window and make it's OpenGL context current    
        getContext()->window.createWindow(Settings::videoScale()->x, Settings::videoScale()->y, L"LigidPainter");

        //Show the created window
        getContext()->window.show();
       
        //Init GLAD
        if (!gladLoadGLLoader((GLADloadproc)LigidGL::getProcAddress))
        {
            LGDLOG::start<< "Failed to initialize GLAD" << LGDLOG::end;
        }    

        Renderer renderer;
        renderer.initRenderer();
        // Start the export thread
        std::thread projectUpdatingThreadX(projectUpdatingThread, std::ref(renderer.project));

        while(!getContext()->window.shouldClose())
        {   
            renderer.render();
            
            // Notify the testThread to print "working"
            {
                //std::unique_lock<std::mutex> lock(testMutex);
                //exportCV.notify_one();
            }
        }

        // Signal the projectUpdatingThread to exit
        //projectUpdatingThreadElements.exportCV.notify_one();
        projectUpdatingThreadElements.isRunning = false;

        // Wait for the projectUpdatingThread to finish
        projectUpdatingThreadX.join();

        return 1;
    }
};