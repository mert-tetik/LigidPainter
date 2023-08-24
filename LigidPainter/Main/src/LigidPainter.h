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

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"
#include "SettingsSystem/Settings.hpp"

#include "../../thirdparty/include/glm/glm.hpp"

#include "Renderer.h"

Renderer::Renderer(){

}

class LigidPainter{
public:
    int run(){
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
        
        while(!getContext()->window.shouldClose())
        {   
            renderer.render();
        }

        return 1;
    }
};